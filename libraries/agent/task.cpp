#include "task.h"

namespace Cosmos {
    namespace Support {
        Task::Task(string node, string agent)
        {
            if (agent.empty())
            {
                AgentName = "exec";
            }
            if (node.empty())
            {
                char hostname[60];
                gethostname(hostname, sizeof (hostname));
                NodeName = hostname;
            }
            state = 1;
            mythread = thread([=] { Runner(); });
        }

        Task::~Task()
        {
            state = 2;
            ElapsedTime et;
            while (et.split() < 5. && state != 3)
            {
                std::this_thread::yield();
            }
            if (mythread.joinable())
            {
                mythread.join();
            }
        }

        void Task::Runner()
        {
            state = 1;
            while (state != 2)
            {
                mtx.lock();
                for(auto iter=tasks.begin(); iter!=tasks.end();)
                {
                    if ((*iter).state == 0)
                    {
                        (*iter).result = std::async(std::launch::async, [=] { return data_task((*iter).command, (*iter).path+".out"); });
                        (*iter).state = 1;
                    }
                    else if ((*iter).state == 1)
                    {
                        (*iter).runtime = 86400. * (currentmjd() - (*iter).startmjd);
                        if ((*iter).result.valid() && (*iter).result.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            (*iter).iretn = (*iter).result.get();
                            (*iter).state = 2;
                            log_move((*iter).path, data_base_path(NodeName, "outgoing", AgentName, data_name(currentmjd(), "", NodeName, AgentName)), true);
                            (*iter).path = data_base_path(NodeName, "temp", AgentName, data_name(currentmjd(), "", NodeName, AgentName));
                        }
                    }
                    ++iter;
                }
                mtx.unlock();
                std::this_thread::yield();
            }
            state = 3;
            return;
        }

        int32_t Task::Add(string command)
        {
            mtx.lock();
            tasks.resize(tasks.size()+1);
            tasks.back().startmjd = currentmjd();
            tasks.back().state = 0;
            tasks.back().command = command;
            tasks.back().path = data_base_path(NodeName, "temp", AgentName, data_name(currentmjd(), "", NodeName, AgentName));
            mtx.unlock();
            return tasks.size();
        }

        int32_t Task::Del(uint32_t deci)
        {
            mtx.lock();
            for(auto iter=tasks.begin(); iter!=tasks.end();)
            {
                if (deci == decisec((*iter).startmjd))
                {
                    std::swap(*iter, tasks.back());//swap with the back
                    tasks.pop_back();     //erase the element
                }
                else
                {
                    ++iter;
                }
            }
            mtx.unlock();
            return tasks.size();
        }

        int32_t Task::Iretn(uint16_t number)
        {
            int32_t iretn = 0;
            mtx.lock();
            if (tasks.empty())
            {
                iretn = 0;
            }
            else if (number < tasks.size())
            {
                iretn = tasks[number].iretn;
            }
            else
            {
                iretn = tasks.back().iretn;
            }
            mtx.unlock();
            return (iretn);
        }

        uint32_t Task::Deci(uint16_t number)
        {
            uint32_t deci = 0;
            mtx.lock();
            if (tasks.empty())
            {
                deci = 0;
            }
            else if (number < tasks.size())
            {
                deci = decisec(tasks[number].startmjd);
            }
            else
            {
                deci = decisec(tasks.back().startmjd);
            }
            mtx.unlock();
            return (deci);
        }

        double Task::Startmjd(uint16_t number)
        {
            double startmjd = 0.;
            mtx.lock();
            if (tasks.empty())
            {
                startmjd = 0.;
            }
            else if (number < tasks.size())
            {
                startmjd = tasks[number].startmjd;
            }
            else
            {
                startmjd = tasks.back().startmjd;
            }
            mtx.unlock();
            return startmjd;
        }

        uint8_t Task::State(uint16_t number)
        {
            uint8_t state = 0;
            mtx.lock();
            if (tasks.empty())
            {
                state = 0;
            }
            else if (number < tasks.size())
            {
                state = tasks[number].state;
            }
            else
            {
                state = tasks.back().state;
            }
            mtx.unlock();
            return state;
        }

        string Task::Command(uint16_t number)
        {
            string command = "";
            mtx.lock();
            if (tasks.empty())
            {
                command = "";
            }
            else if (number < tasks.size())
            {
                command = tasks[number].command;
            }
            else
            {
                command = tasks.back().command;
            }
            mtx.unlock();
            return command;
        }


        string Task::Path(uint16_t number)
        {
            string path = "";
            mtx.lock();
            if (tasks.empty())
            {
                path = "";
            }
            else if (number < tasks.size())
            {
                path = tasks[number].path;
            }
            else
            {
                path = tasks.back().path;
            }
            mtx.unlock();
            return path;
        }

        uint16_t Task::Size()
        {
            return tasks.size();
        }
    }
}

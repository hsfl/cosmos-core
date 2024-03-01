/**
 * @file task.cpp
 * @brief 
 * 
 * Copyright (C) 2024 by Interstel Technologies, Inc. and Hawaii Space Flight
 * Laboratory.
 * 
 * This file is part of the COSMOS/core that is the central module for COSMOS.
 * For more information on COSMOS go to <http://cosmos-project.com>
 * 
 * The COSMOS/core software is licenced under the GNU Lesser General Public
 * License (LGPL) version 3 licence.
 * 
 * You should have received a copy of the GNU Lesser General Public License. If
 * not, go to <http://www.gnu.org/licenses/>
 * 
 * COSMOS/core is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 * 
 * COSMOS/core is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * Refer to the "licences" folder for further information on the conditions and
 * terms to use this software.
 */

#include "task.h"
#include "support/stringlib.h"

namespace Cosmos {
    namespace Support {
        /**
         * @brief Construct a new Task:: Task object
         * 
         * @param node 
         * @param agent 
         * 
         * @todo Document this.
         */
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
        }

        /**
         * @brief Destroy the Task:: Task object
         * 
         * @todo Document this.
         */
        Task::~Task()
        {
            if (!state)
            {
                return;
            }
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

        /**
         * @brief 
         * 
         * @todo Document this.
         */
        void Task::Start()
        {
            state = 1;
            mythread = thread([=] { Runner(); });
        }

        /**
         * @brief 
         * 
         * @todo Document this.
         */
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
                        (*iter).result = std::async(std::launch::async, [=] { return data_task((*iter).command, (*iter).path); });
                        (*iter).state = 1;
                    }
                    else if ((*iter).state == 1)
                    {
                        if ((*iter).result.valid() && (*iter).result.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            (*iter).iretn = (*iter).result.get();
                            (*iter).state = 2;
//                            log_move_file((*iter).path, data_base_path(NodeName, "outgoing", AgentName, data_name((*iter).startmjd, "out", NodeName, AgentName)), true);
//                            (*iter).path = data_base_path(NodeName, "temp", AgentName, data_name((*iter).startmjd, "out", NodeName, AgentName));
                            log_move_file((*iter).path, string_replace((*iter).path, "/temp/", "/outgoing/"), true);
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

        /**
         * @brief 
         * 
         * @param command 
         * @param node 
         * @return int32_t 
         * 
         * @todo Document this.
         */
        int32_t Task::Add(string command, string node)
        {
            mtx.lock();
            tasks.resize(tasks.size()+1);
            tasks.back().startmjd = currentmjd();
            tasks.back().state = 0;
            tasks.back().command = command;
            if (node.empty())
            {
                tasks.back().path = data_base_path(NodeName, "temp", AgentName, data_name(tasks.back().startmjd, "task", NodeName, AgentName));
            }
            else
            {
                tasks.back().path = data_base_path(node, "temp", AgentName, data_name(tasks.back().startmjd, "task", NodeName, AgentName));
            }
            mtx.unlock();
            return tasks.size();
        }

        /**
         * @brief 
         * 
         * @param deci 
         * @return int32_t 
         * 
         * @todo Document this.
         */
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

        /**
         * @brief 
         * 
         * @param number 
         * @return int32_t 
         * 
         * @todo Document this.
         */
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

        /**
         * @brief 
         * 
         * @param number 
         * @return uint32_t 
         * 
         * @todo Document this.
         */
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

        /**
         * @brief 
         * 
         * @param number 
         * @return double 
         * 
         * @todo Document this.
         */
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

        /**
         * @brief 
         * 
         * @param number 
         * @return uint8_t 
         * 
         * @todo Document this.
         */
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

        /**
         * @brief 
         * 
         * @param number 
         * @return string 
         * 
         * @todo Document this.
         */
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

        /**
         * @brief 
         * 
         * @param number 
         * @return string 
         * 
         * @todo Document this.
         */
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

        /**
         * @brief 
         * 
         * @return uint16_t 
         * 
         * @todo Document this.
         */
        uint16_t Task::Size()
        {
            return tasks.size();
        }
    }
}

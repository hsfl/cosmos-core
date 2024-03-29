#include "unixgpio.h"
#include "support/datalib.h"

namespace Cosmos {
    namespace Devices {
        namespace General {
            //! Establish GPIO by pin number, with alternate name
            //! Exports GPIO pin number. Some implementations use an alternate name, instead
            //! of gpiopin#. If provided name is not empty, it will be used instead.
            //! \param dir IN or OUT
            //! \param pin Pin number as string
            //! \param name Alternate name to use instead of gpioppp (where ppp is pin number)
            UnixGpio::UnixGpio(UnixGpio::Direction dir, string pin, string name)
            {
                int32_t iretn = 0;
                if (name.empty())
                {
                    name = "gpio"+pin;
                }
                path = "/sys/class/gpio/" + pinname;
                if (!data_isfile(path))
                {
                    ofstream fs;
                    fs.open("/sys/class/gpio/export");
                    if (!fs.is_open()){
                        error = -errno;
                        open = false;
                        return;
                    }
                    fs << pin;
                    fs.close();
                    path = "/sys/class/gpio/" + name + "/";
                }
                open = true;
                direction = dir;
                iretn = setDirection(dir);
                if (iretn < 0)
                {
                    error = iretn;
                    open = false;
                    return;
                }
                error = 0;
                open = true;
            }

            int32_t UnixGpio::setDirection(UnixGpio::Direction dir)
            {
                if (open)
                {
                    switch(dir){
                    case INPUT:
                        return write("direction","in");
                    case OUTPUT:
                        return write("direction","out");
                    }
                }
                return GENERAL_ERROR_NOTREADY;
            }

            int32_t UnixGpio::write(string filename, string value)
            {
                if (open)
                {
                    ofstream fs;
                    fs.open((path + filename).c_str());
                    if (!fs.is_open()){
                        error = GENERAL_ERROR_OUTOFRANGE;
                        return error;
                    }
                    fs << value;
                    fs.close();
                    error = 0;
                    return 0;
                }
                else {
                    error = GENERAL_ERROR_NOTREADY;
                    return GENERAL_ERROR_NOTREADY;
                }
            }

            string UnixGpio::read(string filename)
            {
                if (open)
                {
                    ifstream fs;
                    fs.open((path + filename).c_str());
                    if (!fs.is_open()){
                        error = GENERAL_ERROR_OUTOFRANGE;
                        return "";
                    }
                    string input;
                    getline(fs,input);
                    fs.close();
                    error = 0;
                    return input;
                }
                else {
                    error = GENERAL_ERROR_NOTREADY;
                    return "";
                }
            }

            int32_t UnixGpio::write(UnixGpio::Value val)
            {
                if (open)
                {
                    switch(val){
                    case HIGH: return write("value", "1");
                    case LOW: return write("value", "0");
                    }
                }
                else {
                    error = GENERAL_ERROR_NOTREADY;
                    return error;
                }
				return 0;
            }

            int32_t UnixGpio::read()
            {
                if (open)
                {
                    return stoi(read("value"));
                }
                else {
                    error = GENERAL_ERROR_NOTREADY;
                    return error;
                }
            }

            int32_t UnixGpio::geterror()
            {
                return error;
            }

            bool UnixGpio::isopen()
            {
                return  open;
            }

            string UnixGpio::getpath()
            {
                return path;
            }

        }
    }
}

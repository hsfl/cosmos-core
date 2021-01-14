#include "unixgpio.h"

namespace Cosmos {
    namespace Devices {
        namespace General {
            UnixGpio::UnixGpio(UnixGpio::Direction dir, string pin)
            {
                this->direction = dir;
                this->setDirection(dir);
                this->path = "/sys/class/gpio/" + pin;
            }

            int32_t UnixGpio::setDirection(UnixGpio::Direction dir)
            {
                switch(dir){
                case INPUT:
                    return this->write("direction","in");
                case OUTPUT:
                    return this->write("direction","out");
                }
                return -1;
            }

            int32_t UnixGpio::write(string filename, string value)
            {
                ofstream fs;
                fs.open((this->path + filename).c_str());
                if (!fs.is_open()){
                    perror("GPIO: write failed to open file ");
                    return -1;
                }
                fs << value;
                fs.close();
                return 0;
            }

            string UnixGpio::read(string filename)
            {
                ifstream fs;
                fs.open((this->path + filename).c_str());
                if (!fs.is_open()){
                    perror("GPIO: read failed to open file ");
                 }
                string input;
                getline(fs,input);
                fs.close();
                return input;
            }

            int32_t UnixGpio::write(UnixGpio::Value val)
            {
                switch(val){
                case HIGH: return this->write("value", "1");
                case LOW: return this->write("value", "0");
                }
                return -1;
            }



        }
    }
}

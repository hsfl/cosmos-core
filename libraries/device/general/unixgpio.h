#ifndef UnixGpio_H
#define UnixGpio_H

#include "device/serial/serialclass.h"
#include <fstream>
namespace Cosmos {
    namespace Devices {
        namespace General {
            class UnixGpio
            {
            public:

                enum Direction {
                   INPUT,
                   OUTPUT
                };

                enum Value {
                    LOW = 0,
                    HIGH = 1
                };

                UnixGpio(Direction dir, string pin , string name="");

                int32_t setDirection(Direction dir);
                int32_t write(string filename, string value);
                string read(string filename);


                int32_t write(Value val);
                int32_t read();
                int32_t geterror();


            private:
                std::ofstream stream;
                Direction direction;
                string pinname;
                string path;
                string pin;
                int32_t error;
                bool open;

            };
        }
    }
}

#endif // UnixGpio_H

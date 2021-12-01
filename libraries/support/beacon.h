#ifndef BEACON_H
#define BEACON_H

#include <atomic>
#include <tuple>
#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "math/mathlib.h"
#include "support/enumlib.h"
#include "support/transferclass.h"
#include "support/packetcomm.h"

// Alias of a function that updates a beacon struct
//  arg: string, beacon's namespace name
//  return: void
using update_beacon_func = void (*)(string);

/** 
 * Implement common functionalities for handling PacketComm packets
*/
namespace Cosmos {
    namespace Support {
        class Beacon
        {
        public:
            Beacon();
            int32_t Init(Agent*, uint16_t short_beacon = 18, uint16_t long_beacon = 200);
            int32_t Generate(PacketComm &packet);
            int32_t Decode(PacketComm &packet, string& Contents);

            // Short Beacons
            struct __attribute__ ((packed)) cpu1_beacons
            {
                float met = 0.;
                float load = 0.;
                float memory = 0.;
                float disk = 0.;
            };

            struct __attribute__ ((packed)) cpu2_beacons
            {
                float met = 0.;
                uint32_t uptime = 0;
                uint32_t bootcount = 0;
                uint32_t initialdate = 0;
            };

            struct __attribute__ ((packed)) temp_beacons
            {
                float met = 0.;
                float temp[3] = {0.};
            } ;

            struct __attribute__ ((packed)) epscpu_beacons
            {
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epspv_beacons
            {
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsswch_beacons
            {
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsbatt_beacons
            {
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcscpu_beacons
            {
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsmtr_beacons
            {
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsrw_beacons
            {
                float met = 0.;
                float omega1 = 0.;
                float omega2 = 0.;
                float omega3 = 0.;
            } ;

            struct __attribute__ ((packed)) adcsimu_beacons
            {
                float met = 0.;
                float magx = 0.;
                float magy = 0.;
                float magz = 0.;
            } ;

            struct __attribute__ ((packed)) adcsgps_beacons
            {
                float met = 0.;
                float geocx = 0.;
                float geocy = 0.;
                float geocz = 0.;
            } ;

            struct __attribute__ ((packed)) adcsstt_beacons
            {
                float met = 0.;
                float heading = 0.;
                float elevation = 0.;
                float bearing = 0.;
            } ;

            struct __attribute__ ((packed)) adcsssen_beacons
            {
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcssun_beacons
            {
                float met = 0.;
                float azimuth = 0.;
                float elevation = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsnadir_beacons
            {
                float met = 0.;
                float azimuth = 0.;
                float elevation = 0.;
                float temp = 0.;
            } ;

            // Long Beacons
            struct __attribute__ ((packed)) cpu_beaconl
            {
                float uptime = 0.;
                uint32_t bootcount = 0;
                uint32_t initialdate = 0;
                float load = 0.;
                float memory = 0.;
                float disk = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) cpu1_beaconl
            {
                float met = 0.;
                cpu_beaconl cpu[6];
            };

            struct __attribute__ ((packed)) temp_beacon
            {
                float met = 0.;
                float temp[20] = {0.};
            } ;

            struct __attribute__ ((packed)) epspv_beacon
            {
                float volt = 0.;
                float amp = 0.;
            } ;

            struct __attribute__ ((packed)) epspv1_beacon
            {
                float met = 0.;
                epspv_beacon pv[6];
            };

            struct __attribute__ ((packed)) epsswch_beaconl
            {
                float volt = 0.;
                float amp = 0.;
            } ;

            struct __attribute__ ((packed)) epsswch1_beaconl
            {
                float met = 0.;
                epsswch_beaconl swch[32];
            };

            struct __attribute__ ((packed)) epsbatt_beaconl
            {
                float volt = 0.;
                float amp = 0.;
                float percent = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsbatt1_beaconl
            {
                float met = 0.;
                epsbatt_beaconl batt[4];
            };

            struct __attribute__ ((packed)) adcsmtr_beaconl
            {
                float mom = 0.;
                float align[4] = {0.};
            };

            struct __attribute__ ((packed)) adcsmtr1_beaconl
            {
                float met = 0.;
                adcsmtr_beaconl mtr[3];
            };

            struct __attribute__ ((packed)) adcsrw_beaconl
            {
                float omega = 0.;
                float alpha = 0.;
                float moi[3] = {0.};
                float align[4] = {0.};
            };

            struct __attribute__ ((packed)) adcsrw1_beaconl
            {
                float met = 0.;
                adcsrw_beaconl rw[4];
            };

            struct __attribute__ ((packed)) adcsimu_beaconl
            {
                float theta[4] = {0.};
                float omega = 0.;
                float alpha = 0.;
                float accel = 0.;
                float bfield = 0.;
                float bdot = 0.;
                float align[4] = {0.};
            };

            struct __attribute__ ((packed)) adcsimu1_beaconl
            {
                float met = 0.;
                adcsimu_beaconl imu[3];
            };

            struct __attribute__ ((packed)) adcsgps_beaconl
            {
                double utc = 0.;
                double geoc[3] = {0.};
                float geocv[3] = {0.};
            };

            struct __attribute__ ((packed)) adcsgps1_beaconl
            {
                float met = 0.;
                adcsgps_beaconl gps[3];
            };

            struct __attribute__ ((packed)) adcsstt_beaconl
            {
                float theta[4] = {0.};
                float omega[3] = {0.};
                float alpha[3] = {0.};
                float align[4] = {0.};
            };

            struct __attribute__ ((packed)) adcsstt1_beaconl
            {
                float met = 0.;
                adcsstt_beaconl stt[3];
            };

            struct __attribute__ ((packed)) adcsssen_beaconl
            {
                float elevation = 0.;
                int8_t align = 0;
            };

            struct __attribute__ ((packed)) adcsatt_beaconl
            {
                float vector[3] = {0.};
                float align[4] = {0.};
            };

            struct __attribute__ ((packed)) adcsatt1_beaconl
            {
                float met = 0.;
                adcsatt_beaconl sun;
                adcsatt_beaconl earth;
                adcsssen_beaconl coarse[10];
            };



            double get_interval();
            /// Register a new beacon
            int32_t add_beacon(const string& name, uint8_t type, size_t size);
            // Specify send pattern
            int32_t set_pattern(const vector<string>& pattern);
            /// Get next beacon in cycle of set beacon pattern as a SLIP-encoded PacketComm packet
            PacketComm get_next();
            /// Get name-specified beacon as SLIP-encoded PacketComm packet
            PacketComm get_from_name(const string& name);

        private:
            // Reference to calling agent for accessing namespace
            Agent *agent;
            // Function pointer to a function that will update the beacon(s)
            update_beacon_func update;
            // Time interval (in seconds) between beacon sends
            std::atomic<double> interval;
            // Message size limtis
            uint8_t short_beacon_size;
            uint8_t long_beacon_size;
            // Map beacon name to size of the beacon struct in bytes
            map<string, size_t> beacon_size;
            // Map beacon name to it type ID
            map<string, uint8_t> beacon_typeID;
            // Vector of predefined beacons
            vector<string> send_pattern;
            mutex send_pattern_mtx;
            int pattern_idx;
            int current_beacon;
//            cosmosstruc* cinfo;


            bool validate_short_beacon();
            bool validate_long_beacon();
        };
    }
}

#endif

#ifndef BEACON_H
#define BEACON_H

#include <atomic>
#include <tuple>
#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "math/mathlib.h"
#include "support/enumlib.h"
//#include "support/transferclass.h"
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
            Beacon()
            {

            }

            enum class TypeId : uint8_t
                {
                None = 0,
                CPU1BeaconS = 10,
                CPU2BeaconS = 11,
                TempBeaconS = 20,
                EPSCPUBeaconS = 30,
                EPSPVBeaconS = 31,
                EPSSWCHBeaconS = 32,
                EPSBATTBeaconS = 33,
                ADCSCPUBeaconS = 40,
                ADCSMTRBeaconS = 41,
                ADCSRWBeaconS = 42,
                ADCSIMUBeaconS = 43,
                ADCSGPSBeaconS = 44,
                ADCSSTTBeaconS = 45,
                ADCSSSENBeaconS = 46,
                ADCSSunBeaconS = 47,
                ADCSNadirBeaconS = 48,
                CPUBeaconL = 110,
                TempBeaconL = 120,
                EPSPVBeaconL = 130,
                EPSSWCH0BeaconL = 131,
                EPSSWCH1BeaconL = 132,
                EPSBATTBeaconL = 133,
                ADCSMTRBeaconL = 140,
                ADCSRWBeaconL = 141,
                ADCSIMUBeaconL = 142,
                ADCSGPSBeaconL = 143,
                ADCSSTTBeaconL = 144,
                ADCSSSENBeaconL = 145,
                ADCSATTBeaconL = 146,
                };

            map<TypeId, string> TypeString = {
                {TypeId::CPU1BeaconS, "CPU1BeaconS"},
                {TypeId::CPU2BeaconS, "CPU2BeaconS"},
                {TypeId::TempBeaconS, "TempBeaconS"},
                {TypeId::EPSCPUBeaconS, "EPSCPUBeaconS"},
                {TypeId::EPSPVBeaconS, "EPSPVBeaconS"},
                {TypeId::EPSSWCHBeaconS, "EPSSWCHBeaconS"},
                {TypeId::EPSBATTBeaconS, "EPSBATTBeaconS"},
                {TypeId::ADCSCPUBeaconS, "ADCSCPUBeaconS"},
                {TypeId::ADCSMTRBeaconS, "ADCSMTRBeaconS"},
                {TypeId::ADCSRWBeaconS, "ADCSRWBeaconS"},
                {TypeId::ADCSIMUBeaconS, "ADCSIMUBeaconS"},
                {TypeId::ADCSGPSBeaconS, "ADCSGPSBeaconS"},
                {TypeId::ADCSSTTBeaconS, "ADCSSTTBeaconS"},
                {TypeId::ADCSSSENBeaconS, "ADCSSSENBeaconS"},
                {TypeId::ADCSSunBeaconS, "ADCSSunBeaconS"},
                {TypeId::ADCSNadirBeaconS, "ADCSNadirBeaconS"},
                {TypeId::CPUBeaconL, "CPUBeaconL"},
                {TypeId::TempBeaconL, "TempBeaconL"},
                {TypeId::EPSPVBeaconL, "EPSPVBeaconL"},
                {TypeId::EPSSWCH0BeaconL, "EPSSWCH0BeaconL"},
                {TypeId::EPSSWCH1BeaconL, "EPSSWCH1BeaconL"},
                {TypeId::EPSBATTBeaconL, "EPSBATTBeaconL"},
                {TypeId::ADCSMTRBeaconL, "ADCSMTRBeaconL"},
                {TypeId::ADCSRWBeaconL, "ADCSRWBeaconL"},
                {TypeId::ADCSGPSBeaconL, "ADCSGPSBeaconL"},
                {TypeId::ADCSIMUBeaconL, "ADCSIMUBeaconL"},
                {TypeId::ADCSSTTBeaconL, "ADCSSTTBeaconL"},
                {TypeId::ADCSSSENBeaconL, "ADCSSSENBeaconL"},
                {TypeId::ADCSATTBeaconL, "ADCSATTBeaconL"},
            };

            int32_t Init(uint16_t short_beacon = 18, uint16_t long_beacon = 200);
            int32_t Encode(TypeId type, cosmosstruc *cinfo);
            int32_t Decode(vector<uint8_t> data, cosmosstruc *cinfo, string& Contents);
//            int32_t Decode(vector<uint8_t> data, string& Contents);
//            int32_t Decode(string& Contents);
//            int32_t Init(Agent*, uint16_t short_beacon = 18, uint16_t long_beacon = 200);
            int32_t Encode(TypeId type);
            int32_t Decode(const PacketComm& packet, string& Contents);
            int32_t JSONDecode(string& Contents);

            // Short Beacons
            struct __attribute__ ((packed)) cpu1_beacons
            {
                uint8_t type = 10;
                float met = 0.;
                float load = 0.;
                float memory = 0.;
                float disk = 0.;
            };

            struct __attribute__ ((packed)) cpu2_beacons
            {
                uint8_t type = 11;
                float met = 0.;
                uint32_t uptime = 0;
                uint32_t bootcount = 0;
                uint32_t initialdate = 0;
            };

            struct __attribute__ ((packed)) temp_beacons
            {
                uint8_t type = 20;
                float met = 0.;
                float temp[3] = {0.};
            } ;

            struct __attribute__ ((packed)) epscpu_beacons
            {
                uint8_t type = 30;
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epspv_beacons
            {
                uint8_t type = 31;
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsswch_beacons
            {
                uint8_t type = 32;
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsbatt_beacons
            {
                uint8_t type = 33;
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcscpu_beacons
            {
                uint8_t type = 40;
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsmtr_beacons
            {
                uint8_t type = 41;
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsrw_beacons
            {
                uint8_t type = 42;
                float met = 0.;
                float omega1 = 0.;
                float omega2 = 0.;
                float omega3 = 0.;
            } ;

            struct __attribute__ ((packed)) adcsimu_beacons
            {
                uint8_t type = 43;
                float met = 0.;
                float magx = 0.;
                float magy = 0.;
                float magz = 0.;
            } ;

            struct __attribute__ ((packed)) adcsgps_beacons
            {
                uint8_t type = 44;
                float met = 0.;
                float geocx = 0.;
                float geocy = 0.;
                float geocz = 0.;
            } ;

            struct __attribute__ ((packed)) adcsstt_beacons
            {
                uint8_t type = 45;
                float met = 0.;
                float heading = 0.;
                float elevation = 0.;
                float bearing = 0.;
            } ;

            struct __attribute__ ((packed)) adcsssen_beacons
            {
                uint8_t type = 46;
                float met = 0.;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcssun_beacons
            {
                uint8_t type = 47;
                float met = 0.;
                float azimuth = 0.;
                float elevation = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsnadir_beacons
            {
                uint8_t type = 48;
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
                float load = 0.;
                float memory = 0.;
                float disk = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) cpu1_beaconl
            {
                uint8_t type = 110;
                float met = 0.;
                uint32_t initialdate = 0;
                cpu_beaconl cpu[6];
                double last_updated;
                double last_sent;
            };

            struct __attribute__ ((packed)) temp_beacon
            {
                uint8_t type = 120;
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
                uint8_t type = 130;
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
                uint8_t type = 131;
                float met = 0.;
                epsswch_beaconl swch[16];
            };

            struct __attribute__ ((packed)) epsswch2_beaconl
            {
                uint8_t type = 132;
                float met = 0.;
                epsswch_beaconl swch[16];
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
                uint8_t type = 133;
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
                uint8_t type = 140;
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
                uint8_t type = 141;
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
                uint8_t type = 142;
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
                uint8_t type = 143;
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
                uint8_t type = 144;
                float met = 0.;
                adcsstt_beaconl stt[3];
            };

            struct __attribute__ ((packed)) adcsssen_beaconl
            {
                float elevation = 0.;
                int8_t align = 0;
            };

            struct __attribute__ ((packed)) adcsssen1_beaconl
            {
                uint8_t type = 145;
                float met = 0.;
                adcsssen_beaconl ssen[10];
            };

            struct __attribute__ ((packed)) adcsatt_beaconl
            {
                float vector[3] = {0.};
                float align[4] = {0.};
            };

            struct __attribute__ ((packed)) adcsatt1_beaconl
            {
                uint8_t type = 146;
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

            TypeId type;
            /// Data of interest
            vector<uint8_t> data;

        private:
            // Reference to calling agent for accessing namespace
//            Agent *agent;
            // Function pointer to a function that will update the beacon(s)
            update_beacon_func update;
            // Time interval (in seconds) between beacon sends
//            std::atomic<double> interval;
            double interval;
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

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
                TimeBeaconS = 12,
                TsenBeaconS = 20,
                EPSCPUBeaconS = 30,
                EPSBCREGBeaconS = 31,
                EPSSWCHBeaconS = 32,
                EPSBATTBeaconS = 33,
                RadioBeaconS = 34,
                ADCSCPUBeaconS = 40,
                ADCSMTRBeaconS = 41,
                ADCSRWBeaconS = 42,
                ADCSMAGBeaconS = 43,
                ADCSGPSBeaconS = 44,
                ADCSSTTBeaconS = 45,
                ADCSSSENBeaconS = 46,
                ADCSSunBeaconS = 47,
                ADCSNadirBeaconS = 48,
                ADCSEXTRABeaconS = 49,
				ADCSStateBeaconS = 51,
                CPUBeacon = 110,
                TelemBeacon = 111,
                TsenBeacon = 120,
                EPSBCREGBeacon = 130,
                EPSSWCHBeacon = 131,
                EPSBATTBeacon = 132,
//                EPSSUMBeacon = 134,
                ADCSStateBeacon = 140,
                ADCSMTRBeacon = 143,
                ADCSGyroBeacon = 144,
                ADCSRWBeacon = 145,
                ADCSORBITBeacon = 146,
//                ADCSIMUBeacon = 142,
//                ADCSGPSBeacon = 143,
//                ADCSSTTBeacon = 144,
//                ADCSSSENBeacon = 145,
//                ADCSATTBeacon = 146,
                RadioBeacon = 147,
                CameraBeacon = 148,
                };

            map<TypeId, string> TypeString = {
                {TypeId::CPU1BeaconS, "CPU1BeaconS"},
                {TypeId::CPU2BeaconS, "CPU2BeaconS"},
                {TypeId::TsenBeaconS, "TsenBeaconS"},
                {TypeId::EPSCPUBeaconS, "EPSCPUBeaconS"},
                {TypeId::EPSBCREGBeaconS, "EPSBCREGBeaconS"},
                {TypeId::EPSSWCHBeaconS, "EPSSWCHBeaconS"},
                {TypeId::EPSBATTBeaconS, "EPSBATTBeaconS"},
                {TypeId::RadioBeaconS, "RadioBeaconS"},
                {TypeId::ADCSCPUBeaconS, "ADCSCPUBeaconS"},
                {TypeId::ADCSMTRBeaconS, "ADCSMTRBeaconS"},
                {TypeId::ADCSRWBeaconS, "ADCSRWBeaconS"},
                {TypeId::ADCSMAGBeaconS, "ADCSMAGBeaconS"},
                {TypeId::ADCSGPSBeaconS, "ADCSGPSBeaconS"},
                {TypeId::ADCSSTTBeaconS, "ADCSSTTBeaconS"},
                {TypeId::ADCSSSENBeaconS, "ADCSSSENBeaconS"},
                {TypeId::ADCSSunBeaconS, "ADCSSunBeaconS"},
                {TypeId::ADCSNadirBeaconS, "ADCSNadirBeaconS"},
                {TypeId::ADCSEXTRABeaconS, "ADCSEXTRABeaconS"},
                {TypeId::ADCSORBITBeacon, "ADCSORBITBeacon"},
                {TypeId::ADCSStateBeaconS, "ADCSStateBeaconS"},
                {TypeId::CPUBeacon, "CPUBeacon"},
                {TypeId::TelemBeacon, "TelemBeacon"},
                {TypeId::TsenBeacon, "TsenBeacon"},
                {TypeId::EPSBCREGBeacon, "EPSBCREGBeacon"},
                {TypeId::EPSSWCHBeacon, "EPSSWCHBeacon"},
                {TypeId::EPSBATTBeacon, "EPSBATTBeacon"},
//                {TypeId::EPSSUMBeacon, "EPSSUMBeacon"},
                {TypeId::ADCSStateBeacon, "ADCSStateBeacon"},
                {TypeId::ADCSMTRBeacon, "ADCSMTRBeacon"},
                {TypeId::ADCSGyroBeacon, "ADCSGyroBeacon"},
                {TypeId::ADCSRWBeacon, "ADCSRWBeacon"},
//                {TypeId::ADCSGPSBeacon, "ADCSGPSBeacon"},
//                {TypeId::ADCSIMUBeacon, "ADCSIMUBeacon"},
//                {TypeId::ADCSSTTBeacon, "ADCSSTTBeacon"},
//                {TypeId::ADCSSSENBeacon, "ADCSSSENBeacon"},
//                {TypeId::ADCSATTBeacon, "ADCSATTBeacon"},
                {TypeId::RadioBeacon, "RadioBeacon"},
            };

            int32_t Init();
            int32_t EncodeBinary(TypeId type, cosmosstruc *cinfo, vector<uint8_t> &data);
            int32_t EncodeJson(TypeId type, cosmosstruc *cinfo, string& Contents);
            int32_t EncodeJson(TypeId type, cosmosstruc *cinfo, vector<uint8_t>& Contents);
            int32_t Decode(vector<uint8_t> &data, cosmosstruc *cinfo);
//            int32_t Encode(TypeId type);
//            int32_t JSONDecode(string& Contents);

            // Short Beacons
            struct __attribute__ ((packed)) cpu1_beacons
            {
                uint8_t type = 10;
                uint32_t deci = 0;
                float load = 0.;
                float memory = 0.;
                float cdisk = 0.;
            };

            struct __attribute__ ((packed)) cpu2_beacons
            {
                uint8_t type = 11;
                uint32_t deci = 0;
                uint32_t uptime = 0;
                uint32_t bootcount = 0;
                uint32_t initialdate = 0;
            };

            struct __attribute__ ((packed)) tsen_beacons
            {
                uint8_t type = 20;
                uint32_t deci = 0;
                uint16_t ctemp[6] = {0};
            } ;

            struct __attribute__ ((packed)) epscpu_beacons
            {
                uint8_t type = 30;
                uint32_t deci = 0;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsbcreg_beacons
            {
                uint8_t type = 31;
                uint32_t deci = 0;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsswch_beacons
            {
                uint8_t type = 32;
                uint32_t deci = 0;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) epsbatt_beacons
            {
                uint8_t type = 33;
                uint32_t deci = 0;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) radio_beacons
            {
                uint8_t type = 34;
                uint32_t deci = 0;
                uint16_t active = 0;
                uint32_t lastdeciup = 0;
                uint32_t lastdecidown = 0;
            } ;

            struct __attribute__ ((packed)) adcscpu_beacons
            {
                uint8_t type = 40;
                uint32_t deci = 0;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsmtr_beacons
            {
                uint8_t type = 41;
                uint32_t deci = 0;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsrw_beacons
            {
                uint8_t type = 42;
                uint32_t deci = 0;
                float omega1 = 0.;
                float omega2 = 0.;
                float omega3 = 0.;
            } ;

            struct __attribute__ ((packed)) adcsmag_beacon
            {
                uint8_t type = 43;
                uint32_t deci = 0;
                float magx = 0.;
                float magy = 0.;
                float magz = 0.;
            } ;
            //struct __attribute__ ((packed)) adcsimu_beacons
            //{
                //uint8_t type = 43;
                //float met = 0.;
                //float magx = 0.;
                //float magy = 0.;
                //float magz = 0.;
            //} ;

            struct __attribute__ ((packed)) adcsgps_beacons
            {
                uint8_t type = 44;
                uint32_t deci = 0;
                float geocx = 0.;
                float geocy = 0.;
                float geocz = 0.;
            } ;

            struct __attribute__ ((packed)) adcsstt_beacons
            {
                uint8_t type = 45;
                uint32_t deci = 0;
                float heading = 0.;
                float elevation = 0.;
                float bearing = 0.;
            } ;

            struct __attribute__ ((packed)) adcsssen_beacons
            {
                uint8_t type = 46;
                uint32_t deci = 0;
                float volt = 0.;
                float amp = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcssun_beacons
            {
                uint8_t type = 47;
                uint32_t deci = 0;
                float azimuth = 0.;
                float elevation = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsnadir_beacons
            {
                uint8_t type = 48;
                uint32_t deci = 0;
                float azimuth = 0.;
                float elevation = 0.;
                float temp = 0.;
            } ;

            struct __attribute__ ((packed)) adcsextra_beacon
            {
                uint8_t type = 49;
                uint32_t deci = 0;
                float x = 0.;
                float y = 0.;
                float z = 0.;
            } ;

            // Long Beacons
            struct __attribute__ ((packed)) adcsstate_beacon
            {
                uint8_t		type = (uint8_t)TypeId::ADCSStateBeacon;
                uint32_t	deci = 0;

				double x_eci;
				double y_eci;
				double z_eci;

				double vx_eci;
				double vy_eci;
				double vz_eci;

				double att_icrf_x;
				double att_icrf_y;
				double att_icrf_z;
				double att_icrf_w;

				double att_icrf_omega_x;
				double att_icrf_omega_y;
				double att_icrf_omega_z;
            };

            struct __attribute__ ((packed)) cpu_beacon
            {
                uint32_t uptime = 0.;
                uint32_t bootcount = 0;
                uint16_t mload = 0;
                uint16_t mmemory = 0;
                uint16_t mdisk = 0;
                uint16_t ctemp = 0;
            } ;

            static constexpr uint8_t cpu_count = 200 / sizeof(cpu_beacon);
            struct __attribute__ ((packed)) cpus_beacon
            {
                uint8_t type = (uint8_t)TypeId::CPUBeacon;
                uint32_t deci = 0;
                uint32_t initialdate = 0;
                cpu_beacon cpu[cpu_count];
            };

//            struct telem_beacon
//            {
//                vector<uint8_t> bytes;
//            } ;

            static constexpr uint8_t telem_count = 200-5;
            struct __attribute__ ((packed)) telems_beacon
            {
                uint8_t type = (uint8_t)TypeId::TelemBeacon;
                uint32_t deci = 0;
                uint8_t content[telem_count];
            };

            static constexpr uint8_t tsen_count = 200 / sizeof(uint16_t);
            struct __attribute__ ((packed)) tsen_beacon
            {
                uint8_t type = (uint8_t)TypeId::TsenBeacon;
                uint32_t deci = 0;
                uint16_t ctemp[tsen_count] = {0};
            } ;

            struct __attribute__ ((packed)) epsbcreg_beacon
            {
                int16_t mvolt = 0.;
                int16_t mamp = 0.;
                int16_t mpptin_mvolt = 0.;
                int16_t mpptin_mamp = 0.;
                int16_t mpptout_mvolt = 0.;
                int16_t mpptout_mamp = 0.;
            } ;

            static constexpr uint8_t epsbcreg_count = 200 / sizeof(epsbcreg_beacon);
            struct __attribute__ ((packed)) epsbcregs_beacon
            {
                uint8_t type = (uint8_t)TypeId::EPSBCREGBeacon;
                uint32_t deci = 0;
                epsbcreg_beacon bcreg[epsbcreg_count];
            };

            struct __attribute__ ((packed)) epsswch_beacon
            {
                uint8_t state = 0;
                int16_t mvolt = 0.;
                int16_t mamp = 0.;
            } ;

            static constexpr uint8_t epsswch_count = 200 / sizeof(epsswch_beacon);
            struct __attribute__ ((packed)) epsswchs_beacon
            {
                uint8_t type = (uint8_t)TypeId::EPSSWCHBeacon;
                uint32_t deci = 0;
                epsswch_beacon swch[epsswch_count];
            };

//            struct __attribute__ ((packed)) epsswch2_beacon
//            {
//                uint8_t type = 132;
//                uint32_t deci = 0;
//                epsswch_beacon swch[16];
//            };

            struct __attribute__ ((packed)) epsbatt_beacon
            {
                int16_t mvolt = 0.;
                int16_t mamp = 0.;
                uint16_t cpercent = 0.;
                uint16_t ctemp = 0.;
            } ;

            static constexpr uint8_t epsbatt_count = 200 / sizeof(epsbatt_beacon);
            struct __attribute__ ((packed)) epsbatts_beacon
            {
                uint8_t type = (uint8_t)TypeId::EPSBATTBeacon;
                uint32_t deci = 0;
                epsbatt_beacon batt[epsbatt_count];
            };

            struct __attribute__ ((packed)) adcsmtr_beacon
            {
				// JIMNOTE:  this beacon is missing type and deci?
                float mom = 0.;
                float align[4] = {0.};
            };

            static constexpr uint8_t adcsmtr_count = 200 / sizeof(adcsmtr_beacon);
            struct __attribute__ ((packed)) adcsmtrs_beacon
            {
                uint8_t type = (uint8_t)TypeId::ADCSMTRBeacon;
                uint32_t deci = 0;
                adcsmtr_beacon mtr[adcsmtr_count];
            };

            struct __attribute__ ((packed)) adcsgyro_beacon
            {
                float omega = 0.;
            };

            static constexpr uint8_t adcsgyro_count = 200 / sizeof(adcsgyro_beacon);
            struct __attribute__ ((packed)) adcsgyros_beacon
            {
                uint8_t type = (uint8_t)TypeId::ADCSGyroBeacon;
                uint32_t deci = 0;
                adcsgyro_beacon gyro[adcsgyro_count];
            };

            struct __attribute__ ((packed)) adcsrw_beacon
            {
                float amp = 0.;
                float omg = 0.;
                float romg = 0.;
            };

           static constexpr uint8_t adcsrw_count = 200 / sizeof(adcsrw_beacon);
           struct __attribute__ ((packed)) adcsrws_beacon
           {
               uint8_t type = (uint8_t)TypeId::ADCSRWBeacon;
               uint32_t deci = 0;
               adcsrw_beacon rw[adcsrw_count];
           };

           struct __attribute__ ((packed)) adcsorbit_beacon
           {
               uint8_t type = (uint8_t)TypeId::ADCSORBITBeacon;
               uint32_t deci = 0;
               double utc = 0.;
               double i = 0.;
               double e = 0.;
               double raan = 0.;
               double ap = 0.;
               double bstar = 0.;
               double mm = 0.;
               double ma = 0.;
           } ;

//            struct __attribute__ ((packed)) adcsimu_beacon
//            {
//                float theta[4] = {0.};
//                float omega = 0.;
//                float alpha = 0.;
//                float accel = 0.;
//                float bfield = 0.;
//                float bdot = 0.;
//                float align[4] = {0.};
//            };

//            static constexpr uint8_t adcsimu_count = 200 / sizeof(adcsimu_beacon);
//            struct __attribute__ ((packed)) adcsimu1_beacon
//            {
//                uint8_t type = 142;
//                uint32_t deci = 0;
//                adcsimu_beacon imu[adcsimu_count];
//            };

//            struct __attribute__ ((packed)) adcsgps_beacon
//            {
//                double utc = 0.;
//                double geoc[3] = {0.};
//                float geocv[3] = {0.};
//            };

//            struct __attribute__ ((packed)) adcsgps1_beacon
//            {
//                uint8_t type = 143;
//                uint32_t deci = 0;
//                adcsgps_beacon gps[3];
//            };

//            struct __attribute__ ((packed)) adcsstt_beacon
//            {
//                float theta[4] = {0.};
//                float omega[3] = {0.};
//                float alpha[3] = {0.};
//                float align[4] = {0.};
//            };

//            struct __attribute__ ((packed)) adcsstt1_beacon
//            {
//                uint8_t type = 144;
//                uint32_t deci = 0;
//                adcsstt_beacon stt[3];
//            };

//            struct __attribute__ ((packed)) adcsssen_beacon
//            {
//                float elevation = 0.;
//                int8_t align = 0;
//            };

//            struct __attribute__ ((packed)) adcsssen1_beacon
//            {
//                uint8_t type = 145;
//                uint32_t deci = 0;
//                adcsssen_beacon ssen[10];
//            };

//            struct __attribute__ ((packed)) adcsatt_beacon
//            {
//                float vector[3] = {0.};
//                float align[4] = {0.};
//            };

//            struct __attribute__ ((packed)) adcsatt1_beacon
//            {
//                uint8_t type = 146;
//                uint32_t deci = 0;
//                adcsatt_beacon sun;
//                adcsatt_beacon earth;
//                adcsssen_beacon coarse[10];
//            };

           struct __attribute__ ((packed)) radio_beacon
           {
               uint16_t packet_size;
               uint16_t kbyte_rate;
               uint16_t ctemp;
               uint16_t kpower;
               uint32_t uptime;
               uint32_t bytes;
               uint32_t lastdeci;
           };

           static constexpr uint8_t rxrtxr_count = 200 / sizeof(radio_beacon);
           struct __attribute__ ((packed)) radios_beacon
           {
               uint8_t type = (uint8_t)TypeId::RadioBeacon;
               uint32_t deci = 0;
               radio_beacon radio[rxrtxr_count];
           };

           struct __attribute__ ((packed)) camera_beacon
           {
               uint16_t width;
               uint16_t height;
               uint16_t tctemp;
               uint16_t fctemp;
               uint16_t lctemp;
               uint16_t lstep;
           };

           static constexpr uint8_t camera_count = 200 / sizeof(camera_beacon);
           struct __attribute__ ((packed)) cameras_beacon
           {
               uint8_t type = (uint8_t)TypeId::CameraBeacon;
               uint32_t deci = 0;
               camera_beacon camera[camera_count];
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
//            vector<uint8_t> data;

        private:
            // Reference to calling agent for accessing namespace
//            Agent *agent;
            // Function pointer to a function that will update the beacon(s)
            update_beacon_func update;
            // Time interval (in seconds) between beacon sends
//            std::atomic<double> interval;
            double interval;
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

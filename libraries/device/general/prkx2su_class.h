#ifndef PRKX2SU_CLASS_H
#define PRKX2SU_CLASS_H

#include "support/configCosmos.h"
#include "math/mathlib.h"
#include "support/cosmos-errno.h"
#include "device/serial/serialclass.h"
#include <cstdio>

#define PRKX2SU_ID 1
#define PRKX2SU_BAUD 9600
#define PRKX2SU_BITS 8
#define PRKX2SU_PARITY 0
#define PRKX2SU_STOPBITS 1
#define PRKX2SU_AXIS_AZ 0
#define PRKX2SU_AXIS_EL 1

namespace Cosmos {
    namespace Devices {

        class Prkx2su
        {
        public:
            Prkx2su(string dev);

            uint8_t azid;
            uint8_t azstatus;
            int32_t az_center;
            int32_t az_mode;
            int32_t az_offset_waiting;
            float az_offset;
            float minaz;
            float maxaz;
            float currentaz;
            float targetaz;
            uint8_t elid;
            uint8_t elstatus;
            int32_t el_offset_waiting;
            float el_offset;
            float minel;
            float maxel;
            float currentel;
            float targetel;
            float minimumel;
            float sensitivity;
            int32_t in_command;
            int32_t data_received;

            Serial *serial[2];

            int32_t init(string device);
            int32_t connect();
            int32_t disconnect();
            int32_t get_status(uint8_t axis);
            float get_az();
            float get_el();
            int32_t get_az_el(float &az, float &el);
            float get_az_offset();
            float get_el_offset();
            int32_t gotoazel(float az, float el);
            int32_t stop(uint8_t axis);
            int32_t ramp(uint8_t axis, uint8_t speed);
            int32_t minimum_speed(uint8_t axis, uint8_t speed);
            int32_t maximum_speed(uint8_t axis, uint8_t speed);
            int32_t getdata(uint8_t axis, string &buf, uint16_t buflen);
            int32_t send(uint8_t axis, string buf, bool force);
            int32_t test(uint8_t axis);
            int32_t set_sensitivity(float sensitivity);
            int32_t get_limits(uint8_t axis);
            int32_t write_calibration(uint8_t axis, float value);
        };
    }
}
#endif // PRKX2SU_CLASS_H

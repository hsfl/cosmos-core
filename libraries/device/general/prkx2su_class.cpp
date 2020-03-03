#include "prkx2su_class.h"

namespace Cosmos {
    namespace Devices {

        Prkx2su::Prkx2su(string dev)
        {
            string device;

            device = dev + "_az";
            serial[PRKX2SU_AXIS_AZ] = new Serial(device, PRKX2SU_BAUD, PRKX2SU_BITS, PRKX2SU_PARITY, PRKX2SU_STOPBITS);
            device = dev + "_el";
            serial[PRKX2SU_AXIS_EL] = new Serial(device, PRKX2SU_BAUD, PRKX2SU_BITS, PRKX2SU_PARITY, PRKX2SU_STOPBITS);

        }

        /**
        * Connects to am MII prkx2su antenna controller, which in turn
        * drives a Yaesu G-5500 antenna controller.
        * @param dev pointer to a character string with the serial port it is
        * connected to.
        * @see cssl_start
        * @see cssl_open
        * @see cssl_setflowcontrol
        */
        int32_t Prkx2su::connect()
        {
                int32_t iretn;

            if (serial[PRKX2SU_AXIS_AZ]->get_open())
                {
                iretn = Prkx2su::disconnect();
                if (iretn < 0)
                {
                    return iretn;
                }
            }

            iretn = serial[PRKX2SU_AXIS_AZ]->open_device();
            if (iretn < 0)
                {
                return iretn;
                }

            iretn = serial[PRKX2SU_AXIS_AZ]->set_timeout(.5);
                if (iretn < 0)
                {
                Prkx2su::disconnect();
                        return iretn;
                }

            iretn = Prkx2su::send(PRKX2SU_AXIS_AZ, "", true);
                if (iretn < 0)
                {
                Prkx2su::disconnect();
                        return iretn;
                }

            if (serial[PRKX2SU_AXIS_EL]->get_open())
                {
                return SERIAL_ERROR_OPEN;
            }

            iretn = serial[PRKX2SU_AXIS_EL]->open_device();
            if (iretn < 0)
            {
                Prkx2su::disconnect();
                return iretn;
            }

            iretn = serial[PRKX2SU_AXIS_EL]->set_timeout(.5);
            if (iretn < 0)
                {
                        Prkx2su::disconnect();
                        return iretn;
                }

            iretn = Prkx2su::send(PRKX2SU_AXIS_EL, "", true);
                if (iretn < 0)
                {
                        Prkx2su::disconnect();
                        return iretn;
                }

                return 0;
        }

        /**
        * Close currently open prkx2su.
        */
        int32_t Prkx2su::disconnect()
        {
            int32_t iretn = 0;
            if (serial[PRKX2SU_AXIS_AZ]->get_open())
                {
                iretn = serial[PRKX2SU_AXIS_AZ]->close_device();
            }

                if (serial[PRKX2SU_AXIS_EL] != nullptr)
                {
                iretn = serial[PRKX2SU_AXIS_EL]->close_device();
                }

            return iretn;
        }

        /**
        * Routine to use in blocking mode. Reads the serial port until a New
        * Line is received, then returns entire buffer.
        * @param axis Axis to read.
        * @param buf Pointer to a char buffer
        * @param buflen 32 bit signed integer indicating the maximum size of the buffer
        * @return 32 bit signed integer containing the number of bytes read.
        */
        int32_t Prkx2su::getdata(uint8_t axis, std::string &buf, uint16_t buflen)
        {
            int32_t j;

            buf.clear();
            while((j=serial[axis]->get_char()) >= 0)
                {
                buf.push_back(static_cast<char>(j));
                if (j == ';' || static_cast<uint16_t>(buf.size()) == buflen)
                        {
                                break;
                        }
                }

            if (j < 0)
            {
                return j;
            }
            else
            {
                return static_cast<int32_t>(buf.size());
            }
        }

        /**
        * Poll controller for Status and Heading. Turns
        * calibration mode on in prkx2su.
        * @param axis 32 bit signed integer , 0 = Azimuth, 1 = Elevation
        */
        int32_t Prkx2su::get_status(uint8_t axis)
        {
            int32_t iretn;
            iretn = Prkx2su::send(axis, "BIn;", true);
            if (iretn < 0)
            {
                return iretn;
            }
            string buf;
            iretn = Prkx2su::getdata(axis, buf, 2000);
            if (iretn < 0)
            {
                return iretn;
            }
            switch (axis)
            {
            case PRKX2SU_AXIS_AZ:
                sscanf(buf.c_str(), "%c%c%f", &azid, &azstatus, &currentaz);
                currentaz = RADOF(currentaz);
                break;
            case PRKX2SU_AXIS_EL:
                sscanf(buf.c_str(), "%c%c%f", &elid, &elstatus, &currentel);
                currentel = RADOF(currentel);
                break;
            }
            return iretn;
        }

        int32_t Prkx2su::get_limits(uint8_t axis)
        {
            string buf;
            int32_t iretn;
            iretn = Prkx2su::send(axis, "RH0;", true);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = Prkx2su::getdata(axis, buf, 200);
            if (iretn < 0)
            {
                return iretn;
            }
            switch (axis)
            {
            case PRKX2SU_AXIS_AZ:
                sscanf(buf.c_str(), "%*2c%f", &minaz);
                if (minaz > 0)
                {
                    minaz -= 360;
                }
                minaz = RADOF(minaz);
                break;
            case PRKX2SU_AXIS_EL:
                sscanf(buf.c_str(), "%*2c%f", &minel);
                minel = RADOF(minel);
                break;
            }
            iretn = Prkx2su::send(axis, "RI0;", true);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = getdata(axis, buf, 200);
            if (iretn < 0)
            {
                return iretn;
            }
            switch (axis)
            {
            case PRKX2SU_AXIS_AZ:
                sscanf(buf.c_str(), "%*2c%f", &maxaz);
                if (maxaz < 0.)
                {
                    maxaz += 360.;
                }
                maxaz = RADOF(maxaz);
                break;
            case PRKX2SU_AXIS_EL:
                sscanf(buf.c_str(), "%*2c%f", &maxel);
                maxel = RADOF(maxel);
                break;
            }
            return iretn;
        }

        /**
        * Routine to stop current action. Whatever the current command is, it will
        * cancelled before completeion.
        * @return 0 or negative error.
        */
        int32_t Prkx2su::stop(uint8_t axis)
        {
                int32_t iretn;
            iretn = Prkx2su::send(axis, ";", true);
                return iretn;
        }

        int32_t Prkx2su::ramp(uint8_t axis, uint8_t speed)
        {
            int32_t iretn;
            char out[50];

            if (speed < 1 || speed > 10)
                return (PRKX2SU_ERROR_OUTOFRANGE);
            sprintf(out,"WNn%03hhu;", speed);
            iretn = Prkx2su::send(axis, out, true);
            if (iretn < 0)
            {
                return iretn;
            }
            else
            {
                return (speed);
            }
        }

        int32_t Prkx2su::minimum_speed(uint8_t axis, uint8_t speed)
        {
            int32_t iretn;
            char out[50];

            if (speed < 1 || speed > 10)
                return (PRKX2SU_ERROR_OUTOFRANGE);
            sprintf(out,"WFn%03hhu;", speed);
            iretn = Prkx2su::send(axis, out, true);
            if (iretn < 0)
            {
                return iretn;
            }
            else
            {
                return (speed);
            }
        }

        int32_t Prkx2su::maximum_speed(uint8_t axis, uint8_t speed)
        {
            int32_t iretn;
            char out[50];

            if (speed < 1 || speed > 10)
                return (PRKX2SU_ERROR_OUTOFRANGE);
            sprintf(out,"WGn%03hhu;", speed);
            iretn = Prkx2su::send(axis, out, true);
            if (iretn < 0)
            {
                return iretn;
            }
            else
            {
                return (speed);
            }
        }

        int32_t Prkx2su::gotoazel(float az, float el)
        {
                int32_t iretn;
                char out[50];

                az = fmodf(az, D2PI);
                if (az < Prkx2su::minaz)
                {
                    az = Prkx2su::minaz;
                }
                else if (az > Prkx2su::maxaz)
                {
                        az = Prkx2su::maxaz;
                }

                el = fmodf(el, DPI);
                if (el < Prkx2su::minel)
                {
                        el = Prkx2su::minel;
                }
                else if (el > Prkx2su::minel)
                {
                        el = Prkx2su::minel;
                }

                float daz = az - targetaz;
                float del = el - targetel;
                float sep = sqrtf(daz*daz+del*del);

                if (sep > sensitivity)
                {
                        targetaz = az;
                        targetel = el;
                        az = DEGOF(az);
                        el = DEGOF(el);
                        sprintf(out, "APn%03d.%1d\r;", static_cast <uint16_t>(az), static_cast <uint16_t>(10 * (az - static_cast <uint16_t>(az))));
                        iretn = Prkx2su::send(PRKX2SU_AXIS_AZ, out, true);
                        if (iretn >= 0)
                        {
                                sprintf(out, "APn%03d.%1d\r;", static_cast <uint16_t>(el), static_cast <uint16_t>(10 * (el - static_cast <uint16_t>(el))));
                                iretn = Prkx2su::send(PRKX2SU_AXIS_EL, out, true);
                                if (iretn >= 0)
                                {
                                        iretn = Prkx2su::get_az_el(currentaz, currentel);
                                }
                        }
                }

                return 0;
        }

        float Prkx2su::get_az()
        {
                return (currentaz);
        }

        float Prkx2su::get_el()
        {
                return (currentel);
        }

        int32_t Prkx2su::get_az_el(float &az, float &el)
        {
                int32_t iretn = 0;

                iretn = Prkx2su::get_status(PRKX2SU_AXIS_AZ);
                if (iretn >= 0)
                {
                        iretn = Prkx2su::get_status(PRKX2SU_AXIS_EL);
                        if (iretn >= 0)
                        {
                                az = currentaz;
                                el = currentel;
                        }
                }
                return iretn;
        }

        int32_t Prkx2su::write_calibration(uint8_t axis, float value)
        {
                int32_t iretn;
                char out[50];

                if (value < 0.)
                {
                        value = 0.;
                }
                switch (axis)
                {
                case PRKX2SU_AXIS_AZ:
                        if (value > D2PI)
                        {
                                value = D2PI;
                        }
                        break;
                case PRKX2SU_AXIS_EL:
                        if (value > DPI)
                        {
                                value = DPI;
                        }
                }

                value = DEGOF(value);
                sprintf(out, "Awn%03d.%1d;", (int16_t)value, (int16_t)(10 * (value - (int16_t)value)));
                iretn = Prkx2su::send(axis, out, true);
                return iretn;
        }

        float Prkx2su::get_az_offset()
        {
                return (az_offset);
        }

        float Prkx2su::get_el_offset()
        {
                return (el_offset);
        }

        int32_t Prkx2su::test(uint8_t axis)
        {
                int32_t iretn;

            iretn = serial[axis]->put_string("R10;");
                if (iretn < 0)
                {
                        return iretn;
                }

            string buf;
                iretn = Prkx2su::getdata(axis, buf, 100);
                if (iretn < 0)
                {
                        return iretn;
                }
            if (buf[0] != '1' || buf[1] != 0x1 || buf[buf.size()-1] != ';')
                {
                        return PRKX2SU_ERROR_SEND;
                }

                return 0;
        }

        int32_t Prkx2su::send(uint8_t axis, string buf, bool force)
        {
                int32_t iretn = 0;
            string lastbuf;

                iretn = Prkx2su::test(axis);
                if (iretn < 0)
                {
                        return iretn;
                }

            if (lastbuf != buf || force)
                {
                serial[axis]->put_string(buf);
                lastbuf = buf;
                }

                return iretn;
        }

        int32_t Prkx2su::set_sensitivity(float sensitivity)
        {
                sensitivity = sensitivity;
                return 0;
        }
    }

}

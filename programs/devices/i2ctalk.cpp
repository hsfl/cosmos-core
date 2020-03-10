#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include "support/elapsedtime.h"
#include "support/stringlib.h"
#include "device/i2c/i2c.h"
#include <iostream>
#include <string>


#define I2C_BUFFER_LIMIT 256

int32_t testi2c_read_write();
static vector <uint8_t> dataout;
static vector <uint8_t> datain;
static uint8_t address = 0x10; // default address
static string device = "/dev/i2c-2"; // default device
static float delay = .05;
static I2C *i2cport;
static uint16_t rcount = 0;

int main(int argc, char *argv[])
{
    int32_t iretn;

//    if (argc == 1) {
//        cout << "--------------------- " << endl;
//        cout << "COSMOS i2c Test Program " << endl;
//        cout << "Requires arguments: " << endl;
//        cout << "address tx_byte[:tx_byte:tx_byte:...] [delaysec [device]] " << endl;
//        cout << "example: 57 80:40 .1 /dev/i2c-2" << endl;
//        cout << "talk to device i2c-2 to slave address decimal 57, send the two bytes decimal 80 and decimal 40, and wait up to .1 seconds for the response" << endl;
//        exit(0);
//    }


    // input format
    // <device> <address> <rx_nbytes> <tx_byte0> <tx_byte1>

    string outstring = "";
    switch (argc)
    {
    case 6:
        device = argv[5];
    case 5:
        delay = atof(argv[4]);
    case 4:
        rcount = atol(argv[3]);
    case 3:
        outstring = argv[2];
    case 2:
        //        address = strtol(argv[1], nullptr, 16);
        address = strtol(argv[1], nullptr, 10);
        break;
    default:
        printf("Usage: i2ctalk addressx dd[:dd:dd:dd] rcount [ delaysec [ device ]]\n");
        exit(0);
    }

    if (outstring.empty())
    {
        printf("Nothing to send\n");
        exit(0);
    }

    vector <string> outs = string_split(outstring,":");
    dataout.clear();
    for (string tout : outs)
    {
        dataout.push_back(strtol(tout.c_str(), nullptr, 10));
    }

    ElapsedTime i2ct;
    i2cport = new I2C(device, address, delay);
    if (i2cport->get_error() < 0)
    {
        printf("%s\n", cosmos_error_string(i2cport->get_error()).c_str());
        exit(0);
    }

    printf("Send data to address 0x%x, receive %u bytes: (%f seconds)\n", address, rcount, i2ct.lap());


    datain.resize(rcount);
    iretn = i2cport->send(dataout.data(), dataout.size());
    if (iretn > 0)
    {

        printf("TX (hex): ");

        for (unsigned int i = 0; i < dataout.size(); i++)
        {
            printf("%2x ", dataout[i]);
        }

        printf(" (%lu bytes @ %f seconds)", dataout.size(), i2ct.lap());
        printf("\n");
    }
    else
    {
        printf("TX: %s\n", cosmos_error_string(i2cport->get_error()).c_str());
        exit(0);
    }


    iretn = i2cport->receive(datain.data(), rcount);

    if (datain.size() > 0)
    {

        printf("RX (hex): ");

        for (unsigned int i = 0; i < datain.size(); i++)
        {
            printf("%2x ", datain[i]);
        }

        printf(" (%lu bytes @ %f seconds)", datain.size(), i2ct.lap());
        printf("\n");
    }


    return 0;
}


int32_t testi2c_read_write()
{
    int fh;
    int len, sent;
    uint8_t buff[I2C_BUFFER_LIMIT];

    fh = open(device.c_str(), O_RDWR);

    if (fh < 0) {
        perror("open");
        return -errno;
    }

    if (ioctl(fh, I2C_SLAVE, address) < 0) {
        perror("ioctl");
        return -errno;
    }

    len = dataout.size(); // sizeof(buff);


    memset(buff, 0, sizeof(dataout.size())); // reset buffer
    std::copy(dataout.begin(), dataout.end(), buff);
    sent = write(fh, buff, len);

    if (sent != len) {
        perror("write");
        return -errno;
    } else {
        printf("TX (hex): ");

        for (int i=0; i< sent; i++) {
            printf("%2x ", buff[i]);
        }
        printf(" (%d bytes)", sent);
        cout << endl;
    }


    //    printf("Sent: 0x%x | %d\n", buff[0], buff[0]);

    dataout.clear();
    memset(buff, 0, sizeof(buff));


    ElapsedTime et;
    do
    {
        int32_t rcvd = 0;
        rcvd = read(fh, buff, I2C_BUFFER_LIMIT);

        if (rcvd < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            return -errno;
        }
        else if (rcvd > 0)
        {
            for (int32_t i=0; i<rcvd; ++i)
            {
                dataout.push_back(buff[i]);
            }
            et.reset();
        }
    } while(et.split() <= delay);


    if (dataout.size() > 0)
    {

        printf("RX (hex): ");

        for (unsigned int i = 0; i < dataout.size(); i++)
        {
            printf("%2x ", dataout[i]);
        }

        printf(" (%lu bytes)", dataout.size());
        printf("\n");
    }

    close(fh);
    return 0;
}



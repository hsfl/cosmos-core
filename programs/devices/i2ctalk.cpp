#include "support/configCosmos.h"
#include <iostream>
#include <string>

#include "device/i2c/i2c.h"

#define I2C_BUFFER_LIMIT 32 //32

uint8_t testi2c_read_write(uint8_t address, int nbytes, int nbytes_rx, uint8_t buff[]);

int main(int argc, char *argv[])
{

    if (argc == 1) {
        cout << "--------------------- " << endl;
        cout << "COSMOS i2c Test Program " << endl;
        cout << "Requies arguments: " << endl;
        cout << "<address> <nbytes_rx> <byte0> <byte1> ..." << endl;
        exit(0);
    }

    uint8_t buff[I2C_BUFFER_LIMIT];

    int nbytes_tx = argc-2;
    int nbytes_rx = 0;
    uint8_t address = 0x10; // default address

    memset(buff, 0, sizeof(buff));

    // input format
    // <address> <nbytes_rx> <byte0> <byte1> ...
    if (argc > 1) {
        //address = *argv[1];
        address = strtol(argv[1], NULL, 16);
        nbytes_rx = atoi(argv[2]); //

        if (argc == 3) {
            // no data specifief

        } else if (argc == 4) {
            // data: 1 byte
            // convert hex string into real hex ("A1" = 0xA1)
            buff[0] = strtol(argv[3], NULL, 16);
        } else if (argc == 5) {
            // data: 2 bytes
            buff[0] = strtol(argv[4], NULL, 16);
            buff[1] = strtol(argv[5], NULL, 16);
        }
        else {
            //strcpy(buff, "11");
            buff[0] = 0x1;
            buff[1] = 0x2;
        }
    }

    printf("Send data to address 0x%x: \n", address);

    testi2c_read_write(address, nbytes_tx, nbytes_rx, buff);

    return 0;
}


uint8_t testi2c_read_write(uint8_t address, int nbytes, int nbytes_rx, uint8_t buff[I2C_BUFFER_LIMIT]) {
    int fh;
    int len, sent, rcvd;




    fh = open("/dev/i2c-3", O_RDWR);

    if (fh < 0) {
        perror("open");
        return 1;
    }

    if (ioctl(fh, I2C_SLAVE, address) < 0) {
        perror("ioctl");
        return 1;
    }

    len = nbytes; // sizeof(buff);


    sent = write(fh, buff, len);

    if (sent != len) {
        perror("write");
        return 1;
    } else {
        printf("TX: ");

        for (int i=0; i< sent; i++) {
            printf("0x%2x ", buff[i]);
        }
        printf(" (%d bytes)", sent);
        cout << endl;
    }


    //    printf("Sent: 0x%x | %d\n", buff[0], buff[0]);

    memset(buff, 0, sizeof(buff));
    //printf("Sent: 0x%x | %d\n", buff[0], buff[0]);
    rcvd = read(fh, buff, nbytes_rx);

    if (rcvd > 0){

        printf("RX: ");

        for (int i=0; i< rcvd; i++) {
            printf("0x%2x ", buff[i]);
        }

        printf(" (%d bytes)", rcvd);
        printf("\n");
    }

    close(fh);
}



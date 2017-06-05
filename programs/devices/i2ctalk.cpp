#include "support/configCosmos.h"
#include <iostream>
#include <string>

#include "device/i2c/i2c.h"

#define I2C_BUFFER_LIMIT 32

uint8_t testi2c_read_write(std::string device, uint8_t address, int nbytes, int nbytes_rx, uint8_t buff[]);

int main(int argc, char *argv[])
{

  if (argc == 1) {
      cout << "--------------------- " << endl;
      cout << "COSMOS i2c Test Program " << endl;
      cout << "Requies arguments: " << endl;
      cout << "<device> <address> <rx_nbytes> tx_byte[0] tx_byte[1]" << endl;
      cout << "example: /dev/i2c-2 57 8 80" << endl;
      cout << "talk to device i2c-2 to slave address 57, receive 8 bytes and send the byte 80" << endl;
      exit(0);
    }

  uint8_t buff[I2C_BUFFER_LIMIT];

  int nbytes_tx = argc-4;
  int nbytes_rx = 0;
  uint8_t address = 0x10; // default address
  string device = "/dev/i2c-2"; // default device

  memset(buff, 0, sizeof(buff));

  // input format
  // <device> <address> <rx_nbytes> <tx_byte0> <tx_byte1>

  if (argc > 1) {

      device = argv[1];
      address = strtol(argv[2], NULL, 16);
      nbytes_rx = atoi(argv[3]); //

      // collect bytes to send
      if (argc > 3) {

          for (int i=0; i<argc-4; i++) {
              buff[i] = strtol(argv[4+i], NULL, 16);
            }
        }
    }

  printf("Send data to address 0x%x: \n", address);

  testi2c_read_write(device, address, nbytes_tx, nbytes_rx, buff);

  return 0;
}


uint8_t testi2c_read_write(string device, uint8_t address, int nbytes, int nbytes_rx, uint8_t buff[I2C_BUFFER_LIMIT]) {
  int fh;
  int len, sent, rcvd;

  fh = open(device.c_str(), O_RDWR);

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
      printf("TX (hex): ");

      for (int i=0; i< sent; i++) {
          printf("%2x ", buff[i]);
        }
      printf(" (%d bytes)", sent);
      cout << endl;
    }


  //    printf("Sent: 0x%x | %d\n", buff[0], buff[0]);

  memset(buff, 0, sizeof(buff));
  //printf("Sent: 0x%x | %d\n", buff[0], buff[0]);
  rcvd = read(fh, buff, nbytes_rx);

  if (rcvd > 0){

      printf("RX (hex): ");

      for (int i=0; i< rcvd; i++) {
          printf("%2x ", buff[i]);
        }

      printf(" (%d bytes)", rcvd);
      printf("\n");
    }

  close(fh);
}



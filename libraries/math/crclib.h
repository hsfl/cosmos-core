#ifndef _CRCLIB_H
#define _CRCLIB_H

#include <cstdint>
#include <map>
#include <fstream>
using std::ifstream;
#include <string>
using std::string;
#include <vector>
using std::vector;

#include "support/cosmos-errno.h"
#include "math/bytelib.h"


//! CRC-16-CCITT Normal
#define CRC16CCITT 0x1021
#define CRC16CCITTMSB 0x1021
#define CRC16CCITTMSBINIT 0xffff
//! CRC-16-CCITT Reversed
#define CRC16CCITTR 0x8408
#define CRC16CCITTLSB 0x8408
#define CRC16CCITTLSBINIT 0x0000
//! CRC-16-CCITT Reversed Reciprocal
#define CRC16CCITTRR 0x8810

uint16_t calc_crc16ccitt(uint8_t *buf, int size, bool lsb=true);
uint16_t calc_crc16(uint8_t *buf, uint16_t size, uint16_t poly=CRC16CCITTMSB, uint16_t crc=CRC16CCITTMSBINIT, uint16_t xorout=0x0, bool lsbfirst=false);
uint16_t calc_crc16(vector<uint8_t> buf, uint16_t poly=CRC16CCITTMSB, uint16_t crc=CRC16CCITTMSBINIT, uint16_t xorout=0x0, bool lsbfirst=false);
uint16_t calc_crc16ccitt_lsb(string buf, uint16_t initialcrc=CRC16CCITTLSBINIT, uint16_t skip=0);
uint16_t calc_crc16ccitt_lsb(vector<uint8_t> &buf, uint16_t initialcrc=CRC16CCITTLSBINIT, uint16_t skip=0);
uint16_t calc_crc16ccitt_lsb(uint8_t *buf, uint16_t size, uint16_t initialcrc=CRC16CCITTLSBINIT);
uint16_t calc_crc16_lsb(uint8_t *buf, uint16_t size, uint16_t poly=CRC16CCITTLSB, uint16_t crc=CRC16CCITTLSBINIT, uint16_t xorout=0x0);
uint16_t calc_crc16_lsb(vector<uint8_t> &buf, uint16_t poly=CRC16CCITTLSB, uint16_t crc=CRC16CCITTLSBINIT, uint16_t xorout=0x0, uint16_t skip=0);
uint16_t calc_crc16ccitt_msb(string buf, uint16_t initialcrc=CRC16CCITTMSBINIT, uint16_t skip=0);
uint16_t calc_crc16ccitt_msb(vector<uint8_t> &buf, uint16_t initialcrc=CRC16CCITTMSBINIT, uint16_t skip=0);
uint16_t calc_crc16ccitt_msb(uint8_t *buf, uint16_t size, uint16_t initialcrc=CRC16CCITTMSBINIT);
uint16_t calc_crc16_msb(uint8_t *buf, uint16_t size, uint16_t poly=CRC16CCITTMSB, uint16_t crc=CRC16CCITTMSBINIT, uint16_t xorout=0x0);
uint16_t calc_crc16_msb(vector<uint8_t> &buf, uint16_t poly=CRC16CCITTMSB, uint16_t crc=CRC16CCITTMSBINIT, uint16_t xorout=0x0, uint16_t skip=0);

class CRC16
{

public:
    uint16_t lookup[256];
    struct crcset
    {
        bool lsbfirst;
        uint16_t polynomial;
        uint16_t initialcrc;
        uint16_t xorout;
        uint16_t test;
    };

    std::map<string, crcset> types;
//    CRC16(uint16_t polynomial=0x1021, uint16_t initial=0xffff, uint16_t xorout=0x0, bool lsbfirst=false);
    CRC16();
    uint16_t set(string type);
    uint16_t set(uint16_t polynomial=0x1021, uint16_t initialcrc=0xffff, uint16_t xorout=0x0, bool lsbfirst=false);
    uint16_t calc(vector<uint8_t> message);
    uint16_t calc(vector<uint8_t> message, uint16_t size);
    uint16_t calc(string message, uint16_t size);
    uint16_t calc(string message);
    uint16_t calc(uint8_t *message, uint16_t size);
    int32_t calc_file(string file_path);

private:
    string type = "ccitt-false";
    uint16_t test;
    uint16_t initial;
    uint16_t polynomial;
    uint16_t xorout;
    bool lsbfirst = true;
};

#endif

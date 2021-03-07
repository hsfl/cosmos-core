#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>

#define BSQ 1
#define BIL 2
#define BIP 3
#define DT_BYTE 1
#define DT_INT 2
#define DT_U_INT 12
#define DT_LONG 3
#define DT_U_LONG 13
#define DT_FLOAT 4
#define DT_DOUBLE 5
#define BO_INTEL 0
#define BO_NETWORK 1

typedef struct envi_hdr
	{
    size_t planes;
    size_t rows;
    size_t columns;
    size_t offset;
    size_t endian;
    size_t datatype;
    size_t interleave;
    size_t byteorder;
	float x0;
	float y0;
	float xmpp;
	float ympp;
    std::string map_info;
    std::string projection_info;
    std::vector <std::string> band_name;
    std::vector <float> wavelength;
    std::string description;
    std::vector <std::string> keys;
    std::vector <std::string> values;
    std::string basename;
    } envi_hdr;


int read_envi_hdr(std::string file, envi_hdr &hdr);
int write_envi_hdr(envi_hdr &hdr);

#include "support/cosmos-errno.h"

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
    string map_info;
    string projection_info;
    vector <string> band_name;
    vector <float> wavelength;
    string description;
    vector <string> keys;
    vector <string> values;
    string basename;
    string hdrname;
    string dataname;
    } envi_hdr;


int32_t write_envi_hdr(envi_hdr &hdr);
int32_t write_envi_data(string name, size_t columns, size_t rows, size_t planes, uint8_t datatype, uint8_t interleave, uint8_t *data);
int32_t write_envi_data(envi_hdr &ehdr, uint8_t *data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<vector<double>>> &data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<vector<uint16_t>>> &data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<double>> &data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<uint16_t>> &data);
int32_t read_envi_hdr(string file, envi_hdr &hdr);
int32_t read_envi_data(string filename, envi_hdr &hdr, vector<vector<vector<double>>> &data);

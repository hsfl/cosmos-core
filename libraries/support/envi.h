#ifndef _ENVI_H
#define _ENVI_H

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

struct MapInfoGeoLatLon
{
    //! Projection type 
    string projection_type = "Geographic Lat/Lon";
    //! Pixel tie point. Which x pixel corresponds to the lat/lon point.
    double tie_point_x = 1.5;
    //! Pixel tie point. Which y pixel corresponds to the lat/lon point.
    double tie_point_y = 1.5;
    //! Longitude
    double longitude = 0;
    //! Latitude
    double latitude = 0;
    //! Size of each pixel, in units of either degrees or radians
    double pixel_size_x = 0.001;
    //! Size of each pixel, in units of either degrees or radians
    double pixel_size_y = 0.001;
    //! Datum
    string datum = "WGS-84";
    //! Set to true for degrees or false for radians
    bool is_degrees = true;
    //! Rotation of the image as projected onto a flat map, where 0 offset from North and positive is CCW
    double rotation = 0;

    //! Returns an ENVI readable map info field
    string to_string();
};

struct envi_hdr
{
    size_t planes;
    size_t rows;
    size_t columns;
    size_t offset = 0;
    size_t endian;
    size_t datatype;
    size_t interleave = BSQ;
    size_t byteorder = BO_INTEL;
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
    uint8_t datasize = 0;
};


int32_t write_envi_hdr(envi_hdr &hdr);
int32_t write_envi_hdr(string name, size_t columns, size_t rows, size_t planes, uint8_t datatype, uint8_t interleave);
int32_t write_envi_data(string name, size_t columns, size_t rows, size_t planes, uint8_t datatype, uint8_t interleave, uint8_t *data);
int32_t write_envi_data(envi_hdr &ehdr, uint8_t *data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<vector<double>>> &data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<vector<uint16_t>>> &data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<double>> &data);
int32_t write_envi_data(string name, uint8_t interleave, vector<vector<uint16_t>> &data);
int32_t read_envi_hdr(string file, envi_hdr &hdr);
int32_t read_envi_data(string filename, envi_hdr &hdr, vector<vector<vector<double>>> &data);
int32_t read_envi_data(envi_hdr hdr, vector<vector<double>> &data, size_t plane);
int32_t read_envi_data(envi_hdr hdr, vector<vector<double>> &data);

#endif // ENVI_H

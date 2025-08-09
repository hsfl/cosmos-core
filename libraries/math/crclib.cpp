#include "crclib.h"

//! Calculate CRC-16
/*! Calculate 16-bit CRC for the indicated type, buffer and number of bytes.
*/

//CRC16::CRC16(uint16_t polynomial, uint16_t initial, uint16_t xorout, bool lsbfirst)
CRC16::CRC16()
{
    types[string("ccitt-false")] = {false, 0x1021, 0xffff, 0x0000, 0x29b1};
    types[string("xmodem")] = {false, 0x1021, 0x0000, 0x0000, 0x31c3};
    types[string("hdlc")] = {true, 0x1021, 0xffff, 0xffff, 0x906e};
    types[string("kermit")] = {true, 0x1021, 0x0000, 0x0000, 0x2189};
    types[string("maxim")] = {true, 0x8005, 0x0000, 0xffff, 0x44c2};
    types[string("usb")] = {true, 0x8005, 0xffff, 0xffff, 0xb4c8};

    set("ccitt-false");
}

uint16_t CRC16::set(string type)
{
    if (types.find(type) != types.end())
    {
        this->type = type;
        this->test = types[type].test;
        return set(types[type].polynomial, types[type].initialcrc, types[type].xorout, types[type].lsbfirst);
    }
    else
    {
        return COSMOS_GENERAL_ERROR_UNDEFINED;
    }
}

uint16_t CRC16::set(uint16_t polynomial, uint16_t initialcrc, uint16_t xorout, bool lsbfirst)
{
    this->lsbfirst = lsbfirst;
    this->initial = initialcrc;
    this->xorout = xorout;
    this->polynomial = polynomial;

// Compute the remainder of each possible dividend.
    for (int32_t dividend = 0; dividend < 256; ++dividend)
    {
        uint8_t byte = dividend;
        lookup[dividend] = calc_crc16(&byte, 1, this->polynomial, 0, 0, this->lsbfirst);
    }
    return 0;
}

//uint16_t CRC16::calc(uint8_t *buf, uint16_t size)
//{
//    vector<uint8_t> vbuf(buf, buf+(size));
//    return calc(vbuf);
//}

uint16_t CRC16::calc(string message, uint16_t size)
{
//    vector<uint8_t> vmessage(&message[0], &message[size]);
//    return calc(vmessage);
    if (size <= message.length())
    {
        return calc((uint8_t *)message.c_str(), size);
    }
    else
    {
        return calc((uint8_t *)message.c_str(), message.length());
    }
}

uint16_t CRC16::calc(string message)
{
//    vector<uint8_t> vmessage(&message[0], &message[message.size()]);
//    return calc(vmessage);
    return calc((uint8_t *)message.c_str(), message.length());
}

uint16_t CRC16::calc(vector<uint8_t> message, uint16_t size)
{
//    vector<uint8_t> vmessage(&message[0], &message[size]);
//    return calc(vmessage);
    if (size <= message.size())
    {
        return calc(message.data(), size);
    }
    else
    {
        return calc(message.data(), message.size());
    }
}

uint16_t CRC16::calc(vector<uint8_t> message)
{
    return calc(message.data(), message.size());
}

uint16_t CRC16::calc(uint8_t *message, uint16_t size)
{
    // std::lock_guard<mutex> lock(*mtx);
    uint8_t data;
    uint16_t remainder = initial;

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (size_t byte = 0; byte < size; ++byte)
    {
        if (lsbfirst)
        {
            data = message[byte] ^ (remainder & 0xff);
            remainder = lookup[data] ^ (remainder >> 8);
        }
        else
        {
            data = message[byte] ^ (remainder >> (8));
            remainder = lookup[data] ^ (remainder << 8);
        }
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder ^ xorout);
}

/**
 * @brief Read file data into a buffer, using a pre-read buffer to minimize I/O.
 * 
 * @param file File to read from
 * @param dest Destination buffer to read data into
 * @param offset Offset into the file to read from
 * @param size Number of bytes to read from the file
 * @return Number of bytes read or negative on error.
 */
static int32_t buffered_read(FILE* file, uint8_t* dest, size_t offset, size_t size)
{
    static constexpr size_t READ_BUFFER_SIZE = 2*4096; // modify as needed for best performance and memory usage
    static uint8_t read_buffer[READ_BUFFER_SIZE];
    // Index of the start of the read buffer in the file. Used to determine if a buffered read request can be fulfilled by the pre-read buffered data.
    static size_t start_of_buffer_fseek_index = std::numeric_limits<size_t>::max();
    // Keep track of the current file that is being read from
    static FILE* current_file = nullptr;
    // The amount of data in the read buffer, which could be smaller than READ_BUFFER_SIZE if the file is smaller than the buffer size or if the end of the file was reached. 
    static size_t buffer_size = 0;
    if (file != current_file                    // If a different file is requested
    || offset < start_of_buffer_fseek_index     // If the requested offset is before the start of the buffered read
    || start_of_buffer_fseek_index + buffer_size <= offset + size) // If the requested read is larger than the buffered read
    {
        // Read a new buffer
        if (file == nullptr || fseek(file, offset, SEEK_SET) != 0)
        {
            return COSMOS_GENERAL_ERROR_BAD_FD;
        }
        start_of_buffer_fseek_index = offset;
        current_file = file;
        buffer_size = fread(read_buffer, 1, READ_BUFFER_SIZE, file);
        if (buffer_size == 0)
        {
            return 0;
        }
        else if (buffer_size < READ_BUFFER_SIZE && !feof(file))
        {
            return COSMOS_GENERAL_ERROR_BAD_FD;
        }
    }
    // Read from the buffer
    memcpy(dest, read_buffer + (offset - start_of_buffer_fseek_index), size);
    return size;
}

// Modified for calculating the crc of a file
// Returns non-negative uint16_t crc on success, negative on error
int32_t CRC16::calc_file(string file_path)
{
    uint8_t data;
    uint16_t remainder = initial;
    int32_t iretn = 0;

    // Check file validity
    FILE* fp = fopen(file_path.c_str(), "rb");
    if (fp == nullptr)
    {
        return COSMOS_TRANSFER_ERROR_FILENAME;
    }

    // Divide message by the polynomial a byte at a time until EOF
    char byte;
    size_t offset = 0;
    while ((iretn = buffered_read(fp, reinterpret_cast<uint8_t*>(&byte), offset, sizeof(byte))) > 0)
    {
        ++offset;
        if (lsbfirst)
        {
            // Note, char is signed
            data = static_cast<uint8_t>(byte) ^ (remainder & 0xff);
            remainder = lookup[data] ^ (remainder >> 8);
        }
        else
        {
            // Note, char is signed
            data = static_cast<uint8_t>(byte) ^ (remainder >> (8));
            remainder = lookup[data] ^ (remainder << 8);
        }
    }
    if (iretn < 0)
    {
        fclose(fp);
        return iretn;
    }

    iretn = (remainder ^ xorout);
    fclose(fp);

    /*
     * The final remainder is the CRC.
     */
    return iretn;
}

uint16_t calc_crc16ccitt_lsb(string buf, uint16_t crc, uint16_t skip)
{
    vector<uint8_t> vbuf(buf.begin(), buf.end()-skip);
    return calc_crc16ccitt_lsb(vbuf, crc);
}

uint16_t calc_crc16ccitt_lsb(uint8_t* buf, uint16_t size, uint16_t crc)
{
    vector<uint8_t> vbuf(buf, buf+(size));
    return calc_crc16ccitt_lsb(vbuf, crc);
}

uint16_t calc_crc16ccitt_lsb(vector<uint8_t> &buf, uint16_t crc, uint16_t skip)
{
    uint8_t ch;

    for (uint16_t i=0; i<buf.size()-skip; i++)
    {
        ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {
            crc = (crc >> 1) ^ (((ch^crc)&0x01)?CRC16CCITTLSB:0);
            ch >>= 1;

        }
    }
    return (crc);
}

uint16_t calc_crc16_lsb(uint8_t* buf, uint16_t size, uint16_t poly, uint16_t crc, uint16_t xorout)
{
    vector<uint8_t> vbuf(buf, buf+(size));
    return calc_crc16_lsb(vbuf, poly, crc, xorout);
}

uint16_t calc_crc16_lsb(vector<uint8_t> &buf, uint16_t poly, uint16_t crc, uint16_t xorout, uint16_t skip)
{
    uint8_t ch;

    for (uint16_t i=0; i<buf.size()-skip; i++)
    {
        ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {
            crc = (crc >> 1) ^ (((ch^crc)&0x01)?poly:0);
            ch >>= 1;

        }
    }
    return (crc ^ xorout);
}

uint16_t calc_crc16ccitt_msb(string buf, uint16_t crc, uint16_t skip)
{
    vector<uint8_t> vbuf(buf.begin(), buf.end()-skip);
    return calc_crc16ccitt_msb(vbuf, crc);
}

uint16_t calc_crc16ccitt_msb(uint8_t* buf, uint16_t size, uint16_t crc)
{
    vector<uint8_t> vbuf(buf, buf+(size));
    return calc_crc16ccitt_msb(vbuf, crc);
}

uint16_t calc_crc16ccitt_msb(vector<uint8_t> &buf, uint16_t crc, uint16_t skip)
{
    //    uint16_t crc;
    uint8_t ch;

    //        crc = 0;

    for (uint16_t i=0; i<buf.size()-skip; i++)
    {
        ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {

            crc = (crc << 1) ^ (((ch&0x80)^((crc&0x8000)>>8))?CRC16CCITTMSB:0);
            ch <<= 1;

        }
    }
    return (crc);
}

uint16_t calc_crc16_msb(uint8_t* buf, uint16_t size, uint16_t poly, uint16_t crc, uint16_t xorout)
{
    vector<uint8_t> vbuf(buf, buf+(size));
    return calc_crc16_msb(vbuf, poly, crc, xorout);
}

uint16_t calc_crc16_msb(vector<uint8_t> &buf, uint16_t poly, uint16_t crc, uint16_t xorout, uint16_t skip)
{
    uint8_t ch;

    for (uint16_t i=0; i<buf.size()-skip; i++)
    {
        ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {

            crc = (crc << 1) ^ (((ch&0x80)^((crc&0x8000)>>8))?poly:0);
            ch <<= 1;

        }
    }
    return (crc ^ xorout);
}

uint16_t calc_crc16ccitt(uint8_t *buf, int size, bool lsb)
{
    uint16_t crc;
    uint8_t ch;

    if (lsb)
    {
        crc = CRC16CCITTLSBINIT;
    }
    else
    {
        crc = CRC16CCITTMSBINIT;
    }
    for (uint16_t i=0; i<size; i++)
    {
        ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {
            if (lsb)
            {
                crc = (crc >> 1) ^ (((ch^crc)&0x01)?CRC16CCITTLSB:0);
                ch >>= 1;
            }
            else
            {
                crc = (crc << 1) ^ (((ch&0x80)^((crc&0x8000)>>8))?CRC16CCITTMSB:0);
                ch <<= 1;
            }
        }
    }
    return (crc);
}
uint16_t calc_crc16ibm(uint8_t *buf, int size, bool lsb)
{
    uint16_t crc;
    uint8_t ch;

    if (lsb)
    {
        crc = CRC16IBMLSBINIT;
    }
    else
    {
        crc = CRC16IBMMSBINIT;
    }
    for (uint16_t i=0; i<size; i++)
    {
        ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {
            if (lsb)
            {
                crc = (crc >> 1) ^ (((ch^crc)&0x01)?CRC16IBMLSB:0);
                ch >>= 1;
            }
            else
            {
                crc = (crc << 1) ^ (((ch&0x80)^((crc&0x8000)>>8))?CRC16IBMMSB:0);
                ch <<= 1;
            }
        }
    }
    return (crc);
}

uint16_t calc_crc16(uint8_t* buf, uint16_t size, uint16_t poly, uint16_t crc, uint16_t xorout, bool lsbfirst)
{
    vector<uint8_t> vbuf(buf, buf+(size));
    return calc_crc16(vbuf, poly, crc, xorout, lsbfirst);
}

uint16_t calc_crc16(vector<uint8_t> buf, uint16_t poly, uint16_t crc, uint16_t xorout, bool lsbfirst)
{
    if (lsbfirst)
    {
        uint8_t lowbyte = poly&0xff;
        lowbyte = uint8to(&lowbyte, ByteOrder::BIGENDIAN);
        uint8_t highbyte = poly>>8;
        highbyte = uint8to(&highbyte, ByteOrder::BIGENDIAN);
        poly = 256L * lowbyte + highbyte;
    }

    for (uint16_t i=0; i<buf.size(); i++)
    {
        uint16_t ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {
            if (lsbfirst)
            {
                crc = (crc >> 1) ^ (((ch^crc)&0x01)?poly:0);
                ch >>= 1;
            }
            else
            {
                crc = (crc << 1) ^ (((ch&0x80)^((crc&0x8000)>>8))?poly:0);
                ch <<= 1;
            }
        }
    }
    return (crc ^ xorout);
}


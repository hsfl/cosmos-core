//! \file comm-support.h
//! \brief Generic packet communications protocol

//! \ingroup devices
//! \defgroup comm_support Generic packet communications protocol support
//! COSMOS has defined a simplet protocol for communications over any interface.
//! Small packets are used with a short header. Each packet is SLIP encoded and a
//! CRC is appended.

/* **************************************************************************
 * File/Desc:		comm_support.cpp
 * Description:		High level hardware independent comm support routines
 * Target Device/Freq:	Any
 * By:  		20130813 Eric Pilger
 * Updated:		20140930 Jeremy Chan
 *              - Added error number for bad size.  Added load/unload/check packet functions.
 *              20140610 Jeremy Chan
 *              - Updated with 20140604 specifically for FSW (using cssl)
 *              - Disabled comm_send_raw and putslip to eliminate comm support low level need
 *              - Integrated into isc_lib and suchi_lib.
 *              - Initiating elimination of packet types args0 and args1 in favor of binary data structures
 *                  o Added comm_bindata_1arg_ui16 data structure to accomodate removal of args1 format
 *              - Removed cssl_lib.h from this file, unnecessary
 *              20140604 Jeremy Chan
 *              - Added error flag to indicate good command, but error detected in executing requested command
 *              - New Flag/Response Rules
 *                o No Response: Command not understood
 *                o Acknowledge Flag: Command understood and parameters were accepted
 *                o Negative Acknowledge Flag: Command understood, but parameters were rejected
 *                o Error Flag: If acknowledged, this flag indicates that requested command failed to execute properly
 *              20140424 Jeremy Chan
 *              - Modified comm_packet structure to include source interface meta data for many to one FIFO operation
 *              20140408 Jeremy Chan
 *              - Added comm_send_raw function
 *              20140407 Jeremy Chan
 *              - Microchip MPLAB C30 Compiler has no stdint.h, added in use of GNU library by Dean Ferreyra (Bourbon Street Software)
 *              20140328 Jeremy Chan
 *              - Modified to reflect HEC protocol v1.1 rev 6 concepts
 *              20140327 Jeremy Chan
 *              - Updated for HEC Protocol v1.1
 *              20140320 Jeremy Chan
 *              - Removed packet prep functions in favor of streaming
 *                          data directly to low level functions.  Requires
 *                          implementation of low level comm library.
 *                          Did a runthrough of cleaning up comments.
 *              - Added NACK packet flag
 *              - Changed error enumeration into a series of definitions, added new defs
 *              20140318 Jeremy Chan
 *              - Modified Trailer Size to be CRC size only, +2 was breaking things
 *              20140313 Jeremy Chan
 *                      - Updated to align references to packet and frame
 *                      - Moved header into packet definitions
 *                      - Removed comm_t definition, unused
 *                      - Changed name of comm_slip_unstuff_to_buffer to comm_slip_unframe_data_to_buffer
 *                      - Added new data types and macros for calculations
 *              20131016 Jeremy Chan
 *                      - Updated for use with HECv1
 *              20130924 Jeremy Chan
 *                       - Updated with more structures
 *                      20130831 Jeremy Chan
 *                        - Per discussions with EP, split low level routines
 *                          to this file.  This file to be used as a template
 * For: 		HawaiiSat-1 Mission, Hawaii Space Filght Laboratory
 * Dependancies:	Standard C libraries
 * IDE:			N/A
 * Special Compilation:
 * Problems:
 * Other Comments:	Entire comm_support library should be copied to target
 * 			project and only low level routines modified for target.
 *
 *          - 20140610JC TODO: Bring return values inline with 'negative' being error, 0 being success, positive being success w/ information
 * ************************************************************************** */

#ifndef __COMMSUPP_H__
#define __COMMSUPP_H__

// **************************************************************************
// Usage Configuration
// **************************************************************************

// Configuration of Comm Support Library
//#define COMM_SUP_LOWLEVEL_ENABLE       // Define if low level functions are being implemented
//#define COMM_SUP_EMBEDDED_MCHP_C30     // Define if using Microchip C30 compiler, requires different libraries



// **************************************************************************
// Libraries
// **************************************************************************

// Include low level library if needed
#ifdef COMM_SUP_LOWLEVEL_ENABLE
#include "comm_support_lowlevel.h"
#endif

// Include alternate libraries for supporting Microchip C30 compiler
#ifdef COMM_SUP_EMBEDDED_MCHP_C30
    // Use C30 Compiler Ints and Bool (3rd party)
    #include "inttypes.h"
    #include "boolean.h"
#else
    // Use C99 Compatible Compiler Ints and Bool
    #include <stdbool.h>
    #include <stdint.h>
#endif

// Standard Comm Support Library Libraries
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>



// **************************************************************************
// SLIP Byte Definitions
// **************************************************************************
//! \ingroup comm_support
//! \defgroup comm_support_constants Communications support library constants
//! @{
//! SLIP Buffer End character
#ifndef SLIP_FEND
    #define SLIP_FEND 0xC0
#endif
//! SLIP Buffer Escape character
#ifndef SLIP_FESC
    #define SLIP_FESC 0xDB
#endif
//! SLIP Buffer Escaped End character
#ifndef SLIP_TFEND
    #define SLIP_TFEND 0xDC
#endif
//! SLIP Buffer Escaped Escape character
#ifndef SLIP_TFESC
    #define SLIP_TFESC 0xDD
#endif


// **************************************************************************
// Default HEC Command Dictionary, 0x00 thru 0x0F reserved for generic comm support commands only
// **************************************************************************
#define COMM_CMD_PING           0x00    // Command for requesting device ID and version
#define COMM_CMD_MESSAGE        0x0B    // Text message data type
#define COMM_CMD_STATUS         0x0C    // Command for requseting status of device (e.g. on or off, sleeping or running)
#define COMM_CMD_SLEEP          0x0D    // Command for making target sleep (power saving mode)
#define COMM_CMD_WAKE           0x0E    // Command for making target wake up (full operation mode)
#define COMM_CMD_RESET          0x0F    // Command for resetting target (e.g. power cycle self)

// **************************************************************************
// Flag Dictionary
// **************************************************************************
#define COMM_FLAG_NONE 0x00     // No flags for originating packet (deprecated use for NACK)
#define COMM_FLAG_ACK  0x80     // Response flag: Indicates positive acknowledge - Command received and had good parameters
#define COMM_FLAG_NACK 0x40     // Response flag: Indicates pegative acknowledge - Command received and had bad parameters
#define COMM_FLAG_CTS  0x20     // Response flag: Indicates Clear to Send (Indicates destination has more buffer space to receive from sender)
#define COMM_FLAG_RTS  0x10     // Response flag: Indicates Ready to Send (Indicates data is available for sender to retrieve from destination device number, only used for synchronous operations, unused in async)
#define COMM_FLAG_ERR  0x08     // Response flag: Indicates error executing command - Command received with good parameters, but error detected in carrying out command


// **************************************************************************
// Packet & Frame Sizes
// **************************************************************************
#define COMM_PACKET_HEADER_SIZE         8               // Manually calculated from packet_header
#define COMM_PACKET_DATA_SIZE           300             // Manually set, be careful to accomodate for the largest packets you'll handle in dependent libraries
#define COMM_PACKET_CRC_SIZE            2               // Constant for CRC-16 operation

// Derived defines
#define COMM_PACKET_TRAILER_SIZE        COMM_PACKET_CRC_SIZE // Just in case byte alignment is 32 bits
#define COMM_PACKET_OVERHEAD_SIZE       COMM_PACKET_HEADER_SIZE + COMM_PACKET_TRAILER_SIZE
#define COMM_PACKET_MAX_SIZE            COMM_PACKET_DATA_SIZE + COMM_PACKET_HEADER_SIZE + COMM_PACKET_TRAILER_SIZE
#define COMM_PACKET_DATA_OFFSET         COMM_PACKET_HEADER_SIZE      // Data starts right after header
#define COMM_CRC_SIZE                   COMM_PACKET_CRC_SIZE
#define COMM_FRAME_BUFFER_MAX           (COMM_PACKET_MAX_SIZE*2)+2  // For SLIP frame storage

// Deprecated defines
#define COMM_FRAME_HEADER_SIZE          COMM_PACKET_HEADER_SIZE     // For backward compatibility
#define COMM_FRAME_DATA_SIZE            COMM_PACKET_DATA_SIZE       // For backward compatibility
#define COMM_FRAME_TRAILER_SIZE         COMM_PACKET_CRC_SIZE        // For backward compatibility
#define COMM_FRAME_SIZE                 COMM_PACKET_MAX_SIZE        // For backward compatibility

/* Common Formulas
 * [Packet Length] = sizeof packet = 8(header) + n(data) + m(reserved) + 2(CRC-16)
 * [Packet Header: size of packet data] = sizeof (packet) - 10 = sizeof (packet) - 8(header) - 2(CRC-16 trailer)
 * [CRC-16: number of bytes in packet data to calculate CRC-16] = sizeof (packet) - 2
 * [CRC-16: index position of CRC-16 in packet] = sizeof (packet) - 2
 * [SLIP: bytes to frame into SLIP format] = sizeof (packet)
 * [SLIP: bytes to unframe from SLIP format] = number of bytes in frame received
*/

// Common Formula Macros, does not work on variable length packet type
#define COMM_PACKETTYPE_BYTES(packetType)              sizeof(packetType)     // Used for framing a packet
#define COMM_PACKETTYPE_HEADER_DATABYTES(packetType)   sizeof(packetType)-10  // Used for setting up header
#define COMM_PACKETTYPE_CRC_BYTES_TO_CALC(packetType)  sizeof(packetType)-2   // Used for CRC calculation coverage
#define COMM_PACKETTYPE_CRC_POSITION(packetType)       sizeof(packetType)-2   // Used for get/set CRC in packet
#define COMM_RX_PKT_CRC_COVERAGE(var1)             (var1)-2               // Used for calculating CRC for var sized packet
#define COMM_RX_PKT_CRC_POSITION_OFFSET(var1)      (var1)-2               // Used for getting CRC in var sized packet
#define COMM_TX_PKT_DATA_POSITION_OFFSET           8                      // Used for memory copy operations in transmitting varying length data

// Truncate Flags
#define TRUNCATE_ABORT_OVERSIZE 0
#define TRUNCATE_OK             1


// **************************************************************************
// Data Type Dictionary
// **************************************************************************
#define COMM_DATA_RAW                  0       // Raw, No Encapsulation
#define COMM_DATA_HEC_PACKET           1       // HSFL Embedded Communication Packet (Layer 6)
#define COMM_DATA_ASTRODEV_LI1         2       // Astrodev Lithium-1 Radio Frame (Layer 2)



// **************************************************************************
// Error Definitions
// **************************************************************************

#define COMM_OK                     0      /* everything is all right */
#define COMM_ERROR_NOSIGNAL         -1     /* there's no free signal */
#define COMM_ERROR_NOTSTARTED       -2     /* you should first start cssl */
#define COMM_ERROR_NULLPOINTER      -3     /* you gave a null pointer to the function */
#define COMM_ERROR_OOPS             -4     /* internal error, something's erong */
#define COMM_ERROR_MEMORY           -5     /* there's no memory for COMM_t structure */
#define COMM_ERROR_OPEN             -6     /* file doesnt exist or you aren't good user */
#define COMM_ERROR_CRC              -7     /* crc error */
#define COMM_ERROR_OVERSIZE         -8     /* input was too large */
#define COMM_ERROR_ZEROSIZE         -9     /* input cannot be zero size */
#define COMM_ERROR_UNIMPLEMENTED    -10    /* command is not implemented*/
#define COMM_ERROR_EMPTY            -11    /* usually indicates that a buffer was empty */
#define COMM_ERROR_UNHANDLED_ERR    -12    /* error that is not currently handled */
#define COMM_ERROR_UNDERSIZE        -13    /* packet is smaller than expected size */
#define COMM_ERROR_BAD_SIZE         -14    /* packet is an invalid size */
#define COMM_ERROR_TIMEOUT          -15    /* a timer expired */
#define COMM_ERROR_INPUT            -16    /* invalid input was provided */
#define COMM_ERROR_OUTPUT           -17    /* output from a routine returned bad output, possibly related to bad input */

//! @{

//! \ingroup comm_support
//! \defgroup comm_support_typedefs Communications support library definitions
//! @{

// **************************************************************************
// Structures
// **************************************************************************

/* ********************* */
/* Supporting Structures */
/* ********************* */

// Supporting Structure: Command Header, 8 Bytes, if changed, update COMM_FRAME_HEADER_SIZE
typedef struct {
    uint8_t dst;                // Destination (ignored for point to point connection)
    uint8_t src;                // Source (ignored for point to point connection)
    uint8_t type;               // Packet Type, used to specify commands or information type
    uint8_t flags;              // Flags, used to signal responses, acknowledgements, or negative-acknowledgement
    uint16_t sequence_number;   // Sequence Number, originatorâ€™s sequence number
    uint16_t data_size;         // Size of data section in bytes, max 32757
} comm_packet_header;

// Packet Structure:  Variable Length Generic Type (default for storage of unknown type)
typedef struct
{
    comm_packet_header header;              // 8 Byte Header
    uint8_t data[COMM_PACKET_MAX_SIZE-COMM_PACKET_HEADER_SIZE-COMM_PACKET_TRAILER_SIZE];    // Max Data Size Including CRC
} comm_packet_max_var_len;              // Total: Max Data Size + 2 Trail + 8 Head Bytes

// Packet Binary Data Structure: 1 Argument 16-Bit Type, [Largest Data Alignment Size: 2-byte]
typedef struct
{
    uint16_t arg1_ui16;      // 2 Byte Argument
} comm_bindata_1arg_ui16;    // Total: 2 Bytes


// Packet Binary Data Structure: 1 Argument 32-Bit Type, [Largest Data Alignment Size: 4-byte]
typedef struct
{
    uint32_t arg1_ui32;      // 4 Byte Argument
} comm_bindata_1arg_ui32;    // Total: 4 Bytes


// Packet Binary Data Structure: 2 Argument 8-Bit UI Type, [Largest Data Alignment Size: 2-byte]
typedef struct
{
    uint8_t arg1_ui8;           // 1 Byte Argument
    uint8_t arg2_ui8;           // 1 Byte Argument
} comm_bindata_2arg_ui8;     // Total: 2 Bytes


// Packet Binary Data Structure: 2 Argument 16-Bit UI Type, [Largest Data Alignment Size: 2-byte]
typedef struct
{
    uint16_t arg1_ui16;         // 2 Byte Argument
    uint16_t arg2_ui16;         // 2 Byte Argument
} comm_bindata_2arg_ui16;    // Total: 4 Bytes


// Packet Binary Data Structure: 3 Argument 16-Bit UI Type, [Largest Data Alignment Size: 2-byte]
typedef struct
{
    uint16_t arg1_ui16;      // 2 Byte Argument
    uint16_t arg2_ui16;      // 2 Byte Argument
    uint16_t arg3_ui16;      // 2 Byte Argument
} comm_bindata_3arg_ui16;    // Total: 6 Bytes


// Packet Binary Data Structure: 2 Argument 16-Bit UI and 32-Bit UI Type, [Largest Data Alignment Size: 4-byte]
typedef struct
{
    uint32_t arg2_ui32;             // 4 Byte Argument  arg2 placed first in memory for 32-bit data structure alignment
    uint16_t arg1_ui16;             // 2 Byte Argument  arg1 placed second, but named as arg1 since typically arg1 is smaller than arg2 (e.g. power switch limiting)
    uint16_t reserved;              // 2 Byte CRC-16 Trailer
} comm_bindata_2arg_ui16_ui32;   // Total: 8 Bytes


// Packet Binary Data Structure: Flexible 5 Argument, Two 8-Bit Unsigned Int, One 16-Bit Unsigned Int, One 32-bit Unsigned Int, and One 32-Bit Float
typedef struct
{
    uint8_t arg1_ui8;               // 1 Byte Argument, Unsigned Int 8 Bit
    uint8_t arg2_ui8;               // 1 Byte Argument, Unsigned Int 8 Bit
    uint16_t arg3_ui16;             // 2 Byte Argument, Unsigned Int 16 Bit
    uint32_t arg4_ui32;             // 4 Byte Argument, Unsigned Int 32 Bit
    float arg5_float;               // 4 Byte Argument, Floating Point
} comm_bindata_5arg_flex;        // Total: 12 Bytes

// Data Structure: Time, UNIX Epoch w/ Millisecond Offset
typedef struct {
	int32_t     sec;              // Time in seconds, UNIX Epoch
	uint16_t    ms;    // Milliseconds elapsed in current second
    uint16_t    reserved;          // Byte Alignment Reservation
} comm_bindata_time;            // Total: 8 Bytes



/* ***************** */
/* Usable Structures */
/* ***************** */

// SLIP Frame Grabbing FSM Variables
typedef struct {
    bool frame_sync;            // Whether we're operating in sync with FENDs
    bool next_byte_escaped;     // Next byte is escaped
    uint16_t max_data_size;     // Max number of bytes in frame buffer
    uint16_t position;          // Next position available in frame buffer.  Also number of bytes in buffer
    uint8_t *packet_buffer;     // Destination packet buffer (raw), if NULL, FSM aborts
} slip_frame_rx_fsm_variables;


// Generic Frame Data Storage, max raw length defined above
// See device specific comm dictionaries for more specific framing
typedef struct
{
    union {
        comm_packet_header              header;         // Top level 'header' element
        //comm_packet_args0               args_0;         // Structure: no arguments  20140610JC: Deprecated Use
        //comm_packet_args1               args_1;         // Structure: one 16-bit unsigned int argument  20140610JC: Deprecated Use
        comm_packet_max_var_len         max_var_len;    // Defines max limits of storage unit
        uint8_t raw[COMM_PACKET_MAX_SIZE];   // Direct access to entire frame, including size variable (for frame copies)

        struct
        {
            comm_packet_header header;  // Structure: Header, 8 Bytes: type, flags, seq, size
            union {
                comm_bindata_1arg_ui16      args_1_16;      // Structure: 1 uint16 argument
                comm_bindata_1arg_ui32      args_1_32;      // Structure: 1 uint32 argument
                comm_bindata_2arg_ui16      args_2_16;      // Structure: 2 uint16 arguments
                comm_bindata_2arg_ui8       args_2_8;       // Structure: 2 uint8 arguments
                comm_bindata_2arg_ui16_ui32 args_2_16_32;   // Structure: 1 ui16 and 1 ui32
                comm_bindata_3arg_ui16      args_3_16;      // Structure: 3 uint16 arguments
                comm_bindata_5arg_flex      args_5_flex;    // Structure: 3 uint16 arguments
                comm_bindata_time           time;           // Structure: time, int32_t epoch, uint16_t ms offset

                uint8_t data[COMM_FRAME_DATA_SIZE];     // Direct data section access up to 300 Bytes, use for binary messages
            };
            uint32_t size;  // Use primarily to keep track of received data, can be used for tracking raw sending number as well
        } packet;

    };
    uint32_t size;  // Use primarily to keep track of received data, can be used for tracking raw sending number as well
    uint8_t source_interface;   // 20140424JC: Used primarily for tracking multiple sources going into one FIFO
    uint8_t data_format; // 20140424JC: Tracks type of data contained.  Used to discriminate between radio or HEC packets.  HEC packets are simply forwarded.  Radio packets are forwarded as encapsulated.
} comm_packet;

//! @}

// **************************************************************************
// Function Prototypes
// **************************************************************************

#ifdef	__cplusplus
extern "C" {
#endif

// Platform Independent Code

//! \ingroup comm_support
//! \defgroup comm_support_functions Communications support library function declarations
//! @{

// High Level Frame Grabbing Finite State Machine Operations
int32_t comm_getslip_fsm_newbyte(slip_frame_rx_fsm_variables *fsm, uint8_t new_byte);
void comm_getslip_fsm_init(slip_frame_rx_fsm_variables *fsm, uint16_t max_data_size, uint8_t *buf);
void comm_getslip_fsm_next_frame_init(slip_frame_rx_fsm_variables *fsm, uint8_t *buf);
void comm_getslip_fsm_reset(slip_frame_rx_fsm_variables *fsm);

// High Level Buffer Operations
int32_t comm_slip_frame_data_to_buffer(const uint8_t *msgbuf, int16_t numbytes, uint8_t *slipbuf, int16_t maxbytes);
int32_t comm_slip_unframe_data_to_buffer(const uint8_t *slipbuf, int16_t numbytes, uint8_t *msgbuf, int16_t maxbytes);
int32_t comm_packet_header_setup(comm_packet_header* header, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint16_t size);
int32_t comm_packet_crc_setup(uint8_t *buf, uint16_t size);
int32_t comm_packet_load(comm_packet *packet, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint8_t *databuf, uint16_t datasize, uint8_t truncate_ok);
int32_t comm_packet_set(comm_packet *packet, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint16_t datasize);
int32_t comm_packet_unload(comm_packet *packet, uint16_t packetbytes, uint8_t *databuf, uint16_t maxlen);
int32_t comm_packet_check(comm_packet *packet, uint16_t packetbytes);

// CRC Operations
int32_t comm_check_crc16(uint8_t *sbuf, uint16_t ssize);
uint16_t comm_calc_crc16(uint8_t *buf, uint16_t size);
uint16_t comm_get_crc16(uint8_t *buf, uint16_t index);

// Platform Specific Low Level Library Functions (Requires Low Level Implementation)
#ifdef COMM_SUP_LOWLEVEL_ENABLE
    // Raw Write Operations
    int32_t comm_send_raw(const char *port_name, int port_id, uint8_t *buf, uint16_t size);

    // Frame Write Operations
    int32_t comm_putslip(const char *port_name, int port_id, uint8_t *buf, uint16_t size);
    int32_t comm_send_packet(const char *port_name, int port_id, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint8_t *binary_data, uint16_t numbytes);
    int32_t comm_send_packet_blind(const char *port_name, int port_id, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint8_t *binary_data, uint16_t numbytes);
    int32_t comm_send_packet_varlen_string(const char *port_name, int port_id, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, char *string_data);
    
    // Frame Read Operations
    int16_t comm_packet_check(const char *port_name, int port_id, comm_packet *packetbuf, slip_frame_rx_fsm_variables *fsm_info);
#endif
	//! @}

#ifdef	__cplusplus
}
#endif


#endif /* __COMMSUPP_H__ */




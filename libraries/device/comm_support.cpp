//! \file comm-support.cpp
//! \brief Communications support library source file

/* **************************************************************************
 * File/Desc:		comm_support.cpp
 * Description:		High level hardware independent comm support routines
 * Target Device/Freq:	Any
 * By:  		20130813 Eric Pilger
 * Updated:		20140610 Jeremy Chan
 *              - Modified comm_send_packet_varlen_binary to use name comm_send_packet.  Now supports 0 byte payloads.
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
 *              20140408 Jeremy Chan
 *              - Added comm_send_raw function
 *              20140328 Jeremy Chan
 *                      - Modified to reflect HEC protocol v1.1 rev 6 concepts
 *              20140327 Jeremy Chan
 *                      - Updated for HEC Protocol v1.1
 *                      - Removed CRC test problem from comments, all tested within HEC protocol devices to date
 *              20140320 Jeremy Chan
 *                      - Removed packet prep functions in favor of streaming
 *                          data directly to low level functions.  Requires
 *                          implementation of low level comm library.
 *                          Did a runthrough of cleaning up comments.
 *              20140318 Jeremy Chan
 *                      - Added packet prep functions for arg0 and arg_varlen
 *              20140313 Jeremy Chan
 *                      - Renamed library to .c since it is written in C
 *                      - Confirmed per HECv1, that CRC is part of packet
 *                      - Confirmed that CRC is not part of layer 2 framing
 *                      - Further expanded comments for readability
 *                      - Changed name of comm_slip_unstuff_to_buffer to comm_slip_unframe_data_to_buffer
 *              20131016 Jeremy Chan
 *                      - Updated for use with HECv1
 *              20130924 Jeremy Chan
 *                       - Updated with new FSM routine for single character ops
 *                       - Added buffer manipulation routines for SLIP
 *              20130831 Jeremy Chan
 *                       - Per discussions with EP, split low level routines
 *                         to this file.  This file to be used as a template
 * For: 		HawaiiSat-1 Mission, Hawaii Space Filght Laboratory
 * Dependancies:	Standard C libraries
 * IDE:			N/A
 * Special Compilation:	
 * Problems:		** Need to test CRC code
 * Other Comments:
 *          Entire comm_support library should be copied to target
 * 			project and only low level routines modified for target. 
 *
 *          - 20140610JC TODO: Bring return values inline with 'negative' being error, 0 being success, positive being success w/ information
 * ************************************************************************** */

#include "configCosmos.h"
#include "comm_support.h"

//! \addtogroup comm_support_functions
//! @{

// **************************************************************************
// **************************************************************************
// Platform Independent Code
// **************************************************************************
// **************************************************************************

// **************************************************************************
// Functions - High Level Frame Grabbing Finite State Machine Operations
// **************************************************************************
#define DEBUG_SLIP_FSM 0


// **************************************************************************
/*! Finite State Machine for Retrieving SLIP frames one byte at a time
 * Handles reception of SLIP frames one byte at a time
 * Redone 20130926JC: was having problems with previous FSM, planned out better for this iteration
    \param port_name  Use determined by low level comm support
    \param port_id    Use determined by low level comm support
    \param slip_frame_rx_fsm_variables FSM variables structure, init before use
    \param new_byte  New byte to process
    \return Number of bytes in frame, 0 if no frame completed
*/
int32_t comm_getslip_fsm_newbyte(slip_frame_rx_fsm_variables *fsm, uint8_t new_byte)
{
    // Check if frame buffer exists
    if (fsm->packet_buffer == NULL)
    {
        // Error: Frame buffer points to nowhere, exiting
        if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] Error: Null pointer to buffer\n"); fflush(stdout); }
        return 0;
    }

    if(fsm->frame_sync)
    {
        // Frame Synchronized
        if (new_byte == SLIP_FEND)
        {
            // Frame sync found, return position, expect outer routine to re-init FSM immediately
            if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] FEND MSG %d bytes\n", fsm->position); fflush(stdout); }
            return fsm->position;
        }
        else
        {
            // Non-FEND Byte found, check if there's more space available
            if (fsm->position >= fsm->max_data_size)
            {
                // Error: No more space, abort with full reset
                if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] Error: message exceeded buffer\n"); fflush(stdout); }
                comm_getslip_fsm_reset(fsm);
                return(0);
            }
            else
            {
                // Space avaialble, check if this is an escaped byte
                if (fsm->next_byte_escaped == true)
                {
                    // Yes, this byte is escaped, check which one
                    switch (new_byte)
                    {
                    case SLIP_TFESC:
                        // Escaped FESC
                        if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] TFESC\n"); fflush(stdout); }
                        fsm->packet_buffer[fsm->position++] = SLIP_FESC;
                        fsm->next_byte_escaped = false;
                        return 0;

                    case SLIP_TFEND:
                        // Escaped FEND
                        if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] TFEND\n"); fflush(stdout); }
                        fsm->packet_buffer[fsm->position++] = SLIP_FEND;
                        fsm->next_byte_escaped = false;
                        return 0;
                    default:
                        // Error: this should have been a transposed special value, abort with full reset
                        if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] Error: bad transposed value\n"); fflush(stdout); }
                        comm_getslip_fsm_reset(fsm);
                        return(0);
                    }
                }
                else
                {
                    // Is this an escape byte?
                    if (new_byte == SLIP_FESC)
                    {
                        // Yes, flag next as escaped and exit
                        fsm->next_byte_escaped = true;
                        if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] FESC\n"); fflush(stdout); }
                        return(0);
                    } else {
                        // No, this is a regular piece of data
                        fsm->packet_buffer[fsm->position++] = new_byte;
                        if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] D\n"); fflush(stdout); }
                        return(0);
                    }
                }

            }
        }
    }
    else
    {
        // Searching for frame sync
        if (new_byte == SLIP_FEND)
        {
            // Frame sync found, start frame
            if (DEBUG_SLIP_FSM) { printf("[SLIPFSM] FEND SOF\n"); fflush(stdout); }
            fsm->frame_sync = true;
            return(0);
        }
        return(0);
    }

    // No news to report
    return 0;
}



// **************************************************************************
/*! Initialize SLIP RX Finite State Machine Variables
 * Initializes FSM variables, run at program initialization
    \param fsm SLIP Receive FSM varaible structure
    \param max_data_size Size of frame buffer in bytes
    \param buf Pointer to frame buffer
*/
void comm_getslip_fsm_init(slip_frame_rx_fsm_variables *fsm, uint16_t max_data_size, uint8_t *buf)
{
    fsm->frame_sync = false;
    fsm->next_byte_escaped = false;
    fsm->max_data_size = max_data_size;
    fsm->position = 0;
    fsm->packet_buffer = buf;
}



// **************************************************************************
/*! Reset SLIP RX Finite State Machine Variables
 * Reset FSM variables, use whenever frame sync is lost (e.g. drops/overflows)
    \param fsm SLIP Receive FSM varaible structure
*/
void comm_getslip_fsm_reset(slip_frame_rx_fsm_variables *fsm)
{
    fsm->frame_sync = false;
    fsm->next_byte_escaped = false;
    fsm->position = 0;
}



// **************************************************************************
/*! Setup SLIP RX Finite State Machine Variables for next frame
 * Prep FSM to receive next frame, assuming still in sync
    \param fsm SLIP Receive FSM varaible structure
    \param buf Next uint8_t buffer to fill up with frame
    \return Return number of bytes in packet
*/
void comm_getslip_fsm_next_frame_init(slip_frame_rx_fsm_variables *fsm, uint8_t *buf)
{
    fsm->next_byte_escaped = false;
    fsm->position = 0;
    fsm->packet_buffer = buf;
}




// **************************************************************************
// Functions - Buffer Encode/Decode Functions
// **************************************************************************

// **************************************************************************
/*! Generate a SLIP frame for a given message data set
 * Generate a SLIP frame for a given message data set
 * 20130923JC: Verified with SUCHI Support Processor and Microchip XC32 Compiler
    \param msgbuf Source message bytes
    \param numbytes Number of bytes in msgbuf (doesnt check against max packet size, user responsible)
    \param slipbuf Destination SLIP buffer
    \param maxbytes Destination SLIP buffer limitation in bytes
        \return Returns number of bytes in SLIP frame, returns 0 on failure
*/
int32_t comm_slip_frame_data_to_buffer(const uint8_t *msgbuf, int16_t numbytes, uint8_t *slipbuf, int16_t maxbytes)
{
    uint16_t msg_pos; // Tracks source message buffer position
    uint16_t slip_pos; // Tracks destination SLIP frame position

    msg_pos = 0;
    slip_pos = 0;

    // Start with FEND
    slipbuf[slip_pos] = 0xC0;
    slip_pos++;

    for (; msg_pos<numbytes;)
    {
        // Check if there's space for a new character, plus stuffing, and the trailing FEND
        if (slip_pos >= (maxbytes - 2) )
        {
            // Error, ran out of space on destination buffer
            return 0; // Error, return zero bytes for transmission
        }

        // Condition based handling of next message byte
        switch (msgbuf[msg_pos])
        {
            case SLIP_FEND:
                slipbuf[slip_pos] = SLIP_FESC;
                slipbuf[slip_pos+1] = SLIP_TFEND;
                slip_pos += 2;
                msg_pos++;
                break;
            case SLIP_FESC:
                slipbuf[slip_pos] = SLIP_FESC;
                slipbuf[slip_pos+1] = SLIP_TFESC;
                slip_pos += 2;
                msg_pos++;
                break;
            default:
                slipbuf[slip_pos++] = msgbuf[msg_pos++];
                break;
        }
    }

    // End with FEND
    slipbuf[slip_pos++] = 0xC0;

    return (slip_pos);
}



// **************************************************************************
/*! Extract message data from SLIP frame
 * Extract message data from SLIP frame.  Works with and without FEND stripping.
 * Only capable of processing one frame at a time.
 * 20130923JC: Verified with SUCHI Support Processor and Microchip XC32 Compiler
    \param slipbuf Source SLIP frame bytes bytes
    \param numbytes Number of bytes in slipbuf
    \param msgpbuf Destination message buffer
    \param maxbytes Destination message buffer limitation in bytes
    \return Returns number of bytes in recovered message, returns 0 on failure
*/
int32_t comm_slip_unframe_data_to_buffer(const uint8_t *slipbuf, int16_t numbytes, uint8_t *msgbuf, int16_t maxbytes)
{

    uint16_t slip_pos; // Tracks source SLIP buffer position
    uint16_t msg_pos; // Tracks destination message position

    msg_pos=0;  // Initialize destination position

    // If beginning SLIP_FEND detected, skip it
    if (slipbuf[0] == SLIP_FEND)
    {
        slip_pos = 1; // Skipped
    } else {
        slip_pos = 0; // No FEND detected
    }

    // Start unstuffing data
    for ( ; slip_pos < numbytes; )
    {
        // Check for destination buffer full
        if (msg_pos >= maxbytes)
        {
            // Error, ran out of space on destination buffer
            return 0;
        }

        // Conditional processing of next character
        switch(slipbuf[slip_pos])
        {
            case SLIP_FESC: // SLIP Frame Escape Detected
                switch(slipbuf[slip_pos+1])
                {
                    case SLIP_TFESC: // Escaped Frame Escape Detected
                        msgbuf[msg_pos++] = SLIP_FESC;
                        break;
                    case SLIP_TFEND: // Escaped Frame Escape Detected
                        msgbuf[msg_pos++] = SLIP_FEND;
                        break;
                }
                slip_pos += 2; // Advance by 2 positions
                break;
            case SLIP_FEND: // End Frame Detected
                // Unstuff finished, return num bytes written to msgbuf
                return msg_pos;
                break;
            default:
                msgbuf[msg_pos++] = slipbuf[slip_pos++];
                break;
        }
    }

    // Unstuff finished, return num bytes written to msgbuf
    return msg_pos;
}



// **************************************************************************
/*! Setup packet header variables
 * Setup all packet header variables with one function.  Use only after
 * packet size is known.  Error if packet header location is null
    \param header Location of packet header
    \param src Source address (usually yourself)
    \param dst Destination address (target device address)
    \param type Packet Type (e.g. command, or device specific type)
    \param flags Flags to set (e.g. acknowledgement or no flags)
    \param seq If originator, locally tracked sequence number.  Else if
        responding, use original packet sequence number
    \param size Size of packet
	\return Returns 0 on success, negative error on failure
*/
int32_t comm_packet_header_setup(comm_packet_header* header, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint16_t size)
{
    // Sanity Checks
	if (header == NULL)
	{
		return PIC_ERROR_SIZE;
	}

    // Setup Header
    header->src = src;
    header->dst = dst;
    header->type = type;
    header->flags = flags;
    header->sequence_number = seq;
    header->data_size = size;

    // Return Success
	return 0;
}

// **************************************************************************
/*! Load HEC packet with optional data, setup header, and calculate CRC
 * Load HEC packet with optional data, setup header, and calculate CRC
 *  \param packet Location of packet
 *  \param src Source address (usually yourself)
 *  \param dst Destination address (target device address)
 *  \param type Packet Type (e.g. command, or device specific type)
 *  \param flags Flags to set (e.g. acknowledgement or no flags)
 *  \param seq If originator, locally tracked sequence number.  Else if
        responding, use original packet sequence number
 *  \param databuf optional field for
 *  \param datasize Size of optional data buffer
 *  \param truncate_ok Set to non-zero for allowing truncation of data, or set to 0 to error on oversized data
 *	\return Returns number of total packet bytes on success, negative error on failure
*/
int32_t comm_packet_load(comm_packet *packet, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint8_t *databuf, uint16_t datasize, uint8_t truncate_ok)
{
    uint16_t copylen;

    // Null pointer check
    if (packet == NULL) return COMM_ERROR_NULLPOINTER;

    // Set up header
    if ( comm_packet_header_setup(&packet->header, src, dst, type, flags, seq, datasize) < 0 ) return COMM_ERROR_OOPS;

    // If data provided, copy in data
    if (datasize > 0)
    {
        // Null pointer check
        if (databuf == NULL) return COMM_ERROR_NULLPOINTER;

        // Sanity check input data size
        if ( truncate_ok > 0)
        {
            copylen = ( datasize > COMM_PACKET_DATA_SIZE ) ? COMM_PACKET_DATA_SIZE : datasize;
        }
        else
        {
            if ( datasize > COMM_PACKET_DATA_SIZE ) return COMM_ERROR_OVERSIZE;
            copylen = datasize;
        }

        // Copy data into packet
        memcpy(&packet->packet.data[0], databuf, copylen);
    } else {
        copylen = 0;
    }

    // Set up CRC
    if ( comm_packet_crc_setup((uint8_t*)packet, COMM_PACKET_HEADER_SIZE+copylen) < 0 ) return COMM_ERROR_OOPS;

    // Debug
    if (0) printf("HEAD+DAT / COPYLEN: %d, %d\n", COMM_PACKET_HEADER_SIZE+datasize, copylen);

    // Return packet size
    return COMM_PACKET_OVERHEAD_SIZE+copylen;
}

// **************************************************************************
/*! Sets parameters for packet, as well as sets up CRC.  Used for pre-loaded packet.
 * Sets parameters for packet, as well as sets up CRC.  Used for pre-loaded packet or packet header/CRC updates
 *  \param packet Location of packet
 *  \param src Source address (usually yourself)
 *  \param dst Destination address (target device address)
 *  \param type Packet Type (e.g. command, or device specific type)
 *  \param flags Flags to set (e.g. acknowledgement or no flags)
 *  \param seq If originator, locally tracked sequence number.  Else if
        responding, use original packet sequence number
 *  \param datasize Size of data
 *	\return Returns number of total packet bytes on success, negative error on failure
*/
int32_t comm_packet_set(comm_packet *packet, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint16_t datasize)
{
    // Null pointer check
    if (packet == NULL) return COMM_ERROR_NULLPOINTER;

    // Set up header
    if ( comm_packet_header_setup(&packet->header, src, dst, type, flags, seq, datasize) < 0 ) return COMM_ERROR_OOPS;

    // Set up CRC
    if ( comm_packet_crc_setup((uint8_t*)packet, COMM_PACKET_HEADER_SIZE+datasize) < 0 ) return COMM_ERROR_OOPS;

    // Return packet size
    return COMM_PACKET_OVERHEAD_SIZE+datasize;
}

// **************************************************************************
/*! Check HEC packet for validity
 *  Check HEC packet for validity
    \param packet Location of packet buffer
    \param packetbytes Number of bytes in packet buffer
    \return Returns number of bytes in packet on success, negative error on failure
*/
int32_t comm_packet_check(comm_packet *packet, uint16_t packetbytes)
{
    int32_t iretn;

    // Null pointer check
    if (packet == NULL) return COMM_ERROR_NULLPOINTER;

    // Sanity check size
    if ( packetbytes > COMM_PACKET_MAX_SIZE ) return COMM_ERROR_OVERSIZE;
    if ( packetbytes < COMM_PACKET_OVERHEAD_SIZE ) return COMM_ERROR_UNDERSIZE;

    // Check if packet reported data size within bounds of packetbytes
    // (assume that there could be extra padding, due to encryption/encapsulation, at the end)
    if ( packet->header.data_size > (packetbytes-COMM_PACKET_OVERHEAD_SIZE) ) return COMM_ERROR_OVERSIZE;

    // Check CRC
    if ( ( iretn = comm_check_crc16((uint8_t*)packet, COMM_PACKET_OVERHEAD_SIZE+packet->header.data_size) ) < 0)
    {
        if (0) printf("CRC ERR %d\n",iretn);
        return COMM_ERROR_CRC;
    }

    // Return packet size
    return COMM_PACKET_OVERHEAD_SIZE+packet->header.data_size;
}

// **************************************************************************
/*! Checks packet and attempts to extract contents of packet payload
 * Checks packet and attempts to extract contents of packet payload
 *   \param packet Location of packet buffer
 *   \param packetbytes Number of bytes in packet buffer
 *   \param databuf Destination address to extract payload into
 *   \param maxlen Capacity of destination in # bytes
 *   \return Returns number of bytes in packet on success, negative error on failure
*/
int32_t comm_packet_unload(comm_packet *packet, uint16_t packetbytes, uint8_t *databuf, uint16_t maxlen)
{
    int32_t iretn;
    uint16_t copylen;

    // Null pointer check
    if (packet == NULL) return COMM_ERROR_NULLPOINTER;
    if (databuf == NULL) return COMM_ERROR_NULLPOINTER;

    // Run packet check
    if ( ( iretn = comm_packet_check(packet, packetbytes) ) < 0) return iretn;

    // Truncate copy to maxlen
    copylen = ((packet->header.data_size > maxlen) ? maxlen : packet->header.data_size);

    // Copy payload into provided data buffer
    memcpy(databuf, &packet->packet.data[0], copylen);

    return copylen;
}



// **************************************************************************
//! Set CRC16 on packet
/*! Set CRC16 on packet
        \param buf Pointer to packet start
        \param size Size of bytes in packet
	\return Returns 0 on success, negative error on failure
*/
int32_t comm_packet_crc_setup(uint8_t *buf, uint16_t size)
{
    uint16_t crc;

    // Sanity Check
	if (size > (32768-COMM_PACKET_TRAILER_SIZE) )
	{
		return PIC_ERROR_SIZE; // Max packet length is 32,768 per HEC v1.0
	}

    // Calc and append CRC
    crc = comm_calc_crc16(buf, size);
    memcpy(&buf[size], &crc, 2);

    // Debug
    if (0)  printf("CRC GEN: 0x%04x\n", crc);

	return 0; // Success
}



// **************************************************************************
// Functions - CRC Operations on Buffers
// **************************************************************************

// **************************************************************************
//! Check comm frame CRC
/*! Compare a comm frame's reported CRC its calculated CRC (SLIP stripped & unstuffed)
        \param sbuf Comm packet location
        \param packet_size Comm packet size, including CRC bytes
        \return Returns COMM_ERROR_CRC on error, returns 0 on success
*/
int32_t comm_check_crc16(uint8_t *sbuf, uint16_t packet_size)
{
    uint16_t crc, crc2;

    // Debug
    if (0)  printf("CRC COVERAGE: %u\n", packet_size);

    // Grab stored CRC from packet
    crc = comm_get_crc16((uint8_t*)sbuf,(uint16_t)packet_size-2);
    if (0)  printf("CRC GET: 0x%04x\n", crc);

    // Calculate CRC from packet
    crc2 = comm_calc_crc16((uint8_t*)sbuf,(uint16_t)packet_size-2);
    if (0)  printf("CRC CALC: 0x%04x\n", crc2);

    // Compare stored CRC from packet versus calculated packet CRC
    if (crc != crc2)
            return (COMM_ERROR_CRC);
    return 0;
}



// **************************************************************************
//! Calculate CRC16 on Buffer
/*! Calculate CRC16 on Buffer
        \param buf Data to calculate CRC on
        \param size Number of bytes to calculate a CRC on (for comm use, usually header+data bytes count)
        \return Returns CRC
*/
uint16_t comm_calc_crc16(uint8_t *buf, uint16_t size)
{
    uint16_t crc = 0xffff;
    uint8_t ch;
    int i, j;

    // Calculate CRC-16 across (size) bytes starting at buffer pointer
    for (i=0; i<size; ++i)
    {
        ch = buf[i];
        for (j=0; j<8; j++)
        {
            crc = (crc >> 1)^(((ch^crc)&0x01)?0x8408:0);
            ch >>= 1;
        }
    }

    return (crc);
}



// **************************************************************************
//! Get CRC16 from comm frame
/*! Get CRC16 from comm frame
        \param buf Pointer to comm frame
        \param size Location of stored CRC  (for comm use, usually at address of packet offset by the size of header+data bytes without CRC included)
        \return Returns CRC
*/
uint16_t comm_get_crc16(uint8_t *buf, uint16_t index)
{
    uint16_t crc;

    // Grab the two CRC-16 bytes and return value
    memcpy(&crc,&buf[index],2);

    return (crc);
}

//! @}

// **************************************************************************
// **************************************************************************
// Low Level Routines (Requires implementation of comm_support_lowlevel.c)
// **************************************************************************
// **************************************************************************
#ifdef COMM_SUP_LOWLEVEL_ENABLE

// **************************************************************************
// Functions - Raw Write Operations
// **************************************************************************

// **************************************************************************
//! Streams data buffer out serial port without modification
/*! Streams data buffer out serial port without modification
 * WARNING: Requires correct implementation of comm_support_lowlevel.c functions.
 * WARNING2: Does not check for available buffer space on transmitting interface
 * Returns number of characters transmitted
        \param port_name For use by low level routines
        \param port_id For use by low level routines
        \param buf Pointer to buffer with source data
        \param size Size of source data
        \return # of bytes read, up to maximum,  <0 for error (e.g. -1)
*/
int32_t comm_send_raw(const char *port_name, int port_id, uint8_t *buf, uint16_t size)
{
    uint16_t i;
    int32_t iretn;

    for (i = 0; i < size; i++)
    {
        if ( (iretn = comm_ll_putchar(port_name, port_id, buf[i]) < 0) )
        {
            // Return error code
            return iretn;
        }
    }

    // Return number of bytes sent
    return i;
}


// **************************************************************************
// Functions - Frame Write Operations
// **************************************************************************

// **************************************************************************
//! Streams data buffer out serial port with SLIP framing
/*! Streams data buffer out serial port with SLIP framing
 * WARNING: Requires correct implementation of comm_support_lowlevel.c functions.
 * WARNING2: Does not check for available buffer space on transmitting interface
 * Returns number of characters transmitted
        \param port_name For use by low level routines
        \param port_id For use by low level routines
        \param buf Pointer to buffer with source data
        \param size Size of source data
        \return # of bytes written, up to maximum,  <0 for error (e.g. -1)
*/
int32_t comm_putslip(const char *port_name, int port_id, uint8_t *buf, uint16_t size)
{
    uint16_t i, j;

    i=0;  // Initialize num chars transmitted via serial port

    // Sanity check: no packet allowed greater than max packet size
    if (size > COMM_PACKET_MAX_SIZE)
    {
        return COMM_ERROR_OVERSIZE;
    }

    // Sanity check: no packet allowed with zero size
    if (size == 0)
    {
        return COMM_ERROR_ZEROSIZE;
    }

    // 1. Send FEND (0xC0) byte to start frame
    comm_ll_putchar(port_name, port_id, SLIP_FEND);
    i++;

    // 2. Send each byte, checking for byte escaping needs for each
    for (j=0; j<size; j++)
    {
        switch (buf[j])
        {
            case SLIP_FEND:  // Escape FENDs
                comm_ll_putchar(port_name, port_id, SLIP_FESC);
                comm_ll_putchar(port_name, port_id, SLIP_TFEND);
                i+=2;
                break;
            case SLIP_FESC:
                comm_ll_putchar(port_name, port_id, SLIP_FESC);
                comm_ll_putchar(port_name, port_id, SLIP_TFESC);
                i+=2;
                break;
            default:
                comm_ll_putchar(port_name, port_id, buf[j]);
                i++;
                break;
        }
    }

    // 3. Send FEND (0xC0) byte to end frame
    comm_ll_putchar(port_name, port_id, SLIP_FEND);
    i++;

    // 4. Return number of bytes sent
    return (i);
}



// **************************************************************************
//! Prepares a HEC packet of arbitrary type, loads in variable length string data, and passes data to comm_putslip for direct serial write
/*! Prepares a HEC packet of arbitrary type, loads in variable length string data, and passes data to comm_putslip for direct serial write
 * WARNING: Requires correct implementation of comm_support_lowlevel.c functions.
 * Returns number of characters transmitted
        \param port_name For use by low level routines
        \param port_id For use by low level routines
        \param src HEC packet source
        \param dst HEC packet destination
        \param type HEC packet type
        \param flags HEC packet flags
        \param seq HEC packet sequence number
        \param string_data Pointer to null terminated string buffer of type 'char'.  Null is transmitted to allow for easy use as string on receiver.
        \return # of bytes written to low level serial port write, <0 for error (e.g. -1)
*/
int32_t comm_send_packet_varlen_string(const char *port_name, int port_id, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, char *string_data)
{
    int32_t iretn;
    comm_packet packet;
    int16_t numbytes;
    numbytes = strlen(string_data);

    // Sanity check, no packet allowed greater than max packet size
    if (numbytes + COMM_PACKET_OVERHEAD_SIZE > COMM_PACKET_MAX_SIZE || numbytes <= 0)
    {
        return COMM_ERROR_OVERSIZE; // Error, oversized
    }
    else
    {
        comm_packet_header_setup(&packet.header, src, dst, type, flags, seq, numbytes);                     // 1. Setup Header, Sending From 0x10 to Device 0x80, no flags
        memcpy(&packet.raw[COMM_TX_PKT_DATA_POSITION_OFFSET], string_data, numbytes+1);                     // 2. Copy in string data, null included (allows to be used as string on receiver)
        comm_packet_crc_setup(&packet.raw[0], COMM_PACKET_HEADER_SIZE+numbytes);                            // 3. Calculate CRC and Append
        iretn = comm_putslip(port_name, port_id, &packet.raw[0], COMM_PACKET_OVERHEAD_SIZE+numbytes);       // 4. Stream SLIP to Serial Port
        return iretn;
    }
}



// **************************************************************************
//! Prepares a HEC packet of arbitrary type, loads in variable length binary data, and passes data to comm_putslip for direct serial write
/*! Prepares a HEC packet of arbitrary type, loads in variable length binary data, and passes data to comm_putslip for direct serial write
 * WARNING: Requires correct implementation of comm_support_lowlevel.c functions.
 * Returns number of characters transmitted
        \param port_name For use by low level routines
        \param port_id For use by low level routines
        \param src HEC packet source
        \param dst HEC packet destination
        \param type HEC packet type
        \param flags HEC packet flags
        \param seq HEC packet sequence number
        \param binary_data Pointer to arbitrary binary buffer of type uint8_t
        \param numbytes Number of bytes in binary buffer
        \return # of bytes written to low level serial port write, <0 for error (e.g. -1)
*/
int32_t comm_send_packet(const char *port_name, int port_id, uint8_t src, uint8_t dst, uint8_t type, uint8_t flags, uint16_t seq, uint8_t *binary_data, uint16_t numbytes)
{
    int32_t iretn;
    comm_packet packet;

    // Sanity check, no packet allowed greater than max packet size
    if (numbytes + COMM_PACKET_OVERHEAD_SIZE > COMM_PACKET_MAX_SIZE)
    {
        return COMM_ERROR_OVERSIZE; // Error, oversized
    }
    else
    {
        comm_packet_header_setup(&packet.header, src, dst, type, flags, seq, numbytes);                 // 1. Setup Header, Sending From 0x10 to Device 0x80, no flags
        if (numbytes > 0) memcpy(&packet.raw[COMM_TX_PKT_DATA_POSITION_OFFSET], binary_data, numbytes);                   // 2. Copy in binary data
        comm_packet_crc_setup(&packet.raw[0], COMM_PACKET_HEADER_SIZE+numbytes);                        // 3. Calculate CRC and Append
        iretn = comm_putslip(port_name, port_id, &packet.raw[0], COMM_PACKET_OVERHEAD_SIZE+numbytes);   // 4. Stream SLIP to Serial Port
        return iretn;
    }
}

// **************************************************************************
// Functions - Frame Read Operations
// **************************************************************************

// **************************************************************************
// Returns positive for command type, negative for error
int16_t comm_packet_check(const char *port_name, int port_id, comm_packet *packetbuf, slip_frame_rx_fsm_variables *fsm_info)
{
    uint16_t crc_calc, crc_rx;
    uint8_t newbyte;

    // Command Interface
    // 1. Check for new bytes on UART, execute this code if new commands found
    while (comm_ll_data_available(port_name, port_id))
    {
        // 2. Get new byte
        newbyte = comm_ll_getchar_nonblocking(port_name, port_id);

        // 3. See if the new byte results in a new frame
        if ( ( packetbuf->size = comm_getslip_fsm_newbyte( fsm_info, newbyte) ) )
        {

            // 4. Found Frame, Check if frame is A. a reasonable size, B. less than the max frame size, and C. the correct size
            if (packetbuf->size >= 10 &&               // Header+CRC = 10 Bytes Minimium
                (packetbuf->header.data_size + 10) <= COMM_PACKET_MAX_SIZE &&  // Check to make sure packet does not specify size as larger than max frame size
                packetbuf->size == (COMM_PACKET_HEADER_SIZE + packetbuf->header.data_size + 2) )  // Check to make sure size of received buffer is equal to packet specified size
            {
                // 5. Sanity checks pass, successfully received reasonable amount of frame bytes

                // 6. CRC Check, Assumption is that CRC is located at received buffer - 2 bytes
                crc_calc = comm_calc_crc16(&packetbuf->raw[0], COMM_RX_PKT_CRC_COVERAGE(packetbuf->size));
                crc_rx =   comm_get_crc16(&packetbuf->raw[0], COMM_RX_PKT_CRC_POSITION_OFFSET(packetbuf->size));

                // 7. Reset FSM for next packet
                // Setup FSM for next frame
                comm_getslip_fsm_next_frame_init(fsm_info, packetbuf->raw);  // use next frame init function

                // 7. Report Results

                // For EPS demo only: Process command here
                if(crc_calc == crc_rx)
                {
                    // 8. CRC good, process command (update 20140408JC: Process command externally, use it or lose it.)
                    // Return packet size
                    return packetbuf->size;
                }
                else
                {
                    // If CRC wasn't good, ignore packet.

                    // mod: respond with NACK
                    comm_send_packet(port_name, port_id, packetbuf->header.dst, packetbuf->header.src, packetbuf->header.type, COMM_FLAG_NACK, packetbuf->header.sequence_number, NULL, 0);

                    return COMM_ERROR_CRC;
                }
            } else {
                // Failed sanity checks

                // Missing bytes from frame, out of sync
                comm_getslip_fsm_reset(fsm_info);

                 // Only gets here if the former is a FEND, and we got a runt, FSM handles overflows
                packetbuf->header.data_size = 0;
                packetbuf->size = 0;

                if ( !(packetbuf->size >= 10) ) return COMM_ERROR_UNDERSIZE;
                else if ( (!(packetbuf->header.data_size + 10) <= COMM_PACKET_MAX_SIZE ) ) return COMM_ERROR_OVERSIZE;
                else if ( !(packetbuf->size == (COMM_PACKET_HEADER_SIZE + packetbuf->header.data_size + 2) ) ) return COMM_ERROR_EMPTY;
                else return COMM_ERROR_UNHANDLED_ERR;


                // Report the discrepancy
            } // End Received Frame Processing
        }  // End UART RX FSM Run
    } // End UART Receive Processing

    return COMM_ERROR_EMPTY;
}

#endif





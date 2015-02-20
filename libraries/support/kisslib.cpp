#include "configCosmos.h"
#include "kisslib.h"
//#include <stdio.h>
//#include <iomanip>

kissHandle::kissHandle( int port, int comm, const char dest_call[], char dest_stat, const char sour_call[], char sour_stat, char cont, char prot)
: port_number(port), command(comm), destination_stationID(dest_stat), source_stationID(sour_stat), control(cont), protocolID(prot)
{
	strcpy((char*)destination_callsign, (const char*)dest_call);
	strcpy((char*)source_callsign, (const char*)sour_call);
}

kissHandle::kissHandle() : port_number(1), command(0), destination_stationID('\x60'), source_stationID('\x61'), control('\x31'), protocolID('\xF0')
{
	destination_callsign[0]='\0';
	source_callsign[0]='\0';
}

//Set and get functions for all members of the kissHandle class
void kissHandle::set_port_number(unsigned int P)
{
	port_number = P;
	return;
}

unsigned int kissHandle::get_port_number()
{
	return port_number;
}

void kissHandle::set_command(unsigned int C)
{
	command = C;
	return;	
}

unsigned int kissHandle::get_command()
{
	return command;

}

void kissHandle::set_destination_callsign(const char destination[])
{ 
	strcpy((char*)destination_callsign, (const char*) destination);
	return;
}

unsigned char* kissHandle::get_destination_callsign()
{
	return destination_callsign;
}

void kissHandle::set_destination_stationID(unsigned int ID)
{
	destination_stationID = ID;
	return;
}

unsigned int kissHandle::get_destination_stationID()
{
	return destination_stationID;
}

void kissHandle::set_source_callsign(const char source[])
{
	strcpy((char*)source_callsign, (const char*) source);
	return; 
}

unsigned char* kissHandle::get_source_callsign()
{
	return source_callsign;
}

void kissHandle::set_source_stationID(unsigned int ID)
{
	source_stationID = ID;
	return;
}

unsigned int kissHandle::get_source_stationID()
{
	return source_stationID;
}

void kissHandle::set_control(unsigned int control_number)
{
	control = control_number;
	return;
}

unsigned int kissHandle::get_control()
{
	return control;
}

void kissHandle::set_protocolID(unsigned int protocol)
{
	protocolID = protocol;
	return;
}

unsigned int kissHandle::get_protocolID()
{
	return protocolID;
}

ostream& operator<< (ostream& out, kissHandle& K)	{
	out<<"port_number= "<< K.port_number<<endl;
	out<<"command="<< K.command<<endl;
	out<<"destination callsign=<"<< K.destination_callsign<<">"<<endl;
	out<<"destination station ID="<< K.destination_stationID<<endl;
	out<<"source callsign=<"<< K.source_callsign<<">"<<endl;
	out<<"source station ID="<< K.source_stationID<<endl;
	out<<"control="<< K.control<<endl;
	out<<"protocol ID="<< K.protocolID<<endl;

	return out;
}

// Encodes input (using KISS protocol) into a packet ready for transmission
// returns bytes in payload of packet



int kissEncode(uint8_t *input, uint32_t count, uint8_t *encoded_packet)
{
	uint32_t payload_bytes = 0;

	//If payload is over 255 bytes indicate an error
	if(count > 255)
	{
		printf("Error: Limit input to 255 8-bit character bytes\n");
		return -1;
	}

//Build Packet Header

	//Frame End (1 Byte)
	*encoded_packet = FEND;
	//Port number and command type (1Byte)
	*(encoded_packet+1) = 0x10;
	//Destination call sign(6Bytes)
	*(encoded_packet+2) = ('W' << 1);
	*(encoded_packet+3) = ('H' << 1);
	*(encoded_packet+4) = ('7' << 1);
	*(encoded_packet+5) = ('L' << 1);
	*(encoded_packet+6) = ('G' << 1);
	*(encoded_packet+7) = (0x20 << 1);
	//Destination station ID (1Byte)
	*(encoded_packet+8) = 0X60;
	//Source call sign (6Bytes)
	*(encoded_packet+9) = ('W' << 1);
	*(encoded_packet+10) = ('H' << 1);
	*(encoded_packet+11) = ('7' << 1);
	*(encoded_packet+12) = ('L' << 1);
	*(encoded_packet+13) = ('E' << 1);
	*(encoded_packet+14) = (0x20 << 1);
	//Source station ID (1Byte)
	*(encoded_packet+15) = 0x61;
	//Control(1Byte)
	*(encoded_packet+16) = 0X31;
	//Protocol ID (1Byte)
	*(encoded_packet+17) = 0xF0; // 0xF0 = No layer 3 protocol

	encoded_packet += 18;

	for(uint32_t i=0; i<count; i++)
	{
		// Replace FEND with FESC TFEND
		if (input[i] == FEND)
		{
			*encoded_packet++ = FESC;
			*encoded_packet++ = TFEND;
			payload_bytes += 2;	// Advance Payload Data Count 2 Bytes
		} 
		// Replace FESC with FESC TFESC
		else if (input[i] == FESC)
		{
			*encoded_packet++ = FESC;
			*encoded_packet++ = TFESC;
			payload_bytes += 2;	// Advance Payload Data Count 2 Bytes
		}
		// Copy Input
		else
		{
			*encoded_packet++ = input[i];
			payload_bytes++;	// Advance Payload Data Count 1 Byte
		}
	}

	//Placing final frame-end
	*encoded_packet= FEND;

	//Keeping track of the count (payload bytes + header bytes + trailer)
	payload_bytes += 19;

	return payload_bytes;
}



//*************New kissEncode Starts Here*********************************************************************************************************************************************************************************************************************************

int kissEncode(uint8_t* input, uint32_t count, uint8_t* encoded_packet, kissHandle* handle)
{
	uint32_t payload_bytes = 0;

	//If payload is over 255 bytes indicate an error
	if(count > 255)
	{
		printf("Error: Limit input to 255 8-bit character bytes\n");
		return -1;
	}

//Build Packet Header

	//Frame End (1 Byte)
	*encoded_packet = FEND;
	
	//Port number and command type (1Byte)
	*(encoded_packet+1) = 0x10;
	
	//Destination call sign(6Bytes)
	unsigned char dest_callsign[6];
	strcpy((char*) dest_callsign, (const char*)handle->get_destination_callsign());
	*(encoded_packet+2) =  (dest_callsign[0]) << 1;
	*(encoded_packet+3) =  (dest_callsign[1]) << 1;
	*(encoded_packet+4) =  (dest_callsign[2]) << 1;
	*(encoded_packet+5) =  (dest_callsign[3]) << 1;
	*(encoded_packet+6) =  (dest_callsign[4]) << 1;
	*(encoded_packet+7) =  (0x20) << 1;
    	
	//Destination station ID (1Byte)
	*(encoded_packet+8) = handle->get_destination_stationID();
	
	//Source call sign (6Bytes)
	unsigned char src_callsign[6];
	strcpy((char*) src_callsign, (const char*) handle->get_source_callsign());
	*(encoded_packet+9)  =  (src_callsign[0]) << 1;
	*(encoded_packet+10) =  (src_callsign[1]) << 1;
	*(encoded_packet+11) =  (src_callsign[2]) << 1;
	*(encoded_packet+12) =  (src_callsign[3]) << 1;
	*(encoded_packet+13) =  (src_callsign[4]) << 1;
	*(encoded_packet+14) =  (0x20) << 1;

	//Source station ID (1Byte)
	*(encoded_packet+15) = handle->get_source_stationID();

	//Control(1Byte)
	*(encoded_packet+16) = handle->get_control();

	//Protocol ID (1Byte)
	*(encoded_packet+17) = handle->get_protocolID(); // 0xF0 = No layer 3 protocol

	encoded_packet += 18;

	for(uint32_t i=0; i<count; i++)
	{
		// Replace FEND with FESC TFEND
		if (input[i] == FEND)
		{
			*encoded_packet++ = FESC;
			*encoded_packet++ = TFEND;
			payload_bytes += 2;	// Advance Payload Data Count 2 Bytes
		} 
		// Replace FESC with FESC TFESC
		else if (input[i] == FESC)
		{
			*encoded_packet++ = FESC;
			*encoded_packet++ = TFESC;
			payload_bytes += 2;	// Advance Payload Data Count 2 Bytes
		}
		// Copy Input
		else
		{
			*encoded_packet++ = input[i];
			payload_bytes++;	// Advance Payload Data Count 1 Byte
		}
	}

	//Placing final frame-end
	*encoded_packet= FEND;

	//Keeping track of the count (payload bytes + header bytes + trailer)
	payload_bytes += 19;

	return payload_bytes;
}
//***********New kissEncode Ends Here***********************************************************************************************************************************************************************************************************************************

kissHandle kissInspect(const unsigned char* input)	{

	//fill me up with goodness
	kissHandle KKK;

//	uint8_t port_command;
	unsigned char destination_call_sign[6];
	uint8_t destination_station_id;//Changed this to an array of [1]
	unsigned char source_call_sign[6];
	uint8_t source_station_id;//changed this to an array of [1]
	uint8_t control_id;//changed this to an array of [1]
	uint8_t protocol_id;//changed this to an array of [1]

	
	// First character of input is always FEND
	// The next 17 characters we are interested in...
//	port_command = input[1];
	destination_station_id = input[8];
	source_station_id = input[15];
	control_id = input[16];
	protocol_id = input[17];
	
		
	//Extract destination call sign
	//printf("Destination Call Sign = ");
	for(int i=2;i<=7;i++)
	{
		destination_call_sign[i-2] = input[i] >> 1;
		//printf("%c", destination_call_sign[i-2]);
	}
	//Copying destination callsign into kissHandle
	KKK.set_destination_callsign((const char*) destination_call_sign);
	//strcpy((char *)KKK.destination_callsign, (const char *) destination_call_sign);
	
	//Copying destination callsign ID into kissHandle
	KKK.set_destination_stationID((unsigned int) destination_station_id);
	//KKK.destination_stationID = destination_station_id;

	//printf("\n");
	
	//Extract source call sign
	//printf("Source Call Sign = ");
	for(int i=9;i<=14;i++)
	{
		source_call_sign[i-9] = input[i] >> 1;
		//printf("%c", source_call_sign[i-9]);
	}
	//Copying source callsign into kissHandle
	KKK.set_source_callsign((const char*) source_call_sign);
	//strcpy((char *)KKK.source_callsign, (const char *) source_call_sign);
	
	//Copying source station ID into kissHandle
	KKK.set_source_stationID((unsigned int) source_station_id);
	//KKK.source_stationID = source_station_id;
	
	//Copying control into kissHandle
	KKK.set_control((unsigned int) control_id);
	//KKK.control = control_id;

	//Copying protocol ID into kissHandle
	KKK.set_protocolID((unsigned int) protocol_id);
	//KKK.protocolID = protocol_id;
	
	
	//printf("\n");

	//printf("control id: %d", control_id[0]);
	//printf("protocol id: %d", protocol_id[0]);

	return KKK;
}

// Decodes input (using KISS protocol) into payload
// returns bytes in payload of packet
/*int kissDecode(uint8_t* input, uint32_t count, uint8_t* decoded_payload)
{
	uint32_t i = 0;
	uint32_t j = 0;
	uint8_t intermediate_packet[PACKETMAX];
	//uint8_t port_command[10];
	uint8_t destination_call[10];
	//uint8_t dest_stationID[10];
	uint8_t source_call[10];
	//uint8_t sour_stationID[10];
	//uint8_t control[10];
	//uint8_t protocolID[10];

	//Once kissDecode has been passed the point to the first element of KISS packet
	//it should begin removing stuff
	//port_command[0] = *(input+1);
	//printf("port_command = %x\n", port_command[0]);

	j=0;
	printf("Destination Call Sign = ");
	for(i=2;i<=7;i++)
	{
	 destination_call[j] = input[i] >> 1;
	 printf("%c", destination_call[j]);
	 j++;
	}
	//printf("\n");

	//dest_stationID[0] = *(input+8);
	//printf("Destination Station ID = %x\n", dest_stationID[0]);

	i=0;
	j=0;

	printf("Source Call Sign = ");
	for(i=9;i<=14;i++)
	{
	 source_call[j] = input[i] >> 1;
	 printf("%c", source_call[j]);
	 j++;
	}
	//printf("\n");

	//sour_stationID[0] = *(input + 15);
	//printf("Source Station ID = %x\n", sour_stationID[0]);
	//control[0] = *(input + 16);
	//printf("Control = %x\n", control[0]);
	//protocolID[0] = *(input + 17);
	//printf("Protocol ID = %x\n", protocolID[0]);

	//Make one buffer to read in all the data
	for(i=18; input[i] != FEND; i++)	// FEND = FEND
	{
	 j = i - 18; // pointer to 0 in intermediate
	 intermediate_packet[j] = input[i];
	}
	intermediate_packet[j+1]=FEND;

	//check the read in data
	i = 0;
	j = 0;
	for(;j <= PACKETMAX;)
	{
	 if(intermediate_packet[j] == FEND)
		 return i;

	 if(intermediate_packet[j] == FESC) //if FESC
	 {
		 if(intermediate_packet[j+1] == TFEND) // Check for TFEND || intermediate_packet[j+1] == TFESC) //if transp
		 {
			intermediate_packet[j] = FEND;
			decoded_payload[i] = intermediate_packet[j];
			j+=2;
			i++;
		 }
		 else if(intermediate_packet[j+1] == TFESC)	// Check for TFESC
		 {
			intermediate_packet[j] = FESC;
			decoded_payload[i] = intermediate_packet[j];
			j+=2;
			i++;
		 }
		 else
		 {
			printf("Invalid Transpose Error!!!\n");
		 }
	 }
	 else
	{
	 	decoded_payload[i] = intermediate_packet[j];
	 	j++;
	 	i++;
	 }
	}
	return 0;
}*/


//**************************************************************************************************************New kissDecode Start Here!!***********************************************************************************************************************************************************************************************************************************************


// Decodes input (using KISS protocol) into payload
// returns bytes in payload of packet
int kissDecode(uint8_t* kissed_input, uint32_t, uint8_t* decoded_payload)
{
	uint32_t i = 0;
	uint32_t j = 0;
	uint8_t encoded_payload[PACKETMAX];
	
	///Encoded payload will act as an intermediate packet holding the encoded data.
	//It will be used to hold the encoded data while being decoding.
	for(i=18; kissed_input[i] != FEND; i++)	// FEND = FEND
	{
		j = i - 18; // pointer to 0 in intermediate
		encoded_payload[j] = kissed_input[i];
	}
	
	///Frame end appended to packet after all the header is removed.
	//We do this to pinpoint where we can stop the decoding process.
	encoded_payload[j+1]=FEND;

	
	//check the read in data
	i = 0;
	j = 0;
	
	for(;j <= PACKETMAX;)
	{
	 	//Function will return i once the appended FEND is read in
		if(encoded_payload[j] == FEND)
		 	return i;

	 	if(encoded_payload[j] == FESC) //if FESC
	 	{
		 	if(encoded_payload[j+1] == TFEND) // Check for TFEND
		 	{
				encoded_payload[j] = FEND;
				decoded_payload[i] = encoded_payload[j];
				j+=2;
				i++;
		 	}	
		 
		 	else if(encoded_payload[j+1] == TFESC)	// Check for TFESC
		 	{	
				encoded_payload[j] = FESC;
				decoded_payload[i] = encoded_payload[j];
				j+=2;
				i++;
		 	}
		 
		 	else
		 	{
				printf("Invalid Transpose Error!!!\n"); //Error occurred if TFEND or TFESC does not follow a FESC
		 	}
	 	}
	 
	 	else
	 	{
	 		decoded_payload[i] = encoded_payload[j];
	 		j++;
	 		i++;
	 	}
	}
	
	return 0;
}
//***********************New kissDecode Ends Here*****************************************************************************************************************************************************************************************************************************************************


//Packet printing functions

void print_ascii(unsigned char* packet, unsigned int count)
{

	cout<<"<";
	for(unsigned int i=0; i<count; i++)
		printf("%c", packet[i]);	
	cout<<">"<<endl;

	return;
}


void print_hex(unsigned char* packet, unsigned int count)
{
	cout<<"<";
	for(unsigned int i=0; i<count; i++)
		printf("%02x", packet[i]);	
	cout<<">"<<endl;

	return;
}

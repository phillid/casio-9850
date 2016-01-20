#ifndef PACKET_H
#define PACKET_H

#define MISC_ACK				0x06

/*#define REQUEST_TYPE_VAR		0x4D56 // VM
#define REQUEST_TYPE_PICTURE	0x4350 // PC
#define REQUEST_TYPE_LIST		0x544C // LT
#define REQUEST_TYPE_MATRIX		0x544D // MT
*/

#define PKT_HEADER_MAX_SIZE		50


enum packet_types
{
	PKT_END,
	PKT_VALUE_HEADER,
	PKT_SCREEN_HEADER_BW,
	PKT_SCREEN_HEADER_COL,
	PKT_BACKUP_HEADER,
	PKT_PROGRAM_HEADER,
	PKT_FUNCTION_HEADER,
	PKT_VAL,
	PKT_UNKNOWN
};

unsigned char calculate_checksum(const char *start, const char *stop);
unsigned short calculate_checksum_word(const char *start, const char *stop);
enum packet_types get_packet_type(const unsigned char* data);

#endif

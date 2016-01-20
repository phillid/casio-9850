#include "casio-get.h"
#include <string.h>


/*****************************************************************************
 * Calculate the checksum of the data area of a packet
 *****************************************************************************/
unsigned char calculate_checksum(const char *start, const char *stop)
{
	unsigned char sum = 0;
	while (start != stop)
		sum += *(start++);

	return 1 + ~(sum);
}


unsigned short calculate_checksum_word(const char *start, const char *stop)
{
	unsigned short sum = 0;
	while (start != stop)
		sum += (unsigned char)(*(start++));

	return 1 + ~(sum);
}

/*****************************************************************************
 * Returns the type of packet as determined from the packet header
 *****************************************************************************/
enum packet_types get_packet_type(const unsigned char* data)
{
	if (strncmp((char*)data, ":END", 4) == 0)
		return PKT_END;
	if (strncmp((char*)data, ":VAL", 4) == 0)
		return PKT_VALUE_HEADER;
	if (strncmp((char*)data, ":DD", 3) == 0)
		return PKT_SCREEN_HEADER_BW;
	if (strncmp((char*)data, ":DC", 3) == 0)
		return PKT_SCREEN_HEADER_COL;
	if (strncmp((char*)data, ":MEM", 4) == 0)
		return PKT_BACKUP_HEADER;
	if (strncmp((char*)data, ":FNC", 4) == 0)
		return PKT_FUNCTION_HEADER;
	if (strncmp((char*)data, ":TXT", 4) == 0)
		return PKT_PROGRAM_HEADER;
	if (strncmp((char*)data, ":\x00\x01\x00\x01", 4) == 0)
		return PKT_VAL;

	return PKT_UNKNOWN;
}

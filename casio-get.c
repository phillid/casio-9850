#include "casio-get.h"

#define SMALL_PACKET_WAIT 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_checksum_msg(unsigned char expected, unsigned short actual)
{
	fprintf(stderr, "Checksum\t: %s (needed 0x%x, found 0x%x)\n", expected==actual? "OK" : "BAD", expected, actual);
}

int main()
{
	unsigned int read;
	unsigned char temp_buffer[1024];
	unsigned char rx_buffer[65536];

	setbuf(stdout, NULL);
	memset(rx_buffer, 0, sizeof(rx_buffer));

	long expected;
	unsigned int x,y;
	
	unsigned char colours = 3;
	unsigned char colour;
	unsigned int i;
	struct sp_port *port;
	port = port_init("/dev/ttyUSB0");

	// Wait for calculator and ACK
	wait_calc(port);
	send_calc_byte(port, ATTENTION_ACK);
	fprintf(stderr,"Sent 0x%x (ACK)\n",ATTENTION_ACK);



	/*******************************************
	 * Receive header packet
	 *******************************************/
	read = calc_read(port, rx_buffer, PKT_HEADER_MAX_SIZE);
	fprintf(stderr, "\n== HEADER ========================================\n");
	fprintf(stderr, "Size (bytes)\t: %d\n", read);
	
	unsigned char sum_expected = rx_buffer[read-1];
	unsigned char sum = calculate_checksum((const char*)rx_buffer+1, (const char*)rx_buffer+read-1);

	print_checksum_msg(sum_expected, sum);

	for(i = 0; i < read; i++)
		printf("%c", (unsigned char)rx_buffer[i]);

	colours = 3;

	switch (get_packet_type(rx_buffer))
	{
		/* Screenshot (F<->D button) */
		case PKT_SCREEN_HEADER_BW:
			colours = 1;
		case PKT_SCREEN_HEADER_COL:
			send_calc_byte(port, MISC_ACK);
			x = rx_buffer[4];
			y = rx_buffer[3];
			expected = (x*y)/8 + 2;
			
			/* multi-colour images have an extra byte per colour frame for colour identification */
			if (colours > 1)
				expected++;
			fprintf(stderr,
					"Data type\t: Screen dump\n"
					"Dimensions\t: %d x %d\n", x,y);

			fprintf(stderr, "\n== DATA ==========================================\n");
			
			for (colour = 0; colour < colours; colour++)
			{
				read = calc_read(port, rx_buffer, expected);
				fprintf(stderr,"Colour %d/%d read\t: %d bytes\n", colour+1, colours, read);
				for(i = 0; i < read; i++)
					printf("%c", rx_buffer[i]);
				
				sum_expected = rx_buffer[read-1];
				sum = calculate_checksum((const char*)rx_buffer+1+(colours > 1), (const char*)rx_buffer+read-1);
				print_checksum_msg(sum_expected, sum);
				
				send_calc_byte(port, MISC_ACK);
			
			}
			break;
		
		/* Full memory backup */
		case PKT_BACKUP_HEADER:
			/* Get firmware name (starts at byte 27 and is hival-terminated */
			i = 27;
			while((rx_buffer[i] != 0xff) && i < sizeof(temp_buffer))
			{
				temp_buffer[i-27] = rx_buffer[i];
				i++;
			}
			temp_buffer[i-27] = '\0';

			fprintf(stderr, "Memory backup coming (firmware %s)...\n", temp_buffer);
			break;
		case PKT_FUNCTION_HEADER:
			fprintf(stderr, "Function(s) coming...\nNOT IMPLEMENTED\n");
			break;
		case PKT_VALUE_HEADER:
			fprintf(stderr, "Value coming...\nNOT IMPLEMENTED\n");
			//break;
		case PKT_PROGRAM_HEADER:
			fprintf(stderr, "Program coming...\nNOT IMPLEMENTED\n");
			//break;
		default:
			fprintf(stderr, "List(s) coming...\n");
			fprintf(stderr, "Matrix/Matrices coming...\n");
			fprintf(stderr, "NOT IMPLEMENTEEEED, NOT ANNYYYY OF THIIIIIIS!!!\n");
			break;
	}




	sp_close(port);


	return 0;
}

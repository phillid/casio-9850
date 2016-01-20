#include "casio-get.h"

#define PACKET_WAIT_PER_BYTE 	2

#include <stdio.h>
#include <stdlib.h>
#include <libserialport.h>

/******************************************************************************
 * Wait for the calculator to request a data transfer
 ******************************************************************************/
void wait_calc(struct sp_port *port)
{
	char buf[1];
	int read;
	fprintf(stderr, "Waiting for calculator... ");
	while((read = sp_blocking_read(port, buf, 1, 0)) > 0)
	{
		if (buf[0] == ATTENTION_REQ)
			break;
	}

	if (read <= 0)
		serial_error("sp_blocking_read");

	fprintf(stderr, "transfer request received!\n");
}


/******************************************************************************
 * Send single byte to calculator
 ******************************************************************************/
void send_calc_byte(struct sp_port *port, const char val)
{
	char data[1];
	data[0] = val;
	if (sp_blocking_write(port, data, 1, 0) != 1)
		serial_error("sp_blocking_write");
}

enum sp_return calc_read(struct sp_port *port, unsigned char* buffer, size_t size)
{
	enum sp_return ret = sp_blocking_read(port, buffer, size, PACKET_WAIT_PER_BYTE*size);
	if (ret < 0)
		serial_error("sp_blocking_read");

	return ret;
}


/******************************************************************************
 * Open and initialise serial port
 ******************************************************************************/
struct sp_port* port_init(const char* device)
{
	struct sp_port* port;
	if (sp_get_port_by_name(device, &port) != SP_OK)
		serial_error("sp_get_port_by_name");

	if (sp_open(port, SP_MODE_READ|SP_MODE_WRITE) != SP_OK)
		serial_error("sp_open");


	/* Port config is more or less hard-coded
	   The Casio calcs operate at 9600,8N with 2 stop bits RX
	   and 1 stop bit TX but 2 stop bits both ways can't hurt :) */
	if (sp_set_baudrate(port, 9600) != SP_OK)
		serial_error("sp_set_baudrate");

	if (sp_set_parity(port, SP_PARITY_NONE) != SP_OK)
		serial_error("sp_set_parity");

	if (sp_set_bits(port, 8) != SP_OK)
		serial_error("sp_set_bits");

	if (sp_set_stopbits(port, 1) != SP_OK)
		serial_error("sp_set_stopbits");

	if (sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE) != SP_OK)
		serial_error("sp_set_flowcontrol");

	if (sp_set_xon_xoff(port, SP_XONXOFF_DISABLED) != SP_OK)
		serial_error("sp_set_xon_xoff");

	if (sp_set_dtr(port, SP_DTR_ON) != SP_OK)
		serial_error("sp_set_dtr");

	if (sp_set_rts(port, SP_RTS_OFF) != SP_OK)
		serial_error("sp_set_rts");

	return port;
}

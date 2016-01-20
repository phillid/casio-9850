#ifndef PORT_H
#define PORT_H

#include <libserialport.h>

#define ATTENTION_REQ	0x16
#define ATTENTION_ACK	0x13

void wait_calc(struct sp_port *port);
void send_calc_byte(struct sp_port *port, const char val);
enum sp_return calc_read(struct sp_port *port, unsigned char* buffer, size_t size);
struct sp_port* port_init(const char* device);

#endif

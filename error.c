#include "casio-get.h"

#include <stdio.h>
#include <stdlib.h>

/******************************************************************************
 * Report error related to serial port read/write etc
 ******************************************************************************/
void serial_error(const char *fname)
{
	fprintf(stderr, "Error in %s: %s\n", fname, sp_last_error_message());
	exit(EXIT_FAILURE);
}

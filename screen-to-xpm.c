#include <stdio.h>
#include <string.h>

#include "packet.h"


#define IMAGE_HEADER_BYTES_BW   40
#define IMAGE_HEADER_BYTES_COL  41
#define HEADER_SIZE             3
#define IMAGE_WIDTH             128
#define IMAGE_HEIGHT            64
#define NUM_COLOURS             3

#define COLOUR_BLANK            '0'

const char colours[] = {'B', 'G', 'R'};


void load_mono(char (*image)[IMAGE_WIDTH][IMAGE_HEIGHT])
{
	unsigned short x, y;
	char c;
	char i;
	for (x = 0; x < IMAGE_WIDTH && !feof(stdin); x+=8)
	{
		for (y = 0; y < IMAGE_HEIGHT && !feof(stdin); y++)
		{
			c = fgetc(stdin);
			for (i = 0; i < 8; i++)
				(*image)[x+i][y] = c & (1<<(i))? colours[0] : COLOUR_BLANK;
		}
	}
}

void load_colour(char (*image)[IMAGE_WIDTH][IMAGE_HEIGHT])
{
	short x, y, colour;
	char buffer[IMAGE_WIDTH/8][IMAGE_HEIGHT];
	char c;
	char i;
	for (colour = 0; colour < NUM_COLOURS; colour++)
	{
		fread(buffer, sizeof(buffer), 1, stdin);
		for (x = 0; x < IMAGE_WIDTH && !feof(stdin); x+=8)
		{
			for (y = 0; y < IMAGE_HEIGHT && !feof(stdin); y++)
			{
				c = buffer[x/8][IMAGE_HEIGHT-1-y];
				for (i = 0; i < 8; i++)
				{
					if ((*image)[x+i][y] == COLOUR_BLANK)
						(*image)[x+i][y] = c & (1<<(i))? colours[colour] : COLOUR_BLANK;
				}
			}
		}
		/* eat the checksum */
		fgetc(stdin);
		
		/* eat the : and the colour identification bytes if another channel remains */
		if (colour != NUM_COLOURS-1)
		{
			fgetc(stdin);
			fgetc(stdin);
		}
	}
}



int main(int argc, char **argv)
{
	unsigned char header[PKT_HEADER_MAX_SIZE];
	char image[IMAGE_WIDTH][IMAGE_HEIGHT];
	int x, y, read, expected;
	enum packet_types type;

	if (argc != 1)
	{
		fprintf(stderr,
			"Converts monochrom CASIO FX-9850 screen dumps to XPM images.\n"
			"Reads screen dump from stdin and outputs XPM to stdout\n"
			"Also prints information and warning messages to stderr when necessary\n"
			"\n"
			"Syntax: %s < screen.dump > screen.xpm\n", argv[0]);
		return 1;
	}

	/* Lazy here, we just catch EOF later */
	/* magic number: 3 == strlen(":DD") == strlen(":DC") */
	read = fread(header, 3, 1, stdin);

	switch (get_packet_type(header))
	{
		case PKT_SCREEN_HEADER_BW:
			type = PKT_SCREEN_HEADER_BW;
			expected = IMAGE_HEADER_BYTES_BW;
			break;
		case PKT_SCREEN_HEADER_COL:
			type = PKT_SCREEN_HEADER_COL;
			expected = IMAGE_HEADER_BYTES_COL;
			break;
		default:
			fprintf(stderr, "Invalid header '%s', not a Casio FX 9850 screenshot\n", header);
			return 1;
			break;
	}

	fprintf(stderr, "Detected %s screenshot\n",
		type == PKT_SCREEN_HEADER_BW? "monochrome" : "colour");

	/* skip over rest of header */
	expected -= read;
	while (--expected)
		fgetc(stdin);

	memset(image, COLOUR_BLANK, sizeof(image));

	if (type == PKT_SCREEN_HEADER_BW)
		load_mono(&image);
	else 
		load_colour(&image);

	/* Tasty! A hard-coded XPM header! */
	printf(	"/* XPM */\n"
			"static char *Pic_colors[] = {\n"
			"	\"%d %d 4 1\"\n"
			"	\"B c #000033\"\n"
			"	\"G c #005555\"\n"
			"	\"R c #FF6633\"\n"
			"	\"0 c #FFFFFF\"\n"
			"};\n"
			"static char *Pic_pixels[] = {\n",IMAGE_WIDTH,IMAGE_HEIGHT
	);


	for (y = 0; y < IMAGE_HEIGHT; y++)
	{
		printf("\""); /* XPM stuff */

		for (x = IMAGE_WIDTH-1; x >= 0; x--)
			printf("%c",image[x][y]);

		printf("\",\n"); /* Again, XPM stuff */
	}


	/* Hard-coded XPM footer */
	printf("\"};");

	return 0;
}


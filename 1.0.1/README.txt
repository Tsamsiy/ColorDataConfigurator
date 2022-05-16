/*
----------------------------------------------------------------------------------------------------------------------------------------
Character description
---------------------

Command Identifier	single char, case sensitive
Data block delimiter	;
Command-Data delimiter	:
Parameter delimiter	,
Array start:		{
Array stop:		}

single line comment	//
multi line comment	/* ... * /

(i)	formatting [' ', '\t', '\n'] is removed
(i)	HEX data case insensitive

----------------------------------------------------------------------------------------------------------------------------------------
Command list and syntax
-----------------------

Color		C:rrggbb,t;		displays a fixed color for set amount of time
					t	uint16_t	time in milliseconds (0 - 65535)
					rr	byte		red HEX value (0 - 9 , A - F)
					gg	byte		green HEX value (0 - 9 , A - F)
					bb	byte		blue HEX value (0 - 9 , A - F)

Fade linear	F:rrggbb,rrggbb,t;	fades from first to second color, all channels at same time
					t	uint16_t	time in milliseconds (0 - 65535)
					rr	byte		red HEX value (0 - 9 , A - F)
					gg	byte		green HEX value (0 - 9 , A - F)
					bb	byte		blue HEX value (0 - 9 , A - F)

Loop		L:x,t;			jumps back to the beginning and skips x commands
					x	uint8_t		number of commands to skip (0 - 255)
					t	uint16_t	delay in milliseconds before first command is executed (0 - 65535)

----------------------------------------------------------------------------------------------------------------------------------------
Sample syntax
-------------*/

//displays solid r,g and b colors. with one second interval on first start, then half second
{
	C:FF0000,1000;
	C:00FF00,1000;
	C:0000FF,1000;
	
	C:FF0000,500;
	C:00FF00,500;
	C:0000FF,500;
	C:FF0000,500;
	C:00FF00,500;
	C:0000FF,500;
	C:FF0000,500;
	C:00FF00,500;
	C:0000FF,500;

	L:3,0;
}

//fades through colors
{
	F:00FFFF,FF00FF,1000;
	F:FF00FF,FFFF00,1000;
	F:FFFF00,00FFFF,1000;

	L:0,0;
}














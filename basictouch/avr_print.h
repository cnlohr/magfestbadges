/* AVR PRINTING (C) 2011 <>< Charles Lohr - This file may be licensed under the MIT/x11 or New BSD Licenses */
#ifndef _AVR_PRINT_H

void sendchr( char c );

#define sendstr( s ) { unsigned char rcnt; \
	for( rcnt = 0; s[rcnt] != 0; rcnt++ ) \
		sendchr( s[rcnt] ); }

void sendhex1( unsigned char i );
void sendhex2( unsigned char i );
void sendhex4( unsigned int i );
void setup_spi( void );

#endif


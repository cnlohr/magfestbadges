#include "avr_print.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

static void setup_clock( void )
{
	CLKPR = 0x80;	/*Setup CLKPCE to be receptive*/
	CLKPR = 0x00;	/*No scalar*/

//	OSCCAL = 0xFF;
}

volatile unsigned short tr;
unsigned short touch;

ISR( PCINT0_vect )
{
	tr = TCNT1;
	if( TIFR & _BV(OCF1A) )
		tr = 0xF0;
}


ISR( TIMER1_COMPA_vect )
{
	tr = 0xf0;
	TIMSK &= ~_BV( OCIE1A );
}


//#define smart_sleep_cpu() while( !tr );
#define smart_sleep_cpu() sleep_cpu();

#define TRF( PORTT, ID, PC, PCI, out ) \
	DDR##PORTT |= _BV(ID); \
	PORT##PORTT &= ~_BV(ID); \
	\
	tr = 0; \
	TIMSK |= _BV( OCIE1A ); \
	TCNT1 = 0; \
	DDR##PORTT &= ~_BV(ID); \
	PCMSK##PC = _BV(PCINT##PCI); \
	smart_sleep_cpu(); \
	out = tr; \
	DDR##PORTT |= _BV(ID); \
	PORT##PORTT &= ~_BV(ID); \
	\
	tr = 0; \
	TIMSK |= _BV( OCIE1A ); \
	TCNT1 = 0; \
	DDR##PORTT &= ~_BV(ID); \
	PCMSK##PC = _BV(PCINT##PCI); \
	smart_sleep_cpu(); \
	out += tr; \
	DDR##PORTT |= _BV(ID); \
	PORT##PORTT &= ~_BV(ID); \


unsigned short baseline[2];
unsigned short current[2];
unsigned short basezero[2];

void GoRun( unsigned short * outs, unsigned short  * baseline )
{
	if( baseline[0] < 0x400 )
		{TRF( B, 3, , 3, outs[0] ); }
	if( baseline[1] < 0x400 )
		{TRF( B, 4, , 4, outs[1] ); } 
}



int main( void )
{
	unsigned short i, j;
	unsigned char k;
	cli();

	setup_clock( );
	setup_spi( );

	sei();

	//Setup timer
	TCCR1 = _BV(CS10) | _BV(CS11);
	OCR1A = 0xf0;

//	TCCR1 =  _BV(CS12) | _BV(CS10);

	sleep_enable();

/*
while(1)
{
	char cc = 0;
	sendchr( 0 );
	sendstr( "MARK: " );
	DDRB |= 0x18;
	PORTB = 0x00;
	DDRB &= ~0x18;
	_delay_us(20);
	cc = PINB;
	sendhex2( cc & ( _BV(3) | _BV(4) ) );
	sendchr( ' ' );
	sendhex4( TCNT1 );
	sendstr( "\n" );
}*/

	GIMSK |= _BV(PCIE);

	//Run through set once (lines aren't settled or set up at this point)
	GoRun( baseline, basezero );
	GoRun( baseline, basezero );
	GoRun( baseline, basezero );
	GoRun( baseline, basezero );

	//Lines are settled.  Now e can finally get a good, clean baseline
	sendstr( "Start\n" );
	GoRun( baseline, basezero );


//	TCCR2A = 0;
//	TCCR2B = _BV(CS21);

//	DDRD |= _BV(7);
//	DDRB |= _BV(0);

	while( 1 )
	{
		int16_t a, b;

		GoRun( current, baseline );

		a = current[0] - baseline[0];
		b = current[1] - baseline[1];
//		PORTD ^= _BV(7);
//		PORTB ^= _BV(0);
//		j = TCNT2;

		if( a < 0 ) a = 0;
		if( b < 0 ) b = 0;
		if( a > 0xff ) a = 0xff;
		if( b > 0xff ) b = 0xff;

		sendchr( 0 );
		sendhex2( a );
		sendchr( ' ' );
		sendhex2( b );
		sendchr( '\n' );
	//	_delay_ms(100);
/*
		for( k = 0; k < 1; k++ )
		{
			TIMSK |= _BV( OCIE1A );
			TCNT1 = 0;
			DDRx &= ~_BV(ID);
			while( !(PINx & _BV(ID)) );
			DDRx |= _BV(ID);
			i = TCNT1;
		}
*/
	}

	return 0;
} 

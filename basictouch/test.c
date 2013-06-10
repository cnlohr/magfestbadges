#include "avr_print.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

static void setup_clock( void )
{
	CLKPR = 0x80;	/*Setup CLKPCE to be receptive*/
	CLKPR = 0x00;	/*No scalar*/
	OSCCAL = 0xFF;
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

	sleep_enable();

	DDRB |= _BV(4); //Marker

	PORTB &= ~_BV(3); //When engaged, lock port down.

	while(1)
	{
		DDRB |= _BV(3); //Lock port down
		PORTB |= _BV(4); //Marker
		_delay_ms(1);

		DDRB &= ~_BV(3); //Release Port
		PORTB &= ~_BV(4); //Marker
		_delay_ms(1);
	}


//	sendchr( 0 );
//	sendstr( "MARK: " );

	return 0;
} 

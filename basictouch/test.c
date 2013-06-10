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
}


int main( void )
{
	unsigned short j;
	unsigned char k;

	unsigned short calibrate = 0;
	unsigned char calibrated = 0;
	cli();

	setup_clock( );
	setup_spi( );

	sei();

	//Setup timer

	TCCR1 = _BV(CS11);
	TIMSK |= _BV( OCIE1A );
	OCR1A = 0xf0;
	sleep_enable();

	GIMSK |= _BV(PCIE);

	DDRB |= _BV(4); //Marker
	PORTB &= ~_BV(3); //When engaged, lock port down.

	sendstr( "MARK: " );

	tr = 0x0000;

	while(1)
	{
		DDRB |= _BV(3); //Lock port down
		PORTB |= _BV(4); //Marker
		sendhex2( j );
		sendchr( '\n' );
//		_delay_ms(1);

		DDRB &= ~_BV(3); //Release Port
		PORTB &= ~_BV(4); //Marker

		TCNT1 = 0;
		TIFR |= _BV(OCF1A);
		PCMSK = _BV(3);
		sleep_cpu();

		if( calibrated < 10 )
		{
			calibrated++;
			calibrate = tr;
		}
		else
		{
			j = tr - calibrate;

			if( j > 0xf0 ) j = 0x00;
		}

//		_delay_ms(1);

	}
	return 0;



} 

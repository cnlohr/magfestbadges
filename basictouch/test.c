#include "avr_print.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

static void setup_clock( void )
{
	CLKPR = 0x80;	//Setup CLKPCE to be receptive
	CLKPR = 0x00;	//No scalar
	OSCCAL = 0xFF;  //Max out the system clock speed
}

//tr is a temporary variable for storing the value in TCNT1 (timer 1)
volatile unsigned short tr;

//This is an interrupt that automatically gets called by the AVR when the touch pin rises.
ISR( PCINT0_vect )
{
	tr = TCNT1;
	if( TIFR & _BV(OCF1A) )
		tr = 0xF0;
}

//This is an interrupt that automatically gets called if the pin takes too long to rise.
ISR( TIMER1_COMPA_vect )
{
	tr = 0xf0;
}


int main( void )
{
	unsigned short j; //We store the last timer in this value for safe keeping.
	unsigned char k;

	unsigned short calibrate = 0;  //Initial "zero" value for touch sensor
	unsigned char calibrated = 0;  //Cycles of calibration

	//Turn off interrupts
	cli();

	//Set up the clock
	setup_clock( );

	//Turn on the SPI debuggint tools.
	setup_spi( );

	//Re-enable interrupts
	sei();

	//Setup timer

	TCCR1 = _BV(CS12);       //Use prescalar 2.
	TIMSK |= _BV( OCIE1A );  //Enable overflow compare A (to detect if we're taking too long)
	OCR1A = 0xf0;            //Set overflow A to be 0xf0 cycles

	sleep_enable();          //Allow the CPU to sleep.

	GIMSK |= _BV(PCIE);      //Enable Pin change interrupts.

	DDRB |= _BV(4); //Marker
	PORTB &= ~_BV(3); //When engaged, lock port down.

	//Send a brief debugging message at the beginning to verify it's functioning.
	sendstr( "MARK: " );

	tr = 0x0000;

	while(1)
	{
		DDRB |= _BV(3);  //Lock port down
		PORTB |= _BV(4); //Marker

		//Send the user the last touch value.
		sendhex2( j );
		sendchr( '\n' );

		DDRB &= ~_BV(3);  //Release Port
		PORTB &= ~_BV(4); //Marker

		//Reset the timer
		TCNT1 = 0;
		TIFR |= _BV(OCF1A);

		//Make sure we are set up to listen for a pin change interrupt on PORTB.3
		PCMSK = _BV(3);

		//Put the CPU to sleep until we either get a pin change or overflow compare A.
		sleep_cpu();

		//We want to wait longer than the first cycle so we can be sure we get a good value for the calibration.
		if( calibrated < 10 )
		{
			calibrated++;
			calibrate = tr;
		}
		else
		{
			//Take the current reading and subtract our "zero"
			j = tr - calibrate;

			//If it is negative somehow, set it to 0.
			if( j > 0xf0 ) j = 0x00;
		}

	}
	return 0;

} 



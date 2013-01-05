#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

void delay_ms(uint32_t time) {
  uint32_t i;
  for (i = 0; i < time; i++) {
    _delay_ms(1);
  }
}

#define NOOP asm volatile("nop" ::)

static void setup_clock( void )
{
	/*Examine Page 33*/

	CLKPR = 0x80;	/*Setup CLKPCE to be receptive*/
	CLKPR = 0x00;	/*No scalar*/

//	OSCCAL = 0xFF;

	PLLCSR = _BV(PLLE) | _BV( PCKE );
}

#define BASE 0xC7

static void Play( unsigned char v )
{
	unsigned char cc, c;
	for( c = 0; c < v; c++ )
	{
		OSCCAL = BASE;
		OSCCAL = BASE;
	}

	for( c = 255; c > v; c-- )
	{
		OSCCAL = BASE+1;
		OSCCAL = BASE;
	}
}

int main( )
{
	uint16_t t;
	unsigned char c, d;
	cli();
//	DDRA = 0x0f;  // 0000 1111
//	PORTA = 0x0a; // 0000 1010

	setup_clock();

	DDRB = _BV(4) | _BV(3);
	PORTB = 0xFF;

#define PWMMODE

#ifndef PWMMODE
	TCCR1 = _BV(CTC1) | _BV(CS10);
	GTCCR = 
		_BV(COM1B0) | 
		//_BV(COM1B1) |
		//_BV(PWM1B) |
		0;
	OCR1B = 0;
	OCR1C = 0;
	OCR1A = 0;
	TCNT1 = 0;
#else
	TCCR1 = _BV(CTC1) | _BV(CS10);
	GTCCR = 
		_BV(COM1B0) | 
		//_BV(COM1B1) |
		_BV(PWM1B) |
		0;
	OCR1B = 1;
	OCR1C = 2; //2 for /3, 3 for /4
	OCR1A = 0;
	TCNT1 = 0;
#endif

//E8 = 89.46 MHz
//E9 = 90.32 MHz


	sei();


// Music from very small programs.

	uint8_t last;
	uint8_t new;

	for( ;;t++) 
	{
		//Play( (t*9 & t>>4 | t*5 & t>>7 | t*3 & t/1024) - 1 );
		Play( ( (t*5 & t>>7 )| ((t*3&t>>10)-1) ) );  //neat song
		//t &= 0x7fff;
		//if( t > 0x7fff ) t = 0;

/*
		new = ( (t*5 & t>>7 )| ((t*3&t>>10)-1) );
		Play( new - last );
		last = new;
*/


//		Play( (t*5 & t>>7 )| (t*3&t>>10) );
//		Play( t*(t^t+(t>>15|1)^(t-1280^t)>>10 ) ); _delay_us(10);
//		Play( (t&t%255)-(t*3&t>>13&t>>6) );
	}	



	for( ;; )
	{
//		Play( c+=129 );
	}

	
	return 0;
} 

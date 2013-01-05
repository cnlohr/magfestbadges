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
marka##ID:	smart_sleep_cpu(); \
	if( tr == 0 ) goto marka##ID; \
	out = tr; \
	DDR##PORTT |= _BV(ID); \
	PORT##PORTT &= ~_BV(ID); \
	\
	tr = 0; \
	TIMSK &= ~_BV( OCIE1A ); \
	TCNT1 = 0; \
	DDR##PORTT &= ~_BV(ID); \
	PCMSK##PC = _BV(PCINT##PCI); \
markb##ID:	smart_sleep_cpu(); \
	if( tr == 0 ) goto markb##ID; \
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



int16_t a, b;


int main( void )
{
	unsigned short i, j = 0;
	unsigned char k;
	cli();

	setup_clock( );
//	setup_spi( );

	//For touch!
	TCCR1 = _BV(CS10) | _BV(CS11);
	OCR1A = 0xf0;

	//Synth
	DDRB |= _BV(2);

	//For Synth!
//	TCCR1 = _BV(COM1A0) | _BV(CS10);

	sei();


	sleep_enable();

	_delay_ms(10);
	OCR0A=0x10;


	GIMSK |= _BV(PCIE);

	//Run through set once (lines aren't settled or set up at this point)
	GoRun( baseline, basezero );
	GoRun( baseline, basezero );
	GoRun( baseline, basezero );
	GoRun( baseline, basezero );

	//Lines are settled.  Now e can finally get a good, clean baseline
	GoRun( baseline, basezero );

	while( 1 )
	{
		uint8_t a8, b8;
		GoRun( current, baseline );

		a = current[0] - baseline[0];
		b = current[1] - baseline[1];

		if( a < 0 ) a = 0;
		if( b < 0 ) b = 0;
		if( a > 0xff ) a = 0xff;
		if( b > 0xff ) b = 0xff;

		a8 = a;
		b8 = b;

		if( a > 30 || b > 30 )
		{
			PORTB &= ~_BV(2);
		}
		else
		{
			PORTB ^= _BV(2);
			_delay_ms(50);
		}	
	}

	return 0;
} 

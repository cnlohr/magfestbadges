
all : test.hex burn
PART=attiny85
PROGPART=t85

CFLAGS:=-g -Wall -pedantic -Os    -mmcu=$(PART) -DF_CPU=20000000
LDFLAGS:=-I  -g -Wall -pedantic -Os    -mmcu=$(PART) -Wl,-Map,test.map  -L/usr/lib/binutils/avr/2.18
CC:=avr-gcc

test.elf : test.o avr_print.o
	avr-gcc $(LDFLAGS) -o $@ $^

test.hex : test.elf
	avr-objcopy -j .text -j .data -O ihex test.elf test.hex 

burn : test.hex
	avrdude -c usbtiny -p $(PROGPART) -U flash:w:test.hex


#DO NOT BURN FUSES ON THIS THING - IT DOES NOT HAVE ANY CRYSTAL


readfuses :
	avrdude -c usbtiny -p $(PROGPART) -U hfuse:r:high.txt:b -U lfuse:r:low.txt:b

clean : 
	rm -f *~ high.txt low.txt test.hex test.map test.elf *.o

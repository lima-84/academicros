#include <avr/io.h>
#include "lima.h"

// Tempo de bounce em ms
#define BOUNCE 8

int main(void){
	
	escreve_EEPROM(0x02,0x84);
	escreve_EEPROM(0x11,0xAB);
	
	TCCR0A = 0x02;			// Modo CTC
	TCCR0B = 0x04;			// Prescaler de 256
	TCNT0  = 0x00;			// Zera o timer
	TIFR0 |= (1 << 1);		// Zera o flag de overflow
	
	DDRB |= (1 << 5);		// Seta pino 13 como output
    
    while (1){
		PORTB ^= (1 << 5);
		atraso_timer0(109);
		PORTB ^= (1 << 5);
		atraso_timer0(55);
    }
}


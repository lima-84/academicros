#include <avr/io.h>
#include "lima.h"

// Tempo de bounce em ms
#define BOUNCE 8

// Índices dos atrasos no array
#define ATR_800 0
#define ATR_1200 1

int main(void){
	/*// Relação de clientes
	short lista_clientes [11] =
	{
		28858,
		30238,
		26254,
		28857,
		24372,
		18190,
		29041,
		28858,
		30258,
		28857,
		29140
	};
	short lista_senhas [11] =
	{
		28858,
		30238,
		26254,
		28857,
		24372,
		18190,
		29041,
		28858,
		30258,
		28857,
		29140
	};	
	char lista_planos [11] =
	{
		'M',
		'P',
		'B',
		'M',
		'P',
		'P',
		'X',
		'M',
		'B',
		'X',
		'P'
	};
	*/
	
	volatile unsigned char atrasos[2] = {50,75};
	
	escreve_EEPROM(0x02,0x84);
	escreve_EEPROM(0x11,0xAB);
	
	// Configuração Timer0
	TCCR0A = 0x02;			// Modo CTC
	TCCR0B = 0x04;			// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR0 |= (1 << 1);		// Zera o flag de overflow do Timer0
	
	// Configuração Timer1
	TCCR1A = 0x04;			// Modo CTC
	TCCR1B = 0x04;			// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR1 |= (1 << 1);		// Zera o flag de overflow do Timer1

	DDRB |= (1 << 5);		// Seta pino PB5 como output
	DDRB |= (1 << 4);		// Seta pino PB4 como output
    
    while (1){
		
		PORTB &= ~(1 << 4);						// Zera o pino PB4
		
		PORTB ^= (1 << 5);						// Inverte o pino PB5
		TCNT0 = 0x00;							// Zera o Timer0
		atraso_timer0_ctc(atrasos[ATR_1200]);	// Atraso de 1200us
		PORTB ^= (1 << 5);		
		atraso_timer0_ctc(atrasos[ATR_800]);	// Atraso de 800us
		
		PORTB |= (1 << 4);						// Liga o pino PB4
		
		PORTB ^= (1 << 5);
		TCNT1 = 0x00;							// Zera o Timer1
		atraso_timer1_ctc(atrasos[ATR_1200]);	// Atraso de 1200us
		PORTB ^= (1 << 5);		
		TCNT1 = 0x00;							// Zera o Timer1 (?)
		atraso_timer1_ctc(atrasos[ATR_800]);	// Atraso de 800us
    }
}


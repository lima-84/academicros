#include <avr/io.h>
#include "lima.h"

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
	
	volatile const unsigned char atrasos[2] = {50,75};
	volatile unsigned char dado;
	// EEPROM
	EECR &= (~(1 << EEPM1) & ~(1 << EEPM0));	// Escolhe o modo atômico (00)
	escreve_EEPROM(0x02,0x84);
	escreve_EEPROM(0x11,0xAB);
	escreve_EEPROM(0x00,atrasos[0]);
	// Le o atraso da EEPROM
	dado = le_EEPROM(0x00);
	
	// Configuração Timer0
	TCCR0A = T0OVR;			// Modo do Timer0
	TCCR0B = PRE_256;		// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR0 |= (1 << 0);		// Zera o flag de overflow do Timer0

	// Configuração Timer1
	TCCR1A = T1OVR;			// Modo do Timer1
	TCCR1B = PRE_256;		// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR1 |= (1 << 0);		// Zera o flag de overflow do Timer1

	DDRB |= (1 << 5);		// Seta pino PB5 como output
	//DDRB |= (1 << 4);		// Seta pino PB4 como output
    
	// Inicialização dos pinos do LCD
	DDRB |= 0x0F;			// Seta os pinos de dados do LCD como output (PORT B)
	DDRD |= (1 << 7);		// Seta o pino de RS do LCD como output (PORT D)
	DDRB |= (1 << 4);		// Seta o pino de EN do LCD como output (PORT B)
	
	// Inicialização dos pinos do teclado
	//DDRD |=	0x0F;			// Seta as linhas do teclado como output (PORT D)
	//DDRD &= ~(0x70);		// Seta as colunas do teclado como input (PORT D)
	
	init_display();
	LCD_string("LCD teste");
	
    while (1){
		// TESTE TIMER OVERFLOW USANDO DADO DA EEPROM
		PORTB ^= (1 << 5);
		atraso_timer0(256 - atrasos[ATR_1200]);
		PORTB ^= (1 << 5);
		atraso_timer0(256 - dado);

		PORTB ^= (1 << 5);
		atraso_timer1(65536 - atrasos[ATR_1200]);
		PORTB ^= (1 << 5);
		atraso_timer1(65536 - dado);
		
		/* // TESTE TIMER CTC
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
		TCNT1 = 0x00;
		atraso_timer1_ctc(atrasos[ATR_800]);	// Atraso de 800us
		*/

    }
}


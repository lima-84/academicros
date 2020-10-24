#include <avr/io.h>
#include <avr/interrupt.h>
#include "lima.h"

char horas[2] = {2,2}, minutos[2] = {5,5}, lotacao = 3;
volatile char flag_cliente_apos_horario = 0;
volatile char flag_adm = 0, flag_atualiza_horario = 0;

/* atualiza_hora:
 * Incrementa o relógio de um minuto
 */
void atualiza_hora(){
	if(minutos[1] == 9){
		// Se estiver no minuto x9
		if(minutos[0] == 5){
			// Se estiver no minuto 59
			minutos[0] = 0;
			minutos[1] = 0;
			if(horas[1] == 9){
				// Se estiver na hora x9
				horas[0]++;
				horas[1] = 0;
			}
			else if(horas[0] == 2 && horas[1] == 3){
				// Se estiver na hora 23
				horas[0] = 0;
				horas[1] = 0;
			}
			else{
				// Se estiver na hora diferente de x9 e 23
				horas[1]++;
			}
		}
		else{
			// Se estiver no minuto y9, com y diferente de 5
			minutos[0]++;
			minutos[1] = 0;
		}
	}
	else{
		// Se estiver no minuto diferente de x9
		minutos[1]++;
	}
}

/*	imprime_hora:
 *	Imprime o horário atual no display LCD
 */
void imprime_hora(){
	LCD_caractere((11 & 0x0F) | LCD_LINHA_UM,CMD);	// Posiciona o cursor
	LCD_caractere(horas[0] + '0',DADO);
	LCD_caractere(horas[1] + '0',DADO);
	LCD_caractere(':',DADO);
	LCD_caractere(minutos[0] + '0',DADO);
	LCD_caractere(minutos[1] + '0',DADO);
}

/*	imprime_lotacao:
 *	Imprime a lotação atual no display LCD
 */
void imprime_lotacao(){
	LCD_caractere((15 & 0x0F) | LCD_LINHA_DOIS,CMD); // Posiciona o cursor
	LCD_caractere(lotacao + '0',DADO);
}

/*	imprime_mensagem_padrao:
 *	Imprime a mensagem padrão (hora e lotação)
 */
void LCD_mensagem_padrao(){
	LCD_caractere(LCD_LINHA_UM,CMD);	// Posiciona o cursor na primeira linha
	LCD_string("Hora:      ");
	imprime_hora();
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);	// Posiciona o cursor na segunda linha
	LCD_string("Lotacao:       ");
	imprime_lotacao();
}

void LCD_mensagem_cliente_apos_horario(){
	LCD_caractere(LCD_LINHA_UM,CMD);	// Posiciona o cursor na primeira linha
	LCD_string("AVISO:     ");

	LCD_caractere(LCD_LINHA_DOIS,CMD);	// Posiciona o cursor na segunda linha
	LCD_string("Pessoas dentro:");
}

/* ISR(TIMER1_OVF_vect):
 * Trata interrupção por overflow do Timer1, atualiza a hora no display LCD
 * Tempo de execução da função ~1ms
 */
ISR(TIMER1_OVF_vect){
	// Checa se é a segunda interrupção consecutiva (ou seja, 1 segundo se passou)
	if(flag_atualiza_horario == 1){
		// Atualiza e imprime a hora no display LCD
		atualiza_hora();
		imprime_hora();
		flag_atualiza_horario = 0;
	}
	else{
		// Se não atualizou nesta interrupção, deve atualizar na próxima
		flag_atualiza_horario = 1;
	}
	TCNT1 = 65536 - ATR_500_MS; // Reinicializa o Timer1
	
	// Se houver algum cliente na academia após o horário
	if(flag_cliente_apos_horario == 1){
		//PORTT ^= (1 << TESTE);		// Inverte o pino de teste
		PORTB ^= (1 << LED);	// Inverte o LED
	}
}

int main(void){
	
	// Relação de clientes
	volatile const short lista_clientes [11] =
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
	volatile const short lista_senhas [11] =
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
	volatile char lista_planos [11] =
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
	
	volatile unsigned char dado;
	// EEPROM
	EECR &= (~(1 << EEPM1) & ~(1 << EEPM0));	// Escolhe o modo atômico (00)
	escreve_EEPROM(0x02,0x84);
	escreve_EEPROM(0x11,0xAB);
	escreve_EEPROM(0x00,ATR_800);
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
	TIMSK1 |= (1 << 0);		// Ativa interrupção por overflow no Timer1

	// Configuração Timer2
	TCCR2A = T2OVR;			// Modo do Timer2
	TCCR2B = PRE2_256;		// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR2 |= (1 << 0);		// Zera o flag de overflow do Timer2
	
	DDRB |= (1 << 5);		// Seta pino PB5 como output (LED da placa)
	PORTB &= ~(1 << 5);		// Inicializa PB5 com zero (LED desligado)
	
	DDRT |= (1 << TESTE);	// Seta pino de teste como output
	PORTT &= ~(1 << TESTE);	// Inicializa o pino de teste em zero
	
	// Inicialização dos pinos do LCD
	DDRB |= 0x0F;			// Seta os pinos de dados do LCD como output (PORT B)
	DDRD |= (1 << RS);		// Seta o pino de RS do LCD como output (PORT D)
	DDRB |= (1 << EN);		// Seta o pino de EN do LCD como output (PORT B)
	
	// Inicialização dos pinos do teclado
	DDRD |=	0x0F;			// Seta as linhas do teclado como output (PORT D)
	DDRD &= ~(0x70);		// Seta as colunas do teclado como input (PORT D)
	
	init_display();
	LCD_string("LCD teste");
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Segunda linha");
	
	LCD_mensagem_padrao();
	
	sei();					// Habilita interrupções
	TCNT1 = 65536 - ATR_500_MS;
	
    while (1){
		// Checa se já passou do horário de funcionamento
		if((horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
			// Checa se alguém ainda está na academia
			if(lotacao > 0){
				flag_cliente_apos_horario = 1;
				LCD_mensagem_cliente_apos_horario();
				PORTT ^= (1 << TESTE);		// Inverte o pino de teste			
			}
		}
    }
}


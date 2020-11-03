#ifndef LIMA_H_
#define LIMA_H_

// Tempo de bounce em ms
#define BOUNCE 8

// Número de contagens dos atrasos (PRESCALER=256)
#define ATR_800 50
#define ATR_1200 75

#define ATR_40 3
#define ATR_1600 100
#define ATR_1000 63

#define ATR_200_MS 12500
#define ATR_500_MS 31250
#define ATR_1000_MS 62500

// Modos do Timer0
#define T0OVR 0x00	// Modo overflow
#define T0CTC 0x02	// Modo CTC

// Modos do Timer1
#define T1OVR 0x00	// Modo overflow
#define T1CTC 0x04	// Modo CTC

// Modos do Timer2
#define T2OVR 0x00	// Modo overflow
#define T2CTC 0x02	// Modo CTC

// Prescalers comuns para Timer0 e Timer1
#define PRE_TIMER_OFF 0x00
#define PRE_1 0x01
#define PRE_8 0x02
#define PRE_64 0x03
#define PRE_256 0x04
#define PRE_1024 0x05

// Prescalers Timer2
#define PRE2_TIMER_OFF 0x00
#define PRE2_1 0x01
#define PRE2_8 0x02
#define PRE2_32 0x03
#define PRE2_64 0x04
#define PRE2_128 0x05
#define PRE2_256 0x06
#define PRE2_1024 0x07

// Pinos da placa
#define LED 5		// LED da placa PB5

#define TESTE 0		// Pino para ser utilizado como teste/debug
#define PORTT PORTC	// Porta em que o pino se localiza
#define DDRT DDRC	// Registrador de direção da porta do pino

// Pinos do display LCD
#define RS 7		// RS no pino PD7
#define EN 4		// EN no pino PB4

// Comandos de inicialização do display LCD
#define LCD_4BIT_MODE 0x02	// Modo 4-bits
#define LCD_FUNC_SET 0x28	// Tamanho do display e tipo de caractere
#define LCD_DISP_CTRL 0x0C	// Liga/desliga display e cursor
#define LCD_ENTRY_MODE 0x06	// Incremento do cursor
#define LCD_CLEAR 0x01		// Ativa o comando de CLEAR

// Comandos para o cursor do display LCD
#define LCD_CBLINK	0x0F	// Cursor ligado e piscando
#define LCD_CSTATIC	0x0C	// Cursor desligado

// Comandos de posicionamento do display LCD
#define LCD_LINHA_UM 0x80	// Coloca o cursor na linha um
#define LCD_LINHA_DOIS 0xC0	// Coloca o cursor na linha dois
#define LCD_HORA 11			// Coloca o cursor na coluna das horas
#define LCD_LOTACAO 15		// Coloca o cursor na coluna da lotação
#define LCD_LOGIN 6			// Coloca o cursor após a mensagem de login

// Flag de comando/dado para o display LCD
#define CMD 0				// Valor do flag para envio de comando
#define DADO 1				// Valor do flag para envio de dado

#define TOTAL_CLIENTES 11	// Número total de clientes

// Linhas e colunas do teclado
#define COL1 0x60
#define COL2 0x50
#define COL3 0x30

/* atraso_timer0:
 * Gera um atraso relativo a n contagens com Timer0 em modo normal
 * n = 256 - ROUND((FREQ_TIMER/PRESCALER)*ATRASO)
 */
void atraso_timer0(volatile unsigned char n){
	// O Timer deve ser configurado previamente conforme necessário
	TCNT0 = n;						// Inicializa o timer
	while((TIFR0 & (1 << 0)) == 0);	// Espera o flag de overflow
	TIFR0 |= (1 << 0);				// Zera o flag de overflow
}

/* atraso_timer1:
 * Gera um atraso relativo a n contagens com Timer0 em modo normal
 * n = 65536 - ROUND((FREQ_TIMER/PRESCALER)*ATRASO)
 */
void atraso_timer1(volatile unsigned short n){
	TCNT1 = n;
	while((TIFR1 & (1 << 0)) == 0);
	TIFR1 |= (1 << 0);
}

/* atraso_timer2:
 * Gera um atraso relativo a n contagens com Timer2 em modo normal
 * n = 256 - ROUND((FREQ_TIMER/PRESCALER)*ATRASO)
 */
void atraso_timer2(volatile unsigned char n){
	// O Timer deve ser configurado previamente conforme necessário
	TCNT2 = n;						// Inicializa o timer
	while((TIFR2 & (1 << 0)) == 0);	// Espera o flag de overflow
	TIFR2 |= (1 << 0);				// Zera o flag de overflow
}

/* atraso_timer0_ctc:
 * Gera um atraso relativo a n contagens com Timer0 em modo CTC
 * n = ROUND((FREQ_TIMER/PRESCALER)*ATRASO)
 */
void atraso_timer0_ctc(volatile unsigned char n){
	// O Timer deve ser zerado e configurado previamente conforme necessário
	OCR0A = n;						// Define valor de comparação
	while((TIFR0 & (1 << 1)) == 0);	// Espera o flag de overflow
	TIFR0 |= (1 << 1);				// Zera o flag de overflow
}

/* atraso_timer1_ctc:
 * Gera um atraso relativo a n contagens com Timer1 em modo CTC
 * n = ROUND((FREQ_TIMER/PRESCALER)*ATRASO)
 */
void atraso_timer1_ctc(volatile unsigned short n){
	// O Timer deve ser zerado e configurado previamente conforme necessário
	OCR1A = n;						// Define o valor de comparação
	while((TIFR1 & (1 << 1)) == 0);	// Espera o flag de overflow
	TIFR1 |= (1 << 1);				// Zera o flag de overflow
}

/* atraso_timer2_ctc:
 * Gera um atraso relativo a n contagens com Timer2 em modo CTC
 * n = ROUND((FREQ_TIMER/PRESCALER)*ATRASO)
 */
void atraso_timer2_ctc(volatile unsigned char n){
	// O Timer deve ser zerado e configurado previamente conforme necessário
	OCR2A = n;						// Define valor de comparação
	while((TIFR2 & (1 << 1)) == 0);	// Espera o flag de overflow
	TIFR2 |= (1 << 1);				// Zera o flag de overflow
}

/* LCD_caractere:
 * Envia um caractere de 8-bits para o display LCD em modo 4-bits
 */
void LCD_caractere(unsigned char caractere, unsigned char flag_comando_dado){
	
	if(flag_comando_dado == CMD){		// Se estiver enviando comando
		PORTD &= ~(1 << RS);			// Desliga RS
	}
	PORTB |=  (1 << EN);				// Liga EN
	PORTB &= 0xF0;
	PORTB |= ((0xF0 & caractere) >> 4);	// Envia a parte alta
	PORTB &= ~(1 << EN);				// Desliga EN
	
	PORTB |=  (1 << EN);				// Liga EN
	PORTB &= 0xF0;
	PORTB |= (0x0F & caractere);		// Envia a parte baixa
	PORTB &= ~(1 << EN);				// Desliga EN
	
	atraso_timer0(256 - ATR_40);		// Atraso de ~40us (48us)
	
	if(flag_comando_dado == CMD){		// Se enviou um comando
		PORTD |= (1 << RS);				// Liga RS
	}
}

/* LCD_string:
 * Envia uma string para o display LCD em modo 4-bits
 */
void LCD_string(char* str){
	// Envia caractere até chegar no final da string ('\0')
	while(*str != '\0'){
		LCD_caractere(*str,DADO);	// Envia caractere atual
		str++;						// Passa para o próximo caractere
	}
}

/* init_display:
 * Inicializa o display LCD
 */
void init_display(){
	// Existe um atraso de 48us entre cada comando
	LCD_caractere(LCD_4BIT_MODE,CMD);
	LCD_caractere(LCD_FUNC_SET,CMD);
	LCD_caractere(LCD_DISP_CTRL,CMD);	
	LCD_caractere(LCD_ENTRY_MODE,CMD);	
	LCD_caractere(LCD_CLEAR,CMD);	
	atraso_timer0(256 - ATR_1600);	// Atraso extra de 1600us para o CLEAR
}
/* TCL_trata_tecla:
 * Retorna o dígito correspondente à tecla do teclado apertada
 */
volatile unsigned char TCL_trata_tecla(volatile unsigned char tecla, volatile unsigned char linha){
	// Checa a coluna e, em seguida, a linha
	switch(tecla){
		case COL1:
			switch(linha){
				case 0: return '1'; break;
				case 1: return '4'; break;
				case 2: return '7'; break;
				case 3: return '*'; break;
			}
		break;
		
		case COL2:
			switch(linha){
				case 0: return '2'; break;
				case 1: return '5'; break;
				case 2: return '8'; break;
				case 3: return '0'; break;
			}
		break;
		
		case COL3:
			switch(linha){
				case 0: return '3'; break;
				case 1: return '6'; break;
				case 2: return '9'; break;
				case 3: return '#'; break;
			}
			break;
		// Caso nenhuma tecla for apertada
		case 0x70: return ' ';
	}
	// Retorna, por segurança, um indicador de erro caso haja algum problema na leitura
	return 'E';
	
}

/* TCL_checa_teclado:
 * Realiza a leitura do teclado, retornando o caractere pressionado
 */
volatile unsigned char TCL_checa_teclado(){
	
	volatile unsigned char tecla = 'x', ult_tecla = 0, linha = 0;
	volatile unsigned int count = 0;
	
	while(linha < 4){
		// Lê a i-ésima linha
		PORTD &= ~(1 << linha);
		tecla = PIND & (0x70);
		
		if(tecla == 0x70){ // Se nada nesta linha foi apertado
			PORTD |= (1 << linha);
			linha++;
		}
		else{
			while(count < BOUNCE){					// Trata o bouncing da tecla
				atraso_timer0(256 - ATR_1000);		// Atraso de 1ms
				tecla = PIND & (0x70);
				if(tecla == ult_tecla)
					count++;
				else
					count = 0;
				ult_tecla = tecla;
			}
			
			if(tecla == 0x70) return ' ';
			while(tecla == (PIND & (0x70)));		// Caso o usuário mantenha a tecla pressionada
			
			PORTT ^= (1 << TESTE);					// Pino de teste
			
			PORTD |= (1 << linha);					// Desativa a linha
			return TCL_trata_tecla(tecla,linha);
		}
	}
	// Retorna um espaço, indicador de que nada foi apertado
	return ' ';
	
}

/* EEPROM_escrita: 
 * Escreve um dado em um determinado endereço na EEPROM do chip
 */
void EEPROM_escrita(volatile unsigned short endereco, volatile unsigned char dado){
	// EEPE: 1 -> escrita	0 -> leitura
	while(EECR & (1 << EEPE));			// Espera a última escrita terminar
	EEAR = endereco;					// HIGH e LOW, mas pode ser feito diretamente
	EEDR = dado;
	EECR |= (1 << EEMPE);				// Liga o Master Program Enable
	EECR |= (1 << EEPE);				// Inicia o processo de escrita
}

/* EEPROM_leitura: 
 * Lê um determinado endereço da EEPROM do chip
 */
volatile unsigned char EEPROM_leitura(volatile unsigned short endereco){
	
	volatile unsigned char dado;
	
	while(EECR & (1 << EEPE));			// Espera a última escrita terminar
	EEAR = endereco;					
	EECR |= (1 << EERE);				// Inicia a leitura
	dado = EEDR;						// Lê o registrador de dado
	
	return dado;
}


#endif /* LIMA_H_ */
#ifndef LIMA_H_
#define LIMA_H_

// Tempo de bounce em ms
#define BOUNCE 8

// Índices dos atrasos no array
#define ATR_800 0
#define ATR_1200 1

// Modos do Timer0
#define T0OVR 0x00	// Modo overflow
#define T0CTC 0x02	// Modo CTC

// Modos do Timer1
#define T1OVR 0x00	// Modo overflow
#define T1CTC 0x04	// Modo CTC

// Prescalers (comuns para os timers)
#define PRE_TIMER_OFF 0x00
#define PRE_1 0x01
#define PRE_8 0x02
#define PRE_64 0x03
#define PRE_256 0x04
#define PRE_1024 0x05

// Pinos do display LCD
#define RS 7		// RS no pino PD7
#define EN 4		// EN no pino PB4

// Comandos de inicialização do display LCD
#define LCD_4BIT_MODE 0x02	// Modo 4-bits
#define LCD_FUNC_SET 0x28	// Tamanho do display e tipo de caractere
#define LCD_DISP_CTRL 0x0C	// Liga/desliga display e cursor
#define LCD_ENTRY_MODE 0x06	// Incremento do cursor
#define LCD_CLEAR 0x01		// Ativa o comando de CLEAR


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

/* LCD_caractere:
 * Envia um caractere de 8-bits para o display LCD em modo 4-bits
 */
void LCD_caractere(unsigned char caractere){
	
	PORTB |=  (1 << EN);				// Liga EN
	PORTB &= 0xF0;
	PORTB |= ((0xF0 & caractere) >> 4);	// Envia a parte alta
	PORTB &= ~(1 << EN);				// Desliga EN
	
	PORTB |=  (1 << EN);				// Liga EN
	PORTB &= 0xF0;
	PORTB |= (0x0F & caractere);		// Envia a parte baixa
	PORTB &= ~(1 << EN);				// Desliga EN
	
	atraso_timer0(253);					// Atraso de 48us
}

/* LCD_string:
 * Envia uma string para o display LCD em modo 4-bits
 */
void LCD_string(char* str){
	// Envia caractere até chegar no final da string ('\0')
	while(*str != '\0'){
		LCD_caractere(*str);	// Envia caractere atual
		str++;					// Passa para o próximo caractere
	}
}

/* init_display:
 * Inicializa o display LCD
 */
void init_display(){
	
	PORTD &= ~(1 << RS);			// Desliga RS -> Entra no modo de comando
	// Existe um atraso de 48us entre cada comando
	LCD_caractere(LCD_4BIT_MODE);
	LCD_caractere(LCD_FUNC_SET);
	LCD_caractere(LCD_DISP_CTRL);	
	LCD_caractere(LCD_ENTRY_MODE);	
	LCD_caractere(LCD_CLEAR);	
	atraso_timer0(156);				// Atraso extra de 1600us para o CLEAR
	
	PORTD |=  (1 << RS);			// Liga RS -> Entra no modo de dados
}

/* debounce:
 * Faz o debounce da tecla lida
 */
volatile unsigned char debounce(volatile unsigned char tecla){
	
	volatile unsigned char contador = 0, tecla_anterior = 0;
	while(contador < 8){
		atraso_timer0_ctc(63);	// Atraso de 1ms
		tecla = 0;				// Lê o teclado
		// Checa se a tecla se repetiu
		if(tecla == tecla_anterior){ contador++; }
		else{ contador = 0; }
		// Checa se o contador atingiu o valor de bounce
		if(contador == 8){ break; }
		tecla_anterior = tecla;		
	}
	return tecla;
}

/* escreve_EEPROM: 
 * Escreve um dado em um determinado endereço na EEPROM do chip
 */
void escreve_EEPROM(volatile unsigned short endereco, volatile unsigned char dado){
	// EEPE: 1 -> escrita	0 -> leitura
	while(EECR & (1 << EEPE));			// Espera a última escrita terminar
	EEAR = endereco;					// HIGH e LOW, mas pode ser feito diretamente
	EEDR = dado;
	EECR |= (1 << EEMPE);				// Liga o Master Program Enable
	EECR |= (1 << EEPE);				// Inicia o processo de escrita
}

/* le_EEPROM: 
 * Lê um determinado endereço da EEPROM do chip
 */
volatile unsigned char le_EEPROM(volatile unsigned short endereco){
	
	volatile unsigned char dado;
	
	while(EECR & (1 << EEPE));			// Espera a última escrita terminar
	EEAR = endereco;					
	EECR |= (1 << EERE);				// Inicia a leitura
	dado = EEDR;						// Lê o registrador de dado
	
	return dado;
}


#endif /* LIMA_H_ */
#ifndef LIMA_H_
#define LIMA_H_

/* atraso_timer0:
 * Gera um atraso relativo a n contagens com Timer0 em modo CTC
 * n = ROUND((FREQ_TIMER/PRESCALER)*ATRASO)
 */
void atraso_timer0(volatile unsigned char n){
	// O Timer deve ser zerado previamente se necessário
	OCR0A = n;						// Define valor de comparação
	while((TIFR0 & (1 << 1)) == 0);	// Espera o flag de overflow
	TIFR0 |= (1 << 1);				// Zera o flag de overflow
}

void init_display(){

}

/* debounce:
 * Faz o debounce da tecla lida
 */
volatile unsigned char debounce(volatile unsigned char tecla){
	
	volatile unsigned char contador = 0, tecla_anterior = 0;
	while(contador < 8){
		atraso_timer0(8);		// Atraso de 1ms
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
	EECR = (0 << EEPM1) | (0 << EEPM0); // Escolhe o modo atômico (00, apaga e escreve de uma vez)
	EEAR = endereco;
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
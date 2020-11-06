/**********************************************************************/
/*		    Universidade Federal do Rio Grande do Sul			      */
/*		     	ENG04475 - Microprocessadores I					      */
/*	    Controle de acesso eletrônico para academias  -  AVR	      */
/*																      */
/*	Alunos:														      */
/*	 00302389 - Enrique Leon Paillo Statquevios					      */
/*   00262546 - Guilherme Cabral								      */
/*   00288579 - Pedro Rodrigues de Lima							      */
/*															          */
/**********************************************************************/

/**********************************************************************/
/* lima.h: Biblioteca que contém as funções básicas do AVR e #defines */
/*  Atrasos, display, teclado, conversão de variáveis, EEPROM		  */
/**********************************************************************/
#ifndef LIMA_H_
#define LIMA_H_

#include <avr/io.h>			// Macros básicos do AVR
#include <avr/interrupt.h>	// Funções de interrupção
#include <stdlib.h>			// Funções atoi() e itoa()

/**********************************************************************/
/*							DEFINES									  */
/**********************************************************************/
// Tempo de bounce em ms
#define BOUNCE 8

// Número de contagens dos atrasos, arredondados (PRESCALER=256)
#define ATR_40 3				//   40us
#define ATR_1600 100			// 1600us
#define ATR_1000 63				// 1000us

#define ATR_250_MS 15750		//  250ms (Na verdade 246ms: considera outros comandos da interrupção)

// Modos do Timer0
#define T0OVR 0x00				// Modo overflow
#define T0CTC 0x02				// Modo CTC

// Modos do Timer1
#define T1OVR 0x00				// Modo overflow
#define T1CTC 0x04				// Modo CTC

// Modos do Timer2
#define T2OVR 0x00				// Modo overflow
#define T2CTC 0x02				// Modo CTC

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
#define LED 5					// LED da placa PB5

#define TESTE 0					// Pino para ser utilizado como teste/debug
#define PORTT PORTC				// Porta em que o pino se localiza
#define DDRT DDRC				// Registrador de direção da porta do pino

// Pinos de comando do display LCD
#define RS 7					// RS no pino PD7
#define EN 4					// EN no pino PB4

// Comandos de inicialização do display LCD
#define LCD_4BIT_MODE 0x02		// Modo 4-bits
#define LCD_FUNC_SET 0x28		// Tamanho do display e tipo de caractere
#define LCD_DISP_CTRL 0x0C		// Liga/desliga display e cursor
#define LCD_ENTRY_MODE 0x06		// Incremento do cursor
#define LCD_CLEAR 0x01			// Ativa o comando de CLEAR

// Comandos para o cursor do display LCD
#define LCD_CBLINK	0x0F		// Cursor ligado e piscando
#define LCD_CSTATIC	0x0C		// Cursor desligado

// Comandos de posicionamento do display LCD
#define LCD_LINHA_UM 0x80		// Coloca o cursor na linha um
#define LCD_LINHA_DOIS 0xC0		// Coloca o cursor na linha dois
#define LCD_HORA 11				// Coloca o cursor na coluna das horas
#define LCD_LOTACAO 15			// Coloca o cursor na coluna da lotação
#define LCD_LOGIN 6				// Coloca o cursor após a mensagem de login

// Flag de comando/dado para o display LCD
#define CMD 0					// Valor do flag para envio de comando
#define DADO 1					// Valor do flag para envio de dado

#define TOTAL_CLIENTES 11		// Número total de clientes

// Colunas do teclado em relação aos pinos do microprocessador (PD4,PD5,PD6, acionadas em 0)
#define COL1 0x60				// Coluna 1 (1,4,7,*)
#define COL2 0x50				// Coluna 2 (2,5,8,0)
#define COL3 0x30				// Coluna 3 (3,6,9,#)

// Tempo em minutos das contas
#define MIN_BASICA 3600			// Conta básica:  60 horas
#define MIN_PREMIUM 5400		// Conta premium: 90 horas

/**********************************************************************/
/*							ATRASOS									  */
/**********************************************************************/

/* atraso_timer0:													  */
/* Gera um atraso relativo a n contagens com Timer0 em modo normal	  */
/* n = 256 - ROUND((FREQ_TIMER/PRESCALER)*ATRASO)					  */
/*																	  */
void atraso_timer0(volatile unsigned char n){
	// O Timer0 deve ser configurado previamente conforme necessário
	TCNT0 = n;						// Inicializa o timer
	while((TIFR0 & (1 << 0)) == 0);	// Espera o flag de overflow
	TIFR0 |= (1 << 0);				// Zera o flag de overflow
}

/* atraso_timer1:													  */												
/* Gera um atraso relativo a n contagens com Timer1 em modo normal    */
/* n = 65536 - ROUND((FREQ_TIMER/PRESCALER)*ATRASO)                   */
/*                                                                    */
void atraso_timer1(volatile unsigned short n){
	// O Timer1 deve ser configurado previamente conforme necessário
	TCNT1 = n;						// Inicializa o timer
	while((TIFR1 & (1 << 0)) == 0);	// Espera o flag de overflow
	TIFR1 |= (1 << 0);				// Zera o flag de overflow
}

/* atraso_timer2:													  */
/* Gera um atraso relativo a n contagens com Timer2 em modo normal    */
/* n = 256 - ROUND((FREQ_TIMER/PRESCALER)*ATRASO)					  */
/*																	  */
void atraso_timer2(volatile unsigned char n){
	// O Timer2 deve ser configurado previamente conforme necessário
	TCNT2 = n;						// Inicializa o timer
	while((TIFR2 & (1 << 0)) == 0);	// Espera o flag de overflow
	TIFR2 |= (1 << 0);				// Zera o flag de overflow
}

/* atraso_timer0_ctc:												  */
/* Gera um atraso relativo a n contagens com Timer0 em modo CTC		  */
/* n = ROUND((FREQ_TIMER/PRESCALER)*ATRASO)							  */
/*																	  */
void atraso_timer0_ctc(volatile unsigned char n){
	// O Timer0 deve ser zerado e configurado previamente conforme necessário
	OCR0A = n;						// Define valor de comparação
	while((TIFR0 & (1 << 1)) == 0);	// Espera o flag de comparação
	TIFR0 |= (1 << 1);				// Zera o flag de overflow
}

/* atraso_timer1_ctc:												   */
/* Gera um atraso relativo a n contagens com Timer1 em modo CTC        */
/* n = ROUND((FREQ_TIMER/PRESCALER)*ATRASO)							   */
/*																	   */
void atraso_timer1_ctc(volatile unsigned short n){
	// O Timer1 deve ser zerado e configurado previamente conforme necessário
	OCR1A = n;						// Define o valor de comparação
	while((TIFR1 & (1 << 1)) == 0);	// Espera o flag de comparação
	TIFR1 |= (1 << 1);				// Zera o flag de overflow
}

/* atraso_timer2_ctc:												   */
/* Gera um atraso relativo a n contagens com Timer2 em modo CTC		   */
/* n = ROUND((FREQ_TIMER/PRESCALER)*ATRASO)							   */
/*                                                                     */
void atraso_timer2_ctc(volatile unsigned char n){
	// O Timer2 deve ser zerado e configurado previamente conforme necessário
	OCR2A = n;						// Define valor de comparação
	while((TIFR2 & (1 << 1)) == 0);	// Espera o flag de comparação
	TIFR2 |= (1 << 1);				// Zera o flag de overflow
}

/***********************************************************************/
/*							DISPLAY LCD								   */
/***********************************************************************/

/* LCD_caractere:                                                      */
/* Envia um caractere de 8-bits para o display LCD em modo 4-bits      */
/* caractere: caractere a ser enviado								   */
/* flag_comando_dado: indica se está enviando um dado ou um comando    */
/*                                                                     */
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

/* LCD_string:                                                         */
/* Envia uma string para o display LCD em modo 4-bits                  */
/* str: string a ser enviada                                           */
/*                                                                     */
void LCD_string(char* str){
	// Envia caractere até chegar no final da string ('\0')
	while(*str != '\0'){
		LCD_caractere(*str,DADO);	// Envia caractere atual
		str++;						// Passa para o próximo caractere
	}
}

/* init_display:                                                       */
/* Inicializa o display LCD                                            */
/*                                                                     */
void init_display(){
	// Existe um atraso de ~40us (48us) entre cada comando
	LCD_caractere(LCD_4BIT_MODE,CMD);
	LCD_caractere(LCD_FUNC_SET,CMD);
	LCD_caractere(LCD_DISP_CTRL,CMD);	
	LCD_caractere(LCD_ENTRY_MODE,CMD);	
	LCD_caractere(LCD_CLEAR,CMD);	
	atraso_timer0(256 - ATR_1600);	// Atraso extra de 1600us para o CLEAR
}

/**********************************************************************/
/*							TECLADO 4x3								  */
/**********************************************************************/

/* TCL_trata_tecla:													  */
/* Retorna o dígito correspondente à tecla do teclado apertada        */
/* tecla: valor lido nos pinos do chip (correspondente à coluna)      */
/* linha: linha ativada no momento da leitura                         */
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

/* TCL_checa_teclado:                                                 */
/* Realiza a leitura do teclado, retornando o caractere pressionado   */
/*                                                                    */
volatile unsigned char TCL_checa_teclado(){
	
	volatile unsigned char tecla = 'x', ult_tecla = 0, linha = 0;
	volatile unsigned int count = 0;
	// Ativa as linhas do teclado de forma iterada
	while(linha < 4){
		PORTD &= ~(1 << linha);						// Ativa a i-ésima linha
		tecla = PIND & (0x70);						// Lê a tecla
		
		if(tecla == 0x70){							// Se nada nesta linha foi apertado
			PORTD |= (1 << linha);					// Desativa a i-ésima linha
			linha++;								// Incrementa a linha
		}
		else{
			while(count < BOUNCE){					// Trata o bouncing da tecla
				atraso_timer0(256 - ATR_1000);		// Atraso de 1ms
				tecla = PIND & (0x70);				// Lê a tecla
				if(tecla == ult_tecla)				// Se a tecla se repetiu
					count++;						// Incrementa o contador de bounce
				else								// Se a tecla foi diferente
					count = 0;						// Zera o contador de bounce
				ult_tecla = tecla;					// Atualiza a última tecla
			}
			
			if(tecla == 0x70) return ' ';			// Se nada foi apertado, retorna um espaço
			while(tecla == (PIND & (0x70)));		// Caso o usuário mantenha a tecla pressionada
			
			PORTD |= (1 << linha);					// Desativa a linha
			return TCL_trata_tecla(tecla,linha);	// Retorna o código correspondente
		}
	}
	// Por segurança, retorna um espaço, indicador de que nada foi apertado
	return ' ';
}

/* user_input:*/
/* Lê múltiplas teclas do teclado 4x3, mostrando-as no display		  */
/* num_teclas: número de teclas a serem lidas						  */
/* senha: indica se o input é uma senha, imprimindo '*' caso positivo */
/* flag_caractere_especial: indica se '*' ou '#' foram pressionados   */
/*                                                                    */
/* itoa() converte o string em um único inteiro ("1234" -> 1234)	  */
/* itoa("#") ou itoa("*") retorna zero, criando a necessidade do flag */
int user_input(int num_teclas, char senha, volatile char* flag_caratere_especial){
	char input[num_teclas];
	for(int i = 0; i < num_teclas; i++){			// Repete a operação num_teclas vezes
		char tecla = ' ';
		while(tecla == ' ' || tecla == 'E')			// Enquanto não for lida uma tecla
			tecla = TCL_checa_teclado();			// Lê a tecla
		if(tecla == '#' || tecla == '*')			// Se ler caractere especial
			*flag_caratere_especial = 1;			// Ativa o flag de caractere especial
		if(senha == 0){								 
			LCD_caractere(tecla, DADO);				// Envia o caractere se não for uma senha
		}
		else if(senha == 1){
			LCD_caractere('*',DADO);				// Envia '*' se for uma senha
		}
		input[i] = tecla;							// Atualiza o vetor de teclas
	}
	return atoi(input);								// Converte para inteiro e retorna
}

/**********************************************************************/
/*						VALIDAÇÕES E CONVERSÕES						  */
/**********************************************************************/

/* valida_cliente:													  */
/* Checa se um determinado cliente está na lista					  */
/* cliente: número de login do cliente								  */
/* lista: lista na qual quer-se procurar o cliente					  */
/*																	  */
int valida_cliente(volatile short int cliente, volatile const short lista[11]){
	for(int i = 0; i < 11; i++){
		if(cliente == lista[i]){
			return i;				// Retorna o índice do cliente
		}
	}
	return 'E';						// Retorna código de erro 'E' caso não encontre o cliente na lista
}

/* trata_zeros_horario:												  */
/* Coloca um zero à esquerda para tratar conversões char->int		  */
/*																      */
void trata_zeros_horario(char* str){
	if(str[1] == '\0'){				// Se a string acaba na posição 1
		str[1] = str[0];			// Desloca o caractere da posição 0
		str[0] = '0';				// Zera o caractere da posição 1
		str[2] = '\0';				// Atribui o final da string na posição 2
	}
}

/*	hhmm_para_minutos:												  */
/*  Converte um horário hh:mm para um equivalente em minutos		  */
/*	horas: vetor de horas											  */
/*	minutos: vetor de minutos										  */
/*																	  */
int hhmm_para_minutos(char* horas, char* minutos){
	// Realiza a conversão e retorna
	return ((int) horas[0])*600 + ((int) horas[1])*60 + ((int) minutos[0])*10 + ((int) minutos[1]);
	
}

/*	minutos_para_hhmm:												  */
/*  Converte um horário em minutos para um equivalente em hh:mm		  */
/*	total_minutos: tempo em minutos									  */
/*	str_horas: string destino para as horas							  */
/*	str_minutos: string destino para os minutos						  */
/*																	  */
void minutos_para_hhmm(int total_minutos, char* str_horas, char* str_minutos){
	
	short num_horas, num_minutos;
	
	num_horas = total_minutos/60;		// Divisão inteira corresponde à quantidade de horas
	num_minutos = total_minutos%60;		// Resto da divisão corresponde aos minutos
	
	itoa(num_horas,str_horas,10);		// Converte as horas para string
	itoa(num_minutos,str_minutos,10);	// Converte os minutos para string
	
	trata_zeros_horario(str_horas);		// Trata os zeros após a conversão
	trata_zeros_horario(str_minutos);	// Trata os zeros após a conversão
}

/**********************************************************************/
/*							EEPROM									  */
/**********************************************************************/

/* EEPROM_escrita:													  */
/* Escreve um dado em um determinado endereço na EEPROM do chip		  */
/* endereco: endereço da EEPROM no qual quer-se escrever			  */
/* dado: caractere a ser enviado*/
void EEPROM_escrita(volatile unsigned short endereco, volatile unsigned char dado){
	// EEPE: 1 -> escrita	0 -> leitura
	cli();								// Desabilita interrupções
	while(EECR & (1 << EEPE));			// Espera a última escrita terminar
	EEAR = endereco;					// HIGH e LOW, mas pode ser feito diretamente
	EEDR = dado;
	
	EECR |= (1 << EEMPE);				// Liga o Master Program Enable
	EECR |= (1 << EEPE);				// Inicia o processo de escrita
	sei();								// Habilita interrupções
}

/* EEPROM_leitura:													   */
/* Lê um determinado endereço da EEPROM do chip						   */
/* endereco: endereço da EEPROM do qual quer-se ler					   */
volatile unsigned char EEPROM_leitura(volatile unsigned short endereco){
	
	volatile unsigned char dado;
	
	cli();								// Desativa interrupções
	while(EECR & (1 << EEPE));			// Espera a última escrita terminar
	EEAR = endereco;					
	EECR |= (1 << EERE);				// Inicia a leitura
	dado = EEDR;						// Lê o registrador de dado
	
	sei();								// Habilita interrupções
	return dado;

}

/* EEPROM_escreve_horario:										       */
/* Escreve um horario (palavra de 16 bits) na EEPROM	               */
/* endereco: endereço da EEPROM no qual quer-se escrever			   */
/* horario: horario a ser escrito				                       */
/*																	   */
void EEPROM_escreve_horario(volatile unsigned short endereco, volatile unsigned short horario){
	EEPROM_escrita(endereco,horario & 0xFF);	// Envia a parte baixa
	EEPROM_escrita(endereco+1,horario >> 8);	// Envia a parte alta
}

/* EEPROM_le_horario:												   */
/* Lê um horario (palavra de 16 bits) da EEPROM				           */
/* endereco: endereço da EEPROM do qual quer-se ler                    */
/*																	   */
unsigned short EEPROM_le_horario(unsigned short endereco){
	return ((short) (EEPROM_leitura(endereco) | (EEPROM_leitura(endereco + 1) << 8)));
}

/* EEPROM_clear:														*/
/* Limpa a EEPROM														*/
/*																		*/
void EEPROM_clear(){
	for(short i = 0; i < 128; i++){
		EEPROM_escrita(i,'0');
	} 
}

#endif /* LIMA_H_ */
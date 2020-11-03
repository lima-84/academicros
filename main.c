#include <avr/io.h>
#include <avr/interrupt.h>
#include "lima.h"

char horas[2] = {0,8}, minutos[2] = {5,5}, lotacao = 3;
volatile char flag_cliente_apos_horario = 0;
volatile char flag_adm = 0, flag_atualiza_horario = 0;

/* atualiza_hora:
 * Incrementa o rel�gio de um minuto
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
 *	Imprime o hor�rio atual no display LCD
 */
void imprime_hora(){
	LCD_caractere((LCD_HORA & 0x0F) | LCD_LINHA_UM,CMD);	// Posiciona o cursor
	// Imprime hor�rio, transformando o n�mero em seu equivalente ASCII (+ '0')
	LCD_caractere(horas[0] + '0',DADO);
	LCD_caractere(horas[1] + '0',DADO);
	LCD_caractere(':',DADO);
	LCD_caractere(minutos[0] + '0',DADO);
	LCD_caractere(minutos[1] + '0',DADO);
}

/*	imprime_lotacao:
 *	Imprime a lota��o atual no display LCD
 */
void imprime_lotacao(){
	LCD_caractere((LCD_LOTACAO & 0x0F) | LCD_LINHA_DOIS,CMD); // Posiciona o cursor
	// Imprime lota��o, transformando o n�mero em seu equivalente ASCII (+ '0')
	LCD_caractere(lotacao + '0',DADO);
}

/*	imprime_mensagem_padrao:
 *	Imprime a mensagem padr�o (hora e lota��o)
 */
void LCD_mensagem_padrao(){
	LCD_caractere(LCD_LINHA_UM,CMD);	// Posiciona o cursor na primeira linha
	LCD_string("Hora:      ");
	imprime_hora();
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);	// Posiciona o cursor na segunda linha
	LCD_string("Lotacao:       ");
	imprime_lotacao();
}

/* LCD_mensagem_cliente_apos_horario:
 * Imprime uma mensagem avisando que existem clientes ap�s fechamento
 */
void LCD_mensagem_cliente_apos_horario(){
	LCD_caractere(LCD_LINHA_UM,CMD);	// Posiciona o cursor na primeira linha
	LCD_string("AVISO:     ");

	LCD_caractere(LCD_LINHA_DOIS,CMD);	// Posiciona o cursor na segunda linha
	LCD_string("Pessoas dentro:");
}

/* ISR(TIMER1_OVF_vect):
 * Trata interrup��o por overflow do Timer1, atualiza a hora no display LCD
 * Tempo de execu��o da fun��o ~1ms
 */
ISR(TIMER1_OVF_vect){
	// Checa se � a segunda interrup��o consecutiva (ou seja, 1 segundo se passou)
	if(flag_atualiza_horario == 1){
		// Atualiza e imprime a hora no display LCD
		atualiza_hora();
		imprime_hora();
		flag_atualiza_horario = 0;
	}
	else{
		// Se n�o atualizou nesta interrup��o, deve atualizar na pr�xima
		flag_atualiza_horario = 1;
	}
	TCNT1 = 65536 - ATR_500_MS; // Reinicializa o Timer1
	
	// Se houver algum cliente na academia ap�s o hor�rio
	if(flag_cliente_apos_horario == 1){
		//PORTT ^= (1 << TESTE);		// Inverte o pino de teste
		PORTB ^= (1 << LED);	// Inverte o LED
	}
}

/* EEPROM_carrega_horarios:
 * Carrega a lista de hor�rios na EEPROM
 */
void EEPROM_carrega_horarios(unsigned char lista_horarios[]){
	short i;
	for(i = 0; i < TOTAL_CLIENTES; i++){
		EEPROM_escrita(i, lista_horarios[i]);
	}
}

void msg_erro_cliente_entrando(char codigo_erro);		//funcao que imprime motivo do cliente nao poder entrar (FALTA IMPLEMENTAR)
//swtich case com o codigo de erro

//funcao responsavel por ver se o cliente pode entrar no momento, e pelos processos seguintes
void cliente_entrada(short num_cliente, short *lista_hora_entrada, char *lista_planos){
	
	//verifica, respectivamente, se o cliente nao tem conta bloqueada, se a academia ta cheia, ou se a academia ja fechou
	if(lista_planos[num_cliente] == 'X' || lotacao == 5 || (horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
		//verifica individualmente cada condicao de impedimento, pra poder mostrar a msg de erro correspondente
		if(lista_planos[num_cliente] == 'X'){
			msg_erro_cliente_entrando(1);
		}
		if( lotacao == 5){
			msg_erro_cliente_entrando(2);
		}
		if(horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
			msg_erro_cliente_entrando(3);
		}
	}
	else{						//caso o cliente possa entrar, sao feitos os procedimentos
		lotacao++;				//incrementa o contador de pessoas dentro da academia
		
		//salva horario de entrada, convertendo pra minutos
		lista_hora_entrada[num_cliente] = ((int) horas[0])*600 + ((int) horas[1])*60 + ((int) minutos[0])*10 + ((int) minutos[1]);
	}
}


//funcao para reduzir numero de horas da conta do cliente
// tem que reduzir a lotacao e ver que o cliente nao tem conta master antes de chamar a funcao
void cliente_saida(short num_cliente, short *lista_horarios, short *lista_hora_entrada, char *lista_planos){
	
	//conversao de char pra short da hora atual 
	short t_atual_minutos = ((int) horas[0])*600 + ((int) horas[1])*60 + ((int) minutos[0])*10 + ((int) minutos[1]);
	
	short t_dentro;									//numero de minutos que o cliente ficou dentro
	if(t_atual_minutos < lista_hora_entrada[num_cliente]){					//se o cliente estiver saindo depois da meia noite
		t_atual_minutos = t_atual_minutos + 1440;											//adiciona 24 horas na hora atual pra fazer os calculos
	}
	
	t_dentro = t_atual_minutos - lista_hora_entrada[num_cliente];
	
	if(t_dentro >= lista_horarios[num_cliente]){							//se o cliente estourar o numero de horas
		lista_horarios[num_cliente] = 0;				//zera as horas restantes na conta
		lista_planos[num_cliente] = 'X';				//deixa a conta do cliente bloqueada
	}
	else{
		lista_horarios[num_cliente] = lista_horarios[num_cliente] - t_dentro;		//se as horas nao tiverem estourado, faz a subtracao
	}
}


int main(void){
	
	// Rela��o de clientes
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
	
	unsigned short lista_horarios [11] = {		//mostra quantas horas (na vdd mostra em minutos) restantes cada login ainda tem
		// Conta master tem 9999, conta bloqueada tem 0
		//
		9999,	
		MIN_PREMIUM,
		MIN_BASICA,
		9999,
		MIN_PREMIUM,
		MIN_PREMIUM,
		0,
		9999,
		MIN_BASICA,
		0,
		MIN_PREMIUM
	};
	
	unsigned short lista_hora_entrada [11] = {		//mostra que horas (na vdd minutos) cada cliente entrou, conta master e bloqueada tem 0
		0,	
		1000,
		1700,
		0,
		2200,
		1100,
		0,
		0,
		1345,
		0,
		1640
	};
	

	// EEPROM
	EECR &= (~(1 << EEPM1) & ~(1 << EEPM0));	// Escolhe o modo at�mico (00)
	EEPROM_carrega_horarios(lista_horarios);
	
	unsigned char teste_horarios[TOTAL_CLIENTES];
	short i;
	for(i = 0; i < TOTAL_CLIENTES; i++){
		teste_horarios[i] = EEPROM_leitura(i);
	}
	
	// Configura��o Timer0
	TCCR0A = T0OVR;			// Modo do Timer0
	TCCR0B = PRE_256;		// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR0 |= (1 << 0);		// Zera o flag de overflow do Timer0

	// Configura��o Timer1
	TCCR1A = T1OVR;			// Modo do Timer1
	TCCR1B = PRE_256;		// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR1 |= (1 << 0);		// Zera o flag de overflow do Timer1
	TIMSK1 |= (1 << 0);		// Ativa interrup��o por overflow no Timer1

	// Configura��o Timer2
	TCCR2A = T2OVR;			// Modo do Timer2
	TCCR2B = PRE2_256;		// Prescaler de 256 (FREQ_CPU = 16MHz -> FREQ TIMER = 62.5kHz)
	TIFR2 |= (1 << 0);		// Zera o flag de overflow do Timer2
	
	DDRB |= (1 << 5);		// Seta pino PB5 como output (LED da placa)
	PORTB &= ~(1 << 5);		// Inicializa PB5 com zero (LED desligado)
	
	DDRT |= (1 << TESTE);	// Seta pino de teste como output
	PORTT &= ~(1 << TESTE);	// Inicializa o pino de teste em zero
	
	DDRC = 0xFF;			// Seta PORTC como sa�da para testar EEPROM
	PORTC = 0x00;			// Inicializa PORTC em zero
	
	// Inicializa��o dos pinos do LCD
	DDRB |= 0x0F;			// Seta os pinos de dados do LCD como output (PORT B)
	DDRD |= (1 << RS);		// Seta o pino de RS do LCD como output (PORT D)
	DDRB |= (1 << EN);		// Seta o pino de EN do LCD como output (PORT B)
	
	// Inicializa��o dos pinos do teclado
	DDRD |=	0x0F;			// Seta as linhas do teclado como output (PORT D)
	DDRD &= ~(0x70);		// Seta as colunas do teclado como input (PORT D)
	PORTD |= 0x0F;			// Inicializa as linhas do teclado
	PIND |= 0x70;			// Aciona resistores de pull-up das colunas
	
	init_display();
	LCD_mensagem_padrao();
	
	sei();					// Habilita interrup��es
	TCNT1 = 65536 - ATR_500_MS;
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	
	/*for(i = 0; i < TOTAL_CLIENTES; i++){
		LCD_caractere(teste_horarios[i],DADO);
	}*/

	volatile unsigned char tecla = 0;
    
	while (1){
		// Checa se j� passou do hor�rio de funcionamento
		if((horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
			// Checa se algu�m ainda est� na academia
			if(lotacao > 0){
				flag_cliente_apos_horario = 1;
				LCD_mensagem_cliente_apos_horario();
				PORTT ^= (1 << TESTE);		// Inverte o pino de teste			
			}
		}
		
		tecla = TCL_checa_teclado();
		if(tecla != ' ' && tecla != 'E'){
			LCD_caractere(LCD_LINHA_UM,CMD);
			LCD_caractere(tecla,DADO);
		}
		//atraso_timer1(65536 - ATR_200_MS);
		
    }
}


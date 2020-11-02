#include <avr/io.h>
#include <avr/interrupt.h>
#include "lima.h"

char horas[2] = {1,3}, minutos[2] = {5,5}, lotacao = 3;
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
	LCD_caractere((LCD_HORA & 0x0F) | LCD_LINHA_UM,CMD);	// Posiciona o cursor
	// Imprime horário, transformando o número em seu equivalente ASCII (+ '0')
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
	LCD_caractere((LCD_LOTACAO & 0x0F) | LCD_LINHA_DOIS,CMD); // Posiciona o cursor
	// Imprime lotação, transformando o número em seu equivalente ASCII (+ '0')
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

/* LCD_mensagem_cliente_apos_horario:
 * Imprime uma mensagem avisando que existem clientes após fechamento
 */
void LCD_mensagem_cliente_apos_horario(){
	LCD_caractere(LCD_LINHA_UM,CMD);	// Posiciona o cursor na primeira linha
	LCD_string("AVISO:     ");

	LCD_caractere(LCD_LINHA_DOIS,CMD);	// Posiciona o cursor na segunda linha
	LCD_string("Pessoas dentro:");
}

void LCD_mensagem_erro_login(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("   ERRO!   ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de login  ");
	
}

void LCD_mensagem_erro_senha(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("   ERRO!   ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de senha  ");
}

void LCD_mensagem_erro_conta(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("   ERRO!   ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de conta  ");
}

void LCD_mensagem_erro_lotacao(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("   ERRO!   ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Local lotado  ");
}

void LCD_mensagem_erro_horario(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("   ERRO!   ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Fechado       ");
}

void LCD_mensagem_adm_opcoes(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("*-Hora     ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("#-Cliente      ");
}

void LCD_mensagem_adm_horario(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("*-Hora    ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Horario:       ");
}

void LCD_mensagem_adm_cliente(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("#-Cliente  ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Cliente:       ");
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
		LCD_mensagem_cliente_apos_horario();
	}
}

/* EEPROM_carrega_horarios:
 * Carrega a lista de horários na EEPROM
 */
void EEPROM_carrega_horarios(unsigned char lista_horarios[]){
	short i;
	for(i = 0; i < TOTAL_CLIENTES; i++){
		EEPROM_escrita(i, lista_horarios[i]);
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
	
	unsigned char lista_horarios [11] = {
		// Conta master vira '*', código 42 (0x2A) ASCII
		'*',	
		'2',
		'3',
		'*',
		'5',
		'9',
		'0',
		'*',
		'6',
		'0',
		'2'
	};

	// EEPROM
	EECR &= (~(1 << EEPM1) & ~(1 << EEPM0));	// Escolhe o modo atômico (00)
	EEPROM_carrega_horarios(lista_horarios);
	
	unsigned char teste_horarios[TOTAL_CLIENTES];
	short i;
	for(i = 0; i < TOTAL_CLIENTES; i++){
		teste_horarios[i] = EEPROM_leitura(i);
	}
	
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
	
	DDRC = 0xFF;			// Seta PORTC como saída para testar EEPROM
	PORTC = 0x00;			// Inicializa PORTC em zero
	
	// Inicialização dos pinos do LCD
	DDRB |= 0x0F;			// Seta os pinos de dados do LCD como output (PORT B)
	DDRD |= (1 << RS);		// Seta o pino de RS do LCD como output (PORT D)
	DDRB |= (1 << EN);		// Seta o pino de EN do LCD como output (PORT B)
	
	// Inicialização dos pinos do teclado
	DDRD |=	0x0F;			// Seta as linhas do teclado como output (PORT D)
	DDRD &= ~(0x70);		// Seta as colunas do teclado como input (PORT D)
	PORTD |= 0x0F;			// Inicializa as linhas do teclado
	PIND |= 0x70;			// Aciona resistores de pull-up das colunas
	
	init_display();
	LCD_mensagem_padrao();
	
	sei();					// Habilita interrupções
	TCNT1 = 65536 - ATR_500_MS;
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	
	/*for(i = 0; i < TOTAL_CLIENTES; i++){
		LCD_caractere(teste_horarios[i],DADO);
	}*/

	volatile unsigned char tecla = 0;
    
	while (1){
		// Checa se já passou do horário de funcionamento
		if((horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
			// Checa se alguém ainda está na academia
			if(lotacao > 0){
				flag_cliente_apos_horario = 1;
				PORTT ^= (1 << TESTE);		// Inverte o pino de teste			
			}
		}
		
		tecla = TCL_checa_teclado();
	
		if(tecla == '1'){
			LCD_mensagem_erro_login();
		}
		if(tecla == '2'){
			LCD_mensagem_erro_senha();
		}
		if(tecla == '3'){
			LCD_mensagem_erro_conta();
		}
		if(tecla == '4'){
			LCD_mensagem_erro_lotacao();
		}
		if(tecla == '5'){
			LCD_mensagem_erro_horario();
		}
		if(tecla == '6'){
			LCD_mensagem_adm_opcoes();
		}
		if(tecla == '7'){
			LCD_mensagem_adm_horario();
		}
		if(tecla == '8'){
			LCD_mensagem_adm_cliente();
		}
		
    }
}


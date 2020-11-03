#include <avr/io.h>
#include <avr/interrupt.h>
#include "lima.h"

char horas[2] = {1,3}, minutos[2] = {5,5}, lotacao = 3;
volatile char flag_cliente_apos_horario = 0;
volatile char flag_adm = 0, flag_atualiza_horario = 0;
volatile char flag_tecla_digitada = 0, flag_aguarda_msg = 0;
volatile char flag_cliente_autorizado = 0;

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

void LCD_mensagem_login(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("Login:          ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("                ");
	
	LCD_caractere((LCD_LOGIN & 0x0F) | LCD_LINHA_UM,CMD);
	LCD_caractere(LCD_CBLINK,CMD);
}

void LCD_mensagem_senha(){
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Senha:");
}


void LCD_mensagem_erro_login(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de login  ");
	
}

void LCD_mensagem_erro_senha(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de senha  ");
}

void LCD_mensagem_erro_conta(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de conta  ");
}

void LCD_mensagem_erro_lotacao(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Local lotado  ");
}

void LCD_mensagem_erro_horario(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Fechado       ");
}

void LCD_mensagem_adm_opcoes(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("1-Hora          ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("2-Cliente      ");
}

void LCD_mensagem_adm_horario(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("1-Hora          ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Horario: ");
}

void LCD_mensagem_adm_cliente(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("2-Cliente       ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Cliente: ");
}

void LCD_mensagem_entrada(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("Entrada aprovada");

	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("                ");
}

void atraso_mensagem(){
	for(int i = 0; i < 2500; i++){
		atraso_timer2(256-ATR_1000);
	}
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
		if(flag_tecla_digitada == 0){
			imprime_hora();
		}
		flag_atualiza_horario = 0;
	}
	else{
		// Se não atualizou nesta interrupção, deve atualizar na próxima
		flag_atualiza_horario = 1;
	}
	TCNT1 = 65536 - ATR_500_MS; // Reinicializa o Timer1
	
	// Se houver algum cliente na academia após o horário
	if(flag_cliente_apos_horario == 1){
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

//funcao responsavel por ver se o cliente pode entrar no momento, e pelos processos seguintes
char cliente_entrada(short num_cliente, unsigned short *lista_hora_entrada, char *lista_planos){
	
	//verifica, respectivamente, se o cliente nao tem conta bloqueada, se a academia ta cheia, ou se a academia ja fechou
	if(lista_planos[num_cliente] == 'X' || lotacao == 5 || (horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
		//verifica individualmente cada condicao de impedimento, pra poder mostrar a msg de erro correspondente
		if(lista_planos[num_cliente] == 'X'){
			LCD_mensagem_erro_conta();
			atraso_mensagem();
		}
		if(lotacao == 5){
			LCD_mensagem_erro_lotacao();
			atraso_mensagem();
		}
		if((horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
			LCD_mensagem_erro_horario();
			atraso_mensagem();
		}
		
		return 0;
		
	}
	else{						//caso o cliente possa entrar, sao feitos os procedimentos
		lotacao++;				//incrementa o contador de pessoas dentro da academia
		
		//salva horario de entrada, convertendo pra minutos
		lista_hora_entrada[num_cliente] = hhmm_para_minutos(horas,minutos);
		
		return 1;
	}
}


//funcao para reduzir numero de horas da conta do cliente
// tem que reduzir a lotacao e ver que o cliente nao tem conta master antes de chamar a funcao
void cliente_saida(short num_cliente, unsigned short *lista_horarios, unsigned short *lista_hora_entrada, char *lista_planos){
	
	//conversao de char pra short da hora atual 
	short t_atual_minutos = hhmm_para_minutos(horas,minutos);
	
	short t_dentro;											//numero de minutos que o cliente ficou dentro
	if(t_atual_minutos < lista_hora_entrada[num_cliente]){	//se o cliente estiver saindo depois da meia noite
		t_atual_minutos = t_atual_minutos + 1440;			//adiciona 24 horas na hora atual pra fazer os calculos
	}
	
	t_dentro = t_atual_minutos - lista_hora_entrada[num_cliente];
	
	if(t_dentro >= lista_horarios[num_cliente]){		//se o cliente estourar o numero de horas
		lista_horarios[num_cliente] = 0;				//zera as horas restantes na conta
		lista_planos[num_cliente] = 'X';				//deixa a conta do cliente bloqueada
	}
	else{
		lista_horarios[num_cliente] = lista_horarios[num_cliente] - t_dentro;		//se as horas nao tiverem estourado, faz a subtracao
	}
}

void LCD_dados_cliente(short cliente, char plano, short tempo_restante){
	char str_cliente[6], str_horas[4], str_minutos[3];
	
	LCD_caractere(LCD_LINHA_UM, CMD);
	LCD_string("Plano ");
	switch(plano){
		case 'B': LCD_string("Basico  ");  break;
		case 'P': LCD_string("Premium ");  break;
		case 'M': LCD_string("Master  ");  break;
		case 'X': LCD_string("Bloqueado"); break;
	}
	

	LCD_caractere(LCD_LINHA_DOIS, CMD);
	itoa(cliente, str_cliente, 10);
	LCD_string(str_cliente);
	LCD_string("  ");
	
	if(plano != 'M'){	
		minutos_para_hhmm(tempo_restante,str_horas,str_minutos);
		
		if(str_horas[1] == '\0')
		LCD_caractere('0',DADO);
		LCD_string(str_horas);
		LCD_caractere(':', DADO);
		if(str_minutos[1] == '\0')
		LCD_caractere('0',DADO);
		LCD_string(str_minutos);
		
		LCD_string("   ");
	}
	else{
		LCD_string("         ");
	}
	
	atraso_mensagem();
	
}

int main(void){
	
	// Relação de clientes
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
		835,
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
	
	short lista_clientes_dentro [11] = {
		0,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	};
	

	// EEPROM
	EECR &= (~(1 << EEPM1) & ~(1 << EEPM0));	// Escolhe o modo atômico (00)
	//EEPROM_carrega_horarios(lista_horarios);
	
	/*unsigned char teste_horarios[TOTAL_CLIENTES];
	short i;
	for(i = 0; i < TOTAL_CLIENTES; i++){
		teste_horarios[i] = EEPROM_leitura(i);
	}*/
	
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
	
	// Interrpução externa
	DDRC |= (1 << 1);		// Seta PC1 como saída para forçar interrupções
	PORTC |= (1 << 1);
	
	PCICR |= (1 << PCIE1);	// Interrupt por troca de estado de PCINT1
	PCIFR |= (1 << PCIF1);	// Zera o flag de interrpução por troca de estado de PCINT1
	PCMSK1 |= (1 << PCINT9);	// Habilita interrupção por troca de estado de PC1
	
	init_display();
	LCD_mensagem_padrao();
	
	sei();					// Habilita interrupções
	TCNT1 = 65536 - ATR_500_MS;
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	
	/*for(i = 0; i < TOTAL_CLIENTES; i++){
		LCD_caractere(teste_horarios[i],DADO);
	}*/

	volatile unsigned char tecla = 0;
	volatile short cliente_atual = 0, senha_atual = 0, indice_cliente = 0;
	char str[10];
	volatile short horas_adm = 0, minutos_adm = 0, cliente_adm = 0, indice_cliente_adm = 0;
	
	while (1){
		// Checa se já passou do horário de funcionamento
		if((horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
			// Checa se alguém ainda está na academia
			if(lotacao > 0){
				flag_cliente_apos_horario = 1;	
			}
		}
		
		tecla = TCL_checa_teclado();
		// Se alguma tecla foi apertada, ativa o flag de tecla apertada
		if(tecla == '*'){
			flag_tecla_digitada = 1;
			LCD_mensagem_login();
		}
		if(flag_tecla_digitada == 1){
			// Lê e imprime números do teclado
			cliente_atual =	user_input(5,0);
			if(cliente_atual == 12345){
				// Se for administrador, seta o flag_adm
				flag_adm = 1;
			}
			else{
				// Se não for administrador, valida o cliente
				indice_cliente = valida_cliente(cliente_atual, lista_clientes);
				LCD_caractere(' ',DADO);
				LCD_caractere(indice_cliente + '0',DADO);
			}
						
			// Se for um usuário válido ou o administrador
			if(flag_adm == 1 || indice_cliente != 'E'){
				// Se o cliente já estava dentro, efetiva a saída
				if(lista_clientes_dentro[indice_cliente] == 1 && flag_adm == 0){
					lista_clientes_dentro[indice_cliente] = 0;
					// Teste do decremento de horario
					itoa(lista_horarios[indice_cliente],str,10);
					LCD_caractere(LCD_LINHA_UM,CMD);
					LCD_string(str);
					
					cliente_saida(indice_cliente,lista_horarios,lista_hora_entrada,lista_planos);
					
					itoa(lista_horarios[indice_cliente],str,10);
					LCD_caractere(LCD_LINHA_DOIS,CMD);
					LCD_string(str);
					
					imprime_hora();
					
				}
				else{
					// Pede a senha
					LCD_mensagem_senha();
					senha_atual = user_input(5,1);
					if(flag_adm == 0){
						if(senha_atual == lista_senhas[indice_cliente]){
							if(cliente_entrada(indice_cliente,lista_hora_entrada,lista_planos) == 1);
							LCD_mensagem_entrada();
						}
						else{
							LCD_mensagem_erro_senha();
						}
					}
					else{
						if(senha_atual == 12345){
							while(flag_adm == 1){
								LCD_mensagem_adm_opcoes();
								LCD_caractere(LCD_CBLINK,CMD);
								while(tecla != '1' && tecla != '2'){
									tecla = TCL_checa_teclado();	
								}
								if(tecla == '1'){
									LCD_mensagem_adm_horario();
								
									LCD_caractere((11 & 0x0F) | LCD_LINHA_DOIS,CMD);
									LCD_caractere(':',DADO);		
											
									LCD_caractere(LCD_CBLINK,CMD);
									LCD_caractere((9 & 0x0F) | LCD_LINHA_DOIS,CMD);
									horas_adm = user_input(2,0);
									LCD_caractere((12 & 0x0F) | LCD_LINHA_DOIS,CMD);
									minutos_adm = user_input(2,0);
								}
								else if(tecla == '2'){
									LCD_mensagem_adm_cliente();
									LCD_caractere((9 & 0x0F) | LCD_LINHA_DOIS,CMD);								
									LCD_caractere(LCD_CBLINK,CMD);
									cliente_adm = user_input(5,0);
									indice_cliente_adm = valida_cliente(cliente_adm,lista_clientes);
									if(indice_cliente_adm != 'E'){
										LCD_dados_cliente(lista_clientes[indice_cliente_adm],lista_planos[indice_cliente_adm],lista_horarios[indice_cliente_adm]);
									}
									else{
										LCD_mensagem_erro_login();
									}
								}
								else if(tecla == '#'){
									flag_adm = 0;
								}
								tecla = 0;
							}
						}
						else{
							LCD_mensagem_erro_senha();
						}
					}
				}
			}
			else{
				LCD_mensagem_erro_login();
			}
			
			atraso_mensagem();
			flag_tecla_digitada = 0;
			LCD_mensagem_padrao();
			
		}		
    }
}


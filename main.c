/**********************************************************************/
/*			Universidade Federal do Rio Grande do Sul				  */
/*				ENG04475 - Microprocessadores I						  */
/*	     Controle de acesso eletr�nico para academias	- AVR		  */
/*																	  */
/*	Alunos:															  */
/*	 00302389 - Enrique Leon Paillo Statquevios						  */
/*   00262546 - Guilherme Cabral									  */
/*   00288579 - Pedro Rodrigues de Lima								  */
/*																	  */
/**********************************************************************/

/* main.c: Fun��es mais espec�ficas e l�gica de programa			  */
#include "lima.h"														// Inclui a biblioteca lima.h

/**********************************************************************/
/*			Vari�veis globais: hor�rio, lota��o e flags				  */
/**********************************************************************/
char horas[2] = {0,9}, minutos[2] = {2,5}, lotacao;						// Horas, minutos e lota��o atuais
volatile char flag_cliente_apos_horario = 0;							// Indica se h� alguma cliente ap�s fechamento da academia
volatile char flag_msg_cliente_apos_horario = 1;						// Indica se o aviso de cliente ap�s hor�rio j� foi mostrada
volatile char flag_adm = 0;												// Indica se o administrador est� logado
volatile char flag_atualiza_horario = 0;								// Indica se o rel�gio deve ser incrementado
volatile char flag_tecla_digitada = 0;									// Indica se alguma tecla foi digitada
volatile char flag_caractere_especial = 0;								// Indica se algum caractere especial foi lido do teclado

/*	atualiza_hora:													  */
/*  Incrementa um miunto no rel�gio									  */
/*								     								  */
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

/*	imprime_hora:													  */
/*  Imprime o hor�rio atual no display LCD							  */
/*																	  */
void imprime_hora(){
	LCD_caractere((LCD_HORA & 0x0F) | LCD_LINHA_UM,CMD);	// Posiciona o cursor
	// Imprime hor�rio, transformando o n�mero em seu equivalente ASCII (+ '0')
	LCD_caractere(horas[0] + '0',DADO);
	LCD_caractere(horas[1] + '0',DADO);
	LCD_caractere(':',DADO);
	LCD_caractere(minutos[0] + '0',DADO);
	LCD_caractere(minutos[1] + '0',DADO);
}

/*	imprime_lotacao:												  */
/*	Imprime a lota��o atual no display LCD							  */
/*																	  */
void imprime_lotacao(){
	LCD_caractere((LCD_LOTACAO & 0x0F) | LCD_LINHA_DOIS,CMD); // Posiciona o cursor
	// Imprime lota��o, transformando o n�mero em seu equivalente ASCII (+ '0')
	LCD_caractere(lotacao + '0',DADO);
}

/*	atraso_mensagem:												  */
/*	Chama um atraso que permite a visualiza��o das mensagens		  */
/*																	  */
void atraso_mensagem(){
	// Atraso definido experimentalmente
	for(int i = 0; i < 2500; i++){
		atraso_timer2(256-ATR_1000);
	}
}

/*	LCD_mensagem_academicros:										  */
/*	Imprime a mensagem de inicializa��o 							  */
/*																	  */
void LCD_mensagem_academicros(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("  Academicros   ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Seja bem-vindo");
	
	atraso_mensagem();
}

/*	LCD_mensagem_padrao:											  */
/*	Imprime a mensagem padr�o (hora, lota��o)						  */
/*																	  */
void LCD_mensagem_padrao(){
	
	LCD_caractere(LCD_CSTATIC,CMD);		// Desabilita o cursor piscante
	
	LCD_caractere(LCD_LINHA_UM,CMD);	// Posiciona o cursor na primeira linha
	LCD_string("Hora:      ");
	imprime_hora();
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);	// Posiciona o cursor na segunda linha
	LCD_string("Lotacao:       ");
	imprime_lotacao();
}

/*	LCD_mensagem_cliente_apos_horario:								  */
/*	Imprime uma mensagem avisando que existem clientes ap�s o hor�rio */
/*																	  */
void LCD_mensagem_cliente_apos_horario(){
	LCD_caractere(LCD_LINHA_UM,CMD);	// Posiciona o cursor na primeira linha
	LCD_string("AVISO!     ");

	LCD_caractere(LCD_LINHA_DOIS,CMD);	// Posiciona o cursor na segunda linha
	LCD_string("Pessoas dentro:");
	imprime_lotacao();
	imprime_hora();
	
	flag_msg_cliente_apos_horario = 0;
}

/*	LCD_mensagem_login:												  */
/*	Imprime uma mensagem de requisi��o de login						  */
/*																	  */
void LCD_mensagem_login(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("Login:          ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("                ");
	
	LCD_caractere((LCD_LOGIN & 0x0F) | LCD_LINHA_UM,CMD);	// Posiciona o cursor na posi��o de login
	LCD_caractere(LCD_CBLINK,CMD);							// Ativa o cursor piscante
}

/*	LCD_mensagem_senha:												  */
/*	Imprime uma mensagem de requisi��o de senha						  */
/*																	  */
void LCD_mensagem_senha(){
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Senha:");
}

/*	LCD_mensagem_erro_login:										  */
/*	Imprime uma mensagem de erro de login							  */
/*																	  */
void LCD_mensagem_erro_login(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de login  ");
	
	atraso_mensagem();
	// Caso exista algum cliente ap�s o hor�rio, indica que a mensagem de erro deve ser reimpressa
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
}

/*	LCD_mensagem_erro_senha:										  */
/*	Imprime uma mensagem de erro de senha							  */
/*																	  */
void LCD_mensagem_erro_senha(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de senha  ");
	
	atraso_mensagem();	
	// Caso exista algum cliente ap�s o hor�rio, indica que a mensagem de erro deve ser reimpressa
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
}

/*	LCD_mensagem_erro_conta:										  */
/*	Imprime uma mensagem de erro de conta							  */
/*																	  */
void LCD_mensagem_erro_conta(){
		
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Erro de conta  ");
	
	atraso_mensagem();	
	
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
}

/*	LCD_mensagem_erro_conta_bloqueada:								  */
/*	Imprime uma mensagem de erro de conta bloqueada					  */
/*																	  */
void LCD_mensagem_erro_conta_bloqueada(){
		
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Conta bloqueada ");
	
	atraso_mensagem();
	
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
}

/*	LCD_mensagem_erro_lotacao:										  */
/*	Imprime uma mensagem de erro de lotacao							  */
/*																	  */
void LCD_mensagem_erro_lotacao(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Local lotado  ");
	
	atraso_mensagem();
	
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
}

/*	LCD_mensagem_erro_horario										  */
/*	Imprime uma mensagem de erro de horario							  */
/*																	  */
void LCD_mensagem_erro_horario(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("     ERRO!      ");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Fechado       ");
	
	atraso_mensagem();
	
		
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
}

/*	LCD_mensagem_adm_opcoes:										  */
/*	Imprime uma mensagem com as op��es do administrador			      */
/*																	  */
void LCD_mensagem_adm_opcoes(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("1-Hora       ADM");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("2-Cliente      ");
}

/*	LCD_mensagem_adm_horario:										  */
/*	Imprime uma mensagem relativa � troca de hor�rio do rel�gio	      */
/*																	  */
void LCD_mensagem_adm_horario(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("1-Hora       ADM");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Horario: hh:mm");
	
	LCD_caractere((9 & 0xFF) | LCD_LINHA_DOIS,CMD);
}

/*	LCD_mensagem_adm_cliente:										  */
/*	Imprime uma mensagem relativa � requisi��o de extrato de cliente  */
/*																	  */
void LCD_mensagem_adm_cliente(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("2-Cliente    ADM");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Cliente: ");
}

/*	LCD_mensagem_entrada:											  */
/*	Imprime uma mensagem de confirma��o de entrada				      */
/*																	  */
void LCD_mensagem_entrada(){
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("Entrada aprovada");

	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Bom proveito!     ");
	
	atraso_mensagem();
	
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
}

/*	LCD_mensagem_saida:												  */
/*	Imprime uma mensagem de confirma��o de sa�da				      */
/*	entrada: hor�rio de entrada em minutos							  */
/*	hora_saida: hora de sa�da (hh)	   								  */
/*  min_saida: minuto de sa�da (mm)									  */
/*	restante: tempo restante em minutos								  */
/*																	  */
void LCD_mensagem_saida(short entrada, char* hora_saida, char* min_saida, short restante){
	
	char hh[3], mm[3];									// Strings auxiliares de horas e minutos
		
	LCD_caractere(LCD_CSTATIC,CMD);						// Desabilita o cursor piscante
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("Saida confirmada");

	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Obrigado!");
	
	atraso_mensagem();
	if(entrada >= 0){									// Caso seja um cliente b�sico ou premium
		// Imprime hora de entrada
		minutos_para_hhmm(entrada,hh,mm);
		LCD_caractere(LCD_LINHA_UM,CMD);
		LCD_string("E:");
		LCD_string(hh);
		LCD_caractere(':',DADO);
		LCD_string(mm);
		
		// Imprime hora de sa�da
		LCD_string("  S:");
		LCD_caractere(hora_saida[0] + '0',DADO);
		LCD_caractere(hora_saida[1] + '0',DADO);
		LCD_caractere(':',DADO);
		LCD_caractere(min_saida[0] + '0',DADO);
		LCD_caractere(min_saida[1] + '0',DADO);
		
		// Imprime quantidade de horas restantes
		minutos_para_hhmm(restante,hh,mm);
		LCD_caractere(LCD_LINHA_DOIS,CMD);
		LCD_string("Restante: ");
		LCD_string(hh);
		LCD_caractere(':',DADO);
		LCD_string(mm);
		LCD_caractere(' ',DADO);
	}
	else if(entrada == -1){								// Caso seja uma conta master
		LCD_caractere(LCD_LINHA_DOIS,CMD);
		LCD_string("Conta Master    ");
	}
	else if(entrada == -2){								// Caso seja uma conta bloqueada
		LCD_caractere(LCD_LINHA_DOIS,CMD);
		LCD_string("Conta Bloqueada ");
	}
	
	while(TCL_checa_teclado() != '#');					// Aguarda o usu�rio apertar '#'
	
	if(flag_cliente_apos_horario == 1){
		flag_msg_cliente_apos_horario = 0;
	}
	
}

/*	LCD_mensagem_adm_cliente_horario:								  */
/*	Imprime uma mensagem relativa � mudan�a de horas de cliente		  */
/*																	  */
void LCD_mensagem_adm_cliente_horario(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("2-Cliente    ADM");
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("Horario: hh mm   ");
}

/*	LCD_mensagem_adm_horario_confirmacao							  */
/*	Imprime mensagem de confirma��o de mudan�a de hor�rio do cliente  */
/*																	  */
void LCD_mensagem_adm_horario_confirmacao(){
	
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("Horario alterado");
	
	atraso_mensagem();
}

/*	LCD_mensagem_adm_horario_erro									  */
/*	Imprime mensagem de erro de mudan�a de hor�rio do cliente		  */
/*																	  */
void LCD_mensagem_adm_horario_erro(){
	LCD_caractere(LCD_CSTATIC,CMD);
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	LCD_string("Horario invalido");
	
	atraso_mensagem();
}

/* ISR(TIMER1_OVF_vect):											  */
/* Interrup��o por overflow do Timer1, atualiza a hora no display LCD */
/* Ocorre a cada ~250ms (246ms)										  */
/* Escreve no pino de LED quando necess�rio							  */
/*																	  */
ISR(TIMER1_OVF_vect){
	// Checa se � a quarta interrup��o consecutiva (ou seja, 1 segundo se passou)
	if(flag_atualiza_horario == 3){
		// Atualiza e imprime a hora no display LCD
		atualiza_hora();		
		flag_atualiza_horario = 0;
		if(flag_tecla_digitada == 0){
			imprime_hora();
		}
	}
	else{
		// Se n�o atualizou nesta interrup��o, incrementa o contador
		flag_atualiza_horario++;
	}
	TCNT1 = 65536 - ATR_250_MS; // Reinicializa o Timer1
	
	// Se houver algum cliente na academia ap�s o hor�rio, pisca o LED duas vezes por segundo
	if(flag_cliente_apos_horario == 1){
		PORTB ^= (1 << LED);						// Inverte o pino do LED
		if(flag_msg_cliente_apos_horario == 0){		// Caso a mensagem n�o tenha sido impressa
			LCD_mensagem_cliente_apos_horario();	// Imprime a mensagem de cliente ap�s hor�rio
			flag_msg_cliente_apos_horario = 1;		// Indica que a mensagem de cliente ap�s hor�rio j� foi impressa
		}
	}
}

/* EEPROM_escreve_lista_horarios:									  */
/* Escreve a lista de hor�rios na EEPROM							  */
/*																	  */
void EEPROM_escreve_lista_horarios(unsigned short* lista_horarios){
	// Escreve no endere�o 2*i, pois cada hor�rio ocupa duas posi��es
	for(int i = 0; i < 2*TOTAL_CLIENTES - 1; i++)
	EEPROM_escreve_horario(2*i,lista_horarios[i]);
}

/* EEPROM_le_lista_horarios:										  */
/* L� a lista de hor�rios da EEPROM									  */
/*																	  */
void EEPROM_le_lista_horarios(unsigned short* lista_horarios){
	// L� o endere�o 2*i, pois cada hor�rio ocupa duas posi��es
	for(int i = 0; i < TOTAL_CLIENTES; i++){
		lista_horarios[i] = EEPROM_le_horario(2*i);
	}
}

/* cliente_entrada:													  */
/* Verifica e efetua as opera��es de entrada de cliente				  */
/* indice_cliente: �ndice do cliente no vetor de clientes			  */
/* lista_hora_entrada: lista com o hor�rio de entrada dos clientes	  */
/* lista_plano: lista de planos dos clientes						  */
/*																	  */
char cliente_entrada(short indice_cliente, unsigned short *lista_hora_entrada, char *lista_planos, unsigned short* lista_horarios){
	
	// Verifica, respectivamente, se o cliente possui conta bloqueada, se a lota��o est� no limite ou se j� passou do hor�rio de funcionamento
	if(lista_planos[indice_cliente] == 'X' || lista_horarios[indice_cliente] == 0){
		LCD_mensagem_erro_conta_bloqueada();
	}
	else if(lotacao == 5){
		LCD_mensagem_erro_lotacao();
	}
	else if((horas[0] == 0 && horas[1] <= 7) || (horas[0] == 2 && horas[1] == 3)){
		LCD_mensagem_erro_horario();
	}		
	else{											// Caso o cliente possa entrar	
													// Atualiza a lista de hor�rios
		if(lista_planos[indice_cliente] != 'M')		// Clientes master possuem hor�rio ilimitado
			lista_hora_entrada[indice_cliente] = hhmm_para_minutos(horas,minutos);
		
		return 1;	// Retorna indica��o de que a entrada foi permitida
	}
	
	return 0;		// Retorna indica��o de que a entrada foi negada
}

/* cliente_saida:													  */
/* Realiza as opera��es de sa�da de cliente							  */
/* indice_cliente: �ndice do cliente no vetor de clientes			  */
/* lista_horarios: lista com as horas restantes dos clientes		  */
/* lista_hora_entrada: lista com o hor�rio de entrada dos clientes	  */
/* lista_plano: lista de planos dos clientes						  */
/* lista_cliente_dentro: lista que indica se os clientes est�o dentro */
/*																	  */
void cliente_saida(short indice_cliente, unsigned short *lista_horarios, unsigned short *lista_hora_entrada, char *lista_planos, short *lista_clientes_dentro){
	
	short t_atual_minutos = hhmm_para_minutos(horas,minutos);						// Converte o hor�rio atual para minutos
	
	short t_dentro;																	// N�mero de minutos que o cliente ficou dentro
	if(t_atual_minutos < lista_hora_entrada[indice_cliente]){						// Se o cliente estiver saindo depois da meia noite
		t_atual_minutos = t_atual_minutos + 1440;									// Adiciona 24 horas na hora atual pra fazer os c�lculos
	}
	
	t_dentro = t_atual_minutos - lista_hora_entrada[indice_cliente];
	
	if(t_dentro >= lista_horarios[indice_cliente]){									// Se o cliente estourar o n�mero de horas
		lista_horarios[indice_cliente] = 0;											// Zera as horas restantes na conta
	}
	else{
		lista_horarios[indice_cliente] = lista_horarios[indice_cliente] - t_dentro;	// Se as horas nao tiverem estourado, faz a subtrac�o de hor�rios
	}
	
	lista_clientes_dentro[indice_cliente] = 0;										// Garante que o cliente n�o mais est� dentro
	
}

/* LCD_dados_cliente:												  */
/* Imprime extrato de cliente										  */
/* cliente:	login do cliente										  */
/* plano: plano do cliente											  */
/* tempo_restante: tempo restante do cliente, em minutos			  */
/*																	  */
void LCD_dados_cliente(short cliente, char plano, short tempo_restante){
	char str_cliente[6], str_horas[4], str_minutos[3];
	
	LCD_caractere(LCD_CSTATIC,CMD);									// Desativa o cursor piscante
	LCD_caractere(LCD_LINHA_UM, CMD);
	LCD_string("Plano ");
	switch(plano){													// Imprime o plano do cliente
		case 'B': LCD_string("Basico    ");  break;
		case 'P': LCD_string("Premium   ");  break;
		case 'M': LCD_string("Master    ");  break;
		case 'X': LCD_string("Bloqueado ");  break;
	}
	
	LCD_caractere(LCD_LINHA_DOIS, CMD);
	itoa(cliente, str_cliente, 10);									// Converte o login do cliente para string
	LCD_string(str_cliente);										// Imprime o login do cliente
	LCD_string("  ");
	
	if(plano != 'M'){												// Se n�o for plano master
		minutos_para_hhmm(tempo_restante,str_horas,str_minutos);	// Converte o tempo restante para string
		
		if(str_horas[1] == '\0')									// Adiciona zero � esquerda, se necess�rio
			LCD_caractere('0',DADO);
		LCD_string(str_horas);
		LCD_caractere(':', DADO);
		if(str_minutos[1] == '\0')									// Adiciona zero � esquerda, se necess�rio
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
	
	/***********************************************************************/
	/*							Vari�veis								   */
	/***********************************************************************/
	// Rela��o de clientes
	short lista_clientes [11] =																// Login dos clientes
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
	short lista_senhas [11] =																// Senhas dos clientes
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
	char lista_planos [11] =																// Planos dos clientes
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
	unsigned short lista_horarios_aux [11] =												// Vari�vel auxiliar: tempo restante de cada cliente em minutos
	{	  
		// Conta master tem 9999, conta bloqueada tem -2
		9999,
		MIN_PREMIUM,
		MIN_BASICA,
		9999,
		MIN_PREMIUM,
		MIN_PREMIUM,
		-2,
		9999,
		MIN_BASICA,
		-2,
		MIN_PREMIUM
	};
	unsigned short lista_hora_entrada [11] =												// Hor�rio em minutos em cada cliente entrou, conta master=-1 e bloqueada=-2
	{	  
		-1,	
		840,
		1700,
		-1,
		2200,
		1100,
		-2,
		-1,
		1345,
		-2,
		1640
	};	
	short lista_clientes_dentro [11] =														// Indica se o cliente est� dentro (1) ou fora (0) da academia
	{
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
	
	volatile unsigned char tecla = 0;														// Tecla a ser lida
	volatile short cliente_atual = 0, senha_atual = 0, indice_cliente = 0;					// Dados do cliente atual
	volatile short horas_adm = 0, minutos_adm = 0, cliente_adm = 0, indice_cliente_adm = 0; // Dados do cliente atual nas opera��es de ADM
	unsigned short lista_horarios[TOTAL_CLIENTES];											// Tempo restante de cada cliente em minutos
	
	/***********************************************************************/
	/*					Inicializa��o dos componentes					   */
	/***********************************************************************/
	
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
	DDRD |= (1 << RS);		// Seta o pino de RS do LCD como output		 (PORT D)
	DDRB |= (1 << EN);		// Seta o pino de EN do LCD como output		 (PORT B)
	
	// Inicializa��o dos pinos do teclado
	DDRD |=	0x0F;			// Seta as linhas do teclado como output	 (PORT D)
	DDRD &= ~(0x70);		// Seta as colunas do teclado como input	 (PORT D)
	PORTD |= 0x0F;			// Inicializa as linhas do teclado
	PIND |= 0x70;			// Aciona resistores de pull-up das colunas
	
	// EEPROM
	EECR &= (~(1 << EEPM1) & ~(1 << EEPM0));	// Escolhe o modo at�mico (00)
	
	// Calcula a lota��o inicial
	for(int i = 0; i < TOTAL_CLIENTES; i++){
		lotacao += lista_clientes_dentro[i];
	}
	
	init_display();											// Inicializa o display
	LCD_mensagem_academicros();								// Imprime a mensagem inicial
	
	LCD_caractere(LCD_LINHA_UM,CMD);
	
	EEPROM_clear();											// Limpa a EEPROM
	
	EEPROM_escreve_lista_horarios(lista_horarios_aux);		// Escreve a lista auxiliar na EEPROM
	EEPROM_le_lista_horarios(lista_horarios);				// L� a lista da EEPROM
	
	// Bloco de teste de leitura e escrita da EEPROM
	/*cli();
	LCD_caractere(LCD_LINHA_UM,CMD);
	for(int i = 0; i < TOTAL_CLIENTES; i++){
		LCD_caractere(lista_horarios[i],DADO);
		LCD_caractere(lista_horarios[i] >> 8,DADO);
		while(TCL_checa_teclado() != '#');

		if(i == 4){
			LCD_caractere(LCD_LINHA_DOIS,CMD);
		}
	}
	
	LCD_caractere(LCD_LINHA_DOIS,CMD);
	LCD_string("done");
	*/
	// Fim do bloco de teste
	
	sei();													 // Habilita interrup��es
	TCNT1 = 65536 - ATR_250_MS;								 // Inicializa o Timer1
	
	while (1){
		// Checa se j� passou do hor�rio de funcionamento
		if((horas[0] == 0 && horas[1] < 7) || (horas[0] == 2 && horas[1] == 3)){
			// Checa se algu�m ainda est� na academia
			if(lotacao > 0 && flag_cliente_apos_horario == 0){
				flag_cliente_apos_horario = 1;			// Habilita o flag de cliente apos hor�rio
				flag_msg_cliente_apos_horario = 0;		// Indica que a mensagem de cliente ap�s hor�rio pode ser impressa
			}else if(lotacao == 0){
				flag_cliente_apos_horario = 0;
				flag_msg_cliente_apos_horario = 1;
				LCD_mensagem_padrao();
			} 
		}else if(flag_cliente_apos_horario == 1){
			flag_cliente_apos_horario = 0;
			flag_msg_cliente_apos_horario = 1;
			PORTB &= ~(1 << LED);						// Garante que o pino do LED estar� desligado ap�s abertura
			LCD_mensagem_padrao();
		}
		
		if(flag_cliente_apos_horario == 0 && flag_msg_cliente_apos_horario == 1){
			LCD_mensagem_padrao();
		}
		
		tecla = TCL_checa_teclado();
		// Se a tecla '*' foi apertada, ativa o flag de tecla apertada
		if(tecla == '*'){
			flag_tecla_digitada = 1;
			LCD_mensagem_login();
		}
		if(flag_tecla_digitada == 1){
			// L� e imprime n�meros do teclado
			cliente_atual =	user_input(5,0,&flag_caractere_especial);
			if(cliente_atual == 12345){
				// Se for administrador, seta o flag_adm
				flag_adm = 1;
			}
			else{
				// Se n�o for administrador, valida o cliente
				indice_cliente = valida_cliente(cliente_atual, lista_clientes);
			}
						
			// Se for um usu�rio v�lido ou o administrador
			if(flag_adm == 1 || indice_cliente != 'E'){
				// Se o cliente j� estava dentro, efetiva a sa�da
				if(lista_clientes_dentro[indice_cliente] == 1 && flag_adm == 0){
					lista_clientes_dentro[indice_cliente] = 0;			// Indica que o cliente n�o mais est� dentro
					
					char hh_buffer[3], mm_buffer[3];					// Salva o hor�rio atual, para que a impress�o da mensagem de sa�da esteja coerente
					if(lista_planos[indice_cliente] != 'M'){
						hh_buffer[0] = horas[0];
						hh_buffer[1] = horas[1];
						mm_buffer[0] = minutos[0];
						mm_buffer[1] = minutos[1];
						cliente_saida(indice_cliente,lista_horarios,lista_hora_entrada,lista_planos,lista_clientes_dentro);
						
					}
					lotacao--;											// Decrementa a lota��o
					
					// Bloco para teste da EEPROM
					//char tt1, tt2;
					//LCD_caractere(LCD_LINHA_UM,CMD);
					cli();
					//tt1 = EEPROM_leitura(2*indice_cliente+1);
					//tt2 = EEPROM_leitura(2*indice_cliente);
					
					//LCD_caractere(tt1 + '!',DADO);
					//LCD_caractere(tt2 + '!',DADO);
					
					EEPROM_escreve_horario(2*indice_cliente,lista_horarios[indice_cliente]);
					
					//LCD_caractere(LCD_LINHA_DOIS,CMD);
					//tt1 = EEPROM_leitura(2*indice_cliente+1);
					//tt2 = EEPROM_leitura(2*indice_cliente);					
					sei();
					
					//LCD_caractere(tt1 + '!',DADO);
					//LCD_caractere(tt2 + '!',DADO);
					//while(TCL_checa_teclado() != '#');
					// Fim do bloco de teste
					LCD_mensagem_saida(lista_hora_entrada[indice_cliente],hh_buffer,mm_buffer, lista_horarios[indice_cliente]);
					
				}
				else{
					// Pede a senha
					LCD_mensagem_senha();
					senha_atual = user_input(5,1,&flag_caractere_especial);
					if(flag_adm == 0){																					// Se n�o for o administrador
						if(senha_atual == lista_senhas[indice_cliente]){												// Compara a senha com a lista
							if(cliente_entrada(indice_cliente,lista_hora_entrada,lista_planos,lista_horarios) == 1){	// Se a entrada do cliente for aprovada
								lotacao++;																				// Incrementa a lota��o
								lista_clientes_dentro[indice_cliente] = 1;												// Indica que o usu�rio est� dentro
								LCD_mensagem_entrada();																	// Exibe confirma��o de entrada
							}
						}
						else{																							// Se a senha estiver errada
							LCD_mensagem_erro_senha();																	// Exibe erro de senha
						}
					}
					else{																								// Se for o administrador
						if(senha_atual == 12345){																		// Checa a senha do administrador
							while(flag_adm == 1){
								// Imprime as op��es e aguarda sele��o, ou retorna ao menu ('#')
								LCD_mensagem_adm_opcoes();
								LCD_caractere(LCD_CBLINK,CMD);
								while(tecla != '1' && tecla != '2' && tecla != '#'){
									tecla = TCL_checa_teclado();	
								}
								if(tecla == '1'){								// Op��o de troca de hor�rio
									LCD_mensagem_adm_horario();
								
									LCD_caractere((11 & 0x0F) | LCD_LINHA_DOIS,CMD);
									LCD_caractere(':',DADO);		
									// Ativa o cursor, l� horas e, em seguida, minutos
									LCD_caractere(LCD_CBLINK,CMD);
									LCD_caractere((9 & 0x0F) | LCD_LINHA_DOIS,CMD);
									horas_adm = user_input(2,0,&flag_caractere_especial);
									LCD_caractere((12 & 0x0F) | LCD_LINHA_DOIS,CMD);
									minutos_adm = user_input(2,0,&flag_caractere_especial);
									
									// Strings auxiliares para convers�o
									char aux1[3];
									char aux2[3];
									// Converte para inteiro e trata os hor�rios
									itoa(horas_adm,aux1,10);
									itoa(minutos_adm,aux2,10);																	
									trata_zeros_horario(aux1);
									trata_zeros_horario(aux2);
									aux1[2] = '\0';
									aux2[2] = '\0';
									// Se o hor�rio entrado for v�lido								
									if(horas_adm >= 0 && horas_adm <= 23 && minutos_adm >=0 && minutos_adm < 60 && flag_caractere_especial == 0){
										// Atualiza as horas
										horas[0] = aux1[0] - '0';
										horas[1] = aux1[1] - '0';
										// Atualiza os minutos
										minutos[0] = aux2[0] - '0';
										minutos[1] = aux2[1] - '0';
										
										LCD_mensagem_adm_horario_confirmacao();	// Exibe mensagem de confirma��o de mudan�a de hora
										}
									else{
										flag_caractere_especial = 0;			// Zera o flag de caractere especial, para a pr�xima itera��o
										LCD_mensagem_adm_horario_erro();		// Exibe mensagem de erro de hor�rio
									}
								}
								else if(tecla == '2'){							// Op��o de extrato de cliente e troca das horas restantes
									LCD_mensagem_adm_cliente();
									LCD_caractere((9 & 0x0F) | LCD_LINHA_DOIS,CMD);
									LCD_caractere(LCD_CBLINK,CMD);
									// L� e valida o login do cliente
									cliente_adm = user_input(5,0,&flag_caractere_especial);
									indice_cliente_adm = valida_cliente(cliente_adm,lista_clientes);
									if(indice_cliente_adm != 'E'){	// Se o login for v�lido
										LCD_caractere(LCD_CSTATIC,CMD);
										// Exibe extrato de cliente
										LCD_dados_cliente(lista_clientes[indice_cliente_adm],lista_planos[indice_cliente_adm],lista_horarios[indice_cliente_adm]);
										// Se o cliente n�o for master (horas ilimitadas) nem bloqueado
										if(lista_planos[indice_cliente_adm] != 'M' && lista_planos[indice_cliente_adm] != 'X'){
											LCD_mensagem_adm_cliente_horario();
											LCD_caractere((11 & 0x0F) | LCD_LINHA_DOIS,CMD);
											LCD_caractere(':',DADO);
											// L� as horas
											LCD_caractere(LCD_CBLINK,CMD);
											LCD_caractere((9 & 0x0F) | LCD_LINHA_DOIS,CMD);
											horas_adm = user_input(2,0,&flag_caractere_especial);
											// L� os minutos
											LCD_caractere((12 & 0x0F) | LCD_LINHA_DOIS,CMD);
											minutos_adm = user_input(2,0,&flag_caractere_especial);
											// Checa consist�ncia dos minutos
											if(minutos_adm > 59 || flag_caractere_especial == 1){
												LCD_mensagem_adm_horario_erro(); // Exibe mensagem de hor�rio inv�lido
											}											
											else{
												// Bloco para teste da EEPROM
												//char tt1, tt2;
												//LCD_caractere(LCD_LINHA_UM,CMD);
												cli();
												//tt1 = EEPROM_leitura(2*indice_cliente_adm+1);
												//tt2 = EEPROM_leitura(2*indice_cliente_adm);
												
												//LCD_caractere(tt1 + '!',DADO);
												//LCD_caractere(tt2 + '!',DADO);
												// Atualiza o tempo restante do cliente, grava o dado na EEPROM e imprime o novo extrato
												lista_horarios[indice_cliente_adm] = 60*horas_adm + minutos_adm;
												
												EEPROM_escreve_horario(2*indice_cliente_adm,lista_horarios[indice_cliente_adm]);
												
												//LCD_caractere(LCD_LINHA_DOIS,CMD);
												//tt1 = EEPROM_leitura(2*indice_cliente_adm+1);
												//tt2 = EEPROM_leitura(2*indice_cliente_adm);
												sei();
												
												//LCD_caractere(tt1 + '!',DADO);
												//LCD_caractere(tt2 + '!',DADO);
												//while(TCL_checa_teclado() != '#');
												// Fim do bloco de teste
												
												LCD_dados_cliente(lista_clientes[indice_cliente_adm],lista_planos[indice_cliente_adm],lista_horarios[indice_cliente_adm]);
												
											}
										}
									}
									else{									// Se o login for inv�lido
										LCD_mensagem_erro_login();			// Exibe mensagem de erro de login
									}
								}
								else if(tecla == '#'){						// Cancelamento de opera��o
									flag_adm = 0;							// Sai do modo de administrador
									if(flag_cliente_apos_horario == 1){
										flag_msg_cliente_apos_horario = 0;	// Indica que a mensagem de cliente ap�s hor�rio pode ser impressa
									}
								}
								tecla = 0;									// Limpa o valor da tecla
							}
						}
						else{												// Se a senha n�o estiver correta
							LCD_mensagem_erro_senha();						// Exibe erro de senha
							if(flag_adm == 1){
								flag_adm = 0;								// Sai do modo de administrador, se necess�rio
								if(flag_cliente_apos_horario == 1){
									flag_msg_cliente_apos_horario = 0;		// Indica que a mensagem de cliente ap�s hor�rio pode ser impressa
								}
							}
						}
					}
				}
			}
			else{															// Se o login n�o estiver correto
				LCD_mensagem_erro_login();									// Exibe erro de login
			}			
			flag_tecla_digitada = 0;										// Desativa o flag de tecla digitada
			
		}		
    }
}


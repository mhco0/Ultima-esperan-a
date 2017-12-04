#include <stdio.h>
#include <time.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define LARGURA_TELA 1300
#define ALTURA_TELA 710

#define MAX_PLAYERS 2

typedef struct CLASSES{
	int HP;
	int HP_atual;
	int MP;
	int MP_atual;
	int defesa;
	int dano;
	long long int xp;
}CLASSES;
CLASSES arqueiro, guerreiro, mago;

typedef struct JOGADORES{
	CLASSES classe;
	char nome[16];
	int posicao[2];
	int id;
	char meuTime;
	char floor;
}JOGADORES;

typedef struct TIME{
	int princessa;
	int torre;
	int torre2;
	JOGADORES jogadores[MAX_PLAYERS/2];
}TIME;
TIME timeDireita, timeEsquerda;

ALLEGRO_DISPLAY *janela = NULL;

ALLEGRO_BITMAP *mainFake=NULL, *apresenta=NULL, *backgroundBitMap=NULL, *backgroundBitMapCreditos=NULL, *mainFakeCreditos=NULL;
ALLEGRO_BITMAP *caixa=NULL, *caixaTexto=NULL, *noJoinGame=NULL, *joinGame=NULL, *caixaEsquerda=NULL, *caixaDireita=NULL;
ALLEGRO_BITMAP *vs=NULL;

ALLEGRO_SAMPLE *mainFakeAudio=NULL, *fundoSound;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_EVENT eventoJanela, eventoMouse, eventoKeyboard, eventoTimer;
ALLEGRO_TIMER *timer = NULL;
int mouseX=0, mouseY=0;
int res_x_comp, res_y_comp;

ALLEGRO_EVENT_QUEUE *initJanela(){
	ALLEGRO_EVENT_QUEUE *fila_eventos;
	ALLEGRO_DISPLAY_MODE   disp_data;
	ALLEGRO_BITMAP *icone=NULL;

	fila_eventos = al_create_event_queue();

	ALLEGRO_MONITOR_INFO info;

	al_get_monitor_info(0, &info); //pega as info do monitor do pc

	res_x_comp = info.x2 - info.x1; //largura 
	res_y_comp = info.y2 - info.y1; //altura

	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

	janela = al_create_display(res_x_comp, res_y_comp);

	float red_x = res_x_comp/ (float) LARGURA_TELA;
	float red_y = res_y_comp/ (float) ALTURA_TELA;

	ALLEGRO_TRANSFORM transformar;

	al_identity_transform(&transformar);
	al_scale_transform(&transformar, red_x, red_y);
	al_use_transform(&transformar);

	al_register_event_source(fila_eventos, al_get_display_event_source(janela));

	al_set_window_title(janela, "Liga das Lendas");

	icone = al_load_bitmap("icon.png");

	al_set_display_icon(janela, icone);

	al_clear_to_color(al_map_rgb(255, 255, 255));

   	al_flip_display();

   	return fila_eventos;
}

ALLEGRO_EVENT_QUEUE *initMouse(){
	al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

	ALLEGRO_EVENT_QUEUE *fila_eventos;

	fila_eventos = al_create_event_queue();

	al_register_event_source(fila_eventos, al_get_mouse_event_source());

	return fila_eventos;
}

ALLEGRO_EVENT_QUEUE *initKeyboard(){
	ALLEGRO_EVENT_QUEUE *fila_eventos;

	fila_eventos=al_create_event_queue();

	al_register_event_source(fila_eventos, al_get_keyboard_event_source());

	return fila_eventos;
}

ALLEGRO_EVENT_QUEUE *initTimer(){
	ALLEGRO_EVENT_QUEUE *fila_eventos;

	fila_eventos=al_create_event_queue();

	timer = al_create_timer(0.05);

	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

	al_start_timer(timer);

	return fila_eventos;
}

void initImagens(){
	mainFake = al_load_bitmap("mainFake.png");

	apresenta = al_load_bitmap("apresenta.png");

	backgroundBitMap = al_load_bitmap("background.png");

	backgroundBitMapCreditos = al_load_bitmap("backgroundBitMapCreditos.jpg");

	mainFakeCreditos = al_load_bitmap("mainFakeCreditos.png");

	caixa = al_load_bitmap("caixa.png");

	caixaTexto = al_load_bitmap("caixaTexto.png");

	noJoinGame = al_load_bitmap("noJoinGame.png");

	joinGame = al_load_bitmap("joinGame.png");

	caixaEsquerda = al_load_bitmap("caixaEsquerda.png");

	caixaDireita = al_load_bitmap("caixaDireita.png");

	vs = al_load_bitmap("vs.png");
}

void initAudios(){
	mainFakeAudio = al_load_sample("mainFakeAudio.wav");
	fundoSound = al_load_sample("fundoSound.wav");
}

ALLEGRO_EVENT verificaEventos(ALLEGRO_EVENT_QUEUE *fila_eventos){
	ALLEGRO_EVENT evento;

	ALLEGRO_TIMEOUT timeout;
    al_init_timeout(&timeout, 0.006);

	al_wait_for_event_until(fila_eventos, &evento, &timeout);

	return evento;
}

void verificaClose(){
	if(eventoJanela.type==ALLEGRO_EVENT_DISPLAY_CLOSE) exit(0);
}

char backgroundColor(char color){
	static char colorAtual;

	if(color!='1') colorAtual=color;
	else return colorAtual;

	return '1';
}

int apresentacao(){
	int retorno=0;
	static int estado=0, firstTime=1;
	static float transparencia=0;

	switch(estado){
		case 0: if(firstTime==1){
					al_play_sample(mainFakeAudio, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);

					backgroundColor('b');

					firstTime=0;
				}
				else{
					al_draw_tinted_bitmap(mainFake, al_map_rgba_f(1, 1, 1, transparencia/100.0), 40, 250, 0);

					if(eventoTimer.type == ALLEGRO_EVENT_TIMER) transparencia+=2.0;

					if(transparencia>=100.0) estado=1;
				}
				break;

		case 1: al_draw_tinted_bitmap(mainFake, al_map_rgba_f(1, 1, 1, transparencia/100.0), 40, 250, 0);

				if(eventoTimer.type == ALLEGRO_EVENT_TIMER) transparencia-=5;

				if(transparencia<=0){
					estado=2;

					firstTime=1;
				}
				break;

		case 2: if(firstTime==1){
					backgroundColor('p');

					firstTime=0;
				}
				else{
					al_draw_tinted_bitmap(apresenta, al_map_rgba_f(transparencia/100.0, transparencia/100.0, transparencia/100.0, 1), 525, 300, 0);

					if(eventoTimer.type == ALLEGRO_EVENT_TIMER) transparencia+=2.0;

					if(transparencia>=100.0){
						estado=3;

						firstTime=1;
					}
				}
				break;		

		case 3: al_draw_tinted_bitmap(apresenta, al_map_rgba_f(transparencia/100.0, transparencia/100.0, transparencia/100.0, 1), 525, 300, 0);

				if(eventoTimer.type == ALLEGRO_EVENT_TIMER) transparencia-=5;		

				if(transparencia<=0){
					estado=0;

					firstTime=1;

					retorno=1;
				}
				break;
	}

	return retorno;
}

void background(){
	switch(backgroundColor('1')){
		case 'b': al_clear_to_color(al_map_rgb(255, 255, 255));
				  break;

		case 'p': al_clear_to_color(al_map_rgb(0, 0, 0));
				  break;

		case 'I': al_draw_bitmap(backgroundBitMap, 50, 0, 0);
				  break;
	}
}

int inicio(){
	int retorno=1;
	static int estado=0, firstTime=1, opc=1;
	static float transparencia=0;

	al_play_sample(fundoSound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

	switch(estado){
		case 0:	al_draw_tinted_bitmap(backgroundBitMap, al_map_rgba_f(transparencia/100.0, transparencia/100.0, transparencia/100.0, 1), 0, -100, 0);

				if(eventoTimer.type == ALLEGRO_EVENT_TIMER) transparencia+=1.0;

				if(transparencia>=100) estado=1;
				break;

		case 1:	al_draw_bitmap(backgroundBitMap, 0, -100, 0);

				if(eventoKeyboard.type == ALLEGRO_EVENT_KEY_DOWN){
					switch(eventoKeyboard.keyboard.keycode){
	                	case ALLEGRO_KEY_UP: if(opc>1) opc--;
                    					 	 break;

	                	case ALLEGRO_KEY_DOWN: if(opc<3) opc++;
	                      				       break;

	                    case ALLEGRO_KEY_ENTER: retorno=1+opc;
	                    						break;  				       
  				   }
				}

				switch(opc){
					case 1: al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, ALTURA_TELA-300, ALLEGRO_ALIGN_CENTRE, "Novo Jogo");
							al_draw_text(fonte, al_map_rgb(128, 128, 128),  LARGURA_TELA/2, ALTURA_TELA-200, ALLEGRO_ALIGN_CENTRE, "Créditos");
							al_draw_text(fonte, al_map_rgb(128, 128, 128),  LARGURA_TELA/2, ALTURA_TELA-100, ALLEGRO_ALIGN_CENTRE, "Exit");			
							break;

					case 2:	al_draw_text(fonte, al_map_rgb(128, 128, 128),  LARGURA_TELA/2, ALTURA_TELA-300, ALLEGRO_ALIGN_CENTRE, "Novo Jogo");
							al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, ALTURA_TELA-200, ALLEGRO_ALIGN_CENTRE, "Créditos");
							al_draw_text(fonte, al_map_rgb(128, 128, 128),  LARGURA_TELA/2, ALTURA_TELA-100, ALLEGRO_ALIGN_CENTRE, "Exit");	
							break;

					case 3: al_draw_text(fonte, al_map_rgb(128, 128, 128),  LARGURA_TELA/2, ALTURA_TELA-300, ALLEGRO_ALIGN_CENTRE, "Novo Jogo");
							al_draw_text(fonte, al_map_rgb(128, 128, 128),  LARGURA_TELA/2, ALTURA_TELA-200, ALLEGRO_ALIGN_CENTRE, "Créditos");
							al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, ALTURA_TELA-100, ALLEGRO_ALIGN_CENTRE, "Exit");								
							break;
				}		
				break;
	}

	return retorno;
}

void initFontes(){
	fonte = al_load_font("Friz Quadrata TT.ttf", 36, 0);
}

int creditos(){
	int retorno=3;
	static int pos=ALTURA_TELA;
	
	al_draw_bitmap(backgroundBitMapCreditos, 10, 0, 0);

	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos-50, ALLEGRO_ALIGN_CENTRE, "Créditos");
	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos+100, ALLEGRO_ALIGN_CENTRE, "Gabriel Souza Marques");
	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos+175, ALLEGRO_ALIGN_CENTRE, "Thalisson Moura Tavares");
	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos+250, ALLEGRO_ALIGN_CENTRE, "Luis Felipe Batista Pereira");
	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos+325, ALLEGRO_ALIGN_CENTRE, "João Victor de Lima Peixoto");
	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos+400, ALLEGRO_ALIGN_CENTRE, "Marcos Heitor Carvalho de Oliveira");
	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos+475, ALLEGRO_ALIGN_CENTRE, "Pedro Henrique de Oliveira Silva");
	al_draw_text(fonte, al_map_rgb(255, 255, 255),  LARGURA_TELA/2, pos+550, ALLEGRO_ALIGN_CENTRE, "Rafael Carneiro Reis de Souza");

	al_draw_bitmap(mainFakeCreditos, LARGURA_TELA/4, pos+1050, 0);

	if(pos>=-750 && eventoTimer.type == ALLEGRO_EVENT_TIMER) pos-=2;

	if(eventoKeyboard.type == ALLEGRO_EVENT_KEY_DOWN){
		retorno=1;

		pos=ALTURA_TELA;
	}

	return retorno;
}

void manipular_entrada(ALLEGRO_EVENT evento, char str[], int tam){
    if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
        if (strlen(str) <= tam){
            char temp[] = {evento.keyboard.unichar, '\0'};
            if (evento.keyboard.unichar == ' ') strcat(str, temp);
            else if (evento.keyboard.unichar >= '0' && evento.keyboard.unichar <= '9') strcat(str, temp);
            else if (evento.keyboard.unichar >= 'A' && evento.keyboard.unichar <= 'Z') strcat(str, temp);
            else if (evento.keyboard.unichar >= 'a' && evento.keyboard.unichar <= 'z') strcat(str, temp);
            else if (evento.keyboard.unichar == '.') strcat(str, temp);
        }
 
        if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0) str[strlen(str) - 1] = '\0';
    }
}

int mouse(int x, int y, int tamX, int tamY, int cursorType){
	int retorno=0;

	if((mouseX>=x && mouseX<=x+tamX && mouseY>=y && mouseY<=y+tamY) || cursorType==0){
		switch(cursorType){
			case 0:	al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
					break;

			case 1:	al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT);
					break;

			case 2:	al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
					break;		
		}

		if(eventoMouse.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) retorno=1;
	}
	else if(eventoMouse.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) retorno=2;

	return retorno;
}

int conectar(char ip[20], char nome[20], char *time){
	static int digitarIp=0, digitarNome=0, podeJogar=0;
	int retorno=2;

	al_draw_bitmap(backgroundBitMap, 0, -100, 0);

	al_draw_bitmap(caixa, 300, 350, 0);

	al_draw_text(fonte, al_map_rgb(255, 255, 255),  500, 375, ALLEGRO_ALIGN_CENTRE, "Iniciar sessão");

	al_draw_text(fonte, al_map_rgb(255, 255, 255),  400, 450, ALLEGRO_ALIGN_CENTRE, "IP:");

	al_draw_bitmap(caixaTexto, 450, 450, 0);

	int mouseIp=mouse((res_x_comp*450)/1300, (res_y_comp*450)/710, (res_x_comp*450)/1300, (res_y_comp*50)/710, 1);

	if(mouseIp==1) digitarIp=1;
	else if(mouseIp==2) digitarIp=0;

	al_draw_text(fonte, al_map_rgb(0, 0, 0),  470, 450, ALLEGRO_ALIGN_LEFT, ip);

	al_draw_text(fonte, al_map_rgb(255, 255, 255),  400, 550, ALLEGRO_ALIGN_CENTRE, "Nick:");

	al_draw_bitmap(caixaTexto, 450, 550, 0);

	int mouseNome=mouse((res_x_comp*450)/1300, (res_y_comp*550)/710, (res_x_comp*450)/1300, (res_y_comp*50)/710, 1);
	
	if(mouseNome==1) digitarNome=1;
	else if(mouseNome==2) digitarNome=0;

	al_draw_text(fonte, al_map_rgb(0, 0, 0),  470, 550, ALLEGRO_ALIGN_LEFT, nome);

	if(digitarIp==1) manipular_entrada(eventoKeyboard, ip, 16);

	if(digitarNome==1) manipular_entrada(eventoKeyboard, nome, 16);

	if(strcmp(ip, "")==0 || strcmp(nome, "")==0){
		al_draw_bitmap(noJoinGame, 575, 610, 0);

		podeJogar=0;
	}
	else{
		al_draw_bitmap(joinGame, 575, 610, 0);

		podeJogar=1;
	}

	if(mouse((res_x_comp*575)/1300, (res_y_comp*610)/710, (res_x_comp*190)/1300, (res_y_comp*50)/710, 2)==1 && podeJogar==1) retorno=5;

	if(eventoKeyboard.type == ALLEGRO_EVENT_KEY_DOWN && eventoKeyboard.keyboard.keycode == ALLEGRO_KEY_ESCAPE) retorno=1;

	return retorno;
}

void iniciarAllegro(){
	al_init();
	al_init_image_addon();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1);
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_keyboard();
	al_install_mouse();
}

void verificaMouse(){
	if (eventoMouse.type == ALLEGRO_EVENT_MOUSE_AXES){
		mouseX=eventoMouse.mouse.x;
		mouseY=eventoMouse.mouse.y;
	}
}

int salaDeEspera(){
	int retorno=5;

	al_draw_bitmap(backgroundBitMapCreditos, 10, 0, 0);

	al_draw_bitmap(caixaEsquerda, 10, 50, 0);

	al_draw_bitmap(caixaDireita, 650, 50, 0);

	al_draw_bitmap(vs, 610, 160, 0);

	return retorno;
}

int main(){
	ALLEGRO_EVENT_QUEUE *fila_eventos_janela=NULL, *fila_eventos_mouse=NULL, *fila_eventos_keyBoard=NULL, *fila_eventos_timer=NULL;
	int estado=0;
	char ip[20]="", nome[20]="", time=' ';

	iniciarAllegro();

	fila_eventos_janela=initJanela();
	
	fila_eventos_mouse=initMouse();
	
	fila_eventos_keyBoard=initKeyboard();

	fila_eventos_timer=initTimer();

	initImagens();

	initAudios();

	initFontes();

	while(1){
      	eventoJanela=verificaEventos(fila_eventos_janela);

      	eventoMouse=verificaEventos(fila_eventos_mouse);
      	
      	eventoKeyboard=verificaEventos(fila_eventos_keyBoard);
      	
      	eventoTimer=verificaEventos(fila_eventos_timer);

      	background();

      	verificaClose();

      	mouse(0, 0, 0, 0, 0);

      	verificaMouse();

      	switch(estado){
      		case 0: estado=apresentacao();
      				break;

      		case 1:	estado=inicio();
      				break;		

      		case 2: estado=conectar(ip, nome, &time);
      				break;

      		case 3: estado=creditos(eventoKeyboard);
      				break;

      		case 4: exit(0);
      				break;	

      		case 5: estado=salaDeEspera();
      				break;

      		case 6: 
      				break;		
      	}

      	al_flip_display();
    }

	return 0;
}
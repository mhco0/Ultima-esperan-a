#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

#ifndef EXIT_SUCCESS
 	#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURED
	#define EXIT_FAILURED 1
#endif
#ifndef SERVER_UP
	#define SERVER_UP 0
#endif
#ifndef SERVER_DOWN
	#define SERVER_DOWN 1
#endif
#ifndef SERVER_FULL
	#define SERVER_FULL 2
#endif
#ifndef SERVER_CLOSED
	#define SERVER_CLOSED 3
#endif
#ifndef SERVER_TIMEOUT
	#define SERVER_TIMEOUT 4
#endif


#define MAXTRIZ 420
#define MAXIP 20
#define APT 50
#define NOME_MAX 255

#define FPS 65

#define VEL 10

#define FRAMEDELAY 8

#define WIDTH 420
#define HEIGTH 420

#define MAX_PLAYERS 4

typedef struct JOGADORES{

	ALLEGRO_BITMAP * sprite;
	int curFrame;
	int frameCount;
	int maxFrame;

	char nome[20];
	int x_old;
	int y_old;
	int x;
	int y;
	int id;
	int vida;
	char comedor;
}JOGADORES;

enum conn_ret_t testconnect(void){
	char ip[MAXIP];
	puts("Por favor , digite o ip do seu servidor.\n");
	scanf(" %[^\n]",ip);
	getchar();
	return connectToServer(ip);
}

enum KEYS{W,A,S,D};

bool key[4]={false,false,false,false};

ALLEGRO_EVENT eventoJanela, eventoTimer, eventoKeyboard, eventoMouse;

ALLEGRO_BITMAP *mainFake=NULL, *apresenta=NULL, *backgroundMenu=NULL, *mainFakeCreditos=NULL, *caixaTexto=NULL, *nocaixaTexto=NULL;
ALLEGRO_BITMAP *noJoinGame=NULL, *joinGame=NULL, *backgroundBitMapCreditos=NULL, *come=NULL, *Win=NULL, *Lose=NULL;

ALLEGRO_SAMPLE *mainFakeAudio=NULL, *fundoSound=NULL;

ALLEGRO_TIMER *timer = NULL;

ALLEGRO_FONT *fonte1 = NULL, *fonte2 = NULL, *fonte3=NULL;

ALLEGRO_DISPLAY *Janela = NULL;

ALLEGRO_BITMAP * Cazeh=NULL;
ALLEGRO_BITMAP * Fundo=NULL;

ALLEGRO_EVENT_QUEUE * fila_keyboard=NULL;

ALLEGRO_TIMER * Timer=NULL;

ALLEGRO_EVENT ev;

int mouseX, mouseY;

ALLEGRO_EVENT verificaEventos(ALLEGRO_EVENT_QUEUE *fila_eventos){
	ALLEGRO_EVENT evento;

	ALLEGRO_TIMEOUT timeout;
    al_init_timeout(&timeout, 0.006);

	al_wait_for_event_until(fila_eventos, &evento, &timeout);

	return evento;
}

char backgroundColor(char color){
	static char colorAtual;

	if(color!='1') colorAtual=color;
	else return colorAtual;

	return '1';
}

void background(){
	switch(backgroundColor('1')){
		case 'b': al_clear_to_color(al_map_rgb(255, 255, 255));
				  break;

		case 'p': al_clear_to_color(al_map_rgb(0, 0, 0));
				  break;
	}
}

int Menu(void){
	int retorno=1;
	static int estado=0, opc=1;
	static float transparencia=0;

	al_play_sample(fundoSound, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);

	switch(estado){
		case 0:	al_draw_tinted_bitmap(backgroundMenu, al_map_rgba_f(transparencia/100, transparencia/100, transparencia/100, 1), 0, 0, 0);

				if(eventoTimer.type == ALLEGRO_EVENT_TIMER) transparencia+=1;

				if(transparencia>=100) estado=1;
				break;

		case 1:	al_draw_bitmap(backgroundMenu, 0, 0, 0);

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
					case 1: al_draw_text(fonte1, al_map_rgb(255, 255, 255),  WIDTH/2, HEIGTH-150, ALLEGRO_ALIGN_CENTRE, "Novo Jogo");
							al_draw_text(fonte1, al_map_rgb(128, 128, 128),  WIDTH/2, HEIGTH-100, ALLEGRO_ALIGN_CENTRE, "Creditos");
							al_draw_text(fonte1, al_map_rgb(128, 128, 128),  WIDTH/2, HEIGTH-50, ALLEGRO_ALIGN_CENTRE, "Exit");			
							break;

					case 2:	al_draw_text(fonte1, al_map_rgb(128, 128, 128),  WIDTH/2, HEIGTH-150, ALLEGRO_ALIGN_CENTRE, "Novo Jogo");
							al_draw_text(fonte1, al_map_rgb(255, 255, 255),  WIDTH/2, HEIGTH-100, ALLEGRO_ALIGN_CENTRE, "Creditos");
							al_draw_text(fonte1, al_map_rgb(128, 128, 128),  WIDTH/2, HEIGTH-50, ALLEGRO_ALIGN_CENTRE, "Exit");	
							break;

					case 3: al_draw_text(fonte1, al_map_rgb(128, 128, 128),  WIDTH/2, HEIGTH-150, ALLEGRO_ALIGN_CENTRE, "Novo Jogo");
							al_draw_text(fonte1, al_map_rgb(128, 128, 128),  WIDTH/2, HEIGTH-100, ALLEGRO_ALIGN_CENTRE, "Creditos");
							al_draw_text(fonte1, al_map_rgb(255, 255, 255),  WIDTH/2, HEIGTH-50, ALLEGRO_ALIGN_CENTRE, "Exit");								
							break;
				}		
				break;		
	}

	return retorno;
}

ALLEGRO_EVENT_QUEUE *initMouse(){
	al_set_system_mouse_cursor(Janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

	ALLEGRO_EVENT_QUEUE *fila_eventos;

	fila_eventos = al_create_event_queue();
	if(fila_eventos==NULL){
		puts("fila_eventos mouse nao alocado");
	}
	al_register_event_source(fila_eventos, al_get_mouse_event_source());

	return fila_eventos;
}

ALLEGRO_EVENT_QUEUE *initKeyboard(){
	ALLEGRO_EVENT_QUEUE *fila_eventos;

	fila_eventos=al_create_event_queue();
	if(fila_eventos==NULL){
		puts("fila_eventos teclado nao alocado");
	}
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());

	return fila_eventos;
}

void initFontes(){
	fonte1 = al_load_font("jogo/client/midia/SimsLLHP.ttf", 30, 0);

	fonte2 = al_load_font("jogo/client/midia/SimsLLHP.ttf", 25, 0);

	fonte3 = al_load_font("jogo/client/midia/SimsLLHP.ttf", 15, 0);
}

void initAudios(){
	mainFakeAudio = al_load_sample("jogo/client/midia/mainFakeAudio.wav");
	if(mainFakeAudio==NULL){
		puts("nao aloquei mainFakeAudio");
	}
	fundoSound = al_load_sample("jogo/client/midia/fundoSound.wav");
	if(fundoSound==NULL){
		puts("nao aloquei fundoSound");
	}
}

void initImagens(){
	mainFake = al_load_bitmap("jogo/client/midia/mainFake.png");
	if(mainFake==NULL){
		puts("nao aloquei main");
	}

	apresenta = al_load_bitmap("jogo/client/midia/apresenta.png");
	if(apresenta==NULL){
		puts("nao aloquei apresenta");
	}

	backgroundMenu = al_load_bitmap("jogo/client/midia/background.png");
	if(backgroundMenu==NULL){
		puts("nao aloquei backgroundMenu");
	}

	mainFakeCreditos = al_load_bitmap("jogo/client/midia/mainFakeCreditos.png");
	if(mainFakeCreditos==NULL){
		puts("nao aloquei mainFakeCreditos");
	}

	caixaTexto = al_load_bitmap("jogo/client/midia/caixaTexto.png");
	if(caixaTexto==NULL){
		puts("nao aloquei caixaTexto");
	}

	nocaixaTexto = al_load_bitmap("jogo/client/midia/nocaixaTexto.png");
	if(nocaixaTexto==NULL){
		puts("nao aloquei nocaixaTexto");
	}

	noJoinGame = al_load_bitmap("jogo/client/midia/noJoinGame.png");
	if(noJoinGame==NULL){
		puts("nao aloquei noJoinGame");
	}

	joinGame = al_load_bitmap("jogo/client/midia/joinGame.png");
	if(joinGame==NULL){
		puts("nao aloquei joinGame");
	}

	backgroundBitMapCreditos = al_load_bitmap("jogo/client/midia/backgroundBitMapCreditos.jpg");
	if(backgroundBitMapCreditos==NULL){
		puts("nao aloquei backgroundBitMapCreditos");
	}

	come = al_load_bitmap("jogo/client/midia/come.png");
	if(come==NULL){
		puts("nao aloquei come");
	}

	Win = al_load_bitmap("jogo/client/midia/vencedor.png");
	if(Win==NULL){
		puts("nao aloquei Win");
	}

	Lose = al_load_bitmap("jogo/client/midia/perdedor.png");
	if(Lose==NULL){
		puts("nao aloquei Lose");
	}

	Fundo=al_load_bitmap("jogo/client/midia/mapalimpo.png");
	if(Fundo==NULL){
		puts("Tivemos um problema ao alocar o Fundo.");
		exit(EXIT_FAILURED);
	}
	Cazeh=al_load_bitmap("jogo/client/midia/PAC.png");
	if(Cazeh==NULL){
		puts("Tivemos um problema ao alocar a Cazeh.");
		exit(EXIT_FAILURED);
	}

	al_convert_mask_to_alpha(Cazeh,al_map_rgb(255,0,255));
}

int apresentacao(void){
	int retorno=0;
	static int x=-150;
	static int estado=0, firstTime=1;
	static float transparencia=0;

	switch(estado){
		case 0: if(firstTime==1){
					backgroundColor('b');

					firstTime=0;
				}
				else{
					al_draw_tinted_bitmap(mainFake, al_map_rgba_f(1, 1, 1, transparencia/100), 0, 160, 0);

					if(eventoTimer.type == ALLEGRO_EVENT_TIMER){
						transparencia+=2;

						if(transparencia==26) al_play_sample(mainFakeAudio, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					}

					if(transparencia>=100) estado=1;
				}
				break;

		case 1: al_draw_tinted_bitmap(mainFake, al_map_rgba_f(1, 1, 1, transparencia/100), 0, 160, 0);

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
					al_draw_tinted_bitmap(apresenta, al_map_rgba_f(transparencia/100, transparencia/100, transparencia/100, 1), 0, 0, 0);

					al_draw_bitmap(come, (float)x, 270, 0);

					

					if(eventoTimer.type == ALLEGRO_EVENT_TIMER){
						x+=8;
						transparencia+=2;
					} 

					if(transparencia>=100){
						estado=3;

						firstTime=1;
					}
				}
				break;		

		case 3: al_draw_tinted_bitmap(apresenta, al_map_rgba_f(transparencia/100, transparencia/100, transparencia/100, 1), 0, 0, 0);

				al_draw_bitmap(come, (float)x, 270, 0);

				if(eventoTimer.type == ALLEGRO_EVENT_TIMER){
					x+=8;

					transparencia-=5;		
				}

				if(transparencia<=0){
					estado=0;

					firstTime=1;

					retorno=1;
				}
				break;
	}

	return retorno;
}

int creditos(){
	int retorno=3;
	static int pos=HEIGTH;
	static int x = 0;
	static int control=0;
	
	if(control==0){
		al_draw_bitmap(backgroundBitMapCreditos, 0, 0, 0);

		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos-50, ALLEGRO_ALIGN_CENTRE, "Creditos");
		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos+100, ALLEGRO_ALIGN_CENTRE, "Gabriel Souza Marques");
		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos+175, ALLEGRO_ALIGN_CENTRE, "Thalisson Moura Tavares");
		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos+250, ALLEGRO_ALIGN_CENTRE, "Luis Felipe Batista Pereira");
		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos+325, ALLEGRO_ALIGN_CENTRE, "Joao Victor de Lima Peixoto");
		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos+400, ALLEGRO_ALIGN_CENTRE, "Marcos H. C. Oliveira");
		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos+475, ALLEGRO_ALIGN_CENTRE, "Pedro Henrique de Oliveira Silva");
		al_draw_text(fonte2, al_map_rgb(255, 255, 255),  (float)HEIGTH/2, (float)pos+550, ALLEGRO_ALIGN_CENTRE, "Rafael Carneiro Reis de Souza");

		al_draw_bitmap(mainFakeCreditos, (float)HEIGTH/7, (float)pos+900, 0);
	}

	if(pos>=-750 && eventoTimer.type == ALLEGRO_EVENT_TIMER) pos-=2;

	if(pos<-750){

		control=1;

		al_draw_bitmap(backgroundBitMapCreditos, (float)x, 0, 0);

		al_draw_bitmap(mainFakeCreditos, (float)HEIGTH/7, (float)pos+900, 0);

		if(eventoTimer.type == ALLEGRO_EVENT_TIMER) x+=3;

	}

	if(x==471) retorno=1;

	if(eventoKeyboard.type == ALLEGRO_EVENT_KEY_DOWN){
		retorno=1;

		pos=HEIGTH;
	}

	return retorno;
}

int mouse(int x, int y, int tamX, int tamY, int cursorType, int changeCursor){
	static int cursorChanged=0;
	int retorno=0;

	if(changeCursor==1){
		if(cursorChanged==1) cursorChanged=0;
		else al_set_system_mouse_cursor(Janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
	}

	if(mouseX>=x && mouseX<=x+tamX && mouseY>=y && mouseY<=y+tamY){
		switch(cursorType){
			case 0:	al_set_system_mouse_cursor(Janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW);
					break;

			case 1:	al_set_system_mouse_cursor(Janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT);
					break;

			case 2:	al_set_system_mouse_cursor(Janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
					break;		
		}

		retorno=1;

		if(eventoMouse.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) retorno=2;

		cursorChanged=1;
	}
	else if(eventoMouse.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) retorno=3;

	return retorno;
}

void verificaMouse(ALLEGRO_EVENT_QUEUE *fila_eventos){
	while (!al_is_event_queue_empty(fila_eventos)){
        al_wait_for_event(fila_eventos, &eventoMouse);

        if (eventoMouse.type == ALLEGRO_EVENT_MOUSE_AXES){
			mouseX=eventoMouse.mouse.x;
			mouseY=eventoMouse.mouse.y;
		}
	}
}

void manipular_entrada(ALLEGRO_EVENT evento, char str[], unsigned int tam){
    if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
        if (strlen(str) <= tam){
            char temp[] = {(char)evento.keyboard.unichar, '\0'};
            if (evento.keyboard.unichar == ' ') strcat(str, temp);
            else if (evento.keyboard.unichar >= '0' && evento.keyboard.unichar <= '9') strcat(str, temp);
            else if (evento.keyboard.unichar >= 'A' && evento.keyboard.unichar <= 'Z') strcat(str, temp);
            else if (evento.keyboard.unichar >= 'a' && evento.keyboard.unichar <= 'z') strcat(str, temp);
            else if (evento.keyboard.unichar == '.') strcat(str, temp);
        }
 
        if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0) str[strlen(str) - 1] = '\0';
    }
}

void Destruir(void){
	al_destroy_display(Janela);
	al_destroy_event_queue(fila_keyboard);
}

void InicializarAllegro(void){
	if(!al_init()){
		puts("Tivemos um erro em inicializar a Allegro5.");
		exit(EXIT_FAILURED);	
	}

	if(!al_init_image_addon()){
		puts("Tivemos um erro em iniciar imagens da Allegro5");
		exit(EXIT_FAILURED);
	}
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1);
	al_init_font_addon();
	al_init_ttf_addon();
	if(!al_install_keyboard()){
		puts("Tivemos um erro ao instalar o keyboard");
		exit(EXIT_FAILURED);
	}
	al_install_mouse();
	// al_init_primitives_addon();

}

void Registra_timer(void){
	Timer=al_create_timer(1.0/FPS);
}


void Registra_fila_keyboard(void){

	fila_keyboard=al_create_event_queue();
	if(fila_keyboard==NULL){
		puts("Tivemos um problema em criar a fila_keyboard");
		exit(EXIT_FAILURED);
	}

	al_register_event_source(fila_keyboard,al_get_timer_event_source(Timer));
	al_register_event_source(fila_keyboard,al_get_keyboard_event_source());
	al_register_event_source(fila_keyboard,al_get_display_event_source(Janela));
	
	al_start_timer(Timer);
}

ALLEGRO_EVENT_QUEUE *initJanela(){
	ALLEGRO_EVENT_QUEUE *fila_eventos;
	//ALLEGRO_BITMAP *icone=NULL;

	fila_eventos = al_create_event_queue();

	ALLEGRO_MONITOR_INFO info;

	al_get_monitor_info(0, &info); //pega as info do monitor do pc

	Janela = al_create_display(WIDTH, HEIGTH);

	al_register_event_source(fila_eventos, al_get_display_event_source(Janela));

	al_set_window_title(Janela, "LAST HOPE");

	//icone = al_load_bitmap("jogo/client/midia/icon.ico");

	//al_set_display_icon(Janela, icone);

	al_clear_to_color(al_map_rgb(255, 255, 255));

   	al_flip_display();

   	return fila_eventos;
}

int tentaSeConectar(int *ids){
	int retorno=2;
	static int digitarIp=0, podeJogar=0, mensagem=0, firstTime=1;
	static char ip[20];
	//enum conn_ret_t teste=testconnect();
	//char opcao;

	enum conn_ret_t teste = 5;
	//enum conn_ret_t teste=testconnect();
	//char opcao;

	if(firstTime==1){
		memset(ip, 0, sizeof(ip));

		firstTime=0;
	}

	al_draw_bitmap(backgroundMenu, 0, 0, 0);

	al_draw_text(fonte2, al_map_rgb(255, 255, 255),  100, 250, ALLEGRO_ALIGN_CENTRE, "Iniciar sessao");

	al_draw_text(fonte2, al_map_rgb(255, 255, 255),  100, 300, ALLEGRO_ALIGN_CENTRE, "IP :");

	if(digitarIp==1) al_draw_bitmap(caixaTexto, 125, 305, 0);
	else al_draw_bitmap(nocaixaTexto, 125, 305, 0);

	int mouseIp=mouse(125, 300, 210, 25, 1, 0);

	if(mouseIp==2) digitarIp=1;	
	else if(mouseIp==3) digitarIp=0;

	al_draw_text(fonte2, al_map_rgb(0, 0, 0),  130, 300, ALLEGRO_ALIGN_LEFT, ip);

	if(digitarIp==1) manipular_entrada(eventoKeyboard, ip, 20);

	if(strcmp(ip, "")==0){
		al_draw_bitmap(noJoinGame, 175, 370, 0);

		podeJogar=0;
	}
	else{
		al_draw_bitmap(joinGame, 175, 370, 0);

		podeJogar=1;
	}

	switch(mensagem){
		case 1: al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 330, ALLEGRO_ALIGN_CENTRE, "Server not found");
				break;

		case 2: al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 330, ALLEGRO_ALIGN_CENTRE, "Server is full.");
				al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 335, ALLEGRO_ALIGN_CENTRE, "Try again later.");
				break;

		case 3: al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 330, ALLEGRO_ALIGN_CENTRE, "The server is closed.");
				al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 335, ALLEGRO_ALIGN_CENTRE, "Try again later.");
				break;

		case 4: al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 330, ALLEGRO_ALIGN_CENTRE, "The server is not responding for connections");
				al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 335, ALLEGRO_ALIGN_CENTRE, "Try again later.");
				break;	

		case 5: al_draw_text(fonte2, al_map_rgb(255, 255, 255),  215, 330, ALLEGRO_ALIGN_CENTRE, "IP adress incomplete");
				break;				
	}

	if((mouse(175, 370, 85, 25, 2, 0)==2 || (eventoKeyboard.type == ALLEGRO_EVENT_KEY_DOWN && eventoKeyboard.keyboard.keycode == ALLEGRO_KEY_ENTER)) && podeJogar==1){
		al_set_system_mouse_cursor(Janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);

		ip[strlen(ip)]='\0';

		if(strlen(ip)>=11)  teste=connectToServer(ip);

		if(teste!=SERVER_UP){
			if(teste==SERVER_DOWN) mensagem=1;
			else if(teste==SERVER_FULL) mensagem=2;
			else if(teste==SERVER_CLOSED) mensagem = 3;
			else if(teste==SERVER_TIMEOUT) mensagem = 4;
			else if(teste==5) mensagem = 5;
		}
		else{
			recvMsgFromServer(ids, WAIT_FOR_IT);

			recvMsgFromServer(&mensagem, WAIT_FOR_IT);

			retorno=5;
		}
	}

	if(eventoKeyboard.type == ALLEGRO_EVENT_KEY_DOWN && eventoKeyboard.keyboard.keycode == ALLEGRO_KEY_ESCAPE) retorno=1;

	return retorno;
}

void load_sprite(JOGADORES *jogadorNovo){
		switch(jogadorNovo->id){
		case 0:jogadorNovo->sprite=al_load_bitmap("jogo/client/midia/vermelho.png");
			if(jogadorNovo->sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap vermelho");
				exit(EXIT_FAILURED);
			}
		break;
		case 1:jogadorNovo->sprite=al_load_bitmap("jogo/client/midia/verde.png");
			if(jogadorNovo->sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap verde");
				exit(EXIT_FAILURED);
			}
		break;
		case 2:jogadorNovo->sprite=al_load_bitmap("jogo/client/midia/roxo.png");
			if(jogadorNovo->sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap roxo");
				exit(EXIT_FAILURED);
			}
		break;
		case 3:jogadorNovo->sprite=al_load_bitmap("jogo/client/midia/azul.png");
			if(jogadorNovo->sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap azul");
				exit(EXIT_FAILURED);
			}
		break;
		default:puts("Aconteceu um erro inesperado.");
				exit(EXIT_FAILURED);
		break;
	}
	al_convert_mask_to_alpha(jogadorNovo->sprite,al_map_rgb(255,0,255));
}


void load_sprite2(JOGADORES *jogadorNovo,int linha){
	printf("%d\n",jogadorNovo[linha].id );
	switch(jogadorNovo[linha].id){
		case 0:jogadorNovo[linha].sprite=al_load_bitmap("jogo/client/midia/vermelho.png");
			if(jogadorNovo[linha].sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap vermelho");
				exit(EXIT_FAILURED);
			}
		break;
		case 1:jogadorNovo[linha].sprite=al_load_bitmap("jogo/client/midia/verde.png");
			if(jogadorNovo[linha].sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap verde");
				exit(EXIT_FAILURED);
			}
		break;
		case 2:jogadorNovo[linha].sprite=al_load_bitmap("jogo/client/midia/roxo.png");
			if(jogadorNovo[linha].sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap roxo");
				exit(EXIT_FAILURED);
			}
		break;
		case 3:jogadorNovo[linha].sprite=al_load_bitmap("jogo/client/midia/azul.png");
			if(jogadorNovo[linha].sprite==NULL){
				puts("Tivemos um problema em carregar o bitmap azul");
				exit(EXIT_FAILURED);
			}
		break;
		default:puts("Aconteceu um erro inesperado.");
				exit(EXIT_FAILURED);
		break;
	}
	al_convert_mask_to_alpha(jogadorNovo[linha].sprite,al_map_rgb(255,0,255));
}

void getJogador(int id, JOGADORES *jogadorNovo, char nome[20]){
	jogadorNovo->id=id;

	strcpy(jogadorNovo->nome, nome);

	jogadorNovo->comedor='V';

	jogadorNovo->vida=1;

	switch(jogadorNovo->id){
			case 0:
				jogadorNovo->x=35;
				jogadorNovo->y=35;
			break;
			case 1:
				jogadorNovo->x=MAXTRIZ-35;
				jogadorNovo->y=MAXTRIZ-35;
			break;
			case 2:
				jogadorNovo->x=35;
				jogadorNovo->y=MAXTRIZ-35;
			break;
			case 3:
				jogadorNovo->x=MAXTRIZ-35;
				jogadorNovo->y=35;
			break;
			default:puts("Tivemos um erro inesperado.");
					exit(EXIT_FAILURED);
			break;
	}

	jogadorNovo->x_old=jogadorNovo->x;
	jogadorNovo->y_old=jogadorNovo->y;

	load_sprite(jogadorNovo);

	jogadorNovo->curFrame=0;
	jogadorNovo->frameCount=0;
	jogadorNovo->maxFrame=2;
	
}


int selecionarPersonagem(int id, JOGADORES *jogadorNovo){
	int retorno=5, mensagem;
	static int digitarNick=0, podeJogar=0, firstTime=1;
	static char nome[20];

	if(firstTime==1){
		memset(nome, 0, sizeof(nome));

		firstTime=0;
	}

	al_draw_bitmap(backgroundMenu, 0, 0, 0);

	al_draw_text(fonte2, al_map_rgb(255, 255, 255),  100, 250, ALLEGRO_ALIGN_CENTRE, "Iniciar sessao");

	al_draw_text(fonte2, al_map_rgb(255, 255, 255),  100, 300, ALLEGRO_ALIGN_CENTRE, "Nick :");

	if(digitarNick==1) al_draw_bitmap(caixaTexto, 125, 305, 0);
	else al_draw_bitmap(nocaixaTexto, 125, 305, 0);

	al_draw_text(fonte2, al_map_rgb(0, 0, 0),  130, 300, ALLEGRO_ALIGN_LEFT, nome);

	int mouseNick=mouse(125, 300, 210, 25, 1, 0);

	if(mouseNick==2) digitarNick=1;	
	else if(mouseNick==3) digitarNick=0;

	if(digitarNick==1) manipular_entrada(eventoKeyboard, nome, 20);

	if(strcmp(nome, "")==0){
		al_draw_bitmap(noJoinGame, 175, 370, 0);

		podeJogar=0;
	}
	else{
		al_draw_bitmap(joinGame, 175, 370, 0);

		podeJogar=1;
	}

	if((mouse(175, 370, 85, 25, 2, 0)==2 || (eventoKeyboard.type == ALLEGRO_EVENT_KEY_DOWN && eventoKeyboard.keyboard.keycode == ALLEGRO_KEY_ENTER)) && podeJogar==1){
		al_set_system_mouse_cursor(Janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);

		nome[strlen(nome)]='\0';

		getJogador(id, jogadorNovo, nome);

		sendMsgToServer(&nome, sizeof(char));

		recvMsgFromServer(&mensagem, WAIT_FOR_IT);

		retorno=6;
	}

	return retorno;
}

 void Printar_tela_over_power(JOGADORES * playerss){

 	float xinicial;
	float tamanhox=14;
	float tamanhoy=14;

	int varx;
	int vary;

	//problema ta na alocação do bitmap

	al_draw_bitmap(Fundo,0,0,0);

	register int linha;

	for(linha=0;linha<MAX_PLAYERS;linha++){

		varx=playerss[linha].x-playerss[linha].x_old;
		vary=playerss[linha].y-playerss[linha].y_old;

		xinicial=(float)playerss[linha].curFrame*14;

		load_sprite2(playerss, linha);


		if(playerss[linha].comedor=='V' && playerss[linha].vida){
			if(varx>0){
				al_draw_bitmap_region(playerss[linha].sprite,xinicial,0,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);
			}else if(vary==0){				
				al_draw_bitmap_region(playerss[linha].sprite,14,0,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);
			}

			if(varx<0){
				al_draw_bitmap_region(playerss[linha].sprite,xinicial,14,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);
			}else if(vary==0){
				al_draw_bitmap_region(playerss[linha].sprite,14,14,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}

			if(vary<0){
				al_draw_bitmap_region(playerss[linha].sprite,xinicial,28,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}else if(varx==0){
				al_draw_bitmap_region(playerss[linha].sprite,14,28,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);								
			}

			if(vary>0){
				al_draw_bitmap_region(playerss[linha].sprite,xinicial,42,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}else if(vary==0){
				al_draw_bitmap_region(playerss[linha].sprite,14,42,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}

		}else if(playerss[linha].comedor == 'C' && playerss[linha].vida){


			if(varx>0){
				al_draw_bitmap_region(Cazeh,xinicial,0,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);
			}else if(vary==0){				
				al_draw_bitmap_region(Cazeh,14,0,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);
			}

			if(varx<0){
				al_draw_bitmap_region(Cazeh,xinicial,14,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);
			}else if(vary==0){
				al_draw_bitmap_region(Cazeh,14,14,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}

			if(vary<0){
				al_draw_bitmap_region(Cazeh,xinicial,28,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}else if(varx==0){
				al_draw_bitmap_region(Cazeh,14,28,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);								
			}

			if(vary>0){
				al_draw_bitmap_region(Cazeh,xinicial,42,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}else if(vary==0){
				al_draw_bitmap_region(Cazeh,14,42,tamanhox,tamanhoy,(float)playerss[linha].x,(float)playerss[linha].y,0);				
			}
		}
		if(playerss[linha].vida == 1) {
			al_draw_text(fonte3, al_map_rgb(255, 255, 255), (float)playerss[linha].x, (float)playerss[linha].y-15, ALLEGRO_ALIGN_CENTRE, playerss[linha].nome);
		}

	}


	al_flip_display();


 }

void Atualizar_frames(JOGADORES *player){
	if(player->comedor == 'C'){
		player->maxFrame=3;
	}else{
		player->maxFrame=2;
	}

	if(++player->frameCount >= FRAMEDELAY){
		if(++player->curFrame >= player->maxFrame){
			player->curFrame=0;
		}

		player->frameCount=0;

		player->x_old=player->x;
		player->y_old=player->y;

		player->x -= key[A]*VEL;
		player->x += key[D]*VEL;
		player->y -= key[W]*VEL;
		player->y += key[S]*VEL;

		if(player->x <=0 - al_get_bitmap_width(player->sprite)/player->maxFrame)
			player ->x = MAXTRIZ-1;
		if(player->x <=0 + al_get_bitmap_width(player->sprite)/player->maxFrame)
			player ->x = 0;
		if(player->y <=0 - al_get_bitmap_height(player->sprite)/4)
			player ->y = MAXTRIZ-1;
		if(player->y <=0 + al_get_bitmap_height(player->sprite)/4)
			player ->y = 0;

	}
}

void mecanica(JOGADORES * player){
	while(!al_is_event_queue_empty(fila_keyboard)){

		al_wait_for_event(fila_keyboard,&ev);
	

		if(ev.type==ALLEGRO_EVENT_KEY_DOWN){
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_ESCAPE:
					exit(EXIT_SUCCESS);
					break;
				case ALLEGRO_KEY_W:
					key[W]=true;
					break;
				case ALLEGRO_KEY_A:
					key[A]=true;
					break;
				case ALLEGRO_KEY_S:
					key[S]=true;
					break;
				case ALLEGRO_KEY_D:
					key[D]=true;
					break;

			}
		}else if(ev.type==ALLEGRO_EVENT_KEY_UP){
			switch(ev.keyboard.keycode){
				case ALLEGRO_KEY_ESCAPE:
					exit(EXIT_SUCCESS);
					break;
				case ALLEGRO_KEY_W:
					key[W]=false;
					break;
				case ALLEGRO_KEY_A:
					key[A]=false;
					break;
				case ALLEGRO_KEY_S:
					key[S]=false;
					break;
				case ALLEGRO_KEY_D:
					key[D]=false;
					break;

			}
		}else if(ev.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
			exit(0);
		}else if(ev.type==ALLEGRO_EVENT_TIMER){
			Atualizar_frames(player);
		}
		
	}
	
}

void Wait(void){
	 register int linha;
	 register int contador=0;
	 char cont[2];
	 puts("iniciando jogo.");

	 for(contador=5;contador>0;contador--){
	 	for(linha=0;linha<640000000;linha++);

	 	cont[0] = (char)(contador+48);
	 	cont[1] = '\0';

	 	al_draw_bitmap(backgroundMenu, 0, 0, 0);

		al_draw_text(fonte1, al_map_rgb(255, 255, 255),  210, 270, ALLEGRO_ALIGN_CENTRE, cont);

		al_flip_display();
	 }
}


/*void printanatela(char matriz[MAXTRIZ][MAXTRIZ],JOGADORES * jogador){
	register int linha,coluna,linhal;
	for(linha=0;linha<MAXTRIZ;linha++){
		for(coluna=0;coluna<MAXTRIZ;coluna++){
			matriz[linha][coluna]='*';
		}
	}

	
	for(linhal=0;linhal<MAX_PLAYERS;linhal++){
		if(jogador[linhal].comedor=='C'){
			matriz[jogador[linhal].y][jogador[linhal].x]='C';
		}
		if(jogador[linhal].comedor=='V'){
			if(jogador[linhal].vida==1){
				matriz[jogador[linhal].y][jogador[linhal].x]=jogador[linhal].nome;
			}
		}
	}
	
	for(linha=0;linha<MAXTRIZ;linha++){
		for(coluna=0;coluna<MAXTRIZ;coluna++){
			printf("%2c",matriz[linha][coluna]);
		}
		printf("\n");
	}
	
	
}*/
void atualiza_meu_player(JOGADORES*playerss,JOGADORES *player){
	register int linha;
	for(linha=0;linha<MAX_PLAYERS;linha++){
		if(player->id==playerss[linha].id){
			*player=playerss[linha];
		}
	}
}

ALLEGRO_EVENT_QUEUE *initTimer(){
	ALLEGRO_EVENT_QUEUE *fila_eventos;

	fila_eventos=al_create_event_queue();
	if(fila_eventos==NULL){
		puts("fila_eventos timer nao alocado");
	}

	timer = al_create_timer(0.05);

	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

	al_start_timer(timer);

	return fila_eventos;
}

void verificaClose(){
	if(eventoJanela.type==ALLEGRO_EVENT_DISPLAY_CLOSE) exit(0);
}

int salaDeEspera(JOGADORES *player, int *jogar){
	al_draw_bitmap(backgroundMenu, 0, 0, 0);

	al_draw_text(fonte1, al_map_rgb(255, 255, 255),  210, 270, ALLEGRO_ALIGN_CENTRE, "Esperando Servidor...");

	al_flip_display();

	sendMsgToServer(player, sizeof(JOGADORES));

	recvMsgFromServer(player, WAIT_FOR_IT);

	recvMsgFromServer(jogar, WAIT_FOR_IT);

	Wait();

	return 7;
}

int vencedor(void){
	int retorno = 8;
	static int x = -522;
	static int y = 0;
	
		backgroundColor('p');

		al_draw_bitmap(Win, (float)x, 0, 0);

		if(x>=132){
			al_draw_text(fonte1, al_map_rgb(255, 255, 255),  210, 100, ALLEGRO_ALIGN_CENTRE, "Voce Venceu!!!");
			if(eventoTimer.type == ALLEGRO_EVENT_TIMER) y+=2;
		}


	if(x<132 && eventoTimer.type == ALLEGRO_EVENT_TIMER) x+=2;

	if(y>=250) retorno=4;

	return retorno;
}

int perdedor(void){
	int retorno = 9;
	static int x = -522;
	static int y = 0;
	
		backgroundColor('p');

		al_draw_bitmap(Lose, (float)x, 0, 0);

		if(x>=132){
			al_draw_text(fonte1, al_map_rgb(255, 255, 255),  210, 100, ALLEGRO_ALIGN_CENTRE, "Voce Perdeu!!!");
			if(eventoTimer.type == ALLEGRO_EVENT_TIMER) y+=2;
		}


	if(x<132 && eventoTimer.type == ALLEGRO_EVENT_TIMER) x+=2;

	if(y>=250) retorno=4;

	return retorno;
}

int jogo(JOGADORES *player, JOGADORES playerss[MAX_PLAYERS], int *vitoria){
	int aux;
		
		mecanica(player);
		//manda para o client o time q está
		aux=sendMsgToServer(player,sizeof(JOGADORES));
			if(aux==SERVER_DISCONNECTED){
				puts("--------------------------------\n");
				puts("O servidor foi desconectado.\n");
				puts("--------------------------------\n");
				exit(EXIT_FAILURED);
			}else{
				if(aux==NO_MESSAGE){
					puts("---------------------------------------------\n");
					puts("Nenhuma informacao transmitida pelo servidor.\n");
					puts("---------------------------------------------\n");
				}
			}

		aux=recvMsgFromServer(playerss,WAIT_FOR_IT);
			if(aux==SERVER_DISCONNECTED){
				return 9;
			}else{
				if(aux==NO_MESSAGE){
					puts("---------------------------------------------\n");
					puts("Nenhuma informacao transmitida pelo servidor.\n");
					puts("---------------------------------------------\n");
				}
			}

		atualiza_meu_player(playerss,player);	

		recvMsgFromServer(vitoria,WAIT_FOR_IT);

		if(*vitoria==0){
			return 8;
		}

		system("clear");
		//printanatela(matriz,playerss);
		Printar_tela_over_power(playerss);

	return 7;
}

int main(void){
	//char matriz[MAXTRIZ][MAXTRIZ]={" "};
	ALLEGRO_EVENT_QUEUE *fila_eventos_janela=NULL, *fila_eventos_timer=NULL, *fila_eventos_keyBoard=NULL, *fila_eventos_mouse=NULL;
	int estado=0, id, jogar=0;
	JOGADORES playerss[MAX_PLAYERS];
	JOGADORES player;
	//char msg[NOME_MAX];
	int vitoria=1;
	//int maximo=MAX_PLAYERS;

	InicializarAllegro();

	fila_eventos_janela=initJanela();

	fila_eventos_timer=initTimer();

	fila_eventos_keyBoard=initKeyboard();

	fila_eventos_mouse=initMouse();

	initImagens();

	initAudios();

	initFontes();
	
	Registra_timer();

	Registra_fila_keyboard();	

	while(1){
		eventoTimer=verificaEventos(fila_eventos_timer);

		eventoJanela=verificaEventos(fila_eventos_janela);

		eventoKeyboard=verificaEventos(fila_eventos_keyBoard);

		verificaClose();

		background();

		verificaMouse(fila_eventos_mouse);

		mouse(0, 0, 0, 0, 0, 1);

		switch(estado){
			case 0:
				estado=apresentacao();
			break;

			case 1:
				estado=Menu();
			break;	

			case 2:
				estado=tentaSeConectar(&id);
			break;

			case 3:
				estado=creditos(eventoKeyboard);
			break;

			case 4:
				exit(0);
			break;

			case 5:
				estado=selecionarPersonagem(id, &player);
			break;

			case 6:
				estado=salaDeEspera(&player, &jogar);
			break;

			case 7:
				estado=jogo(&player, playerss, &vitoria);
					break;

			case 8:	
				estado=vencedor();
      				break;

      		case 9: 
      			estado=perdedor();
      				break;
		}

		al_flip_display();
	}

	Destruir();

	return 0;
}
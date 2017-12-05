#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include <time.h>
#include <ctype.h>
#include <math.h>
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

#define MAX_PLAYERS 2

typedef struct JOGADORES{
	char nome;
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

ALLEGRO_DISPLAY * janela=NULL;

// ALLEGRO_BITMAP * Cazeh=NULL, * player1=NULL,* player2=NULL,* player3=NULL, * player4=NULL;
 ALLEGRO_BITMAP * Fundo=NULL;

void Carregar_bitmaps(void){
	Fundo=al_load_bitmap("jogo/client/imagem/mapalimpo.png");
	if(Fundo==NULL){
		puts("Tivemos um problema ao alocar o Fundo.");
		exit(-1);
	}
}

void Destruir(void){
	al_destroy_display(janela);
}

void InicializarAllegro(void){
	if(!al_init()){
		puts("Tivemos um erro em inicializar a Allegro5.");
		exit(-1);	
	}

	if(!al_init_image_addon()){
		puts("Tivemos um erro em iniciar imagens da Allegro5");
		exit(-1);
	}
	// al_install_audio();
	// al_init_acodec_addon();
	// al_reserve_samples(1);
	// al_init_font_addon();
	// al_init_ttf_addon();
	// al_install_keyboard();
	// al_install_mouse();
	// al_init_primitives_addon();
}

void InitJanela(void){

	janela = al_create_display(MAXTRIZ, MAXTRIZ);

	if(janela==NULL){
		puts("Janela nao alocada");
		exit(-1);
	}

	al_set_window_title(janela, "Last Hope");

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(Fundo,0,0,0);
   	al_flip_display();


}

void Menu(void){
	int escolha;
	int ok=0;
	do{
		puts("=====================\n");
		puts("Escolha sua opcao: \n");
		puts("1.Iniciar o jogo.\n");
		puts("2.Sair. \n");
		puts("=====================\n");
		scanf("%d",&escolha);

		switch(escolha){
			case 1:ok=1;
			break;
			case 2:exit(EXIT_SUCCESS);
			break;
			default:puts("Voce nao selecionou uma escolha valida.\n");
			break;
		}


	}while(!ok);

	printf("\n");

}

void apresentacao(void){
	puts("------------------------------------------------------------------------------------\n");
	puts("============================Bem vindo ao Ultima Esperanca!!!!=======================\n");
	puts("------------------------------------------------------------------------------------\n");
	puts("------------------------------------------------------------------------------------\n");
	puts("______________________________________INTRUCOES_____________________________________\n");
	puts("------------------------------------------------------------------------------------\n");
	puts("1.O jogo eh baseado em dois tipos de jogadores, um come e o outro e comido,tipo o \nACM e um aluno qualquer de ip.\n");
	puts("------------------------------------------------------------------------------------\n");
	puts("2.O jogo comeca com um comedor aleatorio cujo objetivo eh tentar comer os demais ate \nque apenas ele reste e ganhe o jogo.\n");
	puts("------------------------------------------------------------------------------------\n");
	puts("3.O nome do seu personagem pode ser qualquer um ,exceto as letras 'C' e 'c'\nelas representam o comedor da partida.\n ");
	puts("------------------------------------------------------------------------------------\n");
	puts("4.A letra '*' tambem eh proibida, ela se trata do campo do mapa.\n");
	puts("------------------------------------------------------------------------------------\n");
	puts("5.A cada 2 minutos um novo monstro aparece na tela randomicamente.Boa sorte!");
	puts("------------------------------------------------------------------------------------\n");
}

int tentaSeConectar(void){
	enum conn_ret_t teste=testconnect();
	char opcao;
	int ids;

	while(teste!=SERVER_UP){
			if(teste==SERVER_DOWN){
				puts("----------------------------------------\n");
				puts("Nao foi possivel encontrar o servidor.\n");
				puts("----------------------------------------\n");
			}else{
				if(teste==SERVER_FULL){
					puts("----------------------------------------\n");
					puts("O Servidor esta cheio.\nTente de novo mais tarde.\n");
					puts("----------------------------------------\n");
				}else{
					if(teste==SERVER_CLOSED){
						puts("----------------------------------------\n");
						puts("O servidor esta fechado.\nTente de novo mais tarde.\n");
						puts("----------------------------------------\n");
					}else{
						if(teste==SERVER_TIMEOUT){
							puts("----------------------------------------\n");
							puts("Servidor nao esta respondendo a conexao.\nTente novamente mais tarde.\n");
							puts("----------------------------------------\n");
						}
					}
				}
			}
		puts("----------------------------------------\n");
		puts("Voce deseja se conectar novamente?[S ou N]:");
		puts("----------------------------------------\n");
			scanf(" %c",&opcao);
			opcao=(char)toupper(opcao);
			while(opcao!='S' && opcao!='N'){
				puts("----------------------------------------\n");
				puts("Voce nao digitou uma opcao valida.\nDigite novamente.\n");
				puts("----------------------------------------\n");
				scanf(" %c",&opcao);
				opcao=(char)toupper(opcao);
			}
		if(opcao=='S'){
			teste=testconnect();
		}else{
			exit(EXIT_SUCCESS);
		}
	}

	recvMsgFromServer(&ids,WAIT_FOR_IT);//recebe mensagem do servidor sendo o id do client 1 msg a ser recebida do server
	printf("Este eh seu id: %d.\n",ids);
	puts("Aguardando os outros jogadores.....\n");

	return ids;
}

char getNome(void){
	char nome;
	int resposta=0,resposta2=0;;
	int aux;


	do{

		printf("Digite seu nick: ");
		scanf(" %c",&nome);

		switch(nome){
			case 'C':puts("Voce nao selecionou uma opcao valida.\n");
			break;
			case 'c':puts("Voce nao selecionou uma opcao valida.\n");
			break;
			default:resposta2=1;
			break;
		}
	
	}while(!resposta2);

		sendMsgToServer(&nome,sizeof(char));
		puts("-----------------------------------\n");
		puts("Esperando resposta do servidor....\n");
		puts("-----------------------------------\n");
		aux=recvMsgFromServer(&resposta,WAIT_FOR_IT);
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
					exit(EXIT_FAILURED);
				}
			}

	return nome;
}

void getJogador(int id, JOGADORES *jogadorNovo){

	jogadorNovo->id=id;

	jogadorNovo->nome=getNome();

	jogadorNovo->comedor='V';

	jogadorNovo->vida=1;

	srand((unsigned)time(NULL));

	jogadorNovo->x=rand()%MAXTRIZ-1;
	jogadorNovo->y=rand()%MAXTRIZ-1;
	jogadorNovo->x_old=jogadorNovo->x;
	jogadorNovo->y_old=jogadorNovo->y;
}

void selecionarPersonagem(int id, JOGADORES *jogadorNovo){

	getJogador(id, jogadorNovo);//inicia o jogador

}


void mecanica(JOGADORES * player){
	char move;
	move=getch();
	move=(char)toupper(move);
	if(move!=NO_KEY_PRESSED){
			
		switch(move){
			case 'W':(player->y_old)=(player->y);
			(player->y)-=1;
			break;
			case 'A':(player->x_old)=(player->x);
			(player->x)-=1;
			break;
			case 'D':(player->x_old)=(player->x);
			(player->x)+=1;
			break;
			case 'S':(player->y_old)=(player->y);
			(player->y)+=1;
			break;
			default:
			break;
		}
		
	}	
}

void Wait(void){
	 register int linha;
	 register int contador=0;
	 puts("iniciando jogo.");
	 for(contador=5;contador>0;contador--){
	 	for(linha=0;linha<640000000;linha++);
	 	printf("%d\n",contador);
	 }

}


void printanatela(char matriz[MAXTRIZ][MAXTRIZ],JOGADORES * jogador){
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
	
	
}
void atualiza_meu_player(JOGADORES*playerss,JOGADORES *player){
	register int linha;
	for(linha=0;linha<MAX_PLAYERS;linha++){
		if(player->id==playerss[linha].id){
			*player=playerss[linha];
		}
	}
}

int main(void){
	char matriz[MAXTRIZ][MAXTRIZ]={" "};
	JOGADORES playerss[MAX_PLAYERS];
	JOGADORES player;
	char msg[NOME_MAX];
	int vitoria=1;
	//int maximo=MAX_PLAYERS;

	InicializarAllegro();
	Carregar_bitmaps();
	InitJanela();	

	apresentacao();

	Menu();


	int id=tentaSeConectar();


	recvMsgFromServer(msg,WAIT_FOR_IT);


	printf("%s\n",msg);
	
	selecionarPersonagem(id, &player);

	int jogo=0,aux;

	sendMsgToServer(&player,sizeof(JOGADORES));

	recvMsgFromServer(&player,WAIT_FOR_IT);

	recvMsgFromServer(&jogo,WAIT_FOR_IT);

	Wait();
	while(jogo){
		
		mecanica(&player);
		//manda para o client o time q está
		aux=sendMsgToServer(&player,sizeof(JOGADORES));
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
				puts("--------------------------------\n");
				puts("-----------Voce morreu----------\n");
				puts("--------------------------------\n");
				exit(EXIT_FAILURED);
			}else{
				if(aux==NO_MESSAGE){
					puts("---------------------------------------------\n");
					puts("Nenhuma informacao transmitida pelo servidor.\n");
					puts("---------------------------------------------\n");
				}
			}

		atualiza_meu_player(playerss,&player);	

		recvMsgFromServer(&vitoria,WAIT_FOR_IT);

		if(!vitoria){
			puts("---------------------------\n");
			puts("_________Voce ganhou!_______");
			puts("---------------------------\n");
			exit(EXIT_SUCCESS);
		}

		system("clear");
		printanatela(matriz,playerss);
	}

	Destruir();


	return 0;
}

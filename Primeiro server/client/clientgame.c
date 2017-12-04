#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include <time.h>
#include <ctype.h>
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
#define MAX_PLAYERS 2
#define MAXTRIZ 20
#define MAXIP 20
#define APT 50
typedef struct p {
	char imagem;
	int x;
	int y;
	int xa;
	int ya;
	int id;
}PERSONAGEM;
enum conn_ret_t testconnect(void){
	char ip[MAXIP];
	puts("Por favor , digite o ip do seu servidor.\n");
	scanf(" %[^\n]",ip);
	getchar();
	return connectToServer(ip);
}
void apresentacao(void){
	puts("---------------------------------------------------------------------\n");
	puts("Bem vindo ao seu looby!!!!.\nPor favor,espere o estabelecimento de conexao.\n");
	puts("---------------------------------------------------------------------\n");

}
void tentaseconectar(PERSONAGEM * player){
	enum conn_ret_t teste=testconnect();
	char opcao;
	char msg[APT];
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
	recvMsgFromServer(&ids,WAIT_FOR_IT);
	printf("Este eh seu id: %d.\n",ids);
	recvMsgFromServer(msg,WAIT_FOR_IT);
	printf("%s\n", msg);
	player->id=ids;
	//puts("Voce se conectou ao servidor.\n");
}
int verificajogador(char imagem){
	int ok=0;
	if(imagem<='z' && imagem>='a'){
		ok=1;
	}else{
		if(imagem<='Z' && imagem>='A'){
			ok=1;
		}else{
			puts("Voce nao digitou um personagem valido.\n");
		}
	}
	return ok;
}
void iniciapersonagem(PERSONAGEM *player){
	puts("----------------------------------------\n");
	puts("Coloque uma letra do alfabeto para iniciar\na interfacede seu personagem: ");
	puts("----------------------------------------\n");
	char interface;
	scanf(" %c",&interface);
	while(!verificajogador(interface)){
		scanf(" %c",&interface);
	}
	player->imagem=interface;
	srand((unsigned)time(NULL));
	player->x=(int)rand()%MAXTRIZ;
	player->y=(int)rand()%MAXTRIZ;
}
void mecanica(PERSONAGEM * player){
	char move;
	move=getch();
	move=(char)toupper(move);
	player->ya= player->y;
	player->xa= player->x;
	if(move!=NO_KEY_PRESSED){
		switch(move){
			case 'W':(player->y)-=1;
					if((player->y)<0){
						(player->y)=(MAXTRIZ-1);
					}
			break;
			case 'A':(player->x)-=1;
					if((player->x)<0){
						(player->x)=(MAXTRIZ-1);
					}
			break;
			case 'S':(player->y)+=1;
					if((player->y)>(MAXTRIZ-1)){
						(player->y)=0;
					}
			break;
			case 'D':(player->x)+=1;
					if((player->x)>(MAXTRIZ-1)){
						(player->x)=0;
					}
			break;
			default:
			break;
		}
	}
	
}
void printanatela(char matriz[MAXTRIZ][MAXTRIZ],PERSONAGEM play[MAX_PLAYERS]){
	register int linha,coluna,linhal;
	for(linha=0;linha<MAXTRIZ;linha++){
		for(coluna=0;coluna<MAXTRIZ;coluna++){
			matriz[linha][coluna]='*';
		}
	}
	for(linhal=0;linhal<MAX_PLAYERS;linhal++){
		matriz[play[linhal].y][play[linhal].x]=play[linhal].imagem;
	}
	for(linha=0;linha<MAXTRIZ;linha++){
		for(coluna=0;coluna<MAXTRIZ;coluna++){
			printf("%2c",matriz[linha][coluna]);
		}
		puts("\n");
	}
}
int main(void){
	PERSONAGEM player;
	PERSONAGEM play[MAX_PLAYERS];
	char matriz[MAXTRIZ][MAXTRIZ]={" "};
	int jogo=0;
	char msgi[APT];
	apresentacao();
	tentaseconectar(&player);
	recvMsgFromServer(msgi,WAIT_FOR_IT);
	printf("%s\n",msgi);
	iniciapersonagem(&player);
	sendMsgToServer(&player,sizeof(PERSONAGEM));
	recvMsgFromServer(&jogo,WAIT_FOR_IT);
	int deumerda=0;
	while(jogo){
		mecanica(&player);
		if(sendMsgToServer(&player,sizeof(PERSONAGEM))==SERVER_DISCONNECTED){
			puts("--------------------------------\n");
			puts("O servidor foi desconectado.\n");
			puts("--------------------------------\n");
			exit(EXIT_FAILURED);
		}
		deumerda=recvMsgFromServer(play,WAIT_FOR_IT);
		if(deumerda==SERVER_DISCONNECTED){
			puts("--------------------------------\n");
			puts("O servidor foi desconectado.\n");
			puts("--------------------------------\n");
			exit(EXIT_FAILURED);
		}else{
			if(deumerda==NO_MESSAGE){
				puts("---------------------------------------------\n");
				puts("Nenhuma informacao transmitida pelo servidor.\n");
				puts("---------------------------------------------\n");
			}
		}
		system("clear");
		printanatela(matriz,play);
	}
}

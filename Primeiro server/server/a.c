#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#define MAX_PLAYERS 2
typedef struct p {
	char imagem;
	int x;
	int y;
	int xa;
	int ya;
	int id;
}PERSONAGEM;
void ordena(PERSONAGEM play[MAX_PLAYERS]){
	register int linhai,linhaf;
	PERSONAGEM aux;
	for(linhaf=MAX_PLAYERS;linhaf>0;linhaf--){
		for(linhai=0;linhai<linhaf;linhai++){
			if(play[linhai].id > play[linhai+1].id){
				aux=play[linhai];
				play[linhai]=play[linhai+1];
				play[linhai+1]=aux;
			}
		}
	}
}
int main(void){
	PERSONAGEM player;
	PERSONAGEM play[MAX_PLAYERS];
	int id  = 0;
	char msg_espera[] = "Voce foi conectado, esperando outros jogadores";
	char msg_inicio[] = "Iniciando jogo";
	serverInit(MAX_PLAYERS);
	puts("Server is Running");
	while(id<MAX_PLAYERS-1){
		id  = acceptConnection();
		if(id != NO_CONNECTION){
			if(id <= MAX_PLAYERS-1){
				sendMsgToClient(&id, sizeof(int), id);
				sendMsgToClient(msg_espera, sizeof(msg_espera), id);
			}
		}
	}
	int iniciajogo=1;
	register int linha;
	broadcast(msg_inicio, sizeof(msg_inicio));
	for(linha=0;linha<MAX_PLAYERS;linha++)
		recvMsgFromClient(&play[linha],linha,WAIT_FOR_IT);
	ordena(play);
	struct msg_ret_t ok1,ok2;
	broadcast(&iniciajogo,sizeof(int));
	while(1){
		rejectConnection();	
		for(linha=0;linha<10000;linha++);
		ok1=recvMsgFromClient(&player,0,WAIT_FOR_IT);
		if(ok1.status==MESSAGE_OK){
			play[player.id]=player;
		}
		ok2=recvMsgFromClient(&player,1,WAIT_FOR_IT);
		if(ok2.status==MESSAGE_OK){
			play[player.id]=player;
		}				
		for(linha=0;linha<MAX_PLAYERS;linha++){
			puts("\n");
			puts("imagem: ");
			printf("%c\n",play[linha].imagem );
			puts("x: ");
			printf("%d\n",play[linha].x );
			puts("y: ");
			printf("%d\n",play[linha].y );
			puts("id: ");
			printf("%d\n",play[linha].id);
		}
		system("clear");
		broadcast(play,MAX_PLAYERS*sizeof(PERSONAGEM));
	}
	return 0;
}

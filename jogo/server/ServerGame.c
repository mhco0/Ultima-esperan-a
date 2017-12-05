#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
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

#define MAXTRIZ 420
#define MAXIP 20
#define APT 50
#define NOME_MAX 255

#define FPS 60

#define VEL 10

#define FRAMEDELAY 8

#define MAX_PLAYERS 2


typedef struct JOGADORES{

	ALLEGRO_BITMAP *sprite;
	int curFrame;
	int frameCount;
	int maxFrame;

	char nome;
	int x_old;
	int y_old;
	int x;
	int y;
	int id;
	int vida;
	char comedor;
}JOGADORES;

void quem_eh_a_cazeh(JOGADORES * player){
	int monstro,deu_certinho=0;
	register int linha;

	for(linha=0;linha<MAXTRIZ;linha++){
		if(player[linha].comedor=='C'){
			player[linha].comedor='V';
		}
	}

	do{
		srand((unsigned)time(NULL));

		monstro=rand()%MAX_PLAYERS;

		for(linha=0;linha<MAX_PLAYERS;linha++){
			if(player[linha].id==monstro && isValidId(player[linha].id)){
				player[linha].comedor='C';
				deu_certinho=1;
			}
		}
	}while(!deu_certinho);
	
}

void rand_move(JOGADORES * player,char matriz[MAXTRIZ][MAXTRIZ]){
	register int linha;
	srand((unsigned)time(NULL));

	for(linha=0;linha<MAX_PLAYERS;linha++){
		player[linha].x=rand()%MAXTRIZ;
		player[linha].y=rand()%MAXTRIZ;
		if(matriz[player[linha].y][player[linha].x] == 1){
			do{
				player[linha].x=rand()%MAXTRIZ;
				player[linha].y=rand()%MAXTRIZ;
			}while(matriz[player[linha].y][player[linha].x] == 1);
		}
	}

}

void initMatriz(char matriz[MAXTRIZ][MAXTRIZ]){
	register int linha=0,coluna=0;
	FILE * arq;
	arq=fopen("jogo/client/imagem/mapa.txt","rt");
	if(arq==NULL){
		puts("Nao conseguimos acessar o arquivo da matriz");
		exit(EXIT_FAILURED);
	}
	while(fscanf(arq,"%[^\n]\n",matriz[linha++])!=EOF);

	fclose(arq);

	for(linha=0;linha<MAXTRIZ;linha++){
		for(coluna=0;coluna<MAXTRIZ;coluna++){
			printf("%c",matriz[linha][coluna]);
		}
		printf("\n");
	}


}

void validar_os_movimentos(JOGADORES*jogadores,char matriz[MAXTRIZ][MAXTRIZ]){
	register int linha;
	for(linha=0;linha<MAX_PLAYERS;linha++){
		if(matriz[jogadores[linha].y][jogadores[linha].x]==1){
			if(matriz[jogadores[linha].y_old][jogadores[linha].x]==0){
				jogadores[linha].y=jogadores[linha].y_old;
			}else if(matriz[jogadores[linha].y][jogadores[linha].x_old]==0){
				jogadores[linha].x=jogadores[linha].x_old;
			}
		}

		if(jogadores[linha].x < 0){
			if(jogadores[linha].y>185 && jogadores[linha].y < 209){
				jogadores[linha].x = MAXTRIZ-1;
			}
		}

		if(jogadores[linha].x > MAXTRIZ-1){
			if(jogadores[linha].y>185 && jogadores[linha].y < 209){
				jogadores[linha].x = 0;
			}
		}
		

	}

}

int organiza_quem_morreu(JOGADORES *jogadores){
	register int linha;
	int x_morte;
	int y_morte;
	static int mortes=0;
	//int venceu=0;


	for(linha=0;linha<MAX_PLAYERS;linha++){
		if(jogadores[linha].comedor=='C'){
			x_morte=jogadores[linha].x;
			y_morte=jogadores[linha].y;
		}	
	}

	for(linha=0;linha<MAX_PLAYERS;linha++){
		if(jogadores[linha].comedor=='V'){
			if(jogadores[linha].x==x_morte && jogadores[linha].y==y_morte && jogadores[linha].vida==1){
				jogadores[linha].vida=0;
				mortes+=1;
				disconnectClient(linha);
				printf("%d\n",mortes );
			}
		}
	}

	return mortes;
}

void verifica_todos_os_nomes(void){
	int todos_ok=0;
	char nome;
	int resposta=0;
	int contador=0;
	register int linha;

	while(!todos_ok){

		for(linha=0;linha<MAX_PLAYERS;linha++){
			recvMsgFromClient(&nome,linha,WAIT_FOR_IT);
			resposta=1;
			sendMsgToClient(&resposta,sizeof(int),linha);
			contador+=1;
		}

		if(contador==MAX_PLAYERS){
			todos_ok=1;
		}

	} 
}


void salaDeEspera(void){
	int id =0;
	char msg[]="Iniciando escolha do personagem....";
	while(id<MAX_PLAYERS-1){
		id =acceptConnection();
		if(id!=NO_CONNECTION){
			if(id <= MAX_PLAYERS-1){
				sendMsgToClient(&id, sizeof(int), id);//manda o ID para o server 1 msg a ser enviada pro client
			}
		}
	}
	broadcast(msg,sizeof(msg));
}


void iniciarJogo(JOGADORES * player){
	int jogo=1;
	register int linha;

	for(linha=0;linha<MAX_PLAYERS;linha++){
		sendMsgToClient(&player[linha],sizeof(JOGADORES),linha);
	}

	broadcast(&jogo,sizeof(int));
}



int main(void){
	serverInit(MAX_PLAYERS);

	register int linha;
	int jogo=0,mortes=0,vitoria=1;
	JOGADORES player[MAX_PLAYERS];
	char matriz[MAXTRIZ][MAXTRIZ];

	initMatriz(matriz);

	int regina=0;

	puts("Server is Running");

	salaDeEspera();

	verifica_todos_os_nomes();


	for(linha=0;linha<MAX_PLAYERS;linha++){
		recvMsgFromClient(&player[linha],linha,WAIT_FOR_IT);
	}

	quem_eh_a_cazeh(player);

	rand_move(player,matriz);

	iniciarJogo(player);

	jogo=1;

	while(jogo){
		while(regina <=120){
			rejectConnection();
			for(linha=0;linha<MAX_PLAYERS;linha++){
				recvMsgFromClient(&player[linha],linha,WAIT_FOR_IT);
			}

			validar_os_movimentos(player,matriz);

			mortes=organiza_quem_morreu(player);
			if(regina==120){
				quem_eh_a_cazeh(player);
			}

			broadcast(player,2*sizeof(JOGADORES));

			if(mortes==MAX_PLAYERS-1){
				vitoria=0;
				broadcast(&vitoria,sizeof(int));
			}else{
				vitoria=1;
				broadcast(&vitoria,sizeof(int));			
			}

			printf("%d\n", isValidId(0));
			printf("%d\n", isValidId(1));
			printf("%d\n", isValidId(2));
			printf("%d\n", isValidId(3));
			system("clear");
			regina++;
			printf("%d\n",regina );
		}
		regina = 0;
	}
	
	return 0;
}

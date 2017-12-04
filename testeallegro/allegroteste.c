#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#ifndef EXIT_FAILURE
    #define EXIT_FAILURE 1
#endif
#define LARGURA 1536
#define ALTURA 848
void inicia_allegro(ALLEGRO_DISPLAY**j,ALLEGRO_BITMAP**i,ALLEGRO_EVENT_QUEUE **q,ALLEGRO_FONT ** f){
    int ok;
    ok=al_init();
    if(!ok){
        fprintf(stderr, "Falha ao iniciar a Allegro.\n");
        exit(EXIT_FAILURE);
    }

    ok=al_init_image_addon();
    if(!ok){
        fprintf(stderr, "Falha ao iniciar o sistema\nde imagens da Allegro.\n");
        exit(EXIT_FAILURE);
    }

    ok=al_init_font_addon();
    if(!ok){
        fprintf(stderr, "Tivemos um problema ao iniciar as fontes.\n");
        exit(EXIT_FAILURE);
    }
    ok=al_init_ttf_addon();
    if(!ok){
        fprintf(stderr, "Tivemos um problema ao iniciar o ttf.\n" );
        exit(EXIT_FAILURE);
    }
    *(j)=al_create_display(LARGURA,ALTURA);
    if(!(j)){
        fprintf(stderr, "Problema ao criar a Janela.\n");
        exit(EXIT_FAILURE);
    }

    *(i)=al_load_bitmap("index.jpeg");
    if(!*(i)){
        fprintf(stderr, "Problema ao carregar a imagem.\n");
        exit(EXIT_FAILURE);
    }

    *(q)=al_create_event_queue();
    if(!*(q)){
        fprintf(stderr, "Problema ao criar uma fila de ventos\n");
        exit(EXIT_FAILURE);
    }
    *(f)=al_load_font("submergd.ttf",48,ALLEGRO_TTF_NO_KERNING);
    if(!*(f)){
        fprintf(stderr, "Problema ao encontrar a fonte.\n");
        exit(EXIT_FAILURE);
    }
}
void destroi_tudo(ALLEGRO_DISPLAY** j,ALLEGRO_EVENT_QUEUE **q,ALLEGRO_FONT**f,ALLEGRO_BITMAP**i){
    al_destroy_display(*j);
    al_destroy_event_queue(*q);
    al_destroy_font(*f);
    al_destroy_bitmap(*i);
}   

int main(void){
    ALLEGRO_DISPLAY *janelinha=NULL;
    ALLEGRO_BITMAP * imagemzinha=NULL;
    ALLEGRO_FONT *fonte=NULL;
    ALLEGRO_EVENT_QUEUE * fila=NULL;
    int jogo=1,tem_fila,carai=72;
    char texto[]="Porque o Rafael quis";
    inicia_allegro(&janelinha,&imagemzinha,&fila,&fonte);
    al_clear_to_color(al_map_rgb(0,0,0));
    al_register_event_source(fila,al_get_display_event_source(janelinha));
    //al_draw_bitmap(imagemzinha,0,0,0);
    //al_flip_display();
    while(jogo){
        ALLEGRO_EVENT evento;
        ALLEGRO_TIMEOUT time;
        al_init_timeout(&time,0.05);
        tem_fila=al_wait_for_event_until(fila,&evento,&time);
        if(tem_fila && evento.type==ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo=0;
        }
        al_draw_bitmap(imagemzinha,600,480,0);
        al_draw_text(fonte, al_map_rgb(0,255,0),LARGURA/4,ALTURA/4,0,"teste verde");
        al_draw_text(fonte, al_map_rgb(255,0,0),600,0,0,"teste vermelho");
        al_draw_text(fonte, al_map_rgb(0,0,255),0,0,0,"teste azul");
        al_draw_textf(fonte, al_map_rgb(255,255,255),800,640,ALLEGRO_ALIGN_CENTRE,"teste %d-%s",carai,texto);
        al_flip_display();
    }
    destroi_tudo(&janelinha,&fila,&fonte,&imagemzinha);
}
//
// Created by student on 11.10.2022.
//

#include "state.h"
#include <stdio.h>

void init_state(struct state* st){
    st->turn=0;
    st->players_counter=0;
    st->curr_board=board_create(51, 25);
    struct square_t* squares=st->curr_board->squares;
    FILE* f=fopen("map.txt", "r");
    char c=0;

    for(int i=0;!feof(f);i++){
        if(fscanf(f, "%c", &c)==-1){
            break;
        }
        if(c=='w'){
            squares[i].object=WALL;
        }else if(c=='#'){
            squares[i].object=BUSHES;
        }else if(c=='c'){
            squares[i].object=COINS;
            squares[i].pid_or_coins=1;
        }else if(c=='t'){
            squares[i].object=COINS;
            squares[i].pid_or_coins=10;
        }else if(c=='T'){
            squares[i].object=COINS;
            squares[i].pid_or_coins=50;
        }else if(c=='A'){
            squares[i].object=CAMPSITE;
        }else if(c=='\n') {
            i--;
        }
    }
    fclose(f);
}
void destroy_state(struct state* st){
    board_destroy(&(st->curr_board));
}

void display_state(struct state* st){

}
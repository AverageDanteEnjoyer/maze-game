//
// Created by student on 11.10.2022.
//

#include "state.h"
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>

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
        if(c!='\n'){
            squares[i].object=(int)c;
        }else{
            i--;
        }
    }
    st->players_counter=0;
    st->turn=0;
    st->campsite.x=23;
    st->campsite.y=11;
    st->server_pid=getpid();
    fclose(f);
}
void destroy_state(struct state* st){
    board_destroy(&(st->curr_board));
}
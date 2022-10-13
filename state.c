//
// Created by student on 11.10.2022.
//

#include "state.h"
#include <stdio.h>
#include <unistd.h>
#include "player.h"

void init_state(struct state* st){
    st->curr_board=board_create(BOARD_WIDTH, BOARD_HEIGHT);
    struct square_t* squares=st->curr_board->squares;
    FILE* f=fopen("map.txt", "r");
    char c=0;

    for(int i=0;!feof(f) && i<BOARD_WIDTH*BOARD_HEIGHT ;i++){
        if(fscanf(f, "%c", &c)==-1){
            break;
        }
        if(c!='\n'){
            squares[i].object=(int)c;
        }else{
            i--;
        }
    }
    for(int i=0;i<4;i++){
        st->players[i].socket_descriptor=-1;
        st->players[i].pid=-1;
    }
    st->turn=0;
    st->campsite.x=23;
    st->campsite.y=11;
    st->server_pid=getpid();
    fclose(f);
}
void destroy_state(struct state* st){
    board_destroy(&(st->curr_board));
}
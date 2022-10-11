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
    FILE* f=fopen("./../mapa.txt", "r");
    char c=0;//Wall is -30
    fread(&c, 1, 1, f);
    printf("kk %d kk", c);
    fclose(f);
}
void destroy_state(struct state* st){
    board_destroy(&(st->curr_board));
}
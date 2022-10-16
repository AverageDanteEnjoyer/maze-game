#ifndef SO2_GAME_STATE_H
#define SO2_GAME_STATE_H

#include "board.h"
#include <stdio.h>
#include <unistd.h>
#include "player.h"

struct state{
    int endpoint;
    int turn;
    struct board_t* curr_board;
    struct player_t players[4];
    struct point_t campsite;
    int server_pid;
};

int init_state(struct state* st);
void destroy_state(struct state* st);
void move_p(struct state* serv_state, struct player_t* player);

#endif //SO2_GAME_STATE_H

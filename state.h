#ifndef SO2_GAME_STATE_H
#define SO2_GAME_STATE_H

#define BOARD_WIDTH 51
#define BOARD_HEIGHT 25

#include "board.h"
#include "player.h"

struct state{
    int endpoint;
    int turn;
    struct board_t* curr_board;
    struct player_t players[4];
    struct point_t campsite;
    int server_pid;
};

void init_state(struct state* st);
void destroy_state(struct state* st);

#endif //SO2_GAME_STATE_H

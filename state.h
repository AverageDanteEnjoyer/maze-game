#ifndef SO2_GAME_STATE_H
#define SO2_GAME_STATE_H

#include "board.h"
#include "player.h"

struct state{
    int turn;
    int players_counter;
    struct board_t* curr_board;
    struct player_t* players;
    struct point_t campsite;
    int server_pid;
};

void init_state(struct state* st);
void destroy_state(struct state* st);

#endif //SO2_GAME_STATE_H

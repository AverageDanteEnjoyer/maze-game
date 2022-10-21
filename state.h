#ifndef SO2_GAME_STATE_H
#define SO2_GAME_STATE_H

#include "board.h"
#include <stdio.h>
#include <unistd.h>
#include "player.h"
#include "beast.h"

struct state{
    int endpoint;
    int turn;
    struct board_t* curr_board;
    struct player_t players[4];
    struct beast_t beasts[10];
    struct point_t campsite;
    int server_pid;
};

int init_state(struct state* st);
void destroy_state(struct state* st);
void move_p(struct state* serv_state, struct player_t* player);
void move_b(struct state* serv_state, struct beast_t* beast);
void add_treasure(struct state* serv_state, enum object_type added_obj);

#endif //SO2_GAME_STATE_H

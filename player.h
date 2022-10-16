#ifndef SO2_GAME_PLAYER_H
#define SO2_GAME_PLAYER_H

#include "board.h"
#include <stdlib.h>
#include <math.h>

struct player_info{
    int server_pid;
    int number;
    int round;
    struct square_t view[BOARD_WIDTH*BOARD_HEIGHT];
    int c_found;
    int c_brought;
    int deaths;
    struct point_t position;
};

struct player_t{
    int pid;
    int number;
    int socket_descriptor;

    enum entity_type type;
    enum object_type last_object;
    char last_pressed_key;

    struct point_t spawn;
    struct point_t position;

    int c_found;
    int c_brought;
    int is_slowed_down;
    int deaths;
};

void init_player_view(struct player_t* player, struct player_info* info, struct board_t* board);

#endif //SO2_GAME_PLAYER_H

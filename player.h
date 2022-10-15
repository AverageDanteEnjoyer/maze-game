#ifndef SO2_GAME_PLAYER_H
#define SO2_GAME_PLAYER_H

#include "board.h"
#include <stdlib.h>

struct player_t{
    int pid;
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

struct player_info{
    int number;
    struct board_t* view;
    int c_found;
    int c_brought;
    int deaths;
};

#endif //SO2_GAME_PLAYER_H

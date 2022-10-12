#ifndef SO2_GAME_PLAYER_H
#define SO2_GAME_PLAYER_H

#include "board.h"

struct player_t{
    int pid;
    char last_pressed_key;
    struct point_t spawn;
    struct point_t position;
    int c_found;
    int c_brought;
    int deaths;
    struct board_t view;
    int socket_descriptor;
};
#endif //SO2_GAME_PLAYER_H

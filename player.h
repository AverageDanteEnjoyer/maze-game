#ifndef SO2_GAME_PLAYER_H
#define SO2_GAME_PLAYER_H

#include "board.h"

enum entity_type{
    player,
    beast,
};

struct player_t{
    int pid;
    int socket_descriptor;
    enum entity_type type;

    char last_pressed_key;

    struct point_t spawn;
    struct point_t position;
    int c_found;
    int c_brought;
    int deaths;

    struct board_t view;
};
#endif //SO2_GAME_PLAYER_H

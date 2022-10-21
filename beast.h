//
// Created by student on 21.10.2022.
//

#ifndef SO2_GAME_BEAST_H
#define SO2_GAME_BEAST_H

#include "board.h"

struct beast_t{
    int beast_id;
    struct point_t position;
    char last_key_pressed;
    enum object_type last_object;
};


#endif //SO2_GAME_BEAST_H

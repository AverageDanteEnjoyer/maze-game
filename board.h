//
// Created by student on 11.10.2022.
//

#ifndef SO2_GAME_BOARD_H
#define SO2_GAME_BOARD_H

enum object_type{
    BUSHES = '#',
    AIR = ' ',
    WALL = 'w',
    PLAYER = 'P',
    BEAST = '*',
    c_COINS = 'c',
    t_COINS = 't',
    T_COINS = 'T',
    DROP = 'D',
    CAMPSITE = 'A'
};

struct point_t{
    unsigned int x;
    unsigned int y;
};
struct square_t{
    struct point_t cords;
    enum object_type object;
    int pid_or_coins;

};
struct board_t{
    struct square_t* squares;//25x52
};
struct board_t* board_create(int width, int height);
void board_destroy(struct board_t** board);
#endif //SO2_GAME_BOARD_H

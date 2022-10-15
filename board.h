#ifndef SO2_GAME_BOARD_H
#define SO2_GAME_BOARD_H

#define BOARD_WIDTH 51
#define BOARD_HEIGHT 25

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

enum direction{
    right = 5,
    left = 4,
    top = 3,
    bot =2
};

enum entity_type{
    player,
    beast
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

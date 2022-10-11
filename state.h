#ifndef SO2_GAME_STATE_H
#define SO2_GAME_STATE_H

enum object_type{
    BUSHES,
    AIR,
    WALL,
    PLAYER,
    BEAST,
    C_COIN,
    T_COIN,
    D_COIN,
    DROPPED_T,
    CAMPSITE
};

struct square{
    unsigned int x;
    unsigned int y;
    enum object_type object;
    int pid_or_coins;

};
struct board{
    struct square** squares;//25x52
};
struct player_action{
    int key_id;
};
struct player{
    int pid;
    unsigned int x;
    unsigned int y;
    int c_found;
    int c_brought;
    int deaths;
    struct board view;
};
struct state{
    int turn;
    struct board curr_board;
    struct player* players;
};

#endif //SO2_GAME_STATE_H

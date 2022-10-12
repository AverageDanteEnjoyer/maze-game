#ifndef SO2_GAME_SERVER_H
#define SO2_GAME_SERVER_H

#include "player.h"

int init_Server(char* ip);
void* handle_connection(void*);
void* handle_information_flow(void* args);
void* Quit(void* args);
void* handle_state_update(void* args);
void init_colors();
void update_screen();
int find_free(struct player_t clients[], int size);

#endif //SO2_GAME_SERVER_H

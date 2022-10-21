#ifndef SO2_GAME_SERVER_H
#define SO2_GAME_SERVER_H

#include "player.h"
#include "state.h"

void* handle_connection(void*);
void* listen_to_client(void* args);
void* handle_server_input(void* args);
void* handle_state_update(void* args);
void init_colors();
void update_screen(struct state* serv_state);
void player_on_join(int player_number);
void player_on_disconnect(int player_number);

int init_server_process(char* ip);;

void* beast_routine(void* args);
int player_to_chase(int beast_x, int beast_y, int* x, int* y);

int find_free(struct player_t clients[], int size);

#endif //SO2_GAME_SERVER_H

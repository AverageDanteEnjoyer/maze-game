#ifndef SO2_GAME_SERVER_H
#define SO2_GAME_SERVER_H

#include "player.h"
#include "state.h"

int init_Server(char* ip);
void* handle_connection(void*);
void* listen_to_client(void* args);
void* Quit(void* args);
void* handle_state_update(void* args);
void init_colors();
void update_screen(struct state* serv_state);
int find_free(struct player_t clients[], int size);
void player_on_join(int player_number);
void player_on_disconnect(int player_number);

#endif //SO2_GAME_SERVER_H

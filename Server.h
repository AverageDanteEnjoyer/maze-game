#ifndef SO2_GAME_SERVER_H
#define SO2_GAME_SERVER_H

int init_Server(char* ip);
void* handle_connection(void*);
void* handle_information_flow(void* args);
void* Quit(void* args);
void* handle_state_update(void* args);

#endif //SO2_GAME_SERVER_H

//
// Created by student on 15.10.2022.
//

#ifndef SO2_GAME_CLIENT_H
#define SO2_GAME_CLIENT_H

#define TEXT_COLOR 1

#include <sys/socket.h>
#include "player.h"
#include <ncurses.h>
#include <unistd.h>
#include "state.h"
#include "ncurses.h"
#include <pthread.h>
#include "Server.h"

void* listen_s(void* args);
void* send_s(void* args);
void client_update_screen(struct player_info* info);
int init_player_client(int endpoint);

#endif //SO2_GAME_CLIENT_H

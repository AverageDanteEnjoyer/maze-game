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

void* listen_s(void* args);
void* send_s(void* args);
void client_update_screen(struct player_info* info);

#endif //SO2_GAME_CLIENT_H

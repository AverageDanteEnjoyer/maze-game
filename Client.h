//
// Created by student on 15.10.2022.
//

#ifndef SO2_GAME_CLIENT_H
#define SO2_GAME_CLIENT_H

#include <sys/socket.h>
#include "player.h"
#include <ncurses.h>
#include <unistd.h>

void* listen_s(void* args);
void* send_s(void* args);

#endif //SO2_GAME_CLIENT_H

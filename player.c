//
// Created by student on 11.10.2022.
//

#include "player.h"

void init_player_view(struct player_t* player, struct board_t* board){
    for(int i=0;i<BOARD_WIDTH*BOARD_HEIGHT;i++){
        player->info.view[i].cords.x=board->squares[i].cords.x;
        player->info.view[i].cords.y=board->squares[i].cords.y;

        int x_diff=abs((int)player->position.x-(i%BOARD_WIDTH));
        int y_diff=abs((int)player->position.y-(i/BOARD_WIDTH));
        if(x_diff > 2 || y_diff > 2) {
            player->info.view[i].object=AIR;
            player->info.view[i].object=0;
        }else{
            player->info.view[i].object=board->squares[i].object;
            player->info.view[i].pid_or_coins=board->squares[i].pid_or_coins;
        }
    }
}
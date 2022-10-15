//
// Created by student on 11.10.2022.
//

#include "player.h"

void init_player_view(struct player_t* player, struct player_info* info, struct board_t* board){
    for(int i=0;i<BOARD_WIDTH*BOARD_HEIGHT;i++){
        info->view[i].cords.x=board->squares[i].cords.x;
        info->view[i].cords.y=board->squares[i].cords.y;

        int x_diff=abs((int)player->position.x-(i%BOARD_WIDTH));
        int y_diff=abs((int)player->position.y-(i/BOARD_WIDTH));
        if(x_diff > 2 || y_diff > 2) {
            info->view[i].object=AIR;
            info->view[i].object=0;
        }else{
            info->view[i].object=board->squares[i].object;
            info->view[i].pid_or_coins=board->squares[i].pid_or_coins;
        }
    }
}
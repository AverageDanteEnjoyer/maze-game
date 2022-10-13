//
// Created by student on 11.10.2022.
//

#include "state.h"
#include <stdio.h>
#include <unistd.h>
#include "player.h"

void init_state(struct state* st){
    st->curr_board=board_create(BOARD_WIDTH, BOARD_HEIGHT);
    struct square_t* squares=st->curr_board->squares;
    FILE* f=fopen("map.txt", "r");
    char c=0;

    for(int i=0;!feof(f) && i<BOARD_WIDTH*BOARD_HEIGHT ;i++){
        if(fscanf(f, "%c", &c)==-1){
            break;
        }
        if(c!='\n'){
            squares[i].object=(int)c;
        }else{
            i--;
        }
    }
    for(int i=0;i<4;i++){
        st->players[i].socket_descriptor=-1;
        st->players[i].pid=-1;
    }
    st->turn=1;
    st->campsite.x=23;
    st->campsite.y=11;
    st->server_pid=getpid();
    fclose(f);
}
void destroy_state(struct state* st){
    board_destroy(&(st->curr_board));
}

void move_p(struct state* serv_state, struct player_t* player){
    int direction=player->last_pressed_key;
    struct square_t* destination;
    struct square_t* curr_square=&serv_state->curr_board->squares[player->position.y*BOARD_WIDTH+player->position.x];

    player->last_pressed_key=0;

    if(player->is_slowed_down){
        player->is_slowed_down=0;
        return;
    }

    if(direction == right){
        destination=(curr_square+1);
    }else if(direction == left){
        destination=(curr_square-1);
    }else if(direction == top){
        destination=(curr_square-BOARD_WIDTH);
    }else if(direction == bot){
        destination=(curr_square+BOARD_WIDTH);
    }else{
        return;
    }
    if(destination->object!=WALL && destination->object!=PLAYER && destination->object!=BEAST) {
        if(destination->object==c_COINS){
            player->c_found++;
        }else if(destination->object==t_COINS){
            player->c_found=player->c_found+10;
        }else if(destination->object==T_COINS){
            player->c_found=player->c_found+50;
        }

        if(destination->object==BUSHES){
            player->is_slowed_down=1;
        }
        if(destination->object==CAMPSITE){
            player->c_brought=player->c_brought+player->c_found;
            player->c_found=0;
        }

        curr_square->object=player->last_object;
        curr_square->pid_or_coins=0;
        if(destination->object == CAMPSITE || destination->object == BUSHES){
            player->last_object=destination->object;
        }else{
            player->last_object=AIR;
        }

        destination->object = PLAYER;
        destination->pid_or_coins=player->pid;

        player->position.x = destination->cords.x;
        player->position.y = destination->cords.y;
    }else if(destination->object==PLAYER){
        struct player_t* killed_player;
        for(int i=0;i<4;i++){
            if(serv_state->players[i].pid==destination->pid_or_coins){
                killed_player=&serv_state->players[i];
            }
        }
        destination->object=DROP;
        destination->pid_or_coins=player->c_found+killed_player->c_found;

        player->c_found=0;
        killed_player->c_found=0;

        curr_square->object=player->last_object;
        curr_square->pid_or_coins=0;

        player->last_object=AIR;
        killed_player->last_object=AIR;

        player->position.x=player->spawn.x;
        player->position.y=player->spawn.y;
        killed_player->position.x=killed_player->spawn.x;
        killed_player->position.y=killed_player->spawn.y;

        serv_state->curr_board->squares[player->spawn.y*BOARD_WIDTH+player->spawn.x].object=PLAYER;
        serv_state->curr_board->squares[player->spawn.y*BOARD_WIDTH+player->spawn.x].pid_or_coins=player->pid;
        serv_state->curr_board->squares[killed_player->spawn.y*BOARD_WIDTH+killed_player->spawn.x].object=PLAYER;
        serv_state->curr_board->squares[killed_player->spawn.y*BOARD_WIDTH+killed_player->spawn.x].pid_or_coins=killed_player->pid;
    }
}
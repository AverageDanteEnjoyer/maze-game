//
// Created by student on 11.10.2022.
//

#include "state.h"

int init_state(struct state* st){
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
    for(int i=0;i<10;i++){
        st->beasts[i].beast_id=-1;
    }
    st->turn=1;
    st->campsite.x=23;
    st->campsite.y=11;
    st->server_pid=getpid();
    fclose(f);

    return 0;
}
void destroy_state(struct state* st){
    board_destroy(&(st->curr_board));
}

void move_p(struct state* serv_state, struct player_t* player){
    int direction=(int)player->last_pressed_key;
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
        }else if(destination->object==DROP){
            player->c_found=player->c_found+destination->pnumber_or_coins;
            destination->pnumber_or_coins=0;
        }

        if(destination->object==BUSHES){
            player->is_slowed_down=1;
        }
        if(destination->object==CAMPSITE){
            player->c_brought=player->c_brought+player->c_found;
            player->c_found=0;
        }

        curr_square->object=player->last_object;
        curr_square->pnumber_or_coins=0;
        if(destination->object == CAMPSITE || destination->object == BUSHES){
            player->last_object=destination->object;
        }else{
            player->last_object=AIR;
        }

        destination->object = PLAYER;
        destination->pnumber_or_coins=player->number;

        player->position.x = destination->cords.x;
        player->position.y = destination->cords.y;
    }else if(destination->object==PLAYER){
        struct player_t* killed_player;
        for(int i=0;i<4;i++){
            if(serv_state->players[i].number==destination->pnumber_or_coins){
                killed_player=&serv_state->players[i];
            }
        }
        destination->object=DROP;
        destination->pnumber_or_coins=player->c_found+killed_player->c_found;

        player->c_found=0;
        killed_player->c_found=0;

        curr_square->object=player->last_object;
        curr_square->pnumber_or_coins=0;

        player->last_object=AIR;
        killed_player->last_object=AIR;

        player->position.x=player->spawn.x;
        player->position.y=player->spawn.y;
        killed_player->position.x=killed_player->spawn.x;
        killed_player->position.y=killed_player->spawn.y;

        player->deaths++;
        killed_player->deaths++;

        serv_state->curr_board->squares[player->spawn.y*BOARD_WIDTH+player->spawn.x].object=PLAYER;
        serv_state->curr_board->squares[player->spawn.y*BOARD_WIDTH+player->spawn.x].pnumber_or_coins=player->number;
        serv_state->curr_board->squares[killed_player->spawn.y*BOARD_WIDTH+killed_player->spawn.x].object=PLAYER;
        serv_state->curr_board->squares[killed_player->spawn.y*BOARD_WIDTH+killed_player->spawn.x].pnumber_or_coins=killed_player->number;
    }
}

void move_b(struct state* serv_state, struct beast_t* beast){
    int direction=(int)beast->last_key_pressed;
    struct square_t* destination;
    struct square_t* curr_square=&serv_state->curr_board->squares[beast->position.y*BOARD_WIDTH+beast->position.x];

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

    if(destination->object==WALL || destination->object==BEAST){
        return;
    }

    curr_square->object=beast->last_object;
    if(destination->object!=PLAYER){
        beast->last_object=destination->object;
    }else{
        struct player_t* killed_player;
        for(int i=0;i<4;i++){
            if(serv_state->players[i].number==destination->pnumber_or_coins){
                killed_player=&serv_state->players[i];
            }
        }
        beast->last_object=DROP;
        destination->pnumber_or_coins=killed_player->c_found;
        killed_player->c_found=0;
        killed_player->last_object=AIR;
        killed_player->position.x=killed_player->spawn.x;
        killed_player->position.y=killed_player->spawn.y;
        killed_player->deaths++;
        serv_state->curr_board->squares[killed_player->spawn.y*BOARD_WIDTH+killed_player->spawn.x].object=PLAYER;
        serv_state->curr_board->squares[killed_player->spawn.y*BOARD_WIDTH+killed_player->spawn.x].pnumber_or_coins=killed_player->number;
    }
    destination->object=BEAST;
    beast->position.x=destination->cords.x;
    beast->position.y=destination->cords.y;
}

void add_treasure(struct state* serv_state, enum object_type added_obj){
    int x;
    int y;
    while(1){
        x=(rand() % (BOARD_WIDTH-1)) + 1;
        y=(rand() % (BOARD_HEIGHT-1)) + 1;

        if(serv_state->curr_board->squares[y*BOARD_WIDTH+x].object==AIR){
            serv_state->curr_board->squares[y*BOARD_WIDTH+x].object=added_obj;
            break;
        }
    }
}
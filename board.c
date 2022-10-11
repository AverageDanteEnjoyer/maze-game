//
// Created by student on 11.10.2022.
//

#include "board.h"
#include <stdlib.h>

struct board_t* board_create(int width, int height){
    struct board_t* board=calloc(1, sizeof(struct board_t));
    if(!board){
        return NULL;
    }
    board->squares=calloc(width*height, sizeof(struct square_t));
    if(!board->squares){
        free(board);
        return NULL;
    }
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            board->squares[i*height+j].cords.x=i;
            board->squares[i*height+j].cords.y=j;
            board->squares[i*height+j].object=AIR;
            board->squares[i*height+j].pid_or_coins=0;
        }
    }
    return board;
}
void board_destroy(struct board_t** board){
    if(!board){
        return;
    }
    if(!*board){
        return;
    }
    if((*board)->squares){
        free((*board)->squares);
    }
    free(*board);
    *board=NULL;
}
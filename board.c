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
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            board->squares[i*width+j].cords.x=j;
            board->squares[i*width+j].cords.y=i;
            board->squares[i*width+j].object=AIR;
            board->squares[i*width+j].pnumber_or_coins=0;
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
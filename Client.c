//
// Created by student on 15.10.2022.
//

#include "Client.h"

void* listen_s(void* args){
    int endpoint=*(int*)args;
    struct player_info received;

    while(recv(endpoint, &received, sizeof(struct player_info), 0) > 0){
        printw("received num %d\n", received.number);
    }
    return NULL;
}

void* send_s(void* args){
    int endpoint=*(int*)args;
    char c;

    keypad(stdscr, TRUE);
    timeout(500);

    while(1){
        c=getch();
        if(c == 'q' || c == 'Q'){
            send(endpoint, &c, 1, 0);
            break;
        }
        if(send(endpoint, &c, 1, 0)<=0){
            break;
        }
    }
    return NULL;
}
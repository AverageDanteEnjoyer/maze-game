//
// Created by student on 15.10.2022.
//

#include "Client.h"

void* listen_s(void* args){
    int endpoint=*(int*)args;
    struct player_info received;

    while(recv(endpoint, &received, sizeof(struct player_info), 0) > 0){
        client_update_screen(&received);
    }
    clear();
    return NULL;
}

void* send_s(void* args){
    int endpoint=*(int*)args;
    char c;

    keypad(stdscr, TRUE);
    curs_set(0);

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

void client_update_screen(struct player_info* info){
    clear();
    struct square_t* sq=info->view;
    for(int i=0;i<BOARD_WIDTH*BOARD_HEIGHT-1;i++){
        if(i%BOARD_WIDTH==0){
            printw("\n");
        }
        attron(COLOR_PAIR(sq[i].object));
        if(sq[i].object==PLAYER){
            mvprintw(sq[i].cords.y+1, sq[i].cords.x, "%d", sq[i].pnumber_or_coins);
        }else{
            mvprintw(sq[i].cords.y+1, sq[i].cords.x, "%c", sq[i].object);
        }
        attroff(COLOR_PAIR(sq[i].object));
    }

    attron(COLOR_PAIR(TEXT_COLOR));
    mvprintw(2, 58, "Server's PID: %d", info->server_pid);
    mvprintw(3, 59, "Campsite X/Y: unknown");
    mvprintw(4, 59, "Round number: %d", info->round);

    mvprintw(6, 58, "Parameter: ");
    mvprintw(7, 59, "Number");
    mvprintw(8, 59, "Type");
    mvprintw(9, 59, "Curr X/Y");
    mvprintw(10, 59, "Deaths");
    mvprintw(13, 59, "Coins");
    mvprintw(14, 63, "carried");
    mvprintw(15, 63, "brought");

    mvprintw(6, 74, "Player");
    mvprintw(7, 74, "%d", info->number);
    mvprintw(8, 74, "HUMAN");
    mvprintw(9, 74, "%d/%d", info->position.x, info->position.y);
    mvprintw(10, 74, "%d", info->deaths);
    mvprintw(14, 74, "%d", info->c_found);
    mvprintw(15, 74, "%d", info->c_brought);

    mvprintw(17, 58, "Legend: ");
    attron(COLOR_PAIR(PLAYER));
    mvprintw(18, 59, "1234");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - players");

    attron(COLOR_PAIR(WALL));
    mvprintw(19, 59, "w");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - wall");

    attron(COLOR_PAIR(BUSHES));
    mvprintw(20, 59, "#");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - bushes(slow down)");

    attron(COLOR_PAIR(BEAST));
    mvprintw(21, 59, "*");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - wild beast");

    attron(COLOR_PAIR(c_COINS));
    mvprintw(22, 59, "c");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - one coin");

    attron(COLOR_PAIR(t_COINS));
    mvprintw(23, 59, "t");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - treasure(10 coins)");

    attron(COLOR_PAIR(T_COINS));
    mvprintw(24, 59, "T");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - large treasure(50 coins)");

    attron(COLOR_PAIR(CAMPSITE));
    mvprintw(25, 59, "A");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - campsite");

    attron(COLOR_PAIR(DROP));
    mvprintw(22, 89, "D");
    attron(COLOR_PAIR(TEXT_COLOR));
    printw(" - dropped treasure");
}
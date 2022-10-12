#include <ncurses.h>
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <ncurses.h>
#include "state.h"
#include <stdlib.h>

#define TEXT_COLOR 1

pthread_t thread_pool[4];
pthread_t state_thread;
pthread_t quit_thread;
pthread_t connection_thread;

char player_a=' ';
struct state curr;

int init_Server(char * ip){
    int endpoint;
    struct sockaddr_in serv_addr;

    initscr();
    start_color();
    init_colors();
    noecho();

    init_state(&curr);

    if((endpoint = socket(AF_INET, SOCK_STREAM, 0))< 0){
        printf("Failed to create socket");
        destroy_state(&curr);
        endwin();
        return 1;
    }
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(8000);

    if(bind(endpoint, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Failed to bind\n");
        destroy_state(&curr);
        endwin();
        return 1;
    }

    listen(endpoint, 4);
    pthread_create(&connection_thread, NULL, &handle_connection, &endpoint);
    pthread_detach(connection_thread);

    pthread_create(&state_thread, NULL, &handle_state_update, NULL);
    pthread_detach(state_thread);

    pthread_create(&quit_thread, NULL, &Quit, NULL);
    pthread_join(quit_thread, NULL);

    close(endpoint);

    for(int i=0;i<4;i++){
       if(curr.players[i].socket_descriptor!=-1){
           close(curr.players[i].socket_descriptor);
       }
    }
    destroy_state(&curr);
    endwin();
    return 0;
}

void* handle_connection(void* args){
    int endpoint=*(int*)args;
    int soc_desc;

    while(1){
        soc_desc = accept(endpoint, (struct sockaddr*)NULL, NULL);
        if(soc_desc!=-1){
            int free_index=find_free(curr.players, 4);
            if(free_index==-1){
                close(endpoint);
                continue;
            }
            curr.players[free_index].socket_descriptor=soc_desc;
            pthread_create(&thread_pool[free_index], NULL, &handle_information_flow, &curr.players[free_index]);
            pthread_detach(thread_pool[free_index]);
        }
    }
}

void* handle_information_flow(void* args){
    struct player_t* speaker=(struct player_t*)args;
    int soc_desc = speaker->socket_descriptor;


    recv(soc_desc, &speaker->pid, sizeof(int), 0);
    speaker->deaths=0;
    speaker->last_pressed_key='\0';
    printf("Client of ID: %d connected to server\n", speaker->pid);

    while(1){
        if(recv(soc_desc, &player_a, 1, 0)==0){
            printf("\nClient of ID: %d has left\n", speaker->pid);
            close(soc_desc);
            speaker->socket_descriptor=-1;
            break;
        }
    }
}

void* handle_state_update(void* args){
    while(1){
        update_screen();
        sleep(1);
    }
}

void* Quit(void* args){
    char c;

    while(1){
        c=getch();
        if(c == 'Q' || c == 'q'){
            return NULL;
        }
    }
}

void init_colors(){
    init_pair(WALL, COLOR_WHITE, COLOR_WHITE);
    init_pair(AIR, COLOR_BLACK, COLOR_BLACK);
    init_pair(c_COINS, COLOR_BLACK, COLOR_YELLOW);
    init_pair(t_COINS, COLOR_BLACK, COLOR_YELLOW);
    init_pair(T_COINS, COLOR_BLACK, COLOR_YELLOW);
    init_pair(DROP, COLOR_BLACK, COLOR_YELLOW);
    init_pair(CAMPSITE, COLOR_WHITE, COLOR_GREEN);
    init_pair(BUSHES, COLOR_GREEN, COLOR_BLACK);
    init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(PLAYER, COLOR_WHITE, COLOR_BLUE);
    init_pair(BEAST, COLOR_RED, COLOR_BLACK);
}
void update_screen(){
    move(0, 0);
    struct square_t* sq=curr.curr_board->squares;
    for(int i=0;i<BOARD_WIDTH*BOARD_HEIGHT;i++){
        if(i%BOARD_WIDTH==0){
            printw("\n");
        }
        attron(COLOR_PAIR(sq[i].object));
        printw("%c", sq[i].object);
        attroff(COLOR_PAIR(sq[i].object));
    }

    attron(COLOR_PAIR(TEXT_COLOR));
    mvprintw(2, 58, "Server's PID: %d", curr.server_pid);
    mvprintw(3, 59, "Campsite X/Y: %d/%d", curr.campsite.x, curr.campsite.y);
    mvprintw(4, 59, "Round number: %d", curr.turn);

    mvprintw(6, 58, "Parameter: ");
    //...

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

    refresh();
}

int find_free(struct player_t clients[], int size){
    for(int i=0;i<size;i++){
        if(clients[i].socket_descriptor==-1){
            return i;
        }
    }
    return -1;
}
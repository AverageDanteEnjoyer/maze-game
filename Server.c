#include <ncurses.h>
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "state.h"
#include <time.h>
#include <stdlib.h>

pthread_mutex_t prevent_validate_disrupt;

#define TEXT_COLOR 1

pthread_t thread_pool[4];
pthread_t state_thread;
pthread_t quit_thread;
pthread_t connection_thread;
struct state serv_state;

int init_Server(char * ip){
    srand(time(NULL));
    struct sockaddr_in serv_addr;

    initscr();
    start_color();
    init_colors();
    noecho();

    init_state(&serv_state);

    if((serv_state.endpoint = socket(AF_INET, SOCK_STREAM, 0))< 0){
        printf("Failed to create socket");
        destroy_state(&serv_state);
        endwin();
        return 1;
    }
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(8000);

    if(bind(serv_state.endpoint, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Failed to bind\n");
        destroy_state(&serv_state);
        endwin();
        return 1;
    }

    listen(serv_state.endpoint, 4);

    pthread_mutex_init(&prevent_validate_disrupt, NULL);
    pthread_create(&connection_thread, NULL, &handle_connection, NULL);
    pthread_detach(connection_thread);

    pthread_create(&state_thread, NULL, &handle_state_update, NULL);
    pthread_detach(state_thread);

    pthread_create(&quit_thread, NULL, &Quit, NULL);
    pthread_join(quit_thread, NULL);

    pthread_mutex_destroy(&prevent_validate_disrupt);
    for(int i=0;i<4;i++){
        if(serv_state.players[i].socket_descriptor!=-1){
            close(serv_state.players[i].socket_descriptor);
            serv_state.players[i].socket_descriptor=-1;
        }
    }
    close(serv_state.endpoint);

    destroy_state(&serv_state);
    endwin();
    return 0;
}

void* handle_connection(void* args){
    int soc_desc;
    while(1){
        soc_desc = accept(serv_state.endpoint, (struct sockaddr*)NULL, NULL);
        if(soc_desc!=-1){
            int free_index=find_free(serv_state.players, 4);
            if(free_index==-1){
                close(soc_desc);
                continue;
            }
            serv_state.players[free_index].socket_descriptor=soc_desc;
            serv_state.players[free_index].type=player;
            pthread_create(&thread_pool[free_index], NULL, &listen_to_client, &serv_state.players[free_index]);
            pthread_detach(thread_pool[free_index]);
        }
    }
}

void* listen_to_client(void* args){
    struct player_t* speaker=(struct player_t*)args;
    player_on_join(speaker);

    char key_pressed=0;
    int recv_size=0;
    while(1){
        recv_size=recv(speaker->socket_descriptor, &key_pressed, 1, 0);
        pthread_mutex_lock(&prevent_validate_disrupt);
        if(recv_size<=0){
            player_on_disconnect(speaker);
            pthread_mutex_unlock(&prevent_validate_disrupt);
           break;
        }
        speaker->last_pressed_key=key_pressed;
        pthread_mutex_unlock(&prevent_validate_disrupt);
    }
}

void* handle_state_update(void* args){
    struct player_info players_data[4];
    while(1){
        pthread_mutex_lock(&prevent_validate_disrupt);
        for(int i=0;i<4;i++){
            if(serv_state.players[i].pid!=-1){
                move_p(&serv_state, &serv_state.players[i]);
            }
        }
        update_screen(&serv_state);
        serv_state.turn++;
        for(int i=0;i<4;i++){
            if(serv_state.players[i].pid!=-1){
                players_data[i].number=i+1;
                players_data[i].c_found=serv_state.players[i].c_found;
                players_data[i].c_brought=serv_state.players[i].c_brought;
                players_data[i].deaths=serv_state.players[i].deaths;
                send(serv_state.players[i].socket_descriptor, &players_data[i], sizeof(struct player_info), 0);
            }
        }
        pthread_mutex_unlock(&prevent_validate_disrupt);
        usleep(500000);
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
void update_screen(struct state* st){
    move(0, 0);
    struct square_t* sq=st->curr_board->squares;
    for(int i=0;i<BOARD_WIDTH*BOARD_HEIGHT;i++){
        if(i%BOARD_WIDTH==0){
            printw("\n");
        }
        attron(COLOR_PAIR(sq[i].object));
        printw("%c", sq[i].object);
        attroff(COLOR_PAIR(sq[i].object));
    }

    attron(COLOR_PAIR(TEXT_COLOR));
    mvprintw(2, 58, "Server's PID: %d", st->server_pid);
    mvprintw(3, 59, "Campsite X/Y: %d/%d", st->campsite.x, st->campsite.y);
    mvprintw(4, 59, "Round number: %d", st->turn);

    mvprintw(6, 58, "Parameter: ");
    mvprintw(7, 59, "PID");
    mvprintw(8, 59, "Type");
    mvprintw(9, 59, "Curr X/Y");
    mvprintw(10, 59, "Deaths");
    mvprintw(13, 59, "Coins");
    mvprintw(14, 63, "carried");
    mvprintw(15, 63, "brought");

    for(int i=0;i<4;i++){
        struct player_t curr_player = serv_state.players[i];
        mvprintw(6, 74+i*10, "Player%d", (i+1));
        if(curr_player.pid!=-1){
            mvprintw(7, 74+i*10, "%d", curr_player.pid);
            mvprintw(8, 74+i*10, "HUMAN");
            mvprintw(9, 74+i*10, "%d/%d", curr_player.spawn.x, curr_player.spawn.y);
            mvprintw(10, 74+i*10, "%d", curr_player.deaths);
            mvprintw(14, 74+i*10, "%d", curr_player.c_found);
            mvprintw(15, 74+i*10, "%d", curr_player.c_brought);
            attron(COLOR_PAIR(PLAYER));
            mvprintw(curr_player.position.y+1, curr_player.position.x, "%d", i+1);
            attron(COLOR_PAIR(TEXT_COLOR));
        }else{
            mvprintw(7, 74+i*10, "-");
            mvprintw(8, 74+i*10, "-");
            mvprintw(9, 74+i*10, "-/-");
            mvprintw(10, 74+i*10, "-");
        }
    }
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
void player_on_join(struct player_t* speaker){
    speaker->last_pressed_key='\0';
    speaker->deaths=0;
    speaker->c_brought=0;
    speaker->c_found=0;
    speaker->is_slowed_down=0;
    speaker->last_object=AIR;

    while(1){
        speaker->spawn.x=(rand() % (BOARD_WIDTH-1)) + 1;
        speaker->spawn.y=(rand() % (BOARD_HEIGHT-1)) + 1;
        if(serv_state.curr_board->squares[speaker->spawn.y*BOARD_WIDTH+speaker->spawn.x].object==AIR){
            break;
        }
    }
    speaker->position.x=speaker->spawn.x;
    speaker->position.y=speaker->spawn.y;
    serv_state.curr_board->squares[speaker->position.y*BOARD_WIDTH+speaker->position.x].object=PLAYER;
    serv_state.curr_board->squares[speaker->position.y*BOARD_WIDTH+speaker->position.x].pid_or_coins=speaker->pid;

    recv(speaker->socket_descriptor, &speaker->pid, sizeof(int), 0);
}
void player_on_disconnect(struct player_t* speaker){
    if(speaker->socket_descriptor!=-1){
        close(speaker->socket_descriptor);
    }
    serv_state.curr_board->squares[speaker->position.y*BOARD_WIDTH+speaker->position.x].object=AIR;
    serv_state.curr_board->squares[speaker->position.y*BOARD_WIDTH+speaker->position.x].pid_or_coins=0;
    bzero(speaker, sizeof(struct player_t));
    speaker->pid=-1;
    speaker->socket_descriptor=-1;
}
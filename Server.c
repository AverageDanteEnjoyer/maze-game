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
#include "beast.h"

#define TEXT_COLOR 1

pthread_mutex_t prevent_validate_disrupt;

pthread_t player_thread_pool[4];
pthread_t beast_thread_pool[10];
pthread_t state_thread;
pthread_t quit_thread;
pthread_t connection_thread;

struct state serv_state;

int init_server_process(char* ip){
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

    pthread_create(&quit_thread, NULL, &handle_server_input, NULL);
    pthread_join(quit_thread, NULL);

    pthread_cancel(connection_thread);
    pthread_cancel(state_thread);
    for(int i=0;i<4;i++){
        if(serv_state.players[i].socket_descriptor!=-1){
            close(serv_state.players[i].socket_descriptor);
            serv_state.players[i].socket_descriptor=-1;
        }
    }
    pthread_mutex_destroy(&prevent_validate_disrupt);
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
            pthread_create(&player_thread_pool[free_index], NULL, &listen_to_client, &serv_state.players[free_index]);
            pthread_detach(player_thread_pool[free_index]);
        }
    }
}

void* listen_to_client(void* args){
    struct player_t* speaker=(struct player_t*)args;
    int player_number;

    for(int i=0;i<4;i++){
        if(speaker == &serv_state.players[i]){
            player_number=i;
        }
    }
    pthread_mutex_lock(&prevent_validate_disrupt);
    player_on_join(player_number);
    pthread_mutex_unlock(&prevent_validate_disrupt);

    char key_pressed=0;
    int recv_size=0;
    while(1){
        recv_size=recv(speaker->socket_descriptor, &key_pressed, 1, 0);
        if(recv_size<=0){

            pthread_mutex_lock(&prevent_validate_disrupt);
            player_on_disconnect(player_number);
            pthread_mutex_unlock(&prevent_validate_disrupt);

            break;
        }
        speaker->last_pressed_key=key_pressed;
    }
}

void* handle_state_update(void* args){
    while(1){
        pthread_mutex_lock(&prevent_validate_disrupt);
        for(int i=0;i<4;i++){
            if(serv_state.players[i].pid!=-1){
                move_p(&serv_state, &serv_state.players[i]);
            }
        }
        for(int i=0;i<10;i++){
            move_b(&serv_state, &serv_state.beasts[i]);
        }
        update_screen(&serv_state);
        serv_state.turn++;
        for(int i=0;i<4;i++){
            struct player_info info;
            if(serv_state.players[i].pid!=-1){

                info.number=serv_state.players[i].number;
                info.c_found=serv_state.players[i].c_found;
                info.c_brought=serv_state.players[i].c_brought;
                info.deaths=serv_state.players[i].deaths;
                info.round=serv_state.turn;
                info.server_pid=serv_state.server_pid;
                info.position.x=serv_state.players[i].position.x;
                info.position.y=serv_state.players[i].position.y;

                init_player_view(&serv_state.players[i], &info, serv_state.curr_board);

                send(serv_state.players[i].socket_descriptor, &info, sizeof(struct player_info), 0);
            }
        }
        pthread_mutex_unlock(&prevent_validate_disrupt);
        usleep(500000);
    }
}

void* handle_server_input(void* args){
    char c;
    int no_beasts=0;
    while(1){
        c=getch();
        switch(c){
            case 'Q':
            case 'q':
                for(int i=0;i<no_beasts;i++){
                    pthread_cancel(beast_thread_pool[i]);
                }
                return NULL;
            case c_COINS:
            case t_COINS:
            case T_COINS:
                add_treasure(&serv_state, c);
                break;
            case 'B':
            case 'b':
                if(no_beasts<10){
                    pthread_create(&beast_thread_pool[no_beasts], NULL, &beast_routine, &no_beasts);
                    pthread_detach(beast_thread_pool[no_beasts]);
                }
                break;
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
        if(sq[i].object==PLAYER){
            printw("%d", sq[i].pnumber_or_coins);
        }else{
            printw("%c", sq[i].object);
        }
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
void player_on_join(int player_number){
    struct player_t* speaker=&serv_state.players[player_number];
    speaker->number=player_number+1;
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
    serv_state.curr_board->squares[speaker->position.y*BOARD_WIDTH+speaker->position.x].pnumber_or_coins=speaker->number;

    recv(speaker->socket_descriptor, &speaker->pid, sizeof(int), 0);
}
void player_on_disconnect(int player_number){
    struct player_t* speaker=&serv_state.players[player_number];
    if(speaker->socket_descriptor!=-1){
        close(speaker->socket_descriptor);
    }
    serv_state.curr_board->squares[speaker->position.y*BOARD_WIDTH+speaker->position.x].object=AIR;
    serv_state.curr_board->squares[speaker->position.y*BOARD_WIDTH+speaker->position.x].pnumber_or_coins=0;
    bzero(speaker, sizeof(struct player_t));
    speaker->pid=-1;
    speaker->socket_descriptor=-1;
    pthread_cancel(player_thread_pool[player_number]);
}

void* beast_routine(void* args){
    int beast_number=*(int*)args;
    struct beast_t* this_beast=&serv_state.beasts[beast_number];
    this_beast->beast_id=beast_number;

    while(1){
        this_beast->position.x=(rand() % (BOARD_WIDTH-1)) + 1;
        this_beast->position.y=(rand() % (BOARD_HEIGHT-1)) + 1;
        if(serv_state.curr_board->squares[this_beast->position.y*BOARD_WIDTH+this_beast->position.x].object==AIR){
            serv_state.curr_board->squares[this_beast->position.y*BOARD_WIDTH+this_beast->position.x].object=BEAST;
            break;
        }
    }
    while(1){
        int player_x, player_y;
        int has_moved=0;
        int tries[4]={0, 0, 0, 0};
        int is_chasing=player_to_chase(this_beast->position.x, this_beast->position.y, &player_x, &player_y);
        for(int i=0;i<1000;i++){
            this_beast->last_key_pressed=rand() % (5-2+1) + 2;
            if(this_beast->last_key_pressed == right){
                if(serv_state.curr_board->squares[this_beast->position.y*BOARD_WIDTH + this_beast->position.x+1].object!=WALL){
                    if(is_chasing){
                        if(this_beast->position.x>player_x){
                            continue;
                        }
                    }
                    has_moved=1;
                    break;
                }
            }else if(this_beast->last_key_pressed == left){
                if(serv_state.curr_board->squares[this_beast->position.y*BOARD_WIDTH + this_beast->position.x-1].object!=WALL){
                    if(is_chasing){
                        if(this_beast->position.x<player_x){
                            continue;
                        }
                    }
                    has_moved=1;
                    break;
                }
            }else if(this_beast->last_key_pressed == top){
                if(serv_state.curr_board->squares[(this_beast->position.y-1)*BOARD_WIDTH + this_beast->position.x].object!=WALL){
                    if(is_chasing){
                        if(this_beast->position.y<player_y){
                            continue;
                        }
                    }
                    has_moved=1;
                    break;
                }
            }else if(this_beast->last_key_pressed == bot){
                if(serv_state.curr_board->squares[(this_beast->position.y+1)*BOARD_WIDTH + this_beast->position.x].object!=WALL){
                    if(is_chasing){
                        if(this_beast->position.y>player_y){
                            continue;
                        }
                    }
                    has_moved=1;
                    break;
                }
            }
            if(!has_moved){
                tries[this_beast->last_key_pressed-2]=1;
                this_beast->last_key_pressed=0;
            }
            if(tries[0]==1 && tries[1]==1 && tries[2]==1 && tries[3]==1){
                break;
            }
        }
        usleep(250000);
    }
}

int player_to_chase(int beast_x, int beast_y, int* x, int* y){
    for(int i=-2;i<=2;i++){
        int temp_x=beast_x+i;
        if(temp_x>0 && temp_x<BOARD_WIDTH){
            if(serv_state.curr_board->squares[beast_y*BOARD_WIDTH+temp_x].object==PLAYER){
                *x=temp_x;
                *y=beast_y;
                return 1;
            }
        }

        int temp_y=beast_y+i;
        if(temp_y>0 && temp_y<BOARD_HEIGHT){
            if(serv_state.curr_board->squares[temp_y*BOARD_WIDTH+beast_x].object==PLAYER){
                *x=beast_x;
                *y=temp_y;
                return 1;
            }
        }

        temp_x=beast_x+i;
        temp_y=beast_y+i;
        if(temp_x>0 && temp_x<BOARD_WIDTH && temp_y>0 && temp_y<BOARD_HEIGHT){
            if(serv_state.curr_board->squares[temp_y*BOARD_WIDTH+temp_x].object==PLAYER){
                *x=temp_x;
                *y=temp_y;
                return 1;
            }
        }

        temp_x=beast_x-i;
        temp_y=beast_y+i;
        if(temp_x>0 && temp_x<BOARD_WIDTH && temp_y>0 && temp_y<BOARD_HEIGHT){
            if(serv_state.curr_board->squares[temp_y*BOARD_WIDTH+temp_x].object==PLAYER){
                *x=temp_x;
                *y=temp_y;
                return 1;
            }
        }

        temp_x=beast_x+i;
        temp_y=beast_y-i;
        if(temp_x>0 && temp_x<BOARD_WIDTH && temp_y>0 && temp_y<BOARD_HEIGHT){
            if(serv_state.curr_board->squares[temp_y*BOARD_WIDTH+temp_x].object==PLAYER){
                *x=temp_x;
                *y=temp_y;
                return 1;
            }
        }

        temp_x=beast_x-i;
        temp_y=beast_y-i;
        if(temp_x>0 && temp_x<BOARD_WIDTH && temp_y>0 && temp_y<BOARD_HEIGHT){
            if(serv_state.curr_board->squares[temp_y*BOARD_WIDTH+temp_x].object==PLAYER){
                *x=temp_x;
                *y=temp_y;
                return 1;
            }
        }
    }
    return 0;
}
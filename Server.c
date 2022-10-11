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

pthread_t thread_pool[6];
pthread_t state_thread;
char player_a=' ';
struct state curr;


int init_Server(char * ip){
    int endpoint;
    struct sockaddr_in serv_addr;

    if((endpoint = socket(AF_INET, SOCK_STREAM, 0))< 0){
        printf("Failed to create socket");
        return 1;
    }
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(8000);

    if(bind(endpoint, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Failed to bind\n");
        return 1;
    }

    pthread_create(&thread_pool[5], NULL, &handle_connection, &endpoint);
    pthread_detach(thread_pool[5]);

    pthread_create(&state_thread, NULL, &handle_state_update, NULL);
    pthread_detach(state_thread);

    init_state(&curr);
    initscr();

    endwin();
    destroy_state(&curr);

    pthread_create(&thread_pool[4], NULL, &Quit, NULL);
    pthread_join(thread_pool[4], NULL);

    close(endpoint);
    for(int i=0;i<4;i++){
        pthread_join(thread_pool[i], NULL);
    }
    return 0;
}

void* handle_connection(void* args){
    int endpoint=*(int*)args;
    int soc_desc[4];
    int curr_clients=0;


    listen(endpoint, 4);

    while(1){
        if(curr_clients < 4){
            soc_desc[curr_clients] = accept(endpoint, (struct sockaddr*)NULL, NULL);
        }else{
            soc_desc[curr_clients]=-1;
        }
        if(soc_desc[curr_clients] >= 0){
            pthread_create(&thread_pool[curr_clients], NULL, &handle_information_flow, &soc_desc[curr_clients]);
            curr_clients++;
        }
    }
}

void* handle_information_flow(void* args){
    int soc_desc = *(int*)args;
    int client_process_id;

    recv(soc_desc, &client_process_id, sizeof(int), 0);
    printf("Client of ID: %d connected to server\n", client_process_id);
    send(soc_desc, &client_process_id, sizeof(int), 0);


    while(1){
        if(recv(soc_desc, &player_a, 1, 0)==0){
            printf("Client of ID: %d has left\n", client_process_id);
            *(int*)args=-1;
            close(soc_desc);
            break;
        }
        printf("Player has pressed: %c\n", player_a);
    }
    return NULL;
}

void* handle_state_update(void* args){
    while(1){
        printf("Player input: %c", player_a);
        sleep(4);
    }
}

void* Quit(void* args){
    char c;

    while(1){
        scanf("%c", &c);
        if(c == 'Q' || c == 'q'){
            return NULL;
        }
        sleep(1);
    }
}
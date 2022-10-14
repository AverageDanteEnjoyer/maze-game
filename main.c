#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "Server.h"
#include "board.h"
#include "state.h"

int main(){
    int endpoint = 0;
    char ip[]="127.0.0.1";
    struct sockaddr_in serv_addr;


    if((endpoint = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Failed to create socket");
        return 1;
    }
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8000);

    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0){
        printf("%s isn't a valid IP", ip);
        return 2;
    }

    if(connect(endpoint, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        init_Server(ip);
        return 0;
    }

    int buff;
    int pid=getpid();

    send(endpoint, &pid, sizeof(int), 0);
    struct board_t* main=board_create(4, 4);

    initscr();
    keypad(stdscr, TRUE); //P-5, L-4, G-3, D-2;
    char c;
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
    close(endpoint);
    board_destroy(&main);
    endwin();
    return 0;
}
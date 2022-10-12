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

    char opcja=' ';
    while(1){
        printf("Wybierz opcje\n");
        scanf("%c", &opcja);
        while(getchar()!='\n');
        if(opcja == 'Q'){
            close(endpoint);
            break;
        }
        send(endpoint, &opcja, 1, 0);
    }
    close(endpoint);
    board_destroy(&main);
    return 0;
}
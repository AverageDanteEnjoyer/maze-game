#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Server.h"
#include "Client.h"
#include <pthread.h>

int main(){
    pthread_t t1, t2;
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
    int pid=getpid();

    initscr();
    start_color();
    init_colors();

    if(send(endpoint, &pid, sizeof(int), 0) > 0){
        pthread_create(&t1, NULL, &listen_s, &endpoint);
    }
    pthread_create(&t2, NULL, &send_s, &endpoint);

    pthread_join(t2, NULL);
    close(endpoint);
    pthread_join(t1, NULL);

    endwin();
    return 0;
}
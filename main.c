#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Client.h"

int main(){
    int endpoint;
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
        close(endpoint);
        return init_server_process(ip);
    }
    return init_player_client(endpoint);
}
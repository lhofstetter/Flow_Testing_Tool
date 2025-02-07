#include "../include/FlowController.h"

void * connection(void * args) {
    
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("No config file found. Assuming default file name (config.txt).\n");
    }

    struct sockaddr_in address;

    int new_socket_fd[AMOUNT_OF_REMOTE_CONNECTIONS] = { 0 };

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    int opt = 1;

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR | SO_KEEPALIVE, &opt, sizeof(opt))) {
	    perror("Failure to set socket-level options. Exiting.\n");
	    return 1;
    }

    address.sin_addr.s_addr = inet_addr("10.15.1.2");
    address.sin_family = AF_INET;
    address.sin_port = htons(7000);

    if (bind(s, &address, sizeof(address))) {
        perror("Failure to bind address to socket. Exiting.\n");
        return 1;
    }





    

    return 0;
}

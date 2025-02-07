#include "../include/FlowController.h"

volatile sig_atomic_t interrupt = 1;

void handle_sigint(int sig) {
    interrupt = 0; // Indicate that the program should stop
}

void * connection(void * args) {
    client * info = (client *)args;

    
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("No config file found. Assuming default file name (config.txt).\n");
    }

    struct sockaddr_in address;
    struct epoll_event connection_notifier, socket_close_notifier[AMOUNT_OF_REMOTE_CONNECTIONS], events[2];
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    int client_count = -1;
    client client_list[AMOUNT_OF_REMOTE_CONNECTIONS] = { 0 };

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    int opt = 1;

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR | SO_KEEPALIVE, &opt, sizeof(opt))) {
	    perror("Failure to set socket-level options. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    address.sin_addr.s_addr = inet_addr(HOST_IP_ADDRESS);
    address.sin_family = AF_INET;
    address.sin_port = htons(COMMUNICATION_PORT);

    if (bind(s, &address, sizeof(address))) {
        perror("Failure to bind address to socket. Exiting.\n");
        close(s);
        exit(EXIT_FAILURE);
    }

    fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK); // sets socket to nonblocking so accept doesn't block and mess with epoll_wait

    listen(s, AMOUNT_OF_REMOTE_CONNECTIONS); // sets up listening on socket - does not block, since it merely tells socket to listen for connections and place them in queue as needed

    int epoll_instance = epoll_create1(0);
    
    connection_notifier.events = EPOLLIN;
    connection_notifier.data.fd = s;

    if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, s, &connection_notifier)) {
        perror("Failed to add socket fd to epoll. Exiting.\n");
        close(s);
        close(epoll_instance);
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in client = { 0 };

    while (interrupt) {
        epoll_wait(epoll_instance, &events, 1, -1);
        
        if (events[0].data.fd == s) {
            client_count++;
            client_list[client_count].socket = accept(s, &client_list[client_count].client_info, sizeof(client_list[client_count].client_info));
            if (client_list[client_count].socket != -1) {
                client_list[client_count].notification_fd = eventfd(0, EFD_SEMAPHORE | EFD_NONBLOCK);

                if (client_list[client_count].notification_fd == -1) {
                    perror("Failed to create event file descriptor for new connection. Exiting.\n");
                    close(s);
                    close(epoll_instance);
                    close(client_list[client_count].socket);
                    exit(EXIT_FAILURE);
                }

                socket_close_notifier[client_count].data.fd = client_list[client_count].notification_fd;
                socket_close_notifier[client_count].events = EPOLLIN;

                if (epoll_ctl(epoll_instance, EPOLL_CTL_ADD, client_list[client_count].notification_fd, &socket_close_notifier[client_count])) {
                    perror("Failed to create event file descriptor for new connection. Exiting.\n");
                    close(s);
                    close(epoll_instance);
                    close(client_list[client_count].socket);
                    exit(EXIT_FAILURE);
                }
                pthread_create(&client_list[client_count].tid, NULL, connection, &client_list[client_count]);
            }
        } else {
            printf("One of the remote connections closed!\n");
        }
    }

    return 0;
}

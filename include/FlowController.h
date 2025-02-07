#define _GNU_SOURCE

// General C Headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Network Headers
#include <sys/socket.h>
#include <arpa/inet.h>

// Multi-Threading Headers
#include <pthread.h>

// Event Notification/Signal Headers
#include <sys/epoll.h>
#include <signal.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/eventfd.h>

#define AMOUNT_OF_REMOTE_CONNECTIONS 10

#define HOST_IP_ADDRESS "10.15.1.2"
#define COMMUNICATION_PORT 7000

typedef struct {
    int socket;
    struct sockaddr_in client_info;
    int notification_fd;
    pthread_t tid;
} client;
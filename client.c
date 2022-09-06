#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define B_SIZE 1024

void chat(int sockfd, struct sockaddr_in *peer, char *argv[]) {
    int ret;
    ssize_t bytes;
    char in_buffer[B_SIZE];
    char out_buffer[B_SIZE];
    
    size_t alias_len = strlen(argv[4]);
    char *c = ": ";
    char prepend[alias_len + strlen(c)];
    strcpy(prepend, argv[4]);
    strcat(prepend, c);
    
    struct pollfd fds[2];
    fds[0].fd = 0;
    fds[1].fd = sockfd;
    fds[0].events = POLLIN | POLLPRI;
    fds[1].events = POLLIN | POLLPRI;

    while (1) {
        ret = poll(fds, 2, -1);
        if (ret < 0) { printf("error poll return\n"); break; }
        if (ret > 0) {
            if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) { printf("error poll stdin"); break;}
            if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) { printf("error poll socket"); break;}

            if (fds[0].revents & (POLLIN | POLLPRI)) {
                bytes = read(0, out_buffer, sizeof(out_buffer));
                if (bytes < 0) {printf("error stdin\n"); break; }
                memmove(out_buffer + strlen(prepend), out_buffer, strlen(out_buffer) + 1);
                memcpy(out_buffer, prepend, strlen(prepend));

                bytes = sendto(sockfd, out_buffer, sizeof(out_buffer), 0, (struct sockaddr *)peer, sizeof(struct sockaddr_in));
                if (bytes < 0) { printf("error sendto\n"); break; }
                memset(out_buffer, '\0', sizeof(out_buffer));
            }
            if (fds[1].revents & (POLLIN | POLLPRI)) {
                bytes = recvfrom(sockfd, in_buffer, sizeof(in_buffer), 0, NULL, NULL);
                if (bytes < 0) { printf("error recvfrom\n"); break; }
                if (bytes > 0) {
                    printf("%.*s\n", (int)bytes, in_buffer);
                }
                memset(in_buffer, '\0', sizeof(in_buffer));
            }
        }
    }
}

int main(int argc, char *argv[]) {
    unsigned long local_port, remote_port;
    int sockfd;
    struct sockaddr_in s_addr, peer_addr;

    memset(&s_addr, 0, sizeof(s_addr));
    memset(&peer_addr, 0, sizeof(peer_addr));

    if (argc < 5) { printf("args: local_port remote_ip remote_port alias\n"); return 0; }

    local_port = strtoul(argv[1], NULL, 0);
    if (local_port < 1024 || local_port >65535) { printf("Valid ports: 1025-65535\n"); return 0;}
    remote_port = strtoul(argv[3], NULL, 0);
    if (remote_port < 1024 || remote_port >65535) { printf("Valid ports: 1025-65535\n"); return 0;}
    
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(remote_port);
    if (inet_pton(AF_INET, argv[2], &peer_addr.sin_addr) == 0) {printf("error invalid ip\n"); return 0; }
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("sockfd creation failed\n");
        exit(EXIT_FAILURE);
    }

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(local_port);

    if (bind(sockfd, (const struct sockaddr *)&s_addr, sizeof(s_addr)) < 0) {
        perror("sockfd bind failed\n");
        exit(EXIT_FAILURE);
    }

    chat(sockfd, &peer_addr, argv);

    close(sockfd);
    return 0;
}
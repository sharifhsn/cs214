#include "csapp.h"
// int Open_listenfd(char *port) {
//     struct addrinfo hints; // options for getaddrinfo
//     struct addrinfo *listp; // linked list to store addresses
//     struct addrinfo *p; // pointer for traversing listp
//     int listenfd; // stores socket file descriptor
//     int optval = 1; // level of option on socket to reuse addresses

//     // get list of potential server addresses
//     memset(&hints, 0, sizeof(struct addrinfo));
//     hints.ai_socktype = SOCK_STREAM; // accepts connections
//     hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // any port address
//     hints.ai_flags |= AI_NUMERICSERV; // any port number
//     getaddrinfo(NULL, port, &hints, &listp); // put all server address on port in listp

//     // walk the list
//     for (p = listp; p != NULL; p = p->ai_next) {
//         // create a socket descripter inside p
//         if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
//             continue; // socket has error
//         }

//         // let us reuse address so bind doesn't give error
//         setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

//         // bind descriptor to address
//         if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
//             break; // bind worked! great success
//         }
//         close(listenfd); // failure, close this socket, move to next
//     }

//     // cleanup
//     freeaddrinfo(listp);
//     // no addresses worked
//     if (p == NULL) {
//         return -1;
//     }

//     // else, let the bound socket listen
//     if (listen(listenfd, LISTENQ) < 0) {
//         // error when listening
//         close(listenfd);
//         return -1;
//     }
//     return listenfd;
// }

int main(int argc, char *argv[]) {
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];
    
    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (fgets(buf, MAXLINE, stdin) != NULL) {
        Rio_writen(clientfd, buf, strlen(buf));
        Rio_readlineb(&rio, buf, MAXLINE);
        fputs(buf, stdout);
    }
    close(clientfd);
    return 0;
}
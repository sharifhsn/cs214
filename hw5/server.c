#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>

// Dimensions for the drawn grid (should be GRIDSIZE * texture dimensions)
#define GRID_DRAW_WIDTH 640
#define GRID_DRAW_HEIGHT 640

#define WINDOW_WIDTH GRID_DRAW_WIDTH
#define WINDOW_HEIGHT (HEADER_HEIGHT + GRID_DRAW_HEIGHT)

// Header displays current score
#define HEADER_HEIGHT 50

// Number of cells vertically/horizontally in the grid
#define GRIDSIZE 10

#define MAXLINE 8192

typedef struct Position
{
    int x;
    int y;
    struct Position *next;
} Position;

typedef enum
{
    TILE_GRASS,
    TILE_TOMATO
} TILETYPE;

TILETYPE grid[GRIDSIZE][GRIDSIZE];

Position playerPositions[4];
int score;
int level;
int numTomatoes;
int curr;

bool shouldExit = false;

int *seedp;

// open listening port
int open_listenfd(char *port) 
{
    struct addrinfo hints, *listp, *p;
    int listenfd, rc, optval=1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                   (const void *)&optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        if (close(listenfd) < 0) { /* Bind failed, try the next */
            fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
            return -1;
        }
    }


    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, 8) < 0) {
        close(listenfd);
	return -1;
    }
    return listenfd;
}

// get a random value in the range [0, 1]
double rand01()
{
    return (double) rand_r(seedp) / (double) RAND_MAX;
}

void initGrid()
{
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            double r = rand01();
            if (r < 0.1) {
                grid[i][j] = TILE_TOMATO;
                numTomatoes++;
            }
            else
                grid[i][j] = TILE_GRASS;
        }
    }

    // force player's position to be grass
    for (int i = 0; i < 4; i++) {
        if (grid[playerPositions[i].x][playerPositions[i].y] == TILE_TOMATO) {
            grid[playerPositions[i].x][playerPositions[i].y] = TILE_GRASS;
            numTomatoes--;
        }
    }


    // ensure grid isn't empty
    while (numTomatoes == 0)
        initGrid();
}

void moveTo(int x, int y)
{
    // Prevent falling off the grid
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
        return;

    // Sanity check: player can only move to 4 adjacent squares
    if (!(abs(playerPositions[curr].x - x) == 1 && abs(playerPositions[curr].y - y) == 0) &&
        !(abs(playerPositions[curr].x - x) == 0 && abs(playerPositions[curr].y - y) == 1)) {
        fprintf(stderr, "Invalid move attempted from (%d, %d) to (%d, %d)\n", playerPositions[curr].x, playerPositions[curr].y, x, y);
        return;
    }

    playerPositions[curr].x = x;
    playerPositions[curr].y = y;

    if (grid[x][y] == TILE_TOMATO) {
        grid[x][y] = TILE_GRASS;
        score++;
        numTomatoes--;
        if (numTomatoes == 0) {
            level++;
            initGrid();
        }
    }
}


int main(int argc, char* argv[])
{
    srand(*seedp = time(NULL));

    level = 1;

    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    initGrid();

    listenfd = open_listenfd(argv[1]);
    Position tempos;
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
        write(connfd, 0, sizeof(int));
        while (1) {
            read(connfd, &curr, sizeof(curr));
            read(connfd, &tempos, tempos);
            moveTo(tempos.x, tempos.y);

            write(connfd, grid, sizeof(grid));
            write(connfd, score, sizeof(score));
            write(connfd, level, sizeof(level));
            write(connfd, numTomatoes, sizeof(numTomatoes));
        }
        close(connfd);
    }
}

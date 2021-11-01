#include <stdio.h>
#include <stdlib.h>

void print_board(int **board, int bx, int by) {
    for (int i = 0; i < by; i++) {
        for (int j= 0; j < bx; j++) {
            switch (board[i][j]) {
                case 0:
                    printf(". ");
                    break;
                case 1:
                    printf("P ");
                    break;
                case 2:
                    printf("G ");
                    break;
                case 3:
                    printf("M ");
                    break;
            }
        }
        printf("\n");
    }
}

void free_board(int **board, int by) {
    for (int i = 0; i < by; i++) {
        free(board[i]);
    }
    free(board);
}

int **board_init(int bx, int by, int px, int py, int gx, int gy, int mx, int my) {
    // calloc board
    // 0 is empty, 1 is player, 2 is goal, and 3 is monster
    int **board = calloc(by, sizeof(int *));
    for (int i = 0; i < by; i++) {
        board[i] = calloc(bx, sizeof(int));
    }
    board[bx - py - 1][px] = 1;
    board[bx - gy - 1][gx] = 2;
    board[bx - my - 1][mx] = 3;
    print_board(board, bx, by);
    return board;
}

void update(int **board, int bx, int by, int px, int py, int gx, int gy, int mx, int my) {
    // clear board first
    for (int i = 0; i < by; i++) {
        for (int j = 0; j < bx; j++) {
            if (board[i][j] == 1 || board[i][j] == 3) {
                board[i][j] = 0;
            }
        }
    }
    // redraw player and monster
    board[bx - py - 1][px] = 1;
    board[bx - gy - 1][gx] = 2;
    board[bx - my - 1][mx] = 3;
}

int main(int argc, char *argv[]) {
    if (argc < 9) {
        printf("insufficient arguments!\n");
        return 0;
    }
    char *ptr = 0;
    int bx = strtol(argv[1], &ptr, 10);
    int by = strtol(argv[2], &ptr, 10);
    int px = strtol(argv[3], &ptr, 10);
    int py = strtol(argv[4], &ptr, 10);
    int gx = strtol(argv[5], &ptr, 10);
    int gy = strtol(argv[6], &ptr, 10);
    int mx = strtol(argv[7], &ptr, 10);
    int my = strtol(argv[8], &ptr, 10);
    int **board = board_init(bx, by, px, py, gx, gy, mx, my);

    char move = 0;
    while (scanf(" %c", &move) != '\n') {
        // move the players
        switch (move) {
            case 'E':
                if (px < bx - 1) {
                    px += 1;
                }
                break;
            case 'W':
                if (px > 0) {
                    px -= 1;
                }
                break;
            case 'N':
                if (py < by - 1) {
                    py += 1;
                }
                break;
            case 'S':
                if (py > 0) {
                    py -= 1;
                }
                break;
            default:
                break;
        }
        // check if player wins
        if (px == gx && py == gy) {
            printf("player wins!\n");
            break;
        }
        // move the monster
        if (abs(mx - px) > abs(my - py)) {
            if (mx > px && mx > 0) {
                mx -= 1;
                printf("monster moves W\n");
            } else if (mx < px && mx < bx - 1) {
                mx += 1;
                printf("monster moved E\n");
            }
        } else {
            if (my > py && my > 0) {
                my -= 1;
                printf("monster moved S\n");
            } else if (my < py && my < by - 1) {
                my += 1;
                printf("monster moved N\n");
            }
        }
        // check if monster wins
        if (px == mx && py == my) {
            printf("monster wins!\n");
            break;
        }
        // update the board
        update(board, bx, by, px, py, gx, gy, mx, my);
        print_board(board, bx, by);
    }

    free_board(board, by);
    return 0;
}
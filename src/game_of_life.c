#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void start_game();
void init_window();
void input(int **matrix, int width, int height);
int speed(char button, int *end_game, int game_speed);
void draw(int **matrix1, int **matrix2, int width, int height);
int empty_field(int **matrix, int width, int height);
int alive_counter(int **matrix1, int width, int height, int a, int b);
int change_status(int neighbors, int current);
void update_matrix(int **matrix1, int **matrix2, int width, int height);

int main() {
    start_game();
    return 0;
}

void start_game() {
    int width = 25;
    int height = 80;
    int **matrix1 = NULL;
    int **matrix2 = NULL;
    int game_speed = 400;
    int end_game = 0;

    matrix1 = (int **)calloc(width, sizeof(int *));
    for (int i = 0; i < width; i++) {
        matrix1[i] = (int *)calloc(height, sizeof(int));
    }

    matrix2 = (int **)calloc(width, sizeof(int *));
    for (int i = 0; i < width; i++) {
        matrix2[i] = (int *)calloc(height, sizeof(int));
    }

    input(matrix1, width, height);
    if (freopen("/dev/tty", "r", stdin)) init_window();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(2, COLOR_GREEN, COLOR_GREEN);
    while (end_game != 1) {
        if (empty_field(matrix1, width, height) == 0) {
            clear();
            printw("Field is empty");
            refresh();
            sleep(8);
            end_game = 1;
        }

        char button = getch();

        game_speed = speed(button, &end_game, game_speed);
        usleep(game_speed * 1000);

        clear();
        draw(matrix1, matrix2, width, height);
        update_matrix(matrix1, matrix2, width, height);
    }
    for (int i = 0; i < width; i++) {
        free(matrix1[i]);
        free(matrix2[i]);
    }
    free(matrix1);
    free(matrix2);
    endwin();
}

void init_window() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE);
}

void input(int **matrix, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            scanf("%d", &matrix[i][j]);
        }
    }
}

int empty_field(int **matrix, int width, int height) {
    int count = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            count += matrix[i][j];
        }
    }
    return count;
}

void draw(int **matrix1, int **matrix2, int width, int height) {
    int alive = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            matrix2[i][j] = change_status(alive_counter(matrix1, width, height, i, j), matrix1[i][j]);
            if (matrix2[i][j] == 1) {
                alive++;
                attron(COLOR_PAIR(1));
                mvaddch(i, j, '#');
                attroff(COLOR_PAIR(1));
            } else
                printw(".");
        }
        printw("\n");
    }
    mvprintw(27, 0, "CITIZENS AMOUNT: %d", alive);
}

int speed(char button, int *end_game, int game_speed) {
    if (button == '+')
        game_speed -= (game_speed < 200) ? 0 : 100;
    else if (button == '-')
        game_speed += (game_speed < 1000) ? 100 : 0;
    else if (button == 'q')
        *end_game = 1;

    return game_speed;
}

void update_matrix(int **matrix1, int **matrix2, int width, int height) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            matrix1[i][j] = matrix2[i][j];
        }
    }
}

int change_status(int neighbors, int current) {
    int new_status = -1;
    if ((neighbors == 2 || neighbors == 3) && current == 1) {
        new_status = 1;
    } else if (neighbors == 3 && current == 0) {
        new_status = 1;
    } else {
        new_status = 0;
    }
    return new_status;
}

int alive_counter(int **matrix1, int width, int height, int a, int b) {
    int neighbours = 0;

    const int offsets[3] = {-1, 0, 1};

    for (int di = 0; di < 3; di++) {
        for (int dj = 0; dj < 3; dj++) {
            if (di == 1 && dj == 1) continue;

            int new_i = (a + offsets[di] + width) % width;
            int new_j = (b + offsets[dj] + height) % height;

            neighbours += matrix1[new_i][new_j];
        }
    }
    return neighbours;
}

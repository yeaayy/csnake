
#include <assert.h>

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <ncurses.h>

#include "snake.h"

void m_on_exit(int sig) {
    printf("\e[?25h");
    printf("\e[?1049l");
    exit(0);
}

#define COLOR_SNAKE 1
#define COLOR_FOOD 2

typedef enum {
    vNull = 0,
    vHead,
    vHeadL,
    vHeadT,
    vHeadR,
    vHeadB,
    vFood
} vBoard;

vBoard __get_vBoard(int value) {
    switch(value) {
        case -1: return vHeadL;
        case 1: return vHeadR;
        case -BOARD_WIDTH: return vHeadT;
        case BOARD_WIDTH: return vHeadB;
    }
    return vNull;
}

void Snake_print(Snake * dst){
	vBoard cboard[BOARD_AREA];

	memset(cboard, 0, sizeof(cboard));
	{
		SnakeBody *next, *curr = dst->head;
		cboard[curr->index] = __get_vBoard(Direction[dst->direction]);
		cboard[dst->FoodIndex] = vFood;
		while((next = SnakeBody_nextToTail(curr)) != NULL) {
            cboard[next->index] = __get_vBoard(curr->index - next->index);
			curr = next;
		}
	}

	int i = 0;
	for (int y = 0; y < BOARD_HEIGHT; y++){
		for (int x = 0; x < BOARD_WIDTH; x++, i++){
            move(y, x * 2);
			switch (cboard[i]){
                case vHeadL:
                    attron(COLOR_PAIR(COLOR_SNAKE));
                    printw("<<");
                    attroff(COLOR_PAIR(COLOR_SNAKE));
                    break;
                case vHeadT:
                    attron(COLOR_PAIR(COLOR_SNAKE));
                    printw("^^");
                    attroff(COLOR_PAIR(COLOR_SNAKE));
                    break;
                case vHeadR:
                    attron(COLOR_PAIR(COLOR_SNAKE));
                    printw(">>");
                    attroff(COLOR_PAIR(COLOR_SNAKE));
                    break;
                case vHeadB:
                    attron(COLOR_PAIR(COLOR_SNAKE));
                    printw("vv");
                    attroff(COLOR_PAIR(COLOR_SNAKE));
                    break;
                case vNull : printw(". "); break;
                case vFood :
                    attron(COLOR_PAIR(COLOR_FOOD));
                    printw("$$");
                    attroff(COLOR_PAIR(COLOR_FOOD));
                    break;
                default    : printw("[]"); break;
			}
		}
	}
}

int main(int argc, char** argv){
	struct timespec __tp;
	clock_gettime(0, &__tp);
	srand(__tp.tv_nsec);

    Snake *snake = Snake_create(NULL);
    WINDOW *main = initscr();
    curs_set(0);
    cbreak();
    noecho();
    nodelay(main, true);
    keypad(main, true);

    start_color();
    init_pair(COLOR_SNAKE, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(COLOR_FOOD, COLOR_WHITE, COLOR_GREEN);

    while (snake->alive)
    {
        int oldDir = snake->direction;
        int newDir = snake->direction;
        switch(getch()) {
            case -1: break;
            case KEY_LEFT : newDir = 0; break;
            case KEY_UP   : newDir = 1; break;
            case KEY_RIGHT: newDir = 2; break;
            case KEY_DOWN : newDir = 3; break;
        }
        // Prevent it to run backward
        if((newDir + 2) % 4 != oldDir) {
            snake->direction = newDir;
        }
        Snake_move(snake);
        Snake_print(snake);
        mvprintw(BOARD_HEIGHT, 0, "Score: %d", snake->eaten);
        refresh();
        usleep(200000);
    }
	endwin();
    printf("Game over your score is: %d\n", snake->eaten);
	return 0;
}

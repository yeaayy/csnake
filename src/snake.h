#ifndef SNAKE_H
#define SNAKE_H

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 15
#define BOARD_AREA (BOARD_WIDTH * BOARD_HEIGHT)

#define getIndex(x, y) ((y) * BOARD_WIDTH + x)

typedef char *Board;

typedef enum {
	EMPTY = 0,
	BODY = 1,
	FOOD = 2
} BoardContent;

typedef struct SnakeBody SnakeBody;
struct SnakeBody {
	SnakeBody *T;			// toward tail
	SnakeBody *H;			// toward head
	int index;
};

typedef struct Snake {
	int direction;
	SnakeBody *head;
	SnakeBody *tail;
	Board board;
	int FoodIndex;
	int alive;
	int eaten;
	int grow;
	int run;
} Snake;

#define SNAKE_INITIAL_LENGTH 4

extern int Direction[4];

#define SnakeBody_nextToTail(x) (x)->T
#define SnakeBody_nextToHead(x) (x)->H

void SnakeBody_attach(SnakeBody *front, SnakeBody *back);
void SnakeBody_dettach(SnakeBody *front, SnakeBody *back);
void SnakeBody_dettach2(SnakeBody *body);
SnakeBody *SnakeBody_create(SnakeBody *front, SnakeBody *back, int index);
void SnakeBody_destroy(SnakeBody *body);

void Snake_putNewFood(Snake *dst);
void Snake_reset(Snake *dst);
Snake* Snake_create(Snake *src);
void Snake_destroy(Snake *dst);
void Snake_move(Snake *dst);
// void Snake_print(Snake *dst);
void Snake_dump(Snake *dst);

#endif /* SNAKE_H */

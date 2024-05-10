#include "snake.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int Direction[4] = { -1, -BOARD_WIDTH, 1, BOARD_WIDTH };

int isOutside(int index, int step){
	int v = index / BOARD_WIDTH + step / BOARD_WIDTH;
	if( v<0 || v>=BOARD_HEIGHT ) return 1;

	v = index % BOARD_WIDTH + step % BOARD_WIDTH;
	if( v < 0 || v >= BOARD_WIDTH ) return 1;

	return 0;
}

#define SnakeBody_nextToTail(x) (x)->T
#define SnakeBody_nextToHead(x) (x)->H

void SnakeBody_attach(SnakeBody *front, SnakeBody *back){
	SnakeBody_nextToTail(front) = back;
	SnakeBody_nextToHead(back) = front;
}

void SnakeBody_dettach(SnakeBody *front, SnakeBody *back){
	assert(SnakeBody_nextToTail(front) == back);
	assert(SnakeBody_nextToHead(back) == front);
	SnakeBody_nextToTail(front) = NULL;
	SnakeBody_nextToHead(back) = NULL;
}

void SnakeBody_dettach2(SnakeBody *body){
	if(SnakeBody_nextToTail(body) != NULL) {
		SnakeBody_dettach(body, SnakeBody_nextToTail(body));
	}
	if(SnakeBody_nextToHead(body) != NULL) {
		SnakeBody_dettach(SnakeBody_nextToHead(body), body);
	}
}

SnakeBody *SnakeBody_create(SnakeBody *front, SnakeBody *back, int index){
	SnakeBody *result = (SnakeBody*) malloc(sizeof(SnakeBody));
	result->index = index;

	if(front != NULL){
		SnakeBody_attach(front, result);
	}else{
		SnakeBody_nextToHead(result) = NULL;
	}

	if(back != NULL){
		SnakeBody_attach(result, back);
	}else{
		SnakeBody_nextToTail(result) = NULL;
	}
	return result;
}

void SnakeBody_destroy(SnakeBody *body){
	assert(SnakeBody_nextToTail(body) == NULL);
	assert(SnakeBody_nextToHead(body) == NULL);
	free(body);
}

void Snake_putNewFood(Snake *dst){
	Board board = dst->board;
	if(!dst->alive){
		board[dst->FoodIndex] = EMPTY;
	}
	int index;
	do {
		index = rand() % BOARD_AREA;
	} while (board[index] != EMPTY);
	board[index] = FOOD;
	dst->FoodIndex = index;
}

void Snake_reset(Snake * dst){
	dst->direction = 3;
	memset(dst->board, 0, sizeof(char) * BOARD_AREA);
	int index = ( SNAKE_INITIAL_LENGTH - 1) * BOARD_HEIGHT + BOARD_WIDTH / 2;
	SnakeBody *last = dst->head;
	for(int i=0; i<SNAKE_INITIAL_LENGTH; i++){
		dst->board[index] = BODY;
		last->index = index;
		dst->tail = last;
		last = SnakeBody_nextToTail(last);
		index -= BOARD_WIDTH;
	}
	while(last != NULL) {
		SnakeBody *next = SnakeBody_nextToTail(last);
		SnakeBody_dettach2(last);
		SnakeBody_destroy(last);
		last = next;
	}
	dst->alive = true;
	dst->eaten = 0;
	dst->grow = 0;
	Snake_putNewFood(dst);
}

Snake* Snake_create(Snake *src){
	Snake *dst = (Snake*) malloc(sizeof(Snake));

	dst->board = (Board) malloc(sizeof(char) * BOARD_AREA);
	SnakeBody *head = SnakeBody_create(NULL, NULL, 0);
	dst->tail = head;
	for (int i = 1; i < SNAKE_INITIAL_LENGTH; i++){
		head = SnakeBody_create(NULL, head, 0);
	}
	dst->head = head;
	Snake_reset(dst);
	dst->run = 1;
	return dst;
}

void Snake_destroy(Snake * dst){
	SnakeBody *next = NULL, *curr = dst->head;
	do{
		next = SnakeBody_nextToTail(curr);
		if(next != NULL) {
			SnakeBody_dettach(curr, next);
		}
		SnakeBody_destroy(curr);
	}while ((curr = next) != NULL);
	free(dst->board);
}

void Snake_move(Snake * dst){
	Board board = dst->board;
	SnakeBody *head = dst->head;
	SnakeBody *tail = dst->tail;

	int newHeadIndex = Direction[dst->direction];
	if(isOutside(head->index, newHeadIndex)){
		dst->alive = 0;
		return;
	}
	newHeadIndex += head->index;
	if(board[newHeadIndex] == FOOD) {
		dst->grow++;
		dst->eaten++;
		Snake_putNewFood(dst);
		goto grow;
	}

	if(board[newHeadIndex] == EMPTY || newHeadIndex == tail->index) {
		if(dst->grow){
        grow:
			dst->grow--;
			if(newHeadIndex == tail->index) {
				goto death;
			}
			board[newHeadIndex] = BODY;
			dst->head = SnakeBody_create(NULL, head, newHeadIndex);
		}else{
			SnakeBody *newHead = tail;
			SnakeBody *newTail = SnakeBody_nextToHead(tail);
			SnakeBody_dettach(SnakeBody_nextToHead(tail), tail);
			SnakeBody_attach(newHead, head);
			board[tail->index] = EMPTY;
			board[newHeadIndex] = BODY;
			newHead->index = newHeadIndex;
			dst->head = newHead;
			dst->tail = newTail;
		}
	}else{
    death:
		dst->alive = 0;
	}
	assert(SnakeBody_nextToTail(dst->tail) == NULL);
	assert(board[dst->FoodIndex] == FOOD);
}

// typedef enum {
//     vNull = 0,
//     vHead,
//     vHeadL,
//     vHeadT,
//     vHeadR,
//     vHeadB,
//     vFood
// } vBoard;

// vBoard __get_vBoard(int value) {
//     switch(value) {
//         case -1: return vHeadL;
//         case 1: return vHeadR;
//         case -BOARD_WIDTH: return vHeadT;
//         case BOARD_WIDTH: return vHeadB;
//     }
//     return vNull;
// }

// void Snake_print(Snake * dst){
// 	vBoard cboard[BOARD_AREA];

// 	memset(cboard, 0, sizeof(cboard));
// 	{
// 		SnakeBody *next, *curr = dst->head;
// 		cboard[curr->index] = __get_vBoard(Direction[dst->direction]);
// 		cboard[dst->FoodIndex] = vFood;
// 		while((next = SnakeBody_nextToTail(curr)) != NULL) {
//             cboard[next->index] = __get_vBoard(curr->index - next->index);
// 			curr = next;
// 		}
// 	}

// 	int i = 0;
// 	for (int y = 0; y < BOARD_HEIGHT; y++){
// 		for (int x = 0; x < BOARD_WIDTH; x++, i++){
// 			switch (cboard[i]){
//                 case vHead:printf("\e[45m@@\e[m"); break;
//                 case vHeadL: printf("\e[45m<<\e[m"); break;
//                 case vHeadT: printf("\e[45m^^\e[m"); break;
//                 case vHeadR: printf("\e[45m>>\e[m"); break;
//                 case vHeadB: printf("\e[45mvv\e[m"); break;
//                 case vNull: printf(". "); break;
//                 case vFood: printf("\e[92m$$\e[m"); break;
//                 default:
//                     printf("\e[102m[]\e[m");
// 			}
// 		}
// 		printf("\n");
// 	}
// }

void Snake_dump(Snake * dst){
	printf("[snake] direction = %d\n", dst->direction);
	printf("[snake] food @ %d\n", dst->FoodIndex);
	printf("[snake] eaten = %d\n", dst->eaten);
	SnakeBody *head = dst->head;
	int i = 0;
	do{
		printf("[snake] @%d = %d\n", i++, head->index);
		head = SnakeBody_nextToTail(head);
	}while (head);
	// Snake_print(dst);
}

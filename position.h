#ifndef SNAKE_POSITION_H
#define SNAKE_POSITION_H


#include <stdlib.h>


typedef struct Position {
	int x;
	int y;
} Position;


Position* positionConstruct(int, int);

void positionDeconstruct(Position*);

void positionIncrementX(Position*);

void positionIncrementY(Position*);


#endif

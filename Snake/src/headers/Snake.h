#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <Texture.h>
#include <DList.h>

// A Snake node should have the following data members
// 1. position 2. dimensions 3. collider 4. orientation

typedef enum Orientation{ // defines the certain movements of the snake
	SNAKE_UP, 
	SNAKE_DOWN, 
	SNAKE_LEFT, 
	SNAKE_RIGHT, 
	SNAKE_NONE
}Orientation;

typedef struct SnakeNode{
	SDL_Rect body; // This will contain the position and dimensions of the snake body
	SDL_Rect collider; // helps in checking if a collision has taken place
	Orientation orientation; // define where the snake body is moving
	SDL_Point velocity; // contains both the x and y velocity
	
}SnakeNode;

// Node function
SnakeNode* SnakeNode_create(int x, int y, Orientation o); // create a snake node with a given position and orientation

// Snake variables
extern DList* Snake; 
extern const int SNAKE_WIDTH;
extern const int SNAKE_HEIGHT; 
extern const int SNAKE_VEL;
extern bool SNAKE_COLLIDED;

// Snake Functions
bool Snake_create(int x, int y, Orientation o); // create and initialize the list with a single node
void Snake_handleEvents(SDL_Event* e); // handles the keyboard input
void Snake_grow(); // growing the snake 
void Snake_render(SDL_Renderer* renderer); // displays the snake on the screen

// function to check for collision
bool Snake_checkCollision(SDL_Rect* a, SDL_Rect* b);
void Snake_move(); // moves the snake as per the input and also check for input collision
void Snake_collision(int PLAY_WIDTH, int PLAY_HEIGHT); // This will set the global collision variable to be true
// create a function to destroy the snake
void Snake_destroy();

#endif
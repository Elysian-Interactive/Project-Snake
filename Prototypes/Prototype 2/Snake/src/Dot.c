#include <Dot.h>

// Global variables
static const int DOT_WIDTH = 20;
static const int DOT_HEIGHT = 20;
static const int DOT_VEL = 10; // moves 10 pixels/frame

// Function Defintions
Dot* Dot_create()
{
	Dot* new_dot = (Dot*)malloc(sizeof(Dot));
	check(new_dot != NULL, "Failed to create the dot!");
	
	new_dot->position.x = 0;
	new_dot->position.y = 0;
	new_dot->x_velocity = 0;
	new_dot->y_velocity = 0;
	
	// Initializing the collider's dimensions
	new_dot->collider.w = DOT_WIDTH;
	new_dot->collider.h = DOT_HEIGHT;
	
	return new_dot;
error:
	return NULL;
}

void Dot_handleEvents(Dot* dot, SDL_Event* e)
{
	check(dot != NULL, "Invalid Dot!");
	
	// if a key was pressed
	if(e->type == SDL_KEYDOWN && e->key.repeat == 0){
		// adjust the velocity, here we keep on adding to the velocity
		switch(e->key.keysym.sym){
			case SDLK_UP: dot->y_velocity -= DOT_VEL; break; 
			case SDLK_DOWN: dot->y_velocity += DOT_VEL; break; 
			case SDLK_LEFT: dot->x_velocity -= DOT_VEL; break; 
			case SDLK_RIGHT: dot->x_velocity += DOT_VEL; break; 
		}
	}
	
	// if a key was released
	if(e->type == SDL_KEYUP && e->key.repeat == 0){
		// adjust the velocity, here redo the changes and put the velocity back to zero
		switch(e->key.keysym.sym){
			case SDLK_UP: dot->y_velocity += DOT_VEL; break; 
			case SDLK_DOWN: dot->y_velocity -= DOT_VEL; break; 
			case SDLK_LEFT: dot->x_velocity += DOT_VEL; break; 
			case SDLK_RIGHT: dot->x_velocity -= DOT_VEL; break; 
		}
	}
	
error: // fallthrough
	return;
}

void Dot_move(Dot* dot, SDL_Rect* object)
{
	check(dot != NULL, "Inalid Dot!");
	
	// move the dot to the left or right
	dot->position.x += dot->x_velocity;
	dot->collider.x = dot->position.x; // update the collider's position
	
	if( (dot->position.x < 0) || (checkCollision(&dot->collider, object)) ){
		dot->position.x -= dot->x_velocity;
		dot->collider.x = dot->position.x; // update the collider's position
	}
	
	// move the do to the up or down
	dot->position.y += dot->y_velocity;	
	dot->collider.y = dot->position.y;
	
	if( (dot->position.y < 0) || (checkCollision(&dot->collider, object)) ){
		dot->position.y -= dot->y_velocity;
		dot->collider.y = dot->position.y;
	}
	
error:  // fallthrough
	return;
}

void Dot_render(SDL_Renderer* renderer, Texture* texture, Dot* dot)
{
	check(dot != NULL, "Invalid Dot!");
	
	Texture_render(renderer, texture, dot->position.x, dot->position.y, NULL);

error: // fallthrough
	return;
}

bool checkCollision(SDL_Rect* a, SDL_Rect* b)
{
	// The sides of the rectangle
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;
	
	// calculate the sides of the rect A
	leftA = a->x;
	rightA = a->x + a->w;
	topA = a->y;
	bottomA = a->y + a->h;
	
	// calculate the sides of the rect B
	leftB = b->x;
	rightB  = b->x + b->w;
	topB = b->y;
	bottomB = b->y + b->h;
	
	// Now we do our separating axis test and if any of the axis from 
	// either of the boxes are separate then there is not collision else
	// they will collide

	if(bottomA <= topB) return false;
	if(topA >= bottomB) return false;
	if(rightA <= leftB) return false;
	if(leftA >= rightB) return false;
	
	// if none of the sides of A are outside B then they are colliding
	return true;
}

// Best one to make you understand about collisions
// https://stackoverflow.com/questions/31022269/collision-detection-between-two-rectangles-in-java
/*
Say you have Rect A, and Rect B. Proof is by contradiction. Any one of four conditions guarantees that no overlap can exist:

Cond1. If A's left edge is to the right of the B's right edge, - then A is Totally to right Of B
Cond2. If A's right edge is to the left of the B's left edge, - then A is Totally to left Of B
Cond3. If A's top edge is below B's bottom edge, - then A is Totally below B
Cond4. If A's bottom edge is above B's top edge, - then A is Totally above B
So condition for Non-Overlap is

Cond1 Or Cond2 Or Cond3 Or Cond4

Therefore, a sufficient condition for Overlap is the opposite (De Morgan)

Not Cond1 And Not Cond2 And Not Cond3 And Not Cond4 This is equivalent to:

A's Left Edge to left of B's right edge, and
A's right edge to right of B's left edge, and
A's top above B's bottom, and
A's bottom below B's Top
Note 1: It is fairly obvious this same principle can be extended to any number of dimensions. 
Note 2: It should also be fairly obvious to count overlaps of just one pixel, change the < and/or the > 
on that boundary to a <= or a >=.
*/
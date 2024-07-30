#include <Dot.h>

// Global variables
static const int DOT_WIDTH = 20;
static const int DOT_HEIGHT = 20;

// Function Defintions
Dot* Dot_create(int x, int y)
{
	Dot* new_dot = (Dot*)malloc(sizeof(Dot));
	check(new_dot != NULL, "Failed to create the dot!");
	
	new_dot->position.x = x;
	new_dot->position.y = y;
	
	// Initializing the collider's dimensions
	new_dot->collider.x = x;
	new_dot->collider.y = y;
	new_dot->collider.w = DOT_WIDTH;
	new_dot->collider.h = DOT_HEIGHT;
	
	return new_dot;
error:
	return NULL;
}

bool Dot_checkCollision(Dot* dot, SDL_Rect* object)
{
	// if collision takes place we simply register it
	check(dot != NULL, "Inalid Dot!");
	
	bool collision = false;
	
	if(checkCollision(&dot->collider, object)){
		collision = true;
	}
	
	return collision;
error:
	return false;
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

#ifndef __DOT_H__
#define __DOT_H__

#include <SDL2/SDL.h>
#include <Texture.h>
#include <dbg.h>

typedef struct Dot{
	SDL_Point position;
	// Collision Box
	SDL_Rect collider;
}Dot;

Dot* Dot_create(int x, int y); // intiializes the variables and define a given position for the dot
void Dot_render(SDL_Renderer* renderer, Texture* texture, Dot* dot);

//  Collision Detection : We use the separating axis test to find if a collision has occured
// In this test we check for both the x and y projections of the objects and if they are not separated then
// they are colliding with each other
bool Dot_checkCollision(Dot* dot, SDL_Rect* object); // checks for collision against the given object
// new function for checking the collision between boxes
bool checkCollision(SDL_Rect* a, SDL_Rect* b);
#endif
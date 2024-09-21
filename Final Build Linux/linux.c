#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <math.h>
#include "dbg.h"

// DList

struct DListNode;

typedef struct DListNode{
	struct DListNode* next;
	struct DListNode* prev;
	void* value;
}DListNode;

typedef struct DList{
	int count;
	DListNode* first;
	DListNode* last;
}DList;

DList* DList_create();
void DList_destroy(DList* DList);
void DList_clear(DList* DList);
void DList_clear_destroy(DList* DList);

#define DList_count(A) ((A)->count)
#define DList_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define DList_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

void DList_push(DList* DList, void* value);
void* DList_pop(DList* DList);
void DList_unshift(DList* DList, void* value);
void* DList_shift(DList* DList);

void* DList_remove(DList* DList, DListNode* value);

#define DList_FOREACH(L, S, M, V) DListNode* _node = NULL; DListNode* V = NULL;\
for(V = _node = L->S; _node != NULL; V = _node = _node->M)
	
// DList Functions


DList* DList_create()
{
	return calloc(1, sizeof(DList));
}

void DList_destroy(DList* DList)
{
	DList_FOREACH(DList, first, next, cur){
		if(cur->prev){
			free(cur->prev);
		}
	}
	
	free(DList->last);
	free(DList);
}

void DList_clear(DList* DList)
{
	DList_FOREACH(DList, first, next, cur){
		free(cur->value);
	}
}

void DList_clear_destroy(DList* DList)
{
	DList_clear(DList);
	DList_destroy(DList);
}

void DList_push(DList* DList, void* value)
{
	DListNode* node = calloc(1, sizeof(DListNode));
	check_mem(node);
	
	node->value = value;
	
	if(DList->last == NULL){
		DList->first = node;
		DList->last = node;
	}else{
		DList->last->next = node;
		node->prev = DList->last;
		DList->last = node;
	}
	
	DList->count++;
	
	error:
		return;
}

void* DList_pop(DList* DList)
{
	DListNode* node = DList->last;
	return node != NULL ? DList_remove(DList, node) : NULL;
}

void DList_unshift(DList* DList, void* value)
{
	DListNode* node = calloc(1, sizeof(DListNode));
	check_mem(node);
	
	node->value = value;
	
	if(DList->first == NULL){
		DList->first = node;
		DList->last = node;
	}else{
		node->next = DList->first;
		DList->first->prev = node;
		DList->first = node;
	}
	
	DList->count++;
	
	error:
		return;
}

void* DList_shift(DList* DList)
{
	DListNode* node = DList->first;
	return node != NULL ? DList_remove(DList, node) : NULL;
}

void* DList_remove(DList* DList, DListNode* node)
{
	void* result = NULL;
	
	check(DList->first && DList->last, "DList is empty.");
	check(node, "node can't be NULL");
	
	if(node == DList->first && node == DList->last){
		DList->first = NULL;
		DList->last = NULL;
	}else if(node == DList->first){
		DList->first = node->next;
		check(DList->first != NULL, "Invalid DList, somehow got a first that is NULL.");
		DList->first->prev = NULL;
	}else if(node == DList->last){
		DList->last = node->prev;
		check(DList->last != NULL, "Invalid DList, somehow got a next that is NULL.");
		DList->last->next = NULL;
	}else{
		DListNode* after = node->next;
		DListNode* before = node->prev;
		after->prev = before;
		before->next = after;
	}
	
	DList->count--;
	result = node->value;
	free(node);
	
	error:
		return result;
}

// Texture
// Texture Data Structure
typedef struct Texture{
	SDL_Texture* texture; // Holds the SDL_Texture
	size_t width; // Defines the width and height and used size_t such that it remains positive
	size_t height;
}Texture;

// Functions
Texture* Texture_create(); // Initializes the variables
bool Texture_loadFromFile(SDL_Renderer* renderer, Texture* texture, const char* filepath); // Loads an image from a specified path
void Texture_render(SDL_Renderer* renderer, Texture* texture, int x, int y, SDL_Rect *clip); // Renders texture at a given point and only a given part
void Texture_renderEx(SDL_Renderer* renderer, Texture* texture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip); // Provides extra functionality while rendering
size_t Texture_getWidth(Texture* texture); // Returns the image width
size_t Texture_getHeight(Texture* texture); // Returns the image height
void Texture_destroy(Texture* texture); // Deallocates the memory from the texture
void Texture_setColor(Texture* texture, Uint8 red, Uint8 green, Uint8 blue); // Set the color modulation of the texture
void Texture_setBlendMode(Texture* texture, SDL_BlendMode blending); // Set the different blending mode on a texture
void Texture_setAlpha(Texture* texture, Uint8 alpha); // Modulates the alpha values
bool Texture_loadFromRenderedText(SDL_Renderer* renderer, Texture* texture, TTF_Font* font, const char* text, SDL_Color text_color); // This function will return images based on the font and the color and obviosuly the text too duh!

// Texture Functions
// Texture Functions 
// Initializes the variables to their equivalent zeroes
Texture* Texture_create()
{
	Texture* result = calloc(1, sizeof(Texture));
	check(result != NULL, "Failed to create a custom Texture");
		
	return result;
error:
	return NULL;
}

void Texture_destroy(Texture* texture)
{
	// Only getting rid of the SDL_Texture
	SDL_DestroyTexture(texture->texture);
	texture->texture = NULL;
	texture->width = 0;
	texture->height = 0;
}

bool Texture_loadFromFile(SDL_Renderer* renderer, Texture* texture, const char* filepath)
{
	// First we deallocate the texture if there is already one loaded in it
	Texture_destroy(texture);
	
	// The final texture
	SDL_Texture* new_texture = NULL;
	
	// Load the image at the specified path
	SDL_Surface* loaded_surface = IMG_Load(filepath);
	check(loaded_surface != NULL, "Failed to load the surface from %s! IMG_Error : %s", filepath, IMG_GetError());
	
	// Color keying the image
	SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, 0, 255, 255));
	// Here we first provide the surface we want to color key, the we let SDL know that we want to enable
	// color key and the last is the pixel we want to color key with
	// Also the most cross platform way to create a pixel from the RGB is by using the SDL_MapRGB() function
	// First we provide the format and then the corresponding RGB values
	
	// Now to create a texture from surface pixels
	new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
	check(new_texture != NULL, "Failed to convert surface of %s to a texture! SDL_Error : %s", filepath, SDL_GetError());
	
	// Get the image dimension
	texture->width = loaded_surface->w;
	texture->height = loaded_surface->h;
	
	// Get rid of the old surface
	SDL_FreeSurface(loaded_surface);
	
	texture->texture = new_texture;
	check(texture->texture != NULL, "Failed to copy the new texture of %s! SDL_Error: %s", filepath, SDL_GetError());
	
	return true;
error:
	return false;
}

void Texture_render(SDL_Renderer* renderer, Texture* texture, int x, int y, SDL_Rect* clip)
{	
	// Here we will define the position as to where we want to display our texture
	// First create a SDL_Rect for defining the dimension and position
	SDL_Rect render_quad = {x, y, texture->width, texture->height};
	
	check_debug(clip != NULL, "The provided clip is NULL! Rendering without it");
	
	// We only provide the area we want to render using the clip 
	// and we give that info to the render_quad
	render_quad.w = clip->w; 
	render_quad.h = clip->h;
	
	SDL_RenderCopy(renderer, texture->texture, clip, &render_quad); // Here the new argumet is the source rect which must be provided
	return;
	// Don't forget to add the return before this as it will execute as a normal flow of the program
error:
	SDL_RenderCopy(renderer, texture->texture, NULL, &render_quad); // This is in the case the clip is not provided 
}
void Texture_renderEx(SDL_Renderer* renderer, Texture* texture, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	// Added more functionality :
	// now the function takes 3 additional arguments that are:
	// 1. The angle of rotation
	// 2. The point around which the texture must rotate
	// 4. A SDL_RendererFlip enum
	
	// The new render function we call is SDL_RenderEx() which accepts these additional arguments
	
	// Here we will define the position as to where we want to display our texture
	// First create a SDL_Rect for defining the dimension and position
	SDL_Rect render_quad = {x, y, texture->width, texture->height};
	
	check_debug(clip != NULL, "The provided clip is NULL! Rendering without it");
	
	// We only provide the area we want to render using the clip 
	// and we give that info to the render_quad
	render_quad.w = clip->w; 
	render_quad.h = clip->h;
	
	SDL_RenderCopyEx(renderer, texture->texture, clip, &render_quad, angle, center, flip);
	return;
	// Don't forget to add the return before this as it will execute as a normal flow of the program
error:
	SDL_RenderCopyEx(renderer, texture->texture, NULL, &render_quad, angle, center, flip); // This is in the case the clip is not provided 
}
 

// Function to set the color / modulate the color
void Texture_setColor(Texture* texture, Uint8 red, Uint8 green, Uint8 blue)
{
	// We just have to make a call to an existing SDL function
	// We must provide the texture as well as the color values
	
	SDL_SetTextureColorMod(texture->texture, red, green, blue);
}

// Function to enable/disable blending on a texture
void Texture_setBlendMode(Texture* texture, SDL_BlendMode blending)
{
	// This function controls how the texture will be blended
	// There are different types of blendings which can be enabled on a texture
	// We will use SDL function SDL_SetTextureBlendMode(SDL_Texture*, SDL_BlendMode)
	SDL_SetTextureBlendMode(texture->texture, blending);
}

// Function to set the alpha value on the texture
void Texture_setAlpha(Texture* texture, Uint8 alpha)
{
	// This function will control the transparency of the texture
	// We will use the SDL function SDL_SetTextureAlphaMod(SDL_Texture*, Uint8)
	SDL_SetTextureAlphaMod(texture->texture, alpha);
}

size_t Texture_getWidth(Texture* texture)
{
	return texture->width;
}

size_t Texture_getHeight(Texture* texture)
{
	return texture->height;
}

bool Texture_loadFromRenderedText(SDL_Renderer* renderer, Texture* texture, TTF_Font* font, const char* text, SDL_Color text_color)
{
	// Get rid of the preexisting texture
	Texture_destroy(texture);
	
	// Render the text surface
	// For rendering text use the TTF_RenderText_*(TTF_Font*, text, SDL_Color)
	// This function returns a SDL_Surface on successful render of the text
	SDL_Surface* temp_text = TTF_RenderText_Solid(font, text, text_color); 
	check(temp_text != NULL, "Unable to render text surface! SDL_ttf Error: %s", TTF_GetError());
	
	// Now create a texture from the surface as usual
	texture->texture = SDL_CreateTextureFromSurface(renderer, temp_text);
	check(texture->texture != NULL, "Unable to create texture from rendered text! SDL Error: %s", SDL_GetError());
	
	// Now get the image dimensions
	texture->width = temp_text->w;
	texture->height = temp_text->h;
	
	// Get rid of the old surface
	SDL_FreeSurface(temp_text);
	
	return true;
error:
	return false;
}

// Timer
typedef struct Timer{
	Uint32 start_ticks; // clocks time when the timer is started
	Uint32 paused_ticks; // clock time when the timer is paused
	// defines timer status
	bool paused;
	bool started;
}Timer;

Timer* Timer_create(); // Initializes and create a timer
// Various clock function
void Timer_start(Timer* timer);
void Timer_stop(Timer* timer);
void Timer_pause(Timer* timer);
void Timer_resume(Timer* timer);
// get the timer's time
Uint32 Timer_getTicks(Timer* timer);
// check the status of the timer
bool Timer_isStarted(Timer* timer);
bool Timer_isPaused(Timer* timer);

// Timer Functions
Timer* Timer_create()
{
	Timer* new_timer = (Timer*)malloc(sizeof(Timer));
	check(new_timer != NULL, "Failed to create a timer! SDL_Error: %s",SDL_GetError());
	
	new_timer->start_ticks = 0;
	new_timer->paused_ticks = 0;
	new_timer->paused = false;
	new_timer->started = false;
	
	return new_timer;
error:
	return NULL;
}

void Timer_start(Timer* timer) // we can simply call it again to restart the timer
{							   // and must clear other flags too
	check(timer != NULL, "Invalid Timer!");
	
	timer->started = true;
	timer->paused = false;
	
	timer->start_ticks = SDL_GetTicks(); // get the elapsed time
	timer->paused_ticks = 0;
	
error: // fallthrough
	return;
}

void Timer_stop(Timer* timer)
{
	check(timer != NULL, "Invalid Timer!");
	
	// basically reinitialize all  the values
	timer->started = false;
	timer->paused = false;
	timer->start_ticks = 0;
	timer->paused_ticks = 0;

error: // fallthrough
	return;
}

void Timer_pause(Timer* timer)
{
	check(timer != NULL, "Invalid Timer!");
	
	// if the timer is running and isn't already paused
	if(timer->started && !timer->paused){ // as it won't make sense to pause a timer which hasn't already started
		// pause the timer
		timer->paused = true;
		// calculate the paused ticks
		timer->paused_ticks = SDL_GetTicks() - timer->start_ticks;
		// reset the start ticks
		timer->start_ticks = 0;
	}
	
error: // fallthrough
	return;
}

void Timer_resume(Timer* timer)
{
	check(timer != NULL, "Invalid Timer!");
	
	// if the timer is running and is paused
	if(timer->started && timer->paused){
		// resume the timer
		timer->paused = false;
		// reset the start ticks
		timer->start_ticks = SDL_GetTicks() - timer->paused_ticks;
		// reset the paused ticks
		timer->paused_ticks = 0;
		
		// this technique maintains the same amount of relative time
		// between our respective timer and the global timer (SDL_GetTicks())
	}

error: // fallthrough
	return;
}

Uint32 Timer_getTicks(Timer* timer)
{
	// storing the timer timer here
	Uint32 time = 0;
	
	check(timer != NULL, "Invalid Timer!");
	
	// if the timer is running
	if(timer->started){
		//if the timer is paused
		if(timer->paused){
			// set the time at which it was paused
			time = timer->paused_ticks;
		}
		else{
			// set the relative time
			time = SDL_GetTicks() - timer->start_ticks;
		}

	}

error: // fallthrough
	return time;
}

bool Timer_isStarted(Timer* timer)
{
	check(timer != NULL, "Invalid Timer!");
	
	return timer->started;
error:
	return false;
}

bool Timer_isPaused(Timer* timer)
{
	check(timer != NULL, "Invalid Timer!");
	
	return timer->started && timer->paused; // as you won't be able to pause if it wasn't started at all
error:
	return false;
}

// UI
// An enumeration to define button action sprites
// This is basically genius since we can take a common texture
// and then depending on the state of the button we can render the
// specific sprite for that button

typedef enum ButtonSprite{
	BUTTON_SPRITE_MOUSE_OUT,
	BUTTON_SPRITE_MOUSE_OVER_MOTION,
	BUTTON_SPRITE_MOUSE_DOWN,
	BUTTON_SPRITE_MOUSE_UP,
	BUTTON_SPRITE_TOTAL
}ButtonSprite;

// a struct to represent a button
typedef struct Button{
	SDL_Point position; // defines the (x,y) position
	unsigned int width;
	unsigned int height;
	ButtonSprite current_sprite; // defines the currently used sprite by the button
}Button;

// Button Functions
Button* Button_create(unsigned int width, unsigned int height); // Creates an empty button
void Button_setPosition(Button* button, int x, int y); // Sets the x and y coordinate of the button
void Button_handleMouseEvents(Button* button, SDL_Event* e); // Handles the mouse event for the mouse
void Button_render(SDL_Renderer* renderer, Button* button, Texture* texture, SDL_Rect button_sprite_clips[]); // Renders the specified button, a clip must be provided
																							  // for the whole texture
// UI Functions

// Function defintions
Button* Button_create(unsigned int width, unsigned int height)
{
	Button* new_button = (Button*) malloc(sizeof(Button));
	check_mem(new_button);
	
	// Initialize the newly created button
	new_button->position.x = 0;
	new_button->position.y = 0;
	new_button->width = width;
	new_button->height = height;
	new_button->current_sprite = BUTTON_SPRITE_MOUSE_OUT;
	
	return new_button;
error:
	return NULL;
}

void Button_setPosition(Button* button, int x, int y)
{
	check(button != NULL,"Invalid Button");
	button->position.x = x;
	button->position.y = y;

error: // fallthrough
	return; 
}

void Button_handleMouseEvents(Button* button, SDL_Event* e)
{
	check(button != NULL, "Invalid Button");
	
	// First we will check if the mouse event happened
	if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP){
		// These events corresponds to 1. moving the mouse 2. clicking down a mouse button 3. releasing a mouse button
		
		// Fetch the mouse position
		int x, y;
		SDL_GetMouseState(&x, &y); // We use this function to obtatin the mouse position
		
		// now we will check if the mouse is inside the button or not
		bool inside = true;
		
		if(x < button->position.x) inside = false; // mouse is to the left of the button
		else if(x > button->position.x + button->width) inside = false; // mouse is to the right of the button
		else if(y < button->position.y) inside = false; // mouse is above the button
		else if(y > button->position.y + button->height) inside = false; // mouse is below the button
		
		// if mouse is outside the button
		if(!inside){
			button->current_sprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		else{ // if mouse inside the button
			switch(e->type){
				case SDL_MOUSEMOTION:
					button->current_sprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
					break;
				
				case SDL_MOUSEBUTTONDOWN:
					button->current_sprite = BUTTON_SPRITE_MOUSE_DOWN;
					break;
				
				case SDL_MOUSEBUTTONUP:
					button->current_sprite = BUTTON_SPRITE_MOUSE_UP;
					break;
			}
		}
		
	}

error: // fallthrough
	return;
}

void Button_render(SDL_Renderer* renderer, Button* button, Texture* texture, SDL_Rect button_sprite_clips[])
{
	check(button != NULL, "Invalid Button");
	check(texture != NULL, "Invalid Texture");
	
	// show the current button sprite
	Texture_render(renderer, texture, button->position.x, button->position.y, &button_sprite_clips[button->current_sprite]); 
	
error: // fallthrough
	return;
}

// Dot
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

// Dot Functions
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

// Snake
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

// Snake Functions
// Our snake
DList* Snake = NULL;
const int SNAKE_WIDTH = 20;
const int SNAKE_HEIGHT = 20;
const int SNAKE_VEL = 20;
bool SNAKE_COLLIDED = false;

// Function defintions

SnakeNode* SnakeNode_create(int x, int y, Orientation o)
{
	SnakeNode* node = (SnakeNode*)malloc(sizeof(SnakeNode)); // creating a body node
	check(node != NULL, "Failed to create the Snake head!");
	
	// Initializing the various components
	node->body.x = x;
	node->body.y = y;
	node->body.w = SNAKE_WIDTH;
	node->body.h = SNAKE_HEIGHT;
	
	// changing the default velocity up on the direction given by the user
		switch(o){
		case SNAKE_UP:
			node->velocity.x = 0;
			node->velocity.y = -SNAKE_VEL;
			break;
		
		case SNAKE_DOWN:
			node->velocity.x = 0;
			node->velocity.y = SNAKE_VEL;
			break;
			
		case SNAKE_LEFT:
			node->velocity.x = -SNAKE_VEL;
			node->velocity.y = 0;
			break;
		
		case SNAKE_RIGHT:
			node->velocity.x = SNAKE_VEL;
			node->velocity.y = 0;
			break;
		
		case SNAKE_NONE:
			node->velocity.x = 0;
			node->velocity.y = 0;
	}

	node->collider.x = x;
	node->collider.y = y;
	node->collider.w = SNAKE_WIDTH;
	node->collider.h = SNAKE_HEIGHT;
	
	node->orientation = o;
	
	return node;
error:
	return NULL;
}

bool Snake_create(int x, int y, Orientation o)
{
	Snake = DList_create(); // create a DList
	check(Snake != NULL, "Failed to create the Snake!");
	
	SnakeNode* head = SnakeNode_create(x, y, o);
	check(head != NULL, "Failed to create the Snake body!");
	
	// pushing the body in the list
	DList_push(Snake, head);

	return true;
error:
	return false;
}

void Snake_handleEvents(SDL_Event* e)
{
	// Upon pressing down on the key
	if(e->type == SDL_KEYDOWN && e->key.repeat == 0){
		switch(e->key.keysym.sym){
			case SDLK_UP: 
				// if the orientation is down the obviously you cannot go up
				if(((SnakeNode*)DList_first(Snake))->orientation != SNAKE_DOWN){
					((SnakeNode*)DList_first(Snake))->velocity.y = -SNAKE_VEL; // Changing VEL to HEIGHT or WIDTH
					((SnakeNode*)DList_first(Snake))->velocity.x = 0;
					((SnakeNode*)DList_first(Snake))->orientation = SNAKE_UP; 
				}
				break;
				
			case SDLK_DOWN:
				if(((SnakeNode*)DList_first(Snake))->orientation != SNAKE_UP){
					((SnakeNode*)DList_first(Snake))->velocity.y = SNAKE_VEL;
					((SnakeNode*)DList_first(Snake))->velocity.x = 0;
					((SnakeNode*)DList_first(Snake))->orientation = SNAKE_DOWN; 
				}
				break;
			
			case SDLK_LEFT:
				if(((SnakeNode*)DList_first(Snake))->orientation != SNAKE_RIGHT){
					((SnakeNode*)DList_first(Snake))->velocity.x = -SNAKE_VEL;
					((SnakeNode*)DList_first(Snake))->velocity.y = 0;
					((SnakeNode*)DList_first(Snake))->orientation = SNAKE_LEFT;
				}
				break;
					
			case SDLK_RIGHT:
				if(((SnakeNode*)DList_first(Snake))->orientation != SNAKE_LEFT){
					((SnakeNode*)DList_first(Snake))->velocity.x = SNAKE_VEL;
					((SnakeNode*)DList_first(Snake))->velocity.y = 0;
					((SnakeNode*)DList_first(Snake))->orientation = SNAKE_RIGHT; 
				}
				break;	
		}
	}	
}

void Snake_move()
{
	// Approach 3
	int x = 0, y = 0;
	Orientation o = SNAKE_NONE;
	SnakeNode* new_node = NULL;
	
	// making few chages in case the snake collides
	if(!SNAKE_COLLIDED){
		x = ((SnakeNode*)DList_first(Snake))->body.x + ((SnakeNode*)DList_first(Snake))->velocity.x;
		y = ((SnakeNode*)DList_first(Snake))->body.y + ((SnakeNode*)DList_first(Snake))->velocity.y;
		o = ((SnakeNode*)DList_first(Snake))->orientation;
			
		// create the new node based upon the parameter
		new_node = SnakeNode_create(x, y, o);
		check(new_node != NULL, "Failed to create the new node!");
		
		// now push this node in the beginning of the list
		DList_unshift(Snake, new_node);
		// and remove the tail
		DList_pop(Snake);
	}
	else{
		// do nothing, hence stopping the snake
	}


error:
	return;
}

void Snake_render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Settinig the color of the snake
	
	DList_FOREACH(Snake, first->next, next, cur){ // Drawing the body of the snake
		SDL_RenderFillRect(renderer, &((SnakeNode*)cur->value)->body);
	}
	
	// now to render the head of the snake
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &((SnakeNode*)DList_first(Snake))->body);
}

// growing the snake now its important to decide where should the new body be placed
void Snake_grow()
{	
	// Approach 2 : Adding the node at the head of the list in the given head direction
	int x = 0, y = 0;
	Orientation o = SNAKE_NONE;
	SnakeNode* new_node = NULL;
	
	// if the last node is oriented up then the snake must grow towards the up
	// i.e the position of the the new node will be (x, y+h)
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_UP ){
		x = ((SnakeNode*)DList_first(Snake))->body.x;
		y = ((SnakeNode*)DList_first(Snake))->body.y - SNAKE_HEIGHT;
		o = SNAKE_UP;
	}
	
	// if the last node is oriented down then the snake must grow towards the down
	// i.e. the position of the new node will be (x, y-h)
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_DOWN ){
		x = ((SnakeNode*)DList_first(Snake))->body.x;
		y = ((SnakeNode*)DList_first(Snake))->body.y + SNAKE_HEIGHT;
		o = SNAKE_DOWN;
	}
	
	// if the last node is oriented to the left then the snake must grow towards the left
	// i.e. the position of the new node will be (x+w, y)
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_LEFT ){
		x = ((SnakeNode*)DList_first(Snake))->body.x - SNAKE_WIDTH;
		y = ((SnakeNode*)DList_first(Snake))->body.y;
		o = SNAKE_LEFT;
	}
	
	// if the last node is oriented to the right then the snake must grow towards the right
	// i.e. the position of the new node will be (x-w, y)
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_RIGHT ){
		x = ((SnakeNode*)DList_first(Snake))->body.x + SNAKE_WIDTH;
		y = ((SnakeNode*)DList_first(Snake))->body.y;
		o = SNAKE_RIGHT;
	}
	
	// create the new node based upon the parameter
	new_node = SnakeNode_create(x, y, o);
	check(new_node != NULL, "Failed to create the new node!");
	
	// now push this node in the list
	DList_unshift(Snake, new_node);
	
error: // fall through
	return;
}

// Function to check for collision
bool Snake_checkCollision(SDL_Rect* a, SDL_Rect* b)
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

void Snake_collision(int PLAY_WIDTH, int PLAY_HEIGHT)
{
	// now to check if the snake has collided with the screen walls or not
	if( ( ((SnakeNode*)DList_first(Snake))->body.x < 0 + SNAKE_WIDTH ) || ( ((SnakeNode*)DList_first(Snake))->body.x > PLAY_WIDTH ))
	{
		printf("Collision on x-axis!\n");
		SNAKE_COLLIDED = true;
		return;
	}
		
	if( ( ((SnakeNode*)DList_first(Snake))->body.y < 0 + SNAKE_HEIGHT ) || ( ((SnakeNode*)DList_first(Snake))->body.y > PLAY_HEIGHT ))
	{
		printf("Collision on y-axis\n");
		SNAKE_COLLIDED = true;
		return;
	}
		
	// now to check if the snake has collided with himself or not
	// we will check for collision against the head of the snake with the rest of the body
	// therefore we will start the loop from the second node
	DList_FOREACH(Snake, first->next, next, cur){
		if(Snake_checkCollision( &((SnakeNode*)DList_first(Snake))->collider, &((SnakeNode*)cur->value)->collider)){
			printf("Collision with itself!!\n");
			SNAKE_COLLIDED = true;
			return;
		}
	}
}

void Snake_destroy()
{
	DList_clear_destroy(Snake);
}

// Game
// Usual Global Variables
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern TTF_Font* font; // globally used font
extern bool quit;
extern int max_scare;

// Functions
bool init(); // Intializes subsystems and 
bool loadMedia(); // Loads the textures, music etc
void handleEvents(); // handles the user and game event
void update(); // Handles physics and other updates
void render(); // Display the sprites and textures on the screen
void close(); // frees the resources and closes the subsystems
int run();

// Function for generating random points 
// We will first obtain the blocks currently occupied by the snake or any object
// Then we will reject those blocks from the possible points
// and only select those points for the possible points
SDL_Point* generateRandomPoint();

// function to reset the game upon user input
void reset();
// function to read and write the max score
bool readMaxScore();
void writeMaxScore();

// Game Functions

// Some globals
// Usual Global Variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
// Icon for the window
SDL_Surface* window_icon = NULL;

// Screen dimension
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Play area dimension
int PLAY_WIDTH = 0;
int PLAY_HEIGHT = 0;

const int SCREEN_FPS = 5;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

int X_BLOCKS = 0; // used for generating the random no.
int Y_BLOCKS = 0; // used for generating the random no

TTF_Font* font = NULL;
bool quit = false;

// Timer to cap our FPS
Timer* cap_timer = NULL;

// our food for the snake
Texture dot_texture;
Dot* dot;

// tile for the containment
Texture top_tile;
Texture bottom_tile;
Texture left_tile;
Texture right_tile;
// and their corresponding colliders
SDL_Rect top_tile_collider;
SDL_Rect bottom_tile_collider;
SDL_Rect left_tile_collider;
SDL_Rect right_tile_collider;

// Variables for our score
// These ones are fixed and are set only once
Texture SCORE; 
Texture MAX;
// These ones are updated whenever snake eats the food
Texture CURRENT_SCORE;
Texture MAX_SCORE;
// Changes depending on the 
Texture MESSAGE_1;
Texture MESSAGE_2;
int current_score = 0;
int max_score = 0;
char str_current_score[32];
char str_max_score[32];
// Our text color
SDL_Color text_color = {0,0,0,255};

int run()
{
	bool r = init();
	check(r == true, "Something went wrong");
	
	r = loadMedia();
	check(r == true, "Something went wrong");
	
	while(!quit)
	{	
		// Starting the cap timer
		Timer_start(cap_timer);

		handleEvents();
		update();
		render();
	}
	
	close();
	
	return 0;
error:
	return 1;
}

// Function definitions
bool init()
{
	check(SDL_Init(SDL_INIT_EVERYTHING) >= 0, "Failed to initialize SDL! SDL_Error: %s", SDL_GetError());
	check((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) > 0, "Failed to initialize SDL_image! IMG_Error: %s", IMG_GetError());
	check(TTF_Init() != -1, "Failed to intialzie SDL_ttf! TTF_Error: %s", TTF_GetError()); 
	check(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0, "Failed to initialize SDL_mixer! Mix_Error: %s", Mix_GetError());
	
	window = SDL_CreateWindow("Snake",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
	check(window != NULL, "Failed to create a window! SDL_Error: %s", SDL_GetError());
	
	// setting the window icon
	window_icon = IMG_Load("Assets/Icon.png");
	SDL_SetWindowIcon(window, window_icon);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	check(renderer != NULL, "Failed to create a renderer! SDL_Error: %s", SDL_GetError());
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	// Initializing the play dimension
	PLAY_WIDTH = SCREEN_WIDTH - (SNAKE_WIDTH * 2); // needs to be reduced on both the sides
	PLAY_HEIGHT = SCREEN_HEIGHT - 100 - (SNAKE_HEIGHT * 2); // needs to be reduced on both the sides
	
	// Initializing the blocks
	X_BLOCKS = PLAY_WIDTH / SNAKE_WIDTH;
	Y_BLOCKS = PLAY_HEIGHT / SNAKE_HEIGHT;
	
	// reading the max score
	if(!readMaxScore()){ // if unable to read the score then set it to 0
		max_score = 0;
	}
	
	return true;
error:
	return false;
}

bool loadMedia()
{
	// creating our snake
	bool r = Snake_create((SCREEN_WIDTH) / 2, (SCREEN_HEIGHT) / 2, SNAKE_NONE); // start with none to describe the starting point
	check(r != false, "Failed to create the Snake!");
	
	// creating our timer
	cap_timer = Timer_create();
	check(cap_timer != NULL, "Failed to create the timer!");
	
	// loading the dot texture
	r = Texture_loadFromFile(renderer, &dot_texture, "Assets/dot.bmp");
	check(r != false, "Failed to load the texture!");
	
	// loading the tile textures
	r = Texture_loadFromFile(renderer, &top_tile, "Assets/Top Tile.png");
	check(r != false, "Failed to load the texture!");
	
	r = Texture_loadFromFile(renderer, &bottom_tile, "Assets/Bottom Tile.png");
	check(r != false, "Failed to load the texture!");
	
	r = Texture_loadFromFile(renderer, &left_tile, "Assets/Left Tile.png");
	check(r != false, "Failed to load the texture!");
	
	r = Texture_loadFromFile(renderer, &right_tile, "Assets/Right Tile.png");
	check(r != false, "Failed to load the texture!");
	
	// Also lets load in our fonts
	font = TTF_OpenFont("Assets/m6x11.ttf", 30);
	check(font != NULL, "Failed to load the font! TTF_Error: %s", TTF_GetError());
	
	// Initialize our texts
	r = Texture_loadFromRenderedText(renderer, &SCORE, font, "SCORE -", text_color);
	check(r != false, "Failed to create the text!");
	
	r = Texture_loadFromRenderedText(renderer, &MAX, font, "MAX -", text_color);
	check(r != false, "Failed to create the text!");
	
	r = Texture_loadFromRenderedText(renderer, &MESSAGE_1, font, "USE ARROW KEYS", text_color);
	check(r != false, "Failed to create the text!");
	
	r = Texture_loadFromRenderedText(renderer, &MESSAGE_2, font, "TO MOVE THE SNAKE AROUND", text_color);
	check(r != false, "Failed to create the text!");
	
	// also to update our score
	sprintf(str_current_score, "%d", current_score);
	sprintf(str_max_score, "%d", max_score);
	
	// generating the texts
	r = Texture_loadFromRenderedText(renderer, &CURRENT_SCORE, font, str_current_score, text_color);
	check(r != false, "Failed to create the text!");
	
	Texture_loadFromRenderedText(renderer, &MAX_SCORE, font, str_max_score, text_color);
	check(r != false, "Failed to create the text!");

	// also initialize their respective colliders
	top_tile_collider.x = 0;
	top_tile_collider.y = 0;
	top_tile_collider.w = Texture_getWidth(&top_tile);
	top_tile_collider.h = Texture_getHeight(&top_tile);
	
	bottom_tile_collider.x = 0;
	bottom_tile_collider.y = 480;
	bottom_tile_collider.w = Texture_getWidth(&bottom_tile);
	bottom_tile_collider.h = Texture_getHeight(&bottom_tile);
	
	left_tile_collider.x = 0;
	left_tile_collider.y = 0;
	left_tile_collider.w = Texture_getWidth(&left_tile);
	left_tile_collider.h = Texture_getHeight(&left_tile);
	
	right_tile_collider.x = 780;
	right_tile_collider.y = 0;
	right_tile_collider.w = Texture_getWidth(&right_tile);
	right_tile_collider.h = Texture_getHeight(&right_tile);
	
	// Creating the dot
	SDL_Point* random = generateRandomPoint();
	check(random != NULL, "Failed to create a random point!");
	
	dot = Dot_create(random->x, random->y);
	check(dot != NULL, "Failed to create the dot!");
	
	return true;
error:
	return false;
}

void handleEvents()
{
	SDL_Event e; // Queue to store the events
	
	while(SDL_PollEvent(&e) != 0){
			
		if(e.type == SDL_QUIT){
			quit = true;
		}
		// checking in case the user resets the game
		if(SNAKE_COLLIDED){
			if(e.type == SDL_KEYDOWN){
				if(e.key.keysym.sym == SDLK_SPACE){
					// reset the snake
					reset();
					// reset the snake collision
					SNAKE_COLLIDED = false;
				}
			}
		}
		// Handles the input
		Snake_handleEvents(&e);
		
	}	
}

void update()
{
	// Updates the snake's movement
	Snake_move();
	
	// Check for snake collisions
	Snake_collision(PLAY_WIDTH, PLAY_HEIGHT);
	
	// Check if the snake has collided
	if(SNAKE_COLLIDED){
		// Then stop the snake somehow
		printf("Stop the Snake!\n");
		// Update the message on the screen
		Texture_loadFromRenderedText(renderer, &MESSAGE_1, font, "GAME OVER!", text_color);
		Texture_loadFromRenderedText(renderer, &MESSAGE_2, font, "PRESS SPACE TO RESTART", text_color);
	}
	
	// we will check collision with the dot and grow the snake
	if(Dot_checkCollision(dot, &((SnakeNode*)DList_first(Snake))->collider)){
		Snake_grow(); // Growing the snake when eating the food
		
		SDL_Point* food_position = generateRandomPoint(); // obtaining a new position for the food
		check(food_position != NULL, "Failed to create the food position!");
		
		dot->position = *food_position; // providing the new position to the food
		// also update its collider
		dot->collider.x = dot->position.x;
		dot->collider.y = dot->position.y;
		
		// updating the score when the snake eats the food
		current_score += 5;
		if(max_score < current_score){ // also update the max score
			max_score = current_score;
			sprintf(str_max_score, "%d", max_score);
			Texture_loadFromRenderedText(renderer, &MAX_SCORE, font, str_max_score, text_color);
		}
		// also to update our score
		sprintf(str_current_score, "%d", current_score);
		// generating the texts
		Texture_loadFromRenderedText(renderer, &CURRENT_SCORE, font, str_current_score, text_color);

	}
	
error: // fallthrough
	return;
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer); // Clears the current frame
	
	// also display its food
	Dot_render(renderer, &dot_texture, dot);
	
	// lets render the snake (rendering the snake on top of the dot)
	Snake_render(renderer);
	
	// Part of our score area must be drawn below the tiles
	Texture_render(renderer, &right_tile, 200, 480, NULL);
	Texture_render(renderer, &left_tile, 580, 480, NULL);
	
	//  Lets render the tiles too
	Texture_render(renderer, &top_tile, 0, 0, NULL);
	Texture_render(renderer, &bottom_tile, 0, 480, NULL);
	Texture_render(renderer, &left_tile, 0, 0, NULL);
	Texture_render(renderer, &right_tile, 780, 0, NULL);
	
	// also lets create a score area
	Texture_render(renderer, &bottom_tile, 0, 580, NULL);
	Texture_render(renderer, &left_tile, 0, 500, NULL);
	Texture_render(renderer, &right_tile, 780, 500, NULL);
	
	// draw our score texts
	Texture_render(renderer, &SCORE, 40, 520, NULL);
	Texture_render(renderer, &MAX, 620, 520, NULL);
	// drawing our actual scores
	Texture_render(renderer, &CURRENT_SCORE, 140, 520, NULL);
	Texture_render(renderer, &MAX_SCORE, 700, 520, NULL);
	
	// Draw our messages on the screen
	if(!SNAKE_COLLIDED){
		Texture_render(renderer, &MESSAGE_1, 300, 520, NULL);
		Texture_render(renderer, &MESSAGE_2, 240, 540, NULL);
	}
	else{
		Texture_render(renderer, &MESSAGE_1, 340, 520, NULL);
		Texture_render(renderer, &MESSAGE_2, 260, 540, NULL);
	}	

	SDL_RenderPresent(renderer); // Display the frame to the screen
	
	// implementing the capping log
	int frame_ticks = Timer_getTicks(cap_timer);
	if(frame_ticks < SCREEN_TICKS_PER_FRAME){
		SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_ticks);
	}
}

void close()
{	
	// write the max score to disk
	writeMaxScore();

	// deallocating the memory from the list
	// DList_destroy(Snake);
	
	// deallocating memory from the dot
	free(dot);
	dot = NULL;
	
	// deallocating memory from the texture
	Texture_destroy(&dot_texture);
	
	// Close our window and the renderer
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
	
	// Quit SDL subsystems
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

// Point generation function
SDL_Point* generateRandomPoint()
{
	
	// Variables for storing the coordinates of the food
	int x = 0;
	int y = 0;
	bool point_found = false; // variable to check if a valid point is found

	// Appraoch 2 : To check that the newly generated point shouldn't collide with the snake
	
	for(int i = 0;i < 5;i++){
		
		x = (rand() % X_BLOCKS) * SNAKE_WIDTH;
		y = (rand() % Y_BLOCKS) * SNAKE_HEIGHT;
		
		point_found = true; // Negative assumption
		
		// create a rectangle to check the collision again
		SDL_Rect collider;
		collider.x = x;
		collider.y = y;
		collider.w = SNAKE_WIDTH;
		collider.h = SNAKE_HEIGHT;
		
		// check if the collider collides against any of the body of the snake
		DList_FOREACH(Snake, first, next, cur){
			if(checkCollision(&collider, &((SnakeNode*)cur->value)->collider)){
				printf("Collision with the new point!\n");
				point_found = false;
			}
			// also it shouldn't collide with any of the walls
			if(checkCollision(&collider, &top_tile_collider)) point_found = false;
			else if(checkCollision(&collider, &bottom_tile_collider)) point_found = false;
			else if(checkCollision(&collider, &left_tile_collider)) point_found = false;
			else if(checkCollision(&collider, &right_tile_collider)) point_found = false;
		}
		
		if(point_found){
			printf("New Point found!\n");
			break;
		}
	}
	
	// However if we are unable to find the point
	// Then let's just put the point in front of the snake'e head
	if(!point_found){
		// and check the orientation of the head
		SnakeNode* head = (SnakeNode*)DList_first(Snake);
		
		switch(head->orientation){
			case SNAKE_UP:
				x = head->body.x;
				y = head->body.y - SNAKE_HEIGHT;
				break;
			
			case SNAKE_DOWN:
				x = head->body.x;
				y = head->body.y + SNAKE_HEIGHT;
				break;
			
			case SNAKE_LEFT:
				x = head->body.x - SNAKE_WIDTH;
				y = head->body.y;
				break;
			
			case SNAKE_RIGHT:
				x = head->body.x + SNAKE_WIDTH;
				y = head->body.y;
				break;
		}
	}
	
	printf("Food position: %d, %d\n",x ,y);
	// Copying the data to the point
	SDL_Point* new_point = (SDL_Point*)malloc(sizeof(SDL_Point));
	check(new_point != NULL, "Failed to create a point!");
	
	new_point->x = x;
	new_point->y = y;
	
	return new_point;
error:
	return NULL;
}

// Function to reset the game state
void reset()
{
	// So first lets lets clear the snake and create a new one
	Snake_destroy();
	bool r = Snake_create((SCREEN_WIDTH) / 2, (SCREEN_HEIGHT) / 2, SNAKE_NONE); // start with none to describe the starting point
	check(r != false, "Failed to create the Snake!");
	
	// also we want to generate a new food point
	SDL_Point* food_position = generateRandomPoint(); // obtaining a new position for the food
	check(food_position != NULL, "Failed to create the food position!");
	
	dot->position = *food_position; // providing the new position to the food
	// also update its collider
	dot->collider.x = dot->position.x;
	dot->collider.y = dot->position.y;
	
	// also don't forget to reset the current score
	current_score = 0;
	sprintf(str_current_score, "%d", current_score);
	// generating the texts
	Texture_loadFromRenderedText(renderer, &CURRENT_SCORE, font, str_current_score, text_color);
	
	// also to update the messages accordingly
	r = Texture_loadFromRenderedText(renderer, &MESSAGE_1, font, "USE ARROW KEYS", text_color);
	check(r != false, "Failed to create the text!");
	
	r = Texture_loadFromRenderedText(renderer, &MESSAGE_2, font, "TO MOVE THE SNAKE AROUND", text_color);
	check(r != false, "Failed to create the text!");
	
error: // fallthrough
	return;
}

// function to read the max score
bool readMaxScore()
{
	FILE* file = fopen("Assets/score.snake","r");
	check(file != NULL, "Failed to open the file");
	
	fscanf(file, "%d", &max_score);
	
	// if we can read the file then

	return true;
error:
	return false;
}
// function to write the max score
void writeMaxScore()
{
	FILE* file = fopen("Assets/score.snake","w"); // if file don't exists it will create it
	fprintf(file, "%d", max_score);
	fclose(file);
}



// Main Function

int main(int argc, char* argv[])
{
	printf("Seems to work!\n");
	int r = run();
	check(r == 0, "Something went wrong!");
	return r;
error:
	return -1;
}
#include <Snake.h>

// Our snake
DList* Snake = NULL;
const int SNAKE_WIDTH = 20;
const int SNAKE_HEIGHT = 20;
const int SNAKE_VEL = 20;

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
				((SnakeNode*)DList_first(Snake))->velocity.y = -SNAKE_VEL; // Changing VEL to HEIGHT or WIDTH
				((SnakeNode*)DList_first(Snake))->velocity.x = 0;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_UP; 
				break;
				
			case SDLK_DOWN:
				((SnakeNode*)DList_first(Snake))->velocity.y = SNAKE_VEL;
				((SnakeNode*)DList_first(Snake))->velocity.x = 0;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_DOWN; 
				break;
			
			case SDLK_LEFT:
				((SnakeNode*)DList_first(Snake))->velocity.x = -SNAKE_VEL;
				((SnakeNode*)DList_first(Snake))->velocity.y = 0;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_LEFT; 
				break;
					
			case SDLK_RIGHT:
				((SnakeNode*)DList_first(Snake))->velocity.x = SNAKE_VEL;
				((SnakeNode*)DList_first(Snake))->velocity.y = 0;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_RIGHT; 
				break;	
		}
	}	
}

void Snake_move(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	// Approach 3
	int x = 0, y = 0;
	Orientation o = SNAKE_NONE;
	SnakeNode* new_node = NULL;
	
	x = ((SnakeNode*)DList_first(Snake))->body.x + ((SnakeNode*)DList_first(Snake))->velocity.x;
	y = ((SnakeNode*)DList_first(Snake))->body.y + ((SnakeNode*)DList_first(Snake))->velocity.y;
	o = ((SnakeNode*)DList_first(Snake))->orientation;
		
	// create the new node based upon the parameter
	new_node = SnakeNode_create(x, y, o);
	check(new_node != NULL, "Failed to create the new node!");
	
	// now push this node in the list
	DList_unshift(Snake, new_node);
	// and remove the tail
	DList_pop(Snake);
	
	// now to check if the snake has collided with the screen walls or not
	if( ( ((SnakeNode*)DList_first(Snake))->body.x < 0 + SNAKE_WIDTH ) || ( ((SnakeNode*)DList_first(Snake))->body.x > SCREEN_WIDTH ))
		printf("Collision on x-axis!\n");
	if( ( ((SnakeNode*)DList_first(Snake))->body.y < 0 + SNAKE_HEIGHT ) || ( ((SnakeNode*)DList_first(Snake))->body.y > SCREEN_HEIGHT ))
		printf("Collision on y-axis\n");
	
	// now to check if the snake has collided with himself or not
	// we will check for collision against the head of the snake with the rest of the body
	// therefore we will start the loop from the second node
	DList_FOREACH(Snake, first->next, next, cur){
		if(Snake_checkCollision( &((SnakeNode*)DList_first(Snake))->collider, &((SnakeNode*)cur->value)->collider))
			printf("Collision with itself!!\n");
	}

error:
	return;
}

void Snake_render(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Settinig the color of the snake
	
	DList_FOREACH(Snake, first, next, cur){ // Drawing the whole body of the snake
		SDL_RenderFillRect(renderer, &((SnakeNode*)cur->value)->body);
	}
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
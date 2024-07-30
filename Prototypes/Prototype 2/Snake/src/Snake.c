#include <Snake.h>

// Our snake
DList* Snake = NULL;
const int SNAKE_WIDTH = 20;
const int SNAKE_HEIGHT = 20;
const int SNAKE_VEL = 5;

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
	
	node->velocity.x = 0;
	node->velocity.y = 0;
	
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
				
				((SnakeNode*)DList_first(Snake))->velocity.y -= SNAKE_HEIGHT; // Changing VEL to HEIGHT or WIDTH
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_UP; 
				Snake_move();
				break;
				
			case SDLK_DOWN:
				((SnakeNode*)DList_first(Snake))->velocity.y += SNAKE_HEIGHT;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_DOWN; 
				Snake_move();
				break;
			
			case SDLK_LEFT:
				((SnakeNode*)DList_first(Snake))->velocity.x -= SNAKE_WIDTH;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_LEFT; 
				Snake_move();
				break;
					
			case SDLK_RIGHT:
				((SnakeNode*)DList_first(Snake))->velocity.x += SNAKE_WIDTH;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_RIGHT; 
				Snake_move();
				break;	
			// make a case to generate the new node manually
			case SDLK_g:
				Snake_grow();
				break;
		}
		
	}
	
	
	// upon releasing the key
	if(e->type == SDL_KEYUP && e->key.repeat == 0){
		switch(e->key.keysym.sym){
			case SDLK_UP: 
				//((SnakeNode*)DList_first(Snake))->velocity.y += SNAKE_HEIGHT;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_UP; 				
				break;
				
			case SDLK_DOWN:
				//((SnakeNode*)DList_first(Snake))->velocity.y -= SNAKE_HEIGHT;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_DOWN; 
				break;
			
			case SDLK_LEFT:
				//((SnakeNode*)DList_first(Snake))->velocity.x += SNAKE_WIDTH;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_LEFT; 
				break;
					
			case SDLK_RIGHT:
				//((SnakeNode*)DList_first(Snake))->velocity.x -= SNAKE_WIDTH;
				((SnakeNode*)DList_first(Snake))->orientation = SNAKE_RIGHT; 
				break;
				
			// make a case to generate the new node manually
			// don't wanna do anything when the key is released
			case SDLK_g:
				break;
				
		}
	}
	
}

void Snake_move()
{
	// Approach 1 : Replace the next nodes with the position of the previous nodes
	
	// Updating both the positions of the head and the collider first
	/*
	((SnakeNode*)DList_first(Snake))->body.x +=((SnakeNode*)DList_first(Snake))->velocity.x;
	((SnakeNode*)DList_first(Snake))->collider.x += ((SnakeNode*)DList_first(Snake))->body.x;

	((SnakeNode*)DList_first(Snake))->body.y += ((SnakeNode*)DList_first(Snake))->velocity.y;
	((SnakeNode*)DList_first(Snake))->collider.y += ((SnakeNode*)DList_first(Snake))->body.y;
	*/
	
	// Then updating the position of the rest of the body
	/*
	DList_FOREACH(Snake, first->next, next, cur){
		((SnakeNode*)cur->value)->body.x += ((SnakeNode*)cur->prev)->body.x;
		((SnakeNode*)cur->value)->collider.x += ((SnakeNode*)cur->value)->body.x;
		
		((SnakeNode*)cur->value)->body.y += ((SnakeNode*)cur->prev)->velocity.y;
		((SnakeNode*)cur->value)->collider.y += ((SnakeNode*)cur->value)->body.y;
		
		// also update the orientation
		((SnakeNode*)cur->value)->orientation = ((SnakeNode*)cur->prev)->orientation;
	}
	*/
	
	/*
	int x = 0, y = 0;
	SnakeNode* new_node = NULL;
	
	
	// Approach 2 : Growing a head in the direction of the player input and removing the tail
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_UP ){
		printf("Moving up!\n");
		x = ((SnakeNode*)DList_first(Snake))->body.x;
		y = ((SnakeNode*)DList_first(Snake))->body.y - SNAKE_HEIGHT;
		
		// create the new node based upon the parameter
		new_node = SnakeNode_create(x, y, SNAKE_UP);
		check(new_node != NULL, "Failed to create the new node!");
		
		// now push this node in the list
		DList_unshift(Snake, new_node);
		// and remove the tail
		DList_pop(Snake);
	}
	
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_DOWN ){
		printf("Moving down!\n");
		x = ((SnakeNode*)DList_first(Snake))->body.x;
		y = ((SnakeNode*)DList_first(Snake))->body.y + SNAKE_HEIGHT;
		
		// create the new node based upon the parameter
		new_node = SnakeNode_create(x, y, SNAKE_DOWN);
		check(new_node != NULL, "Failed to create the new node!");
		
		// now push this node in the list
		DList_unshift(Snake, new_node);
		// and remove the tail
		DList_pop(Snake);
	}
	
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_LEFT ){
		printf("Moving left!\n");
		x = ((SnakeNode*)DList_first(Snake))->body.x - SNAKE_WIDTH;
		y = ((SnakeNode*)DList_first(Snake))->body.y;
		
		// create the new node based upon the parameter
		new_node = SnakeNode_create(x, y, SNAKE_LEFT);
		check(new_node != NULL, "Failed to create the new node!");
		
		// now push this node in the list
		DList_unshift(Snake, new_node);
		// and remove the tail
		DList_pop(Snake);
	}
	
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_RIGHT ){
		printf("Moving right!\n");
		x = ((SnakeNode*)DList_first(Snake))->body.x + SNAKE_HEIGHT;
		y = ((SnakeNode*)DList_first(Snake))->body.y;
		
		// create the new node based upon the parameter
		new_node = SnakeNode_create(x, y, SNAKE_RIGHT);
		check(new_node != NULL, "Failed to create the new node!");
		
		// now push this node in the list
		DList_unshift(Snake, new_node);
		// and remove the tail
		DList_pop(Snake);
	}
	
	*/
	
	// Approach 3 : why am I so fucking dumb!
	int x = 0, y = 0;
	Orientation o = SNAKE_NONE;
	SnakeNode* new_node = NULL;
	
	x = ((SnakeNode*)DList_first(Snake))->body.x + ((SnakeNode*)DList_first(Snake))->velocity.x;
	y = ((SnakeNode*)DList_first(Snake))->body.y + ((SnakeNode*)DList_first(Snake))->velocity.y;
	o = ((SnakeNode*)DList_first(Snake))->orientation;
	
	// create the new node based upon the parameter
	new_node = SnakeNode_create(x, y, SNAKE_RIGHT);
	check(new_node != NULL, "Failed to create the new node!");
	
	// now push this node in the list
	DList_unshift(Snake, new_node);
	// and remove the tail
	DList_pop(Snake);
	
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
	/*
	int x = 0, y = 0; // position of the new node
	Orientation o = SNAKE_NONE; // orientation of the new node;
	SnakeNode* new_node = NULL; // the new node itself
	
	// Approach 1 : Add the node at the end of the list
	
	
	// now we must place the end node at the last of the list
	// with dimensions as per the following conditons
	
	// if the last node is oriented up then the snake must grow towards the bottom
	// i.e the position of the the new node will be (x, y+h)
	if( ((SnakeNode*)DList_last(Snake))->orientation == SNAKE_UP ){
		x = ((SnakeNode*)DList_last(Snake))->body.x;
		y = ((SnakeNode*)DList_last(Snake))->body.y + SNAKE_HEIGHT;
		o = SNAKE_UP;
	}
	
	// if the last node is oriented down then the snake must grow towards the top
	// i.e. the position of the new node will be (x, y-h)
		if( ((SnakeNode*)DList_last(Snake))->orientation == SNAKE_DOWN ){
		x = ((SnakeNode*)DList_last(Snake))->body.x;
		y = ((SnakeNode*)DList_last(Snake))->body.y - SNAKE_HEIGHT;
		o = SNAKE_DOWN;
	}
	
	// if the last node is oriented to the left then the snake must grow towards the right
	// i.e. the position of the new node will be (x+w, y)
		if( ((SnakeNode*)DList_last(Snake))->orientation == SNAKE_LEFT ){
		x = ((SnakeNode*)DList_last(Snake))->body.x + SNAKE_WIDTH;
		y = ((SnakeNode*)DList_last(Snake))->body.y;
		o = SNAKE_LEFT;
	}
	
	// if the last node is oriented to the right then the snake must grow towards the left
	// i.e. the position of the new node will be (x-w, y)
		if( ((SnakeNode*)DList_last(Snake))->orientation == SNAKE_RIGHT ){
		x = ((SnakeNode*)DList_last(Snake))->body.x - SNAKE_WIDTH;
		y = ((SnakeNode*)DList_last(Snake))->body.y;
		o = SNAKE_RIGHT;
	}
	*/
	
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
		
		/*
		printf("%d, %d\n", ((SnakeNode*)DList_first(Snake))->body.x, ((SnakeNode*)DList_first(Snake))->body.y);
		printf("%d, %d\n", x,y);
		*/
		printf("Growing UP!\n");
		
	}
	
	// if the last node is oriented down then the snake must grow towards the down
	// i.e. the position of the new node will be (x, y-h)
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_DOWN ){
		x = ((SnakeNode*)DList_first(Snake))->body.x;
		y = ((SnakeNode*)DList_first(Snake))->body.y + SNAKE_HEIGHT;
		o = SNAKE_DOWN;
		printf("Growing down!\n");
	}
	
	// if the last node is oriented to the left then the snake must grow towards the left
	// i.e. the position of the new node will be (x+w, y)
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_LEFT ){
		x = ((SnakeNode*)DList_first(Snake))->body.x - SNAKE_WIDTH;
		y = ((SnakeNode*)DList_first(Snake))->body.y;
		o = SNAKE_LEFT;
		printf("Growing left!\n");
	}
	
	// if the last node is oriented to the right then the snake must grow towards the right
	// i.e. the position of the new node will be (x-w, y)
	if( ((SnakeNode*)DList_first(Snake))->orientation == SNAKE_RIGHT ){
		x = ((SnakeNode*)DList_first(Snake))->body.x + SNAKE_WIDTH;
		y = ((SnakeNode*)DList_first(Snake))->body.y;
		o = SNAKE_RIGHT;
		printf("Growing right!\n");
	}
	
	// create the new node based upon the parameter
	new_node = SnakeNode_create(x, y, o);
	check(new_node != NULL, "Failed to create the new node!");
	
	// now push this node in the list
	DList_unshift(Snake, new_node);
	
	
error: // fall through
	return;
}

#include <SDL2/SDL_mixer.h>
#include <Game.h>
#include <UI.h>
#include <Dot.h>
#include <Snake.h>
#include <Timer.h>

// Some globals
// Usual Global Variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

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

		// printf("Random no. %d\n", rand()); // Printing random no.s
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
	
	window = SDL_CreateWindow("Snake Prototype 2",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
	check(window != NULL, "Failed to create a window! SDL_Error: %s", SDL_GetError());
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	check(renderer != NULL, "Failed to create a renderer! SDL_Error: %s", SDL_GetError());
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	// Initializing the play dimension
	PLAY_WIDTH = SCREEN_WIDTH - (SNAKE_WIDTH * 2); // needs to be reduced on both the sides
	PLAY_HEIGHT = SCREEN_HEIGHT - 100 - (SNAKE_HEIGHT * 2); // needs to be reduced on both the sides
	
	// Initializing the blocks
	X_BLOCKS = PLAY_WIDTH / SNAKE_WIDTH;
	Y_BLOCKS = PLAY_HEIGHT / SNAKE_HEIGHT;
	
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
		
		// Handles the input
		Snake_handleEvents(&e);
		
	}	
}

void update()
{
	// Updates the snake's movement
	Snake_move(PLAY_WIDTH, PLAY_HEIGHT);
	
	// we will check collision with the dot and grow the snake
	if(Dot_checkCollision(dot, &((SnakeNode*)DList_first(Snake))->collider)){
		Snake_grow(); // Growing the snake when eating the food
		
		SDL_Point* food_position = generateRandomPoint(); // obtaining a new position for the food
		check(food_position != NULL, "Failed to create the food position!");
		
		dot->position = *food_position; // providing the new position to the food
		// also update its collider
		dot->collider.x = dot->position.x;
		dot->collider.y = dot->position.y;
	}
	
error: // fallthrough
	return;
}

void render()
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer); // Clears the current frame
	
	//  Lets render the tiles too
	Texture_render(renderer, &top_tile, 0, 0, NULL);
	Texture_render(renderer, &bottom_tile, 0, 480, NULL);
	Texture_render(renderer, &left_tile, 0, 0, NULL);
	Texture_render(renderer, &right_tile, 780, 0, NULL);
	
	// also display its food
	Dot_render(renderer, &dot_texture, dot);
	
	// lets render the snake (rendering the snake on top of the dot)
	Snake_render(renderer);
		
	SDL_RenderPresent(renderer); // Display the frame to the screen
	
	// implementing the capping log
	int frame_ticks = Timer_getTicks(cap_timer);
	if(frame_ticks < SCREEN_TICKS_PER_FRAME){
		SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_ticks);
	}
}

void close()
{	
	// deallocating the memory from the list
	DList_destroy(Snake);
	
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
	// lets first find out the no. of points in the screen 
	// or basically the size of boxes horizontally and vertically
	
	// Variables for storing the coordinates of the food
	int x = 0;
	int y = 0;
	bool point_found = false; // variable to check if a valid point is found
	
	/*
	// Approach 1 : Looping through points and checking if the newly generated point is one of the points in the snake's body	
	// Obtaining the position of the snake's body
	SDL_Point snake[DList_count(Snake)];

	// To copy the points of the snake
	int i = 0;
	DList_FOREACH(Snake, first, next, cur){
		snake[i].x = ((SnakeNode*)cur->value)->body.x;
		snake[i].y = ((SnakeNode*)cur->value)->body.y;		
		i++;
	}
	
	for(int j = 0;j < 5;j++){ // we will try to find the find atleast 5 times
		// Generating the position for the food
		x = (rand() % X_BLOCKS) * SNAKE_WIDTH;
		y = (rand() % Y_BLOCKS) * SNAKE_HEIGHT; 
		
		// now to check if the point is a point of the snake or not
		for(i = 0; i  < DList_count(Snake); i++){
			if(x != snake[i].x && y != snake[i].y)
				point_found = true;
		}
		
		if(point_found) break;
	}
	*/
	
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
			if(checkCollision(&collider, &bottom_tile_collider)) point_found = false;
			if(checkCollision(&collider, &left_tile_collider)) point_found = false;
			if(checkCollision(&collider, &right_tile_collider)) point_found = false;
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


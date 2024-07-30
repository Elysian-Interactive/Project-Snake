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

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

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
	
	// Initializing the blocks
	X_BLOCKS = SCREEN_WIDTH / SNAKE_WIDTH;
	Y_BLOCKS = SCREEN_HEIGHT / SNAKE_HEIGHT;
	
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
	
	// Creating the dot
	dot = Dot_create(220,200);
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
	Snake_move(SCREEN_WIDTH, SCREEN_HEIGHT);
	
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
	
	// Generating the position for the food
	x = (rand() % X_BLOCKS) * SNAKE_WIDTH;
	y = (rand() % Y_BLOCKS) * SNAKE_HEIGHT; 
	
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


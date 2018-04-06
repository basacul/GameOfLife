#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

enum {
	SCREEN_WIDTH = 640,
	SCREEN_HEIGHT = 480,
};

struct argb8888 {
	unsigned char blue, green, red, alpha;
};

//used to initialize grid
void initialize(struct argb8888 *grid,unsigned char table[SCREEN_HEIGHT][SCREEN_WIDTH]);

//used to update grid applying Conway's fundamental rules
void evolve(struct argb8888 *grid, unsigned char table[SCREEN_HEIGHT][SCREEN_WIDTH]);

extern int
main(void)
{
	SDL_Window *win;
	SDL_Renderer *rend;
	SDL_Event event;
	SDL_Texture *text;
	struct argb8888 *pixels;
	int result, quit = 0;

	result = SDL_Init(SDL_INIT_VIDEO);
	if (result != 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return (EXIT_FAILURE);
	}

	win = SDL_CreateWindow("Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	    SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (win == NULL) {
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		return (EXIT_FAILURE);
	}

	rend = SDL_CreateRenderer(win, -1, 0);
	if (rend == NULL) {
		fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
		return (EXIT_FAILURE);
	}

	text = SDL_CreateTexture(rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
	    SCREEN_WIDTH, SCREEN_HEIGHT);
	if (text == NULL) {
		fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
		return (EXIT_FAILURE);
	}

	pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof *pixels);
	if (pixels == NULL) {
		perror("malloc");
		return (EXIT_FAILURE);
	}

	memset(pixels, 0xff, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof *pixels);
	SDL_UpdateTexture(text, NULL, pixels, SCREEN_WIDTH * sizeof *pixels);
	SDL_RenderCopy(rend, text, NULL, NULL);
	SDL_RenderPresent(rend);

	srand(time(NULL));
	int alreadyInitialized = 0;
	unsigned char matrix[SCREEN_HEIGHT][SCREEN_WIDTH];

	while (!quit) {
		SDL_WaitEvent(&event);
		//sleep(1);
		switch (event.type) {
		case SDL_KEYDOWN:
			fprintf(stderr, "key: %s\n",SDL_GetKeyName(event.key.keysym.sym));

			//initialize with black background and some nice red spots
			if(!alreadyInitialized){
					initialize(pixels, matrix);
					alreadyInitialized = 1;
			}else{
					evolve(pixels, matrix);
			}


			SDL_UpdateTexture(text, NULL, pixels, SCREEN_WIDTH * sizeof *pixels);
			SDL_RenderCopy(rend, text, NULL, NULL);
			SDL_RenderPresent(rend);
			break;

    case SDL_QUIT:
      quit = 1;
      break;

		default:
			fprintf(stderr, "Unhandled event %d\n", (int)event.type);
		}
	}

	SDL_DestroyWindow(win);
	SDL_Quit();
	return (EXIT_SUCCESS);

}


void initialize(struct argb8888 *grid, unsigned char table[SCREEN_HEIGHT][SCREEN_WIDTH]){
	//initialize with black background and some nice red spots
	for (int i = 0; i < SCREEN_HEIGHT; i++)
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			grid[i * SCREEN_WIDTH + j].alpha = 0;

			//provides a nice pictures and good randomization
			if(rand()%2 < 1){
				table[i][j] = 1;
				grid[i * SCREEN_WIDTH + j].red = 255;
			}else{
				table[i][j] = 0;
				grid[i * SCREEN_WIDTH + j].red = 0;
			}

			grid[i * SCREEN_WIDTH + j].green = 0;
			grid[i * SCREEN_WIDTH + j].blue = 0;
		}


}


/*update the the grid following the simple fundamental rules
1) Any live cell with fewer than two live neighbors dies, as if caused by underpopulation.
2) Any live cell with more than three live neighbors dies, as if by overcrowding.
3) Any live cell with two or three live neighbors lives on to the next generation.
4) Any dead cell with exactly three live neighbors becomes a live cell.
with the help of
https://rosettacode.org/wiki/Conway%27s_Game_of_Life
*/
void evolve(struct argb8888 *grid, unsigned char table[SCREEN_HEIGHT][SCREEN_WIDTH]){

	unsigned char temp[SCREEN_HEIGHT][SCREEN_WIDTH];

	//for checking the conditions
	//unsigned char value = 0;

	for (int i = 0; i < SCREEN_HEIGHT; i++){
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			//
			int neighbors = 0;

			//looking at a 3x3 grid checking for conditions
			for(int i_test = i - 1; i_test <= i + 1; i_test++){
				for(int j_test = j - 1; j_test <= j + 1; j_test++){

					//if value is positive then yes and update neighbors
					if(table[(i_test + SCREEN_HEIGHT) % SCREEN_HEIGHT][(j_test + SCREEN_WIDTH) % SCREEN_WIDTH]){
						neighbors++;
					}
				}
			}

			//update for correct calculation and application of rules
			if(table[i][j]){
				neighbors--;
			}

			//new life is created or extinguished by the rules
			temp[i][j] = (neighbors == 3 || (neighbors == 2 && table[i][j]));
		}
	}

	for(int i = 0; i < SCREEN_HEIGHT; i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			//update matrix
			table[i][j] = temp[i][j];

			//either 255 or 0
			grid[i * SCREEN_WIDTH + j].red = 255 * table[i][j];
		}
	}
}

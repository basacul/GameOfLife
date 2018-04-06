// TO COMPILE application:
//	gcc -std=c99 -Wall -Wextra -pedantic -I${HOME}/.local/include main.c -o main -lSDL2 -lpng -L${HOME}/.local/lib

// TO run application:
// 	env LD_LIBRARY_PATH=${HOME}/.local/lib ./main

#include <SDL2/SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <png.h>

enum {
	SCREEN_WIDTH = 640,
	SCREEN_HEIGHT = 480,
};

struct argb8888 {
	unsigned char blue, green, red, alpha;
};

//used to initialize grid
void initialize(struct argb8888 *grid);

//used to update grid applying Conway's fundamental rules
void evolve(struct argb8888 *grid);

//for the intro and start as I used png files which I want to read
//and update the pixels for the appropriate purposes
void read_png_file(struct argb8888 *grid, char *filename);

extern int
main(void)
{
	//line 38 till 78 is boiler plate code from tutorial FU Berlin
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

	//title png displayed on screen
	read_png_file(pixels, "img/start.png");

	SDL_UpdateTexture(text, NULL, pixels, SCREEN_WIDTH * sizeof *pixels);
	SDL_RenderCopy(rend, text, NULL, NULL);
	SDL_RenderPresent(rend);

	//after start.png is shown intro will be shown and changed when any key is
	//pressed when start will be updated to zero
	int start = 1;

	//sleep for 4 seconds
	sleep(4);

	//probably mouse movement is also an event why I choose this logical approach
	SDL_WaitEvent(&event);

	while(start){
		if(SDL_KEYDOWN){
				read_png_file(pixels, "img/intro.png");
				SDL_UpdateTexture(text, NULL, pixels, SCREEN_WIDTH * sizeof *pixels);
				SDL_RenderCopy(rend, text, NULL, NULL);
				SDL_RenderPresent(rend);
				sleep(3);
				start = 0;
		}
		SDL_WaitEvent(&event);
	}



	srand(time(NULL));
	//boolean variable in order to initialize grid only once
	int alreadyInitialized = 0;
	//a loop of of 300 cycles will be run each time and count is used as a counter
	int count = 0;

	while (!quit) {

		SDL_WaitEvent(&event);

		switch (event.type) {
			case SDL_KEYDOWN:
				do{
					if(!alreadyInitialized){
							//initialize with black background and some nice red spots and let it run for 500 turns
							initialize(pixels);
							alreadyInitialized = 1;
					}else{
						//or let the grid evolve by the Conway's rules
							evolve(pixels);
					}

					count++; //counter used for 300 cycles

					SDL_UpdateTexture(text, NULL, pixels, SCREEN_WIDTH * sizeof *pixels);
					SDL_RenderCopy(rend, text, NULL, NULL);
					SDL_RenderPresent(rend);
				}while(count < 300);

				count = 0; //reinitialize counter back to zero for next round
				break;

	    case SDL_QUIT:
	      quit = 1;
	      break;

			default:
				//DO NOTHING
				break;
				//fprintf(stderr, "Unhandled event %d\n", (int)event.type);
		}
	}

	SDL_DestroyWindow(win);
	SDL_Quit();
	return (EXIT_SUCCESS);

}


void initialize(struct argb8888 *grid){
	//initialize with black background and some nice red spots
	for (int i = 0; i < SCREEN_HEIGHT; i++)
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			grid[i * SCREEN_WIDTH + j].alpha = 0;

			//the game goes on with this value ranging between module 2 and 23
			if(rand()%3 < 1){
				grid[i * SCREEN_WIDTH + j].red = 255;
			}else{
				grid[i * SCREEN_WIDTH + j].red = 0;
			}

			grid[i * SCREEN_WIDTH + j].green = 0;
			grid[i * SCREEN_WIDTH + j].blue = 0;
		}
}


/*update the the grid following the 4 simple fundamental rules described by Conway
1) Any live cell with fewer than two live neighbors dies, as if caused by underpopulation.
2) Any live cell with more than three live neighbors dies, as if by overcrowding.
3) Any live cell with two or three live neighbors lives on to the next generation.
4) Any dead cell with exactly three live neighbors becomes a live cell.
*/
void evolve(struct argb8888 *grid){

	//holding aka boolean values for life or death in each pixel
	unsigned char temp[SCREEN_HEIGHT][SCREEN_WIDTH];

	int neighbors = 0;

	for (int i = 0; i < SCREEN_HEIGHT; i++){
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			//reinitialize to zero for each outer iteration of the 3x3 grid testing
			neighbors = 0;

			//looking at a 3x3 grid checking for conditions
			for(int i_test = i - 1; i_test <= i + 1; i_test++){
				for(int j_test = j - 1; j_test <= j + 1; j_test++){

					//if value is positive then yes and update neighbors
					if(grid[((i_test + SCREEN_HEIGHT)%SCREEN_HEIGHT)*SCREEN_WIDTH + ((j_test + SCREEN_WIDTH) % SCREEN_WIDTH)].red){
						neighbors++;
					}
				}
			}

			//update for correct calculation and application of rules, otherwise itself is counted unnecessarily
			if(grid[i*SCREEN_WIDTH + j].red){
				neighbors--;
			}

			//new life is created or extinguished by the rules and information stored in the temporal matrix
			temp[i][j] = (neighbors == 3 || (neighbors == 2 && grid[i*SCREEN_WIDTH + j].red));
		}
	}

	//finally update grid for the new picture
	for(int i = 0; i < SCREEN_HEIGHT; i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			//either 255 or 0
			grid[i * SCREEN_WIDTH + j].red = 255 * temp[i][j]; //temp is either 0 or 1
		}
	}
}

//main concept from https://gist.github.com/niw/5963798 and mostly unchanged except for some lines
void read_png_file(struct argb8888 *grid, char *filename) {

	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;

  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png){
		printf("Unfortunately the file to be read is not a png file.");
		exit(EXIT_FAILURE);
	}


  png_infop info = png_create_info_struct(png);
  if(!info){
		printf("Unfortunately the png file provides no info.");
		exit(EXIT_FAILURE);
	}

  if(setjmp(png_jmpbuf(png))){
		printf("Unfortunately the png file cannot be buffered.");
		exit(EXIT_FAILURE);
	}

	png_init_io(png, fp);

  png_read_info(png, info);

	color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);


  if(color_type == PNG_COLOR_TYPE_PALETTE){
		  png_set_palette_to_rgb(png);
	}


  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8){
    png_set_expand_gray_1_2_4_to_8(png);
	}

  if(png_get_valid(png, info, PNG_INFO_tRNS)){
		png_set_tRNS_to_alpha(png);
	}


  // These color_type don't have an alpha channel then fill it with 0xff.
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE){
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	}

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
    png_set_gray_to_rgb(png);
	}

  png_read_update_info(png, info);


  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * SCREEN_HEIGHT);

	for(int y = 0; y < SCREEN_HEIGHT; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);


	for(int y = 0; y < SCREEN_HEIGHT; y++) {
    png_bytep row = row_pointers[y];
    for(int x = 0; x < SCREEN_WIDTH; x++) {
			//get info from each pixel in png file and update the values appropriately
      png_bytep px = &(row[x * 4]);
			grid[y * SCREEN_WIDTH + x].alpha = 0;

			//the game goes on with this value ranging between module 2 and 23
			grid[y * SCREEN_WIDTH + x].red = px[0];
			grid[y * SCREEN_WIDTH + x].green = 0;
			grid[y * SCREEN_WIDTH + x].blue = 0;
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
    }
	}

	//i guess that everything will be freed this way stored in row_pointers
	free(row_pointers);
	fclose(fp);
}

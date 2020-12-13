#include <SDL2/SDL.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ocr_gui.h"
#include "perceptron-one-hidden-layer.h"

static char TextResult[] = "";


static inline
Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
    int bpp = surf->format->BytesPerPixel;
    return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}


void put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
    Uint8 *p = pixel_ref(surface, x, y);

    switch(surface->format->BytesPerPixel)
    {
        case 1:
            *p = pixel;
            break;

        case 2:
            *(Uint16 *)p = pixel;
            break;

        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            }
            else
            {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4:
            *(Uint32 *)p = pixel;
            break;
    }
}


void filter_image(Uint8 *pixels, int width, int height, SDL_Surface *surface)
{
	int x;
	int y;
	int gray_max = 0, gray_min = 255;
	int threshold;
	Uint32 pixel;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x ++) {
			if (pixels[x + y * width] < gray_min)
				gray_min = pixels[x + y * width];
			if (pixels[x + y * width] > gray_max)
				gray_max = pixels[x + y * width];
		}
	}

	threshold = (gray_max * 2 + gray_min) / 3;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x ++) {
			if (pixels[x + y * width] < threshold){
				pixels[x + y * width] = 255;
				pixel = SDL_MapRGB(surface->format, 0, 0, 0);
			}
			else {
				pixels[x + y * width] = 0;
				pixel = SDL_MapRGB(surface->format, 255, 255, 255);
			}
			put_pixel(surface, x, y, pixel);
		}
	}
}

void drawLines(SDL_Surface *surface, Uint8 *pixels, int x_start, int y_start, int x_end, int y_end, int Find){
	Uint32 pixel;
	if (Find)
		pixel = SDL_MapRGB(surface->format, 50, 255, 50);
	else
		pixel = SDL_MapRGB(surface->format, 50, 50, 255);

	for (int x = x_start; x < x_end; x++) {
		put_pixel(surface, x, y_start, pixel);
		put_pixel(surface, x, y_end, pixel);
	}
	for (int y = y_start; y < y_end; y++) {
		put_pixel(surface, x_start, y, pixel);
		put_pixel(surface, x_end, y, pixel);
	}
}



int empty_line(Uint8 *pixels, int width, int x0, int x1, int y)
{
	for (int x = x0; x < x1; x++) {
		if (pixels[x + y * width] > 0){
			return 0;
		}
	}
	return 1;
}



int empty_column(Uint8 *pixels, int width, int y0, int y1, int x)
{
	for (int y = y0; y < y1; y++) {
		if (pixels[x + y* width] > 0) {
			return 0;
		}
	}
	return 1;
}


#define HORIZONTAL_CROP  0
#define VERTICAL_CROP    1


void ocr(Uint8 *pixels, int width, int x0, int y0, int x1, int y1)
{
	int x, y;
	int precision = 32;
	float precision_b = 32.0;

	double letter[precision][precision];

	float wpas =  (x1-x0)/precision_b ;
	float hpas =  (y1-y0)/precision_b ;
	//printf("wpas :%f,  hpas :%f\n", wpas, hpas);
	//printf("x1, x0 :%d, %d,  y1, y0 :%d, %d\n", x1, x0, y1, y0);
	if (wpas <= 0 || hpas <=0){
		printf("[OCR] - Error image size can't run pre-treatement\n");
		return ;
	}

	for (int i = 0; i < precision; i ++)
		printf("-");
	printf("-\n");

	for (y = 0; y < precision; y ++) {
		printf("|");
		for (x = 0; x < precision; x++) {
			if (pixels[ (int) round( (x*wpas)+x0) + (int) round((y*hpas)+y0 )* width ] > 0){
				printf("1");
				letter[y][x] = 1.0;
			}
			else{
				printf("0");
				letter[y][x] = 0.0;
			}
		}
		printf("|\n");
	}
	for (int i = 0; i < precision; i ++)
		printf("-");
	printf("-\n");



	// HERE CALL YOU RECOGNIZE FUNCTION BY USING THE .H
	// "letter" is the double matrice
	// size of the matrice is "precision"
	//
	// exemple : 
	// recognize(letter, size)
	int result = NeuralOCR(letter, -1, 0, 0);
	if (result >= 0) {
		if (result <= 25)
			result += 'a';
		else
			result -= 26 + 'A';
		printf("Found: %2d (%c), ", result, result );
	}
	char ch = result;
	printf("CHAR : %c\n", ch);
	strncat(TextResult, &ch, 1);
}



void crop_image(SDL_Surface *surface, Uint8 *pixels, int width, int height, int x_start, int y_start, int x_end, int y_end, int direction)
{
	int x0 = x_start;
	int x1 = x_end;
	int y0 = y_start;
	int y1 = y_end;

	drawLines(surface, pixels, x_start, y_start, x_end, y_end, 0);

//	fprintf(stderr, "(%d, %d) - (%d, %d) --> %s\n", x_start, y_start, x_end, y_end, direction ? "VERTICAL" : "HORIZONTAL");

	if (direction == VERTICAL_CROP) {

		while (y_start < y_end) {

			while ((y0 < y_end) && empty_line(pixels, width, x_start, x_end, y0)) {
				y0 ++;
			}
			if (y0 == y_end)
				return;

			y1 = y0;
			while ((y1 < y_end) && !empty_line(pixels, width, x_start, x_end, y1)) {
				y1 ++;
			}
			if (y1 == y_end) {
				if (x_end - x_start >= 32 &&  y1-y0 >= 32){
					drawLines(surface, pixels, x_start, y_start, x_end, y_end, 1);
					printf("Dx = %i, Dy = %i\n", x_end - x_start, y_end-y_start);
					ocr(pixels, width, x_start, y0, x_end, y1);
					printf("\n\n");
				}
				return;
			}


			crop_image(surface, pixels, width, height, x_start, y0, x_end, y1, HORIZONTAL_CROP);
			y0 = y1;
		}
	}

	if (direction == HORIZONTAL_CROP) {
		while (x_start < x_end) {

			while ((x0 < x_end) && empty_column(pixels, width, y_start, y_end, x0)) {
				x0 ++;
			}
			if (x0 == x_end)
				return;

			x1 = x0;
			while ((x1 < x_end) && !empty_column(pixels, width, y_start, y_end, x1)) {
				x1 ++;
			}
			if (x1 == x_end) {
				if (x1 - x0 >= 32 &&  y_end-y_start >= 32){
					drawLines(surface, pixels, x_start, y_start, x_end, y_end, 1);
					printf("[OCR] - Find letter\n");
					printf("Dx = %i, Dy = %i\n", x_end - x_start, y_end-y_start);
					ocr(pixels, width, x0, y_start, x1, y_end);
					printf("\n\n");
				}
				return;
			}

			crop_image(surface, pixels, width, height, x0, y_start, x1, y_end, VERTICAL_CROP);
			x0 = x1;
		}
	}
}



int process(const char* path, int value)
{
	SDL_Surface *surface = NULL;
	Uint8 red;
	Uint8 green;
	Uint8 blue;
	Uint8 *pixels;

	Uint32 pixel;

	/*if (argc != 2) {
		fprintf(stderr, "usage: %s <graphic-filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}*/

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "[OCR] - Erreur SDL_Init : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	surface = SDL_LoadBMP(path);
	if(surface == NULL) {
		fprintf(stderr, "[OCR] - Unable to load image '%s': %s\n", path, SDL_GetError());
		exit(EXIT_FAILURE);
	}


	printf("[OCR] - [Image informations] Width: %d, height: %d, row length in bytes: %d\n", surface->w, surface->h, surface->pitch);

	pixels = malloc(surface->h*surface->w);
	if (pixels == NULL) {
		fprintf(stderr, "[OCR] - Not enough memory to allocate %d x %d bytes\n", surface->h, surface->w);
		exit(EXIT_FAILURE);
	}

//	SDL_LockSurface(surface);

	for (int row = 0; row < surface->h; row ++) {
		for (int col = 0; col < surface->w; col ++) {
			SDL_GetRGB(getpixel(surface, col, row),
			           surface->format, &red, &green, &blue);
			pixels[row * surface->w + col] = (red + green + blue) / 3;

			pixel = SDL_MapRGB(surface->format, (red + green + blue) / 3, (red + green + blue) / 3, (red + green + blue) / 3);
			put_pixel(surface, col, row, pixel);
		}
	}

//	SDL_UnlockSurface(surface);

	if (value > 0)
		filter_image(pixels, surface->w, surface->h, surface);

	if (value > 1 )
		crop_image(surface, pixels, surface->w, surface->h, 0, 0, surface->w - 1, surface->h - 1, VERTICAL_CROP);

	SDL_SaveBMP(surface, path);

	free(pixels);

	SDL_Quit();

	//TextResult = "HELLOWORLD";
	printf("recognize text = %s\n", "HELLOWORLD");
	
	return EXIT_SUCCESS;
}


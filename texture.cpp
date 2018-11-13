#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <JMAP.h>


//draws side x side square with {x,y} as lower-left
void drawSquare(const bmp_t img, const int x, const int y, const int side, const HSV_t color){
	for(int j = 0; j < side; j++){
		for(int i = 0; i < side; i++){
				bmp_set_pixRGB(img, x+i, y+j, bmp_HSV_to_RGB(color));
		}
	}
}



void drawLight(const bmp_t img, const int x, const int y, const HSV_t color, const int step){
	drawSquare(img, x + step, y + step, step, color);
}

void drawMid(const bmp_t img, const int x, const int y, const HSV_t color, const int step){
	drawSquare(img, x + step, y, step, color);
	drawSquare(img, x, y + step, step, color);
	drawSquare(img, x + 2 * step, y + step, step, color);
	drawSquare(img, x + step, y + 2 * step, step, color);
}

void drawMid2(const bmp_t img, const int x, const int y, const HSV_t color, const int step){
	drawSquare(img, x, y, step, color);
	drawSquare(img, x +  2 * step, y, step, color);
	drawSquare(img, x, y + 2 * step, step, color);
	drawSquare(img, x + 2 * step, y + 2 * step, step, color);
}


void drawDark(const bmp_t img, const int x, const int y, const HSV_t color, const int step){
	drawSquare(img, x, y, step, color);
	drawSquare(img, x + 2 * step, y, step, color);
	drawSquare(img, x + step, y + step, step, color);
	drawSquare(img, x, y + 2 * step, step, color);
	drawSquare(img, x + 2 * step, y + 2 * step, step, color);
}

void drawDarker(const bmp_t img, const int x, const int y, const HSV_t color, const int step){
	drawSquare(img, x, y, step, color);
	drawSquare(img, x + step, y, step, color);
	drawSquare(img, x + 2 * step, y, step, color);
	drawSquare(img, x, y + step, step, color);
	drawSquare(img, x + 2 * step, y + step, step, color);
	drawSquare(img, x, y + 2 * step, step, color);
	drawSquare(img, x + step, y + 2 * step, step, color);
	drawSquare(img, x + 2 * step, y + 2 * step, step, color);
}

int main(int argc, char **argv){


	if(argc < 5){
		printf("ERROR: usage is './texture.x [width] [height] [grid size] [filename]'\n");
		return 1;
	}

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int interval = atoi(argv[3]);
	char *file = argv[4];

	int step = interval / 3;

	bmp_random_init();

	bmp_t img = bmp_new(file, width, height);


	for(int x = 0; x < width; x+=interval){
		for(int y = 0; y < height; y+=interval){
//			int roll = (int)bmp_rand_normal(2,1);

			float val = (sin(cos(y * x * .002))) * 5;

			int ival = (int)val % 6;

			switch(ival){
				case 0:
					drawLight(img, x, y, BLACKHSV, step);
					break;
				case 1:
					drawMid(img, x, y, BLACKHSV, step);
					break;
				case 2:
					drawMid2(img, x, y, BLACKHSV, step);
					break;
				case 3:
					drawDark(img, x, y, BLACKHSV, step);
					break;
				case 4: 
					drawDarker(img, x, y, BLACKHSV, step);
					break;
				default:
					break;
			}
		}
	}



	bmp_write_out(img);
	bmp_close(img);


	return 0;

}

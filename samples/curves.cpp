#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <JMAP.h>


#define PERIOD 100
#define AMPLITUDE 100
#define PASSES 2
#define COLORTHRESHOLD 64

int createLines(const int img1, const int img2){

	int x, y, p, newy;
	int width = bmp_get_width(img1);
	int height = bmp_get_height(img1);
	float period = (width/(3.14 * PERIOD));
	RGB_t rgb;

	for(p = 1; p < PASSES+1; p++){
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				bmp_get_pixRGB(img1, x, y, &rgb);
				if(rgb.R == WHITERGB.R && rgb.G == WHITERGB.G && rgb.B == WHITERGB.B)
					continue;
				int sinx = (int)((p*AMPLITUDE*sin(x/period)));
				newy = y + sinx;
				if(newy >= height){
					newy = newy - height;
				}
				if(newy < 0){
					newy = height + newy;
				}
				bmp_set_pixRGB(img2, x, newy, rgb);
			}
		}
	}
	return 0;
}

void other(const int img1, const int img2){
	RGB_t rgb;
	RGB_t rgb2;
	int width = bmp_get_width(img1);
	int height = bmp_get_height(img1);
	
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			bmp_get_pixRGB(img1, x, y, &rgb2);
			rgb = rgb2;
			float b = ((float)((float)rgb.R + (float)rgb.G + (float)rgb.B)/128.0f)/255.0f;
			//printf("b = %f, x = %d, y = %d\n", b, b*width, b*height);
			bmp_set_pixRGB(img2, x + b*width, y - b*height, rgb);
		}
	
	}

}

int main(int argc, char **argv){

	if(argc != 3){
		printf("ERROR: Invalid input: correct usage is : './curves.x [input image] [output image]'\n");
		return 1;
	}

	int img1 = bmp_open(argv[1], 'r');
	int img2 = bmp_new(argv[2], bmp_get_width(img1), bmp_get_height(img1));

	createLines(img1, img2);


	bmp_write_out(img2);

	bmp_close(img1);
	bmp_close(img2);

	return 0;

}

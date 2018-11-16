#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include <JMAP.h>
//#include "../lib/Polygon.h"



int main(int argc, char **argv){

	if(argc != 3){
		printf("ERROR: usage: './shapeTest.x [original file] [twotone file]'\n");
		return 1;
	}


	char *fileIn = argv[1];
	char *fileTT = argv[2];

	bmp_t orig = bmp_open(fileIn, 'r');
	bmp_t img1 = bmp_open(fileTT, 'r');
	int width = bmp_get_width(img1);
	int height = bmp_get_height(img1);
	bmp_random_init();
	bmp_t img2 = bmp_new("./Images/shapeTest.bmp", width, height);
	bmp_t mask = bmp_new("./Images/mask.bmp", width, height);

	bmp_t blue = bmp_new("./Images/blue.bmp", width, height);
	bmp_t green = bmp_new("./Images/green.bmp", width, height);
	bmp_t blue2 = bmp_new("./Images/blue2.bmp", width, height);
	bmp_t green2 = bmp_new("./Images/green2.bmp", width, height);
//	palette_t pal = bmp_generate_palette(5, paletteType::analogous);


	polygon poly = polygon();

	poly.addVertex({(float)bmp_random(width), (float)bmp_random(height)}, 0);
	poly.addVertex({(float)bmp_random(width), (float)bmp_random(height)}, 0);
	poly.addVertex({(float)bmp_random(width), (float)bmp_random(height)}, 0);
	poly.addVertex({(float)bmp_random(width), (float)bmp_random(height)}, 0);

	poly.buffUpdate();
	poly.fill();

	poly.draw(mask, 600, 1800);
	poly.draw(mask, 1750, 1800);

	for(int i = 0; i < 20; i++){
		float s = bmp_rand_normal(1, .1);
		poly.scale(s);
		poly.rotate(((float)bmp_random(100)/100.0f) * 2 * M_PI);
		poly.fill();
		poly.draw(mask, bmp_rand_normal(width/2, width/8), bmp_rand_normal(height/2, height/8));
		poly.scale(1/s);
	}

	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++){
			RGB_t colorBuff;
			bmp_get_pixRGB(orig, i, j, &colorBuff);
			HSV_t col = bmp_RGB_to_HSV(colorBuff);

			if(col.v < .5){
				bmp_set_pixRGB(green, i, j, BLACKRGB);
			}

			else{
				bmp_set_pixRGB(blue, i, j, BLACKRGB);
			}
		}
	}

	bmp_mask(img1, img2, mask);
	bmp_mask(green, green2, mask);
	bmp_mask(blue, blue2, mask);

	bmp_write_out(img2);
	bmp_write_out(green2);
	bmp_write_out(blue2);
	bmp_close(img2);
	bmp_close(green2);
	bmp_close(green);
	bmp_close(blue2);
	bmp_close(blue);
	bmp_close(img1);
	bmp_close(mask);
	remove("./Images/mask.bmp");
	remove("./Images/green.bmp");
	remove("./Images/blue.bmp");



	return 0;

}

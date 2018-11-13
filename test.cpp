#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <JMAP.h>

int draw_circle(const int x, const int y, const int r, const int bmpNum, RGB_t rgb){

	int xx;
	int yy;


	for(xx = -r; xx <=r; xx++){
		for(yy = -r; yy <=r; yy++){
			if(yy*yy+xx*xx <= r*r){
				bmp_set_pixRGB(bmpNum, x+xx, y+yy, rgb);
			}
		}
	}

	return 0;
}

int main(int argc, char **argv){


	srand(time(0));

	
	int image2 = bmp_new("Test2.bmp", 1440, 2560);
	if(image2 == -1)
		return 1;

	RGB_t rgb;
	rgb.R = rand()%255;
	rgb.G = rand()%255;
	rgb.B = rand()%255;

	int x, y, pass;
	int r;
	for(pass = 0; pass < 100; pass++){
		y = rand()%bmp_get_height(image2);
		r = rand()%100;
		rgb.R = rgb.R + rand()%100 - 50;
		for(x = 0; x < bmp_get_width(image2); x+=3){
			draw_circle(x, y, r%200, image2, rgb);
			r = r +  rand()%3 - 1;
		}
	}
	int image3 = bmp_copy("doop.bmp", image2);

	if(image3 == -1){
		return 1;
	}

	bmp_print_info(image2);
	bmp_write_out(image3);
	bmp_write_out(image2);

	bmp_close(image2);
	bmp_close(image3);



	return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <JMAP.h>




int main(int argc, char **argv){

	if(argc != 3)
		return 1;
	srand(time(0));

	bmp_load_brushes();
	brush_t brush = bmp_get_brush(3);
	int width = 1780;
	int height = 1080;

	int img1 = bmp_new(argv[1], width, height);
	int passes = atoi(argv[2]);


	v2i v1;
	v2i v2;
	HSV_t hsv = {180, .8, .9};
	RGB_t rgb = bmp_HSV_to_RGB(hsv);


	int i;
	for(i = 0; i < passes; i++){
		int ybase = rand()%height;
		v1.y = ybase;
		for(v1.x = 0; v1.x < width;){
			int spike = 1 - rand()%3;
			int spikeSize = rand()%200;
			v2.x = v1.x + (rand()%11) + 2;
			v2.y = v1.y + spike*spikeSize;
			bmp_draw_brush_line(img1, v1, v2, rgb, brush);
			v1 = v2;
			v2.x += (rand()%11)+2;
			v2.y = ybase;
			bmp_draw_brush_line(img1, v1, v2, rgb, brush);
			v1 = v2;
		}
		hsv.h += 180;
		hsv.s += .1 -  rand()%3 / 10.0f;
		if(hsv.s < 0)
			hsv.s += 1;
		if(hsv.s > 1)
			hsv.s -= 1;
		hsv.v += .1 - rand()%3 / 10.0f;
		if(hsv.v < 0)
			hsv.v += 1;
		if(hsv.v > 1)
			hsv.v -= 1;
		//hsv.s = (float)(rand()%100) / 100.0f;
		//hsv.v = (float)(rand()%100) / 100.0f;
		//printf("New hsv = %f, %f, %f\n", hsv.h, hsv.s, hsv.v);
		rgb = bmp_HSV_to_RGB(hsv);
		//printf("New rgb = %d, %d, %d\n", rgb.R, rgb.G, rgb.B);
	}


	bmp_write_out(img1);
	bmp_close(img1);
	bmp_close_brushes();

}

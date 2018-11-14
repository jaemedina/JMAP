#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <JMAP.h>




int main(int argc, char **argv){


	if(argc != 3){
		printf("ERROR: INPUT: 2 arguments needed. './dots.x [source] [destination]'\n");
		return 1;
	}


	srand(time(0));	
	bmp_t img1 = bmp_open(argv[1], 'r');
	bmp_t img2 = bmp_new(argv[2], bmp_get_width(img1), bmp_get_height(img1));
	if(bmp_load_brushes()==-1){
		bmp_close(img1);
		bmp_close(img2);
	}
	RGB_t avg;

	int rx = rand()%bmp_get_width(img2);
	int ry = rand()%bmp_get_height(img2);

	bmp_get_pixRGB(img1, rx, ry, &avg);

	bmp_fill(img2, avg);

	int x, y;
	int height = bmp_get_height(img1);
	int width = bmp_get_width(img1);
	int r = 15;
	int yvar;
	brush_t brush;
	for(y = 0; y < height; y+=r){
		for(x = 0; x < width; x+=r){
			yvar = 10-(rand()%21);
			avg = bmp_get_avg_RGB(img1, x, y, 2);//get avg rgb for given x and y
			brush = bmp_get_brush(rand()%BRUSH_COUNT);
			bmp_brush_stroke(x, y+yvar, img2, avg, brush);
			//draw_circle(x, y+yvar, r, img2, avg);//draw circle with avg value found at varied y value
			//r = rand()%((height+width)/100);//avg of height and width/100 to determine circle sizes.
			r = rand()%(bmp_get_width(brush)/2);
		}

	}

	bmp_write_out(img2);

	bmp_close(img1);
	bmp_close(img2);
	bmp_close_brushes();

	return 0;


}


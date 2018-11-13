#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stack>
#include <JMAP.h>


class Circle{

	public:
	float x;
	float y;
	int r;
	HSV_t hsv;
	int dir;

	Circle(){
		x = 0;
		y = 0;
		hsv = {0,0,0};
		dir = bmp_random(-1,1);
	}	

	Circle(float x_, float y_, int r_, HSV_t hsv_){
		x = x_;
		y = y_;
		r = r_;
		hsv = hsv_;
		dir = bmp_random(-1,1);
	}

	void draw(bmp_t bmp){
		bmp_draw_circle(x, y, r, bmp, bmp_HSV_to_RGB(hsv));
	}

	void setHue(float h_){
		hsv.h = (int)h_%360;
	}
};


std::stack<Circle> circles;


int main(int argc, char **argv){


	if(argc != 4){
		printf("ERROR: Usage: proper usage is './circles.x [width] [height] [number of beginning drips]'");
		return 1;
	}

	bmp_random_init();

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int drips = atoi(argv[3]);

	bmp_t img = bmp_new("./CirclesGen/Drip.bmp", width, height);
	HSV_t hsv = palette3[bmp_random(5)];
	HSV_t background = {1, .05, .1};
	bmp_fill(img, bmp_HSV_to_RGB(background));
	Circle cir = Circle(0, 340, 20, hsv);
	int i;

	for(i = 0; i < drips; i++){
		cir.x = bmp_random(width);
		cir.y = bmp_random(height);
		cir.r = bmp_random(5,15);
		cir.hsv = palette3[bmp_random(5)];
		circles.push(cir);
	}

	while(!circles.empty()){
		cir = circles.top();
		while(cir.y >= 0 && cir.r >= 1){
			cir.draw(img);
			//.1% chance of ending
			if(bmp_random(10000) < 10){
				//spawn 2% of the time
				if(bmp_random(100) < 50){
					Circle child = Circle(cir.x + bmp_random(-cir.r,cir.r), cir.y-(cir.r/2), cir.r * .8, cir.hsv);
//					child.setHue(cir.hsv.h + 60);
					circles.pop();
					circles.push(child);
					circles.push(cir);
				}	
				cir.r -= 2;
				cir.hsv = background;
				cir.draw(img);
				break;
			}
			//1 % chance of shrinking
			if(bmp_random(10000) < 1){
				cir.r -= 1;
			}
			if(bmp_random(100) < 2){
				cir.x += cir.dir;
			}
			cir.y -= 1;
			if(cir.hsv.v > 0){
				cir.hsv.v -= .0003;
			}
		}
		circles.pop();
	}

	bmp_write_out(img);
	bmp_close(img);

	return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <JMAP.h>

#define MAX_CHILD 10  
const HSV_t background = {270, .1, .1};
RGB_t backgroundrgb;

bool isCollision(const bmp_t img, const int x, const int y);


class Line{

	public:
	v2i a;
	v2i b;
	HSV_t hsv;
	
	Line(){
		a = {0,0};
		b = {0,0};
		hsv = {0,0,0};
	};

	Line(v2i a_, v2i b_, HSV_t hsv_){
		a = a_;
		b = b_;
		hsv = hsv_;
	};

	void draw(bmp_t img, int depth){
		if(depth > 5){
			return;
		}
		RGB_t rgb = bmp_HSV_to_RGB(hsv);
		int rise = 1;
		int run = 1;
		float slope;
		int dx;
		int dy;
		int itr = 0;

		dx = b.x - a.x;
		dy = b.y - a.y;


		if(dx < 0){
			run = -1;
			dx = abs(dx);
		}

		if(dy < 0){
			rise = -1;
			dy = abs(dy);
		}

		if(dx == 0){
			slope = 0;
		}
		else{
			slope = (float)dy/(float)dx;
		}

		//small slope
		if(slope < 1){
			int x = a.x;
			int y = a.y;
			//draw line until edge of canvas or existing pixel is hit
			while(itr < 4 || !isCollision(img, x + 2*run, y + 2*rise*slope)){
					bmp_draw_circle(x, y, 2, img, rgb);
//					bmp_set_pixRGB(img, x, y, rgb);
					itr++;
					x = itr*run + a.x;
					y = itr*slope*rise + a.y;
			}
			//choose a random amount of children to spawn and draw them
			int numChildren = bmp_random(5,MAX_CHILD);
			for(int i = 0; i < numChildren; i++){
				v2i childA;
				v2i childB;
				int childStart = bmp_random(itr);
				childA.x = a.x + childStart*run;
				childA.y = a.y + childStart*slope*rise;
				childB.x = bmp_random(bmp_get_width(img));
				childB.y = bmp_random(bmp_get_height(img));
				Line child = Line(childA, childB, palette3[bmp_random(5)]);
				child.draw(img, depth+1);
			}
		
		}

		else if(slope > 1){
			slope = 1/slope;
			int swap;
			swap = -run;
			run = rise;
			rise = swap;

			int x = a.x;
			int y = a.y;
			//draw line until edge of canvas or existing pixel is hit
			while(itr < 4 || !isCollision(img, x + run*2*slope, y + rise*2)){
					bmp_draw_circle(x, y, 2, img, rgb);
//					bmp_set_pixRGB(img, x, y, rgb);
					itr++;
					y = itr*run + a.y;
					x = itr*slope*rise + a.x;
			}
			//choose a random amount of children to spawn and draw them
			int numChildren = bmp_random(5,MAX_CHILD);
			for(int i = 0; i < numChildren; i++){
				v2i childA;
				v2i childB;
				int childStart = bmp_random(itr);
				childA.x = a.x + childStart*slope*rise;
				childA.y = a.y + childStart*run;
				childB.x = bmp_random(bmp_get_width(img));
				childB.y = bmp_random(bmp_get_height(img));
				Line child = Line(childA, childB, palette3[bmp_random(5)]);
				child.draw(img, depth+1);
			}
		
		
		
		}

		
	};

};

bool isCollision(const bmp_t img, const int x, const int y){
	RGB_t rgb;
	bmp_get_pixRGB(img, x, y, &rgb);
	return(x < 0 || y < 0 || x >= bmp_get_width(img) || y >= bmp_get_height(img) ||
			rgb.R != backgroundrgb.R || rgb.G != backgroundrgb.G || rgb.B != backgroundrgb.B);
}


int main(int argc, char **argv){

	bmp_random_init();
	int width = 3840, height = 2160;
	bmp_t img = bmp_new("tri2.bmp", width, height);
	Line line = Line();
	line.a.x = 0;
	line.a.y = bmp_random(height);
	line.b.x = bmp_random(width);
	line.b.y = bmp_random(height);
	line.hsv = palette3[bmp_random(5)];

	backgroundrgb = bmp_HSV_to_RGB(background);

	bmp_fill(img, backgroundrgb);
	line.draw(img, 1);

	bmp_write_out(img);
	bmp_close(img);




	return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <JMAP.h>

#define NUMCONTROLS 25

typedef struct CONTROL{
	v2f pos;
	HSV_t color;
}ControlPt;

ControlPt controls[NUMCONTROLS];


ControlPt nearestControl(const int x, const int y){
	
	ControlPt nearest;
	int dist = 100000;
	for(int i = 0; i < NUMCONTROLS; i++){
		int dx = abs(controls[i].pos.x - x);	
		int dy = abs(controls[i].pos.y - y);	
		int len = sqrt(dx*dx + dy*dy);

		if(len < dist){
			dist = len;
			nearest = controls[i];
		}
	}
	return nearest;
}

void nearest3(const int x, const int y, ControlPt points[3]){
	int dist1 = 100000;
	int dist2 = 100001;
	int dist3 = 100002;
	for(int i = 0; i < NUMCONTROLS; i++){
		int dx = abs(controls[i].pos.x - x);	
		int dy = abs(controls[i].pos.y - y);	
		int len = sqrt(dx*dx + dy*dy);

		if(len < dist1){
			dist3 = dist2;
			dist2 = dist1;
			dist1 = len;
			points[2] = points[1];
			points[1] = points[0];
			points[0] = controls[i];
		}
		else if(len < dist2){
			dist3 = dist2;
			dist2 = len;
			points[2] = points[1];
			points[1] = controls[i];
		}
		else if(len < dist3){
			dist3 = len;
			points[2] = controls[i];
		}
	}



}

v2f normalize(const v2f v){

	v2f ret;
	float length = sqrt(v.x * v.x + v.y * v.y);

	ret.x = v.x/length;
	ret.y = v.y/length;
	
	return ret;
}

v2f lineFromValue(const int w, const float val, const int len){

	v2f ret;
	float h = tan(val) * w;
	ret = {(float)w,h};
	ret = normalize(ret);
	ret.x *= len;
	ret.y *= len;

	return ret;

}


float getVal(const int x,const int y){
	return (pow(sin(cos(x * .003)),4) + pow(cos(y * .003),3)) * .2 * 2 * M_PI;
}


int main(int argc, char **argv){


	int width = 2000;//4000;
	int height = 1600;//3200;
	int thickness = 4;
	int numColors = 9;
	
	bmp_t img = bmp_new("./Images/blep.bmp", width, height);
	bmp_random_init();
	palette_t pal = bmp_generate_palette(numColors, paletteType::splitcomp);


	//init control points
	for(int i = 0; i < NUMCONTROLS; i++){
		controls[i].pos = {(float)bmp_random(width),(float)bmp_random(height)};
		controls[i].color = pal.colors[bmp_random(numColors)];
	}


	for(int i = 0; i < 100000; i++){
		int x = bmp_random(-20, width+20);
		int y = bmp_random(-20, height+20);
		float val = getVal(x,y);
		float w = bmp_random(50, 150);
		ControlPt near[3];
		nearest3(x,y,near);
		float weights[] = {.5, .3, .2};
		ControlPt c = bmp_weighted_choice(near,weights,3);


		if(val > M_PI * .5){
			val = M_PI * .49;
		}

		v2f normal = lineFromValue(w,val,w);
		v2i a = {x,y};
		v2i b = {x + (int)normal.x, y + (int)normal.y};
		bmp_draw_line(img, a, b, bmp_HSV_to_RGB(c.color), thickness);
	
	}

	bmp_write_out(img);
	bmp_close(img);


	return 0;

}

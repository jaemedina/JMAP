#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <JMAP.h>



typedef struct controlPT{
	v2i pos;
	HSV_t color;
}control;


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
	return (pow(sin(x * .002),2) + pow(cos(y * .003),3)) * .2 * 2 * M_PI;
}

float getVal(const HSV_t hsv){
	return (1 - hsv.v) * .285 * M_PI * 2;
}

void genImage(bmp_t img, bmp_t src){

	//for(int i = 0; i < 0; i++){
		//int x = bmp_random(width);
		//int y = bmp_random(height);
		//float val = getVal(x,y);
		//float w = bmp_random(length, length * 1.5);
		//if(val > M_PI * .5)
			//val = M_PI * .4999;

		//v2f normal = lineFromValue(w, val, w);
		//v2i a = {x,y};
		//v2i b = {x + (int)normal.x, y + (int)normal.y};
	
		//if(i < coloritr/3){
			//bmp_draw_line(color1, a, b, BLACKRGB, colorSize);
			//bmp_draw_line(combined, a, b, bmp_HSV_to_RGB(palette3[3]), colorSize);
		//}
		//else if(i < (coloritr/3) * 2){	
			//bmp_draw_line(color2, a, b, BLACKRGB, colorSize);
			//bmp_draw_line(combined, a, b, bmp_HSV_to_RGB(palette3[2]), colorSize);
		//}
		//else{
			//bmp_draw_line(color3, a, b, BLACKRGB, colorSize);
			//bmp_draw_line(combined, a, b, bmp_HSV_to_RGB(palette3[1]), colorSize);
		//}
	//}
	int width = bmp_get_width(img);
	int height = bmp_get_height(img);
	int length = 50;

	for(int i = 0; i < 5000; i++){
		RGB_t rgb;
		int x = bmp_random(width);
		int y = bmp_random(height);
		bmp_get_pixRGB(src, x, y, &rgb);
		float angle = getVal(x,y);
		float val = getVal(bmp_RGB_to_HSV(rgb));
		float w = bmp_rand_normal(length, 30);
		if(val > M_PI * .5)
			val = M_PI * .4999;
	
		v2f normal = lineFromValue(w, angle, w);

		v2i a = {x,y};
		v2i b = {x + (int)normal.x, y + (int)normal.y};
		float size = val*23.0f;
		if((int)size <= 1){
			continue;
		}

		bmp_draw_line(img, a, b, BLACKRGB, size + 1);
//		bmp_draw_line(combined, a, b, BLACKRGB, size);
	}


	for(int i = 0; i < 6; i++){
		int x1 = bmp_random(width);
		int y1 = bmp_random(height);

		int x2 = x1 + bmp_random(width - x1);
		int y2 = y1 + bmp_random(height - y1);

		for(int row = y1; row < y2; row += bmp_random(4,8)){
			if(bmp_random(100) > 30){
				continue;
			}
			if(bmp_random(100) < 95){
				bmp_draw_line(img, {x1,row}, {x2,row}, WHITERGB, 2);
			}
			else{
				bmp_draw_line(img, {x1,row}, {x2,row}, BLACKRGB, 2);	
			}
		}
	}

	//draws white lines within random boxes at a random interval
	
	for(int i = 0; i < 6; i++){
		int x1 = bmp_random(width);
		int y1 = bmp_random(height);

		int x2 = x1 + bmp_random(width - x1);
		int y2 = y1 + bmp_random(height - y1);

		for(int col = x1; col < x2; col += bmp_random(4,8)){
			if(bmp_random(100) > 30){
				continue;
			}
			if(bmp_random(100) < 95){
				bmp_draw_line(img, {col,y1}, {col, y2}, WHITERGB, 2);
			}
			else{
				bmp_draw_line(img, {col,y1}, {col, y2}, BLACKRGB, 2);
			}
		}	
	}
}

void paste(bmp_t src, bmp_t dest, RGB_t color, float alpha){

	int width = bmp_get_width(src);
	int height = bmp_get_height(src);

	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			RGB_t buff;
			bmp_get_pixRGB(src, x, y, &buff);
			if(buff.R == BLACKRGB.R && buff.G == BLACKRGB.G && buff.B == BLACKRGB.B){
				bmp_set_pixRGBA(dest, x, y, color, .8);
			}
		}
	}
}


int main(int argc, char **argv){



	bmp_t img = bmp_open(argv[1], 'r');
	bmp_t c = bmp_open("./Images/c.bmp", 'r');
	bmp_t combined = bmp_copy("./Images/combined.bmp", c);
	int width = bmp_get_width(img);
	int height = bmp_get_height(img);
	bmp_t l1 = bmp_new("./Images/layer1.bmp", width, height);
	bmp_t l2 = bmp_new("./Images/layer2.bmp", width, height);

	bmp_random_init();

	HSV_t db = {240, .9, .33};
	HSV_t dr = {0, .9, .33};

	genImage(l1, img);
	paste(l1, combined, bmp_HSV_to_RGB(dr), .4);
	genImage(l2, img);
	paste(l2, combined, bmp_HSV_to_RGB(db), .4);

	bmp_write_out(l1);
	bmp_write_out(l2);
	bmp_write_out(combined);
//	bmp_write_out(color1);
//	bmp_write_out(color2);
//	bmp_write_out(color3);
//	bmp_write_out(combined);
	
	bmp_close(img);
	bmp_close(combined);
	bmp_close(c);
	bmp_close(l1);
	bmp_close(l2);


	return 0;

}

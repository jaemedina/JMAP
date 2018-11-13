#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <JMAP.h>

v2f addv2f(const v2f a, const v2f b){
	return {a.x + b.x, a.y + b.y};
}

v2i v2ftov2i(const v2f f){
	return{(int)f.x,(int)f.y};
}


class Particle{

	public:
	v2f pos;
	v2f v;
	HSV_t color;
	int size;

	Particle(){
		pos = {0,0};
		v = {0,0};
		color = BLACKHSV;
		size = 1;
	};

	Particle(v2f pos_, v2f v_, HSV_t color_, int size_){
		pos = pos_;
		v = v_;
		color = color_;
		size = size_;
	};

	void draw(bmp_t img){
		bmp_draw_line(img, v2ftov2i(pos), v2ftov2i(addv2f(pos,v)), bmp_HSV_to_RGB(color), size);
	};

	void updatePosition(){
		pos = addv2f(pos,v);
	};

	void updateVelocity(float vx, float vy){
		v.x += vx * .8;
		v.y += vy * .8;
	};


};

float getVal(float x, float y){
	return(sin(.001 * x) + cos(y*.001)) * 2  * 2 * M_PI;
}

int main (int charc, char **argv){

	int width = 3000;
	int height = 3000;
	bmp_t img = bmp_new("./Images/particle.bmp", width, height);

	bmp_random_init();

	Particle p = Particle();
	
	float value;
	for(int j = 0; j < 1; j++){
		p.pos.y = bmp_rand_normal(height*2/3, 500);
		p.pos.x = 0;
		for(int i = 0; i < 500000; i++){
			value = getVal(p.pos.x, p.pos.y);
			p.updateVelocity(cos(value),sin(value));
			p.draw(img);
			p.updatePosition();
			if(p.pos.x < 0)
				p.pos.x = width;
			if(p.pos.x >= width)
				p.pos.x = 0;
			if(p.pos.y < 0)
				p.pos.y = height;
			if(p.pos.y >= height)
				p.pos.y = 0;
			p.v.x *= .99;
			p.v.y *= .99;
		}
	}


	for(int i = 0; i < 100000; i++){
		float x = bmp_rand_normal(width/2, width/10);
		float y = bmp_rand_normal(height/2, height/10);
		float weights[] = {.05,.05,.05,.8,.05};
		HSV_t col = bmp_weighted_choice(palette3, weights, 5);
		bmp_draw_circle(x, y, 7, img, bmp_HSV_to_RGB(col));
	}




	bmp_write_out(img);
	bmp_close(img);

	return 0;

}

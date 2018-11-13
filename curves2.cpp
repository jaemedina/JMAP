#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <JMAP.h>

HSV_t BLACK = {80,0,0};
HSV_t WHITE = {0,0,1};
class Curve{

	public:
	std::vector<v2i> anchors;
	std::vector<v2i> tpts;
	HSV_t color;
	float alpha;
	

	Curve(){
		color = {0, 0, .4};
		alpha = 1;
	};

	void generateTPts(){
		float I = 100;	
		int n = anchors.size();
		tpts.clear();
		std::vector<v2i> anchorscopy = anchors;


		//bezier curve algorithm
		for(int i = 0; i < I; i++){
			for (int j = 1; j < n; j++){
				for(int k = 0; k < (n-j); k++){
					float ii = i/I;
					anchorscopy[k].x = (float)((1.0f-ii)*(float)anchorscopy[k].x) + (float)(ii*(float)anchorscopy[k+1].x);
					anchorscopy[k].y = (float)((1.0f-ii)*(float)anchorscopy[k].y) + (float)(ii*(float)anchorscopy[k+1].y);
				}
			}
			tpts.push_back(anchorscopy[0]);
			anchorscopy = anchors;

		}
	
	}

	void draw(bmp_t img){
	
		int n = tpts.size();
		int i;
		for(i = 0; i < n - 1; i ++){
			bmp_draw_line(img, tpts[i], tpts[i+1], bmp_HSV_to_RGB(color));
		}
	
	
	}

};



int main(int argc, char **argv){

	int width = 2000;
	int height = 3000;
	bmp_t newimg = bmp_new("./Images/shoop.bmp", width, height);
	if(newimg == -1){
		return 0;
	}
	bmp_random_init();

	bmp_t img = bmp_open("./Images/IMAG1601.bmp", 'r');
	bmp_t oz = bmp_new("./Images/oz.bmp", bmp_get_width(img), bmp_get_height(img));
	bmp_twotone(img, oz, 5);
	bmp_write_out(oz);
	bmp_close(img);
	bmp_close(oz);

	

	int i;

	Curve curve = Curve();
	v2i pt;
//	float threshold = .6;

	//for(i = 0; i < 10000; i++){
		//int j = bmp_random(width);
		//int k = bmp_random(height);
		//bmp_get_pixRGB(img, j, k, &test);
		//if(bmp_variance(BLACK, bmp_RGB_to_HSV(test)) < threshold){
			//pt = {j,k};
			//curve.anchors.push_back(pt);
		//}
	
	//}

	//for(int j = 0; j < width; j++){
		//for(int k = 0; k < height; k++){
			//bmp_get_pixRGB(newimg, j, k, &test);
			//if(bmp_variance(BLACK, bmp_RGB_to_HSV(test)) > threshold){
//				printf("V:%f\n",bmp_variance(BLACK, bmp_RGB_to_HSV(test)));
				//pt = {j,k};
				//curve.anchors.push_back(pt);
	//			bmp_set_pixRGB(newimg, j, k, bmp_HSV_to_RGB(WHITE));
			//}
		//}
	//}
	for(int r = 0; r < 10; r++){

		curve.anchors.clear();
		curve.color = palette3[bmp_random(5)];
//		curve.color.v = (float)bmp_random(80)/100;
		curve.alpha = 1;
		for(i = 0; i < 15; i++){
			pt.x = bmp_random(width);
			pt.y = bmp_random(height);
			curve.anchors.push_back(pt);
		}
		
		for(i = 0; i < 5000; i++){
			curve.generateTPts();
			curve.draw(newimg);
			curve.anchors[bmp_random(curve.anchors.size())].x += bmp_random(-30,30);
			curve.anchors[bmp_random(curve.anchors.size())].y += bmp_random(-30,30);
			if(curve.color.v > 0)
				curve.color.v -= .0001;
		}
	}
	
	bmp_write_out(newimg);
	
	bmp_t img2 = bmp_new("./Images/shoopy.bmp", width, height);

	bmp_twotone(newimg,img2,4);


	bmp_write_out(img2);
	bmp_close(newimg);
	bmp_close(img2);

	return 0;

}

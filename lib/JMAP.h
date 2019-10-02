#ifndef JMAP_H
#define JMAP_H

#include <vector>


#define MAX_OPEN_BMP 256
#define BRUSH_COUNT 6



typedef unsigned char BYTE;
typedef short DBYTE;
typedef int QBYTE;
//bmp_t
//bmp type: an int to refer to an open bmp file
typedef int bmp_t;
typedef int brush_t;

//RGB_t
//red, green, and blue bytes
typedef struct RGB{
	BYTE R, G, B;
}RGB_t;

typedef struct HSV{
	float h;//[0,360]
       	float s;//[0,1]
        float v;//[0,1]
}HSV_t;

//vector of 2 integers
typedef struct v2{
	int x;
	int y;
}v2i;

typedef struct v2FLOAT{
	float x;
	float y;
}v2f;

typedef struct v3{
	float x;
	float y;
	float z;
}v3f;

//CLASS DECLARATIONS
class polygon{
	private:
	std::vector<v2f> vertices;
	HSV_t color;
	bmp_t buff = -1;

	public:
	polygon();
	polygon(std::vector<v2f> verts, HSV_t col);
	void buffUpdate();
	void draw(bmp_t img, int x, int y);
	void outline();
	bool isExtrema(int x, int y);
	void fill();
	void setColor(HSV_t col);
	void scale(float factor);
	void rotate(float angle);
	int size();
	void addVertex(v2f v, int idx);
	void removeVertex(int idx);
	v2f getVertex(int idx);
	v2f getCentroid();
	void moveVertex(int idx, int x, int y);
};



const HSV_t palette1[]={
	{37, .7, .9},
	{30, .4, .8},
	{31, .56, .82},
	{44, .23, .91},
	{41, .77, .55}
};

const HSV_t palette2[]={
	{245, .15, .46},
	{189, .78, .78},
	{187, .72, .87},
	{47, .67, .96},
	{17, .5, .96}
};

const HSV_t palette3[]={
	{212, .41, .29},
	{0, .74, .83},
	{4, .52, .89},
	{37, .29, .90},
	{158, .27, .65},
	{305, .72, .4}
};

const HSV_t palette4[]={
	{27, .57, .90},
	{26, .65, .40},
	{46, .05, .94},
	{349, .87, .76},
	{3, .72, .87},
	{200, .12, .76}
};

const HSV_t paletteRGB[]={
	{0, .6, .8},
	{120, .6, .8},
	{240, .8, .4}
};
const HSV_t BLACKHSV = {0,0,0};
const RGB_t BLACKRGB = {0,0,0};

const HSV_t WHITEHSV = {0,0,1};
const RGB_t WHITERGB = {255,255,255};

bmp_t bmp_open(const char* fileName, const char mode);

bmp_t bmp_copy(const char* fileName, const bmp_t bmpNum);

bmp_t bmp_new(const char* fileName, const int width, const int height);

int bmp_close(bmp_t bmpNum);

int bmp_print_info(bmp_t bmpNum);

int bmp_write_out(const bmp_t bmpNum);

int bmp_get_pixRGB(const bmp_t bmpNum, const int x, const int y, RGB_t *RGB);

int bmp_set_pixRGB(const bmp_t bmpNum, const int x, const int y, const RGB_t RGB);

int bmp_set_pixRGBA(const bmp_t bmpNum, const int x, const int y, const RGB_t RGB, const float alpha);

int bmp_set_pixHSV(const bmp_t bmpNum, const int x, const int y, const HSV_t HSV);

int bmp_get_width(const bmp_t bmpNum);

int bmp_get_height(const bmp_t bmpNum);

int bmp_fill(const bmp_t bmpNum, const RGB_t fill);

int bmp_draw_line(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb);

int bmp_draw_line(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb, const int lineSize);

int bmp_draw_brush_line(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb, const brush_t brush);

int bmp_draw_line_alpha(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb, float alpha);

int bmp_load_brushes();

int bmp_draw_circle(const int x, const int y, const int r, const int bmpNum, const RGB_t rgb);

brush_t bmp_get_brush(const int brushNum);

int bmp_close_brushes(void);

int bmp_brush_stroke(const int x, const int y, const int bmpNum, const RGB_t rgb, const brush_t brush);

RGB_t bmp_get_avg_RGB(const bmp_t bmpNum, const int x, const int y, const int area);

RGB_t bmp_HSV_to_RGB(HSV_t hsv);

HSV_t bmp_RGB_to_HSV(const RGB_t rgb);

void bmp_grayscale(const bmp_t bmpNum);

void bmp_mask(const bmp_t src, const bmp_t dest, const bmp_t mask);

void bmp_twotone(const bmp_t bmpNum, const int lineSize);

void bmp_twotone(const bmp_t src, const bmp_t dest, const int lineSize);
//finds the similarity between two colors
float bmp_variance(const HSV_t src, const HSV_t other);

void bmp_random_init(void);
//returns a random integer from 0 to range-1
int bmp_random(int range);
//returns a random integer from [min,max]
int bmp_random(int min, int max);

float bmp_rand_normal(float mean, float stddev);
//creates a textured background for image bmpNum
int bmp_background(const bmp_t bmpNum, const HSV_t hsv, const int numLines);

v2i v2fTov2i(const v2f v);

//TEMPLATE//

template <typename T>
T bmp_weighted_choice(const T items[], const float weights[], const int size){


	if(size == 1){
		return items[0];
	}

	float r = (float)bmp_random(101)/100.0f;
	float weight_seen = 0;

	for(int i = 0; i < size; i++){
		float upper = weight_seen + weights[i];
		if(r > weight_seen && r <= upper){
			return items[i];
		}
		weight_seen += weights[i];
	}

	//else nothing chosen, default to first item
	return items[0];

}

//COLOR//
enum paletteType{
	mono, comp, splitcomp, trio, tetra, analogous
};

typedef struct palette{
	int numColors;
	paletteType type;
	HSV_t *colors;
}palette_t;

palette_t bmp_generate_palette(const int numColors, paletteType type);




#endif

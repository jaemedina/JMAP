#include <stdio.h>
#include <stdlib.h>

#include <JMAP.h>


int main(int argc, char **argv){

	int c = 15;
//	palette_t pal = bmp_generate_palette(c, paletteType::mono);
	palette_t pal2 = bmp_generate_palette(c, paletteType::trio);
	int width = 800;
	int height = 1000;
	int r = height / c;

	for(int i = 0; i < pal2.numColors; i++){
		printf("H:%f S:%f V:%f \n", pal2.colors[i].h, pal2.colors[i].s, pal2.colors[i].v);
	}

	bmp_t img = bmp_new("./Images/colors.bmp", width, height);
	int col = 0;

	for(int i = r; i < height; i += r){
		bmp_draw_line(img, {-5, i}, {width, i}, bmp_HSV_to_RGB(pal2.colors[col]), r);
		col += 1;
	}

	bmp_write_out(img);
	bmp_close(img);
}

#include <stdio.h>
#include <stdlib.h>

#include <JMAP.h>


int main (int argc, char **argv){

	if(argc != 4){
		printf("ERROR: usage: './twotone.x [input file] [output file] [max line size]'\n");
		return 1;
	}


	char *fileIn = argv[1];
	char *fileOut = argv[2];
	int size = atoi(argv[3]);

	bmp_t imgIn = bmp_open(fileIn, 'r');

	int width = bmp_get_width(imgIn);
	int height = bmp_get_height(imgIn);

	bmp_t imgOut = bmp_new(fileOut, width, height);


	bmp_twotone(imgIn, imgOut, size);

	bmp_write_out(imgOut);
	bmp_close(imgOut);
	bmp_close(imgIn);
	
	return 0;


}

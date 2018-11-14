#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <time.h>
#include <math.h>

#include "JMAP.h"


typedef struct  __attribute__((__packed__)) BMPFILEHEADER{
	DBYTE BMPSignature;
	QBYTE BMPFileSize;
	QBYTE BMPReserved;
	QBYTE BMPOffset;
}BMP_FILE_HEADER;

typedef struct __attribute__((__packed__)) BMPINFOHEADER{
	QBYTE BMPInfoSize;
	QBYTE BMPWidth;
	QBYTE BMPHeight;
	DBYTE BMPPlanes;
	DBYTE BMPBitsPerPixels;
	QBYTE BMPCompression;
	QBYTE BMPDataSize;
	QBYTE BMPHorizontalRes;
	QBYTE BMPVerticalRes;
	QBYTE BMPNumberOfColors;
	QBYTE BMPImportantColors;
}BMP_INFO_HEADER;

typedef struct BMPtag{
	FILE *fileptr;
	BMP_FILE_HEADER *BFH;
	BMP_INFO_HEADER *BIH;
	BYTE *imageBuf;
}BMP;


BMP *BMPList[MAX_OPEN_BMP] = {NULL};
bool RANDOMINIT = false;
brush_t brushList[BRUSH_COUNT] = {-1};

//Macros
//
//global variables
//


//bmp_next_free_bmp
//
//return: int value of the next free slot to store a bmp. -1 if nothing is free.
int bmp_next_free_bmp(void){
	int i;
	for(i = 0; i < MAX_OPEN_BMP; i++){
		if(BMPList[i]==NULL)
			return i;
	}
	return -1;
}


//bmp_is_open
//
//@ bmpNum: reference bmp number to check if it is open
//
//return: 1 if the bmp referenced by bmpNum is open. 0 otherwise.
int bmp_is_open(bmp_t bmpNum){
	if(bmpNum > MAX_OPEN_BMP){
		printf("ERROR: %d is greater than the max amount of open BMP's(%d)\n", bmpNum, MAX_OPEN_BMP);
		return 0;
	}

	if(BMPList[bmpNum] == NULL){
		printf("ERROR: %d does not refer to an open BMP\n", bmpNum);
		return 0;
	}
	return 1;
}

//bmp_find_idx
//
//@ bmpNum: int reference to open bmp file
//@ x: x-value to search into the bmp's array of chars
//@ y: y-value to search into the bmp's array of chars
//
//return: the index of the pixel referred to be the given x and y values for the
//	  given image. -1 if bmp is not open, or x or y out of range.
int bmp_find_idx(bmp_t bmpNum, int x, int y){

	if(!bmp_is_open(bmpNum))
		return -1;
	
	BMP *bmp = BMPList[bmpNum];
	if(x >= bmp->BIH->BMPWidth || y >= bmp->BIH->BMPHeight || x < 0 || y < 0)
		return -1;

	int padding = (4 - (bmp->BIH->BMPWidth * 3) % 4) % 4;

	return(y*(bmp->BIH->BMPWidth * 3 + padding) + x * 3);
	
}


//bmp_get_width
//
//@ bmpNum: bmp to get the width of
//
//return: Width of the bmp file referenced by bmpNum. -1 if bmpNum is not open.
int bmp_get_width(const bmp_t bmpNum){
	if(!bmp_is_open(bmpNum))
		return -1;

	return BMPList[bmpNum]->BIH->BMPWidth;
}

//bmp_get_height
//
//@ bmpNum: bmp to get the height of
//
//return: Height of the bmp file referenced by bmpNum. -1 if bmpNum is not open.
int bmp_get_height(const bmp_t bmpNum){
	if(!bmp_is_open(bmpNum))
		return -1;

	return BMPList[bmpNum]->BIH->BMPHeight;
}

//bmp_get_height
//
//@ bmpNum: bmp to print the info of
//
//return: 0 if printing successful. -1 otherwise.
int bmp_print_info(bmp_t bmpNum){
	if(!bmp_is_open(bmpNum)){
		return -1;
	}
	
	BMP *bmp = BMPList[bmpNum];

	printf("BMP Information:\nInfo Header Size = %d\nWidth = %d\nHeight = %d\nBits Per pixel = %d\nOffset = %d\nData Size = %d\n",
		bmp->BIH->BMPInfoSize,
		bmp->BIH->BMPWidth,
		bmp->BIH->BMPHeight,
		bmp->BIH->BMPBitsPerPixels,
		bmp->BFH->BMPOffset,
		bmp->BIH->BMPDataSize);
	return 0;

}

//bmp_alloc
//
//@ bmp: pointer to the BMP struct to allocate space for.
//@ fileName: string containing the name to give the file.
//@ mode: char to signify rwx settings of the allocated file.
//
//Return: 0 if success, -1 otherwise

int bmp_alloc(BMP *bmp, const char *fileName, const char mode){
	if(bmp == NULL || fileName == NULL)
		return -1;
	//allocate space for file and header structs
	FILE *fileptr = (FILE *)malloc(sizeof(FILE));
	BMP_FILE_HEADER *bmpFileHeader = (BMP_FILE_HEADER *)malloc(sizeof(BMP_FILE_HEADER));
	BMP_INFO_HEADER *bmpInfoHeader = (BMP_INFO_HEADER *)malloc(sizeof(BMP_INFO_HEADER));

	if(fileptr == NULL || bmpFileHeader == NULL || bmpInfoHeader == NULL){
		free(fileptr);
		free(bmpFileHeader);
		free(bmpInfoHeader);
		return -1;
	}
	
	int nextBmpIdx = bmp_next_free_bmp();
	if(nextBmpIdx == -1){
		printf("ERROR: Too many BMP's open\n");
		free(fileptr);
		free(bmpFileHeader);
		free(bmpInfoHeader);
		return -1;
	}

	//open the file
	//r mode opens the file in read only mode
	if(mode == 'r'){
		fileptr = fopen(fileName, "r+b");
		if(fileptr == NULL){
			printf("ERROR: Could not open file %s\n", fileName);
			free(fileptr);
			free(bmpFileHeader);
			free(bmpInfoHeader);
			return -1;
		}
	}
	//w mode opens the file in write mode
	else if (mode == 'w'){	
		fileptr = fopen(fileName, "w+b");
		if(fileptr == NULL){
			printf("ERROR: Could not open file %s\n", fileName);
			free(fileptr);
			free(bmpFileHeader);
			free(bmpInfoHeader);
			return -1;
		}
	}

	else{
		printf("ERROR:bmp_alloc: invalid value for mode\n");
		free(fileptr);
		free(bmpFileHeader);
		free(bmpInfoHeader);
		return -1;
	
	}

	bmp->fileptr = fileptr;
	bmp->BFH = bmpFileHeader;
	bmp->BIH = bmpInfoHeader;

	return 0;
}

//bmp_open
//
//@ fileName: Name of the bmp file to open
//@ mode: mode to open the file as (read or write)
//
//Return: Index of the newly created bmp file, -1 if opening the given file
//	  failed 
bmp_t bmp_open(const char* fileName, const char mode){
	
	BMP *bmp = (BMP *)malloc(sizeof(BMP));
	if(bmp == NULL){
		free(bmp);
		return -1;
	}

	if(bmp_alloc(bmp, fileName, mode) == -1){
		free(bmp);
		return -1;
	}

	//TODO Error checking for reading
	//read the contents of the file into header structs
	fread(bmp->BFH, sizeof(BMP_FILE_HEADER), 1, bmp->fileptr);
	fread(bmp->BIH, sizeof(BMP_INFO_HEADER), 1, bmp->fileptr);
	

	//check to see if the file is of the right type
	if(bmp->BFH->BMPSignature != 0x4D42){
		printf("ERROR: Not a .bmp file\n");
		fclose(bmp->fileptr);
		//free(bmp->fileptr);
		free(bmp->BFH);
		free(bmp->BIH);
		free(bmp);
		return -1;
	}

	//allocate space for pixel information	
	BYTE *imageData = (BYTE *)malloc(bmp->BIH->BMPDataSize);

	if(!imageData){
		printf("ERROR: Failed to allocate space for Image Data\n");
		fclose(bmp->fileptr);
		free(bmp->BFH);
		free(bmp->BIH);
		free(bmp);
		return -1;
	}
	
	//copy pixel data into image buffer
	fseek(bmp->fileptr, bmp->BFH->BMPOffset, SEEK_SET);
	fread(imageData, bmp->BIH->BMPDataSize, 1, bmp->fileptr);

	bmp->imageBuf = imageData;

	int nextBMP = bmp_next_free_bmp();
	BMPList[nextBMP] = bmp;
	return nextBMP;

}

//bmp_close
//
//@ bmpNum: bmp file to close
//
//Return: 0 if success, -1 if failure
bmp_t bmp_close(bmp_t bmpNum){
	if(!bmp_is_open(bmpNum)){
		printf("ERROR: Attempting to close a non-open BMP\n");
		return -1;
	}

	BMP *bmpToDelete = BMPList[bmpNum];

	fclose(bmpToDelete->fileptr);
	free(bmpToDelete->imageBuf);
	free(bmpToDelete->BFH);
	free(bmpToDelete->BIH);
	free(bmpToDelete);

	return 0;
}


//bmp_copy: opens a new file and copies an existing bmp into it
//
//@ fileName: name of the new file to be copied into
//@ bmpNum: bmp file to copy from
//
//return: bmp file index of the new file. -1 if error.
bmp_t bmp_copy(const char* fileName, const bmp_t bmpNum){
	if(fileName == NULL){
		printf("ERROR: Null filename given for bmp_create\n");
		return -1;
	}
	if(!bmp_is_open(bmpNum)){
		printf("ERROR: BMP not open\n");
		return -1;
	}

	BMP *oldBMP = BMPList[bmpNum];
	int newIdx = bmp_next_free_bmp();

	BMP *newBMP = (BMP *)malloc(sizeof(BMP));
	if(!newBMP){
		free(newBMP);
		return -1;
	}


	if(bmp_alloc(newBMP, fileName,'w')){
		free(newBMP);
		return -1;
	}	

	newBMP->imageBuf = (BYTE *)malloc((oldBMP->BIH->BMPDataSize));
	if(!newBMP->imageBuf){
		free(newBMP->imageBuf);
		free(newBMP);
		return -1;
	}


	memcpy(newBMP->BFH, oldBMP->BFH, sizeof(BMP_FILE_HEADER));
	memcpy(newBMP->BIH, oldBMP->BIH, sizeof(BMP_INFO_HEADER));
	memcpy(newBMP->imageBuf, oldBMP->imageBuf, oldBMP->BIH->BMPDataSize);


	BMPList[newIdx] = newBMP;
	return newIdx;


}



//bmp_new
//
//@ fileName: string with the name of the new bmp file to create
//@ width: width of the new bmp
//@ height: height of the new bmp
//
//Return: bmp index of the new file, -1 if error
int bmp_new(const char* fileName, const int width, const int height){
	if(fileName == NULL || width <= 0 || height <= 0){
		return -1;
	}

	BMP *newBMP = (BMP *)malloc(sizeof(BMP));
	if(bmp_alloc(newBMP, fileName, 'w')){
		free(newBMP);
		return -1;
	}

	int padding = (4 - ((width * 3) % 4)) % 4;
	int imageSize = (height * ((width * 3) + padding));
	int nextBmpIdx = bmp_next_free_bmp();

	BYTE *imageBuff = (BYTE *)malloc(imageSize);
	if(!imageBuff){
		free(newBMP);
		free(imageBuff);
		return -1;
	}

	memset(imageBuff, 255, imageSize);	

	newBMP->BFH->BMPSignature = 0x4D42;
	newBMP->BIH->BMPWidth = width;
	newBMP->BIH->BMPHeight = height;
	newBMP->BFH->BMPOffset = 54;
	newBMP->BIH->BMPBitsPerPixels = 24;
	newBMP->BIH->BMPPlanes = 1;
	newBMP->BIH->BMPHorizontalRes = 7874;//default 200 dpi
	newBMP->BIH->BMPVerticalRes = 7874;//default 200 dpi
	newBMP->BIH->BMPDataSize = imageSize;
	newBMP->BFH->BMPFileSize = sizeof(BMP_FILE_HEADER) + sizeof(BMP_INFO_HEADER) + imageSize;
	newBMP->BIH->BMPInfoSize = sizeof(BMP_INFO_HEADER);

	newBMP->imageBuf = imageBuff;
	BMPList[nextBmpIdx] = newBMP;
	return nextBmpIdx;

}


//bmp_write_out
//writes changes to the image buffer out to the actual file
//
//@ bmpNum: bmp file to write out to
//
//Return: 0 if success, -1 otherwise
int bmp_write_out(const bmp_t bmpNum){
	if(!bmp_is_open(bmpNum)){
		return -1;
	}
	
	BMP *bmp = BMPList[bmpNum];

	fwrite(bmp->BFH, sizeof(BMP_FILE_HEADER), 1, bmp->fileptr);
	fwrite(bmp->BIH, bmp->BIH->BMPInfoSize, 1, bmp->fileptr);
	fseek(bmp->fileptr, bmp->BFH->BMPOffset, SEEK_SET);
	fwrite(bmp->imageBuf, bmp->BIH->BMPDataSize, 1, bmp->fileptr);

	return 0;
}

//bmp_get_pixRGB
//gets pixel information for an image
//
//@ bmpNum: bmp file to access
//@ x: x value of the pixel to find
//@ y: y value of the pixel to find
//@ RGB: RGB struct to store the pixel information in
//
//Return: 0 if success, -1 otherwise
int bmp_get_pixRGB(const bmp_t bmpNum, const int x, const int y, RGB_t *RGB){
	if(!bmp_is_open(bmpNum))
		return -1;

	BMP *bmp = BMPList[bmpNum];
	if(x < 0 || x >= bmp_get_width(bmpNum) || y < 0 || y >= bmp_get_height(bmpNum)){
		return -1;
	}

	int idx = (bmp_find_idx(bmpNum, x, y));

	RGB->R = bmp->imageBuf[idx + 2];
	RGB->G = bmp->imageBuf[idx + 1];
	RGB->B = bmp->imageBuf[idx + 0];

	return 0;

}

//bmp_set_pixRGB
//Sets a pixel vale for an image
//
//@ bmpNum: bmp file to access
//@ x: x value of the pixel to change
//@ y: y value of the pixel to change
//@ RGB: RGB value of the pixel to change
//
//Return: 0 if success, -1 otherwise
int bmp_set_pixRGB(const bmp_t bmpNum, const int x, const int y, const RGB_t RGB){
	if(!bmp_is_open(bmpNum)){
		return -1;
	}

	int idx = bmp_find_idx(bmpNum, x, y);

	BMP *bmp = BMPList[bmpNum];
	if(x >= bmp->BIH->BMPWidth || x < 0 || y >= bmp->BIH->BMPHeight || y < 0){
		return -1;
	}

	bmp->imageBuf[idx] = RGB.B;
	bmp->imageBuf[idx + 1] = RGB.G;
	bmp->imageBuf[idx + 2] = RGB.R;

	return 0;
}

RGB_t bmp_interpolateRGB(const RGB_t a, const RGB_t b, const float ratio){

	RGB_t res;

	res.R = (a.R*ratio) + (b.R*(1-ratio));
	res.G = (a.G*ratio) + (b.G*(1-ratio));
	res.B = (a.B*ratio) + (b.B*(1-ratio));

	return res;
}


int bmp_set_pixRGBA(const bmp_t bmpNum, const int x, const int y, const RGB_t RGB, float alpha){
	if(!bmp_is_open(bmpNum)){
		return -1;
	}
	
	if(alpha > 1)
		alpha = 1;
	if(alpha < 0)
		alpha = 0;

	RGB_t dest, res;
	int idx = bmp_find_idx(bmpNum, x, y);

	BMP *bmp = BMPList[bmpNum];
	if(x >= bmp->BIH->BMPWidth || x < 0 || y >= bmp->BIH->BMPHeight || y < 0){
		return -1;
	}
	
	bmp_get_pixRGB(bmpNum, x, y, &dest);
	res = bmp_interpolateRGB(RGB, dest, alpha);

	bmp->imageBuf[idx] = res.B;
	bmp->imageBuf[idx + 1] = res.G;
	bmp->imageBuf[idx + 2] = res.R;

	return 0;
}

//bmp_copy_header
//Function to copy the header information into the a file.
//
//@ fileIn
int bmp_copy_header(char* fileIn, char* fileOut, char* newImage, int offsetSize, int imageSize){
	FILE *fptrIn, *fptrOut;
	
	fptrIn = fopen(fileIn, "rb");
	fptrOut = fopen(fileOut, "wb");

	char* buf = (char *)malloc(offsetSize);
	if (!buf){
		printf("ERROR: Copy header error, malloc failed\n");
		return 1;
	}

	fread(buf, offsetSize, 1, fptrIn);
	fwrite(buf, offsetSize, 1, fptrOut);
	fwrite(newImage, imageSize, 1, fptrOut);

	fclose (fptrIn);
	fclose (fptrOut);
	free(buf);

	return 0;
}

//bmp_fill
//Fills an image with a single color
//
//@ bmpNum: bmp file to access
//@ fill: RGB struct with color to fill the image with
//
//Return: 0 if success, -1 otherwise
int bmp_fill(const bmp_t bmpNum, const RGB_t fill){
	if(!bmp_is_open(bmpNum))
		return -1;
	
	int x;
	int y;
	int width = bmp_get_width(bmpNum);
	int height = bmp_get_height(bmpNum);

	for(y = 0; y < height; y++){
		for(x = 0; x < width; x++){
			bmp_set_pixRGB(bmpNum, x, y, fill);	
		}
	}

	return 0;
}

//absolute value of x
int abs(const int x){
	return ((x < 0) ? (-x) : (x));
}

float absf(const float x){
	return ((x < 0) ? (-x) : (x));
}

//sign of int x, -1 if negative, 1 if positive
int sign(const int x){
	return (x > 0) - (x < 0);
}

//bmp_draw_line
//function to draw a line using bresenham line-drawing algorithm
//
int bmp_draw_line(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb){
	if(!bmp_is_open(bmpNum))
		return -1;

	int direction = a.x > b.x ? -1 : 1;

	int dx = abs(b.x - a.x);
	int dy = abs(b.y - a.y);
	int ddy = 2*dy;
	int difference = ddy - 2*dx; //2dy - 2dx

	int p = ddy - dx;
	int itr;
	int yk = 0; 

	bmp_set_pixRGB(bmpNum, a.x, a.y, rgb);
	//small slope
	if(dx >= dy){
		int step = a.y > b.y ? -1:1;
		for(itr = 0; itr < dx; itr++){
			if(p<0){
				bmp_set_pixRGB(bmpNum, a.x + itr*direction + direction, a.y + yk*step, rgb);
				p += ddy;
			}
			else{
				bmp_set_pixRGB(bmpNum, a.x + itr*direction + direction, a.y + yk*step + step, rgb);
				yk++;
				p+= difference;
			}
		}
	}

	else{
		direction = a.y > b.y ? -1:1;
		int ddx = 2*dx;
		int diff = ddx - 2*dy;
		p = ddx - dy;
		int xk = 0;
		int step = a.x > b.x ? -1:1;
		for(itr = 0; itr < dy; itr++){
			if(p<0){
				bmp_set_pixRGB(bmpNum, a.x + xk*step, a.y + itr*direction + direction, rgb);
				p += ddx;
			}
			else{
				bmp_set_pixRGB(bmpNum, a.x + xk*step + step, a.y + itr*direction + direction, rgb);
				xk++;
				p+= diff;
			}
		}
	}
	return 0;

}

int bmp_draw_line(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb, int lineSize){
	if(!bmp_is_open(bmpNum))
		return -1;

	int direction = a.x > b.x ? -1 : 1;

	int dx = abs(b.x - a.x);
	int dy = abs(b.y - a.y);
	int ddy = 2*dy;
	int difference = ddy - 2*dx; //2dy - 2dx

	int p = ddy - dx;
	int itr;
	int yk = 0; 

	bmp_set_pixRGB(bmpNum, a.x, a.y, rgb);
	//small slope
	if(dx >= dy){
		int step = a.y > b.y ? -1:1;
		for(itr = 0; itr < dx; itr++){
			if(p<0){
				bmp_draw_circle(a.x + itr*direction + direction, a.y + yk*step, lineSize, bmpNum, rgb);
				p += ddy;
			}
			else{
				bmp_draw_circle(a.x + itr*direction + direction, a.y + yk*step + step, lineSize, bmpNum, rgb);
				yk++;
				p+= difference;
			}
		}
	}

	else{
		direction = a.y > b.y ? -1:1;
		int ddx = 2*dx;
		int diff = ddx - 2*dy;
		p = ddx - dy;
		int xk = 0;
		int step = a.x > b.x ? -1:1;
		for(itr = 0; itr < dy; itr++){
			if(p<0){
				bmp_draw_circle(a.x + xk*step, a.y + itr*direction + direction, lineSize, bmpNum, rgb);
				p += ddx;
			}
			else{
				bmp_draw_circle(a.x + xk*step + step, a.y + itr*direction + direction, lineSize, bmpNum, rgb);
				xk++;
				p+= diff;
			}
		}
	}
	return 0;

}

//bmp_draw_line_alpha
//function to draw a line using bresenham line-drawing algorithm with alpha
//value for transparency
//
int bmp_draw_line_alpha(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb, float alpha){
	if(!bmp_is_open(bmpNum))
		return -1;

	int direction = a.x > b.x ? -1 : 1;

	int dx = abs(b.x - a.x);
	int dy = abs(b.y - a.y);
	int ddy = 2*dy;
	int difference = ddy - 2*dx; //2dy - 2dx

	int p = ddy - dx;
	int itr;
	int yk = 0; 

	bmp_set_pixRGB(bmpNum, a.x, a.y, rgb);
	//small slope
	if(dx >= dy){
		int step = a.y > b.y ? -1:1;
		for(itr = 0; itr < dx; itr++){
			if(p<0){
				bmp_set_pixRGBA(bmpNum, a.x + itr*direction + direction, a.y + yk*step, rgb, alpha);
				p += ddy;
			}
			else{
				bmp_set_pixRGBA(bmpNum, a.x + itr*direction + direction, a.y + yk*step + step, rgb, alpha);
				yk++;
				p+= difference;
			}
		}
	}

	else{
		direction = a.y > b.y ? -1:1;
		int ddx = 2*dx;
		int diff = ddx - 2*dy;
		p = ddx - dy;
		int xk = 0;
		int step = a.x > b.x ? -1:1;
		for(itr = 0; itr < dy; itr++){
			if(p<0){
				bmp_set_pixRGBA(bmpNum, a.x + xk*step, a.y + itr*direction + direction, rgb, alpha);
				p += ddx;
			}
			else{
				bmp_set_pixRGBA(bmpNum, a.x + xk*step + step, a.y + itr*direction + direction, rgb, alpha);
				xk++;
				p+= diff;
			}
		}
	}
	return 0;

}



//bmp_draw_line
//function to draw a line using bresenham line-drawing algorithm
//
int bmp_draw_brush_line(const bmp_t bmpNum, const v2i a, const v2i b, const RGB_t rgb, const brush_t brush){
	if(!bmp_is_open(bmpNum))
		return -1;

	int direction = a.x > b.x ? -1 : 1;

	int dx = abs(b.x - a.x);
	int dy = abs(b.y - a.y);
	int ddy = 2*dy;
	int difference = ddy - 2*dx; //2dy - 2dx

	int p = ddy - dx;
	int itr;
	int yk = 0; 

	bmp_set_pixRGB(bmpNum, a.x, a.y, rgb);
	//small slope
	if(dx >= dy){
		int step = a.y > b.y ? -1:1;
		for(itr = 0; itr < dx; itr++){
			if(p<0){
				bmp_brush_stroke(a.x + itr*direction + direction, a.y + yk*step, bmpNum, rgb, brush);
				p += ddy;
			}
			else{
				bmp_brush_stroke(a.x + itr*direction + direction, a.y + yk*step + step, bmpNum, rgb, brush);
				yk++;
				p+= difference;
			}
		}
	}

	else{
		direction = a.y > b.y ? -1:1;
		int ddx = 2*dx;
		int diff = ddx - 2*dy;
		p = ddx - dy;
		int xk = 0;
		int step = a.x > b.x ? -1:1;
		for(itr = 0; itr < dy; itr++){
			if(p<0){
				bmp_brush_stroke(a.x + xk*step, a.y + itr*direction + direction, bmpNum, rgb, brush);
				p += ddx;
			}
			else{
				bmp_brush_stroke(a.x + xk*step + step, a.y + itr*direction + direction, bmpNum, rgb, brush);
				xk++;
				p+= diff;
			}
		}
	}
	return 0;

}




RGB_t bmp_get_avg_RGB(const bmp_t bmpNum, const int x, const int y, const int area){
	RGB_t avg;
	RGB_t rgbBuf;
	
	int xx, yy;
	int avgr = 0;
	int avgg = 0;
	int avgb = 0;
	int N = 0;
	for(yy = y - area; yy <= y + area; yy++){
		for(xx = x - area; xx <= x + area; xx++){
			if(bmp_get_pixRGB(bmpNum, xx, yy, &rgbBuf) != -1){
				avgr += rgbBuf.R;
				avgg += rgbBuf.G;
				avgb += rgbBuf.B;
				N++;
			}
		}
	}
	
	avg.R = avgr/N;
	avg.G = avgg/N;
	avg.B = avgb/N;

	return avg;

}


int bmp_brush_stroke(const int x, const int y, const int bmpNum, const RGB_t rgb, const brush_t brush){
	
	int xx, yy;
	int brushWidth = bmp_get_width(brush);
	int brushHeight = bmp_get_height(brush);
	RGB_t brushRGB, baseRGB ,RGBbuf;
	float ratio;

	for(yy = 0; yy < brushHeight; yy++){
		for(xx = 0; xx < brushWidth; xx++){
			bmp_get_pixRGB(brush, xx, yy, &brushRGB);
			bmp_get_pixRGB(bmpNum, x+xx-(brushWidth/2), y+yy-(brushHeight/2), &baseRGB);
			ratio = ((float)(255-brushRGB.R)/255);
			RGBbuf = bmp_interpolateRGB(rgb, baseRGB, ratio);
			bmp_set_pixRGB(bmpNum, x+xx-(brushWidth/2), y+yy-(brushHeight/2), RGBbuf); 
		}
	}	

	return 0;
}

int bmp_draw_circle(const int x, const int y, const int r, const int bmpNum, const RGB_t rgb){

	int xx;
	int yy;

	if (r == 0){
		return 0;
	}

	for(xx = -r; xx <=r; xx++){
		for(yy = -r; yy <=r; yy++){
			if(yy*yy+xx*xx <=  r*r){
				bmp_set_pixRGB(bmpNum, x+xx, y+yy, rgb);
			}
		}
	}
	return 0;
}

int bmp_load_brushes(){

	brush_t brush;
	int i;
	char toOpen[] = "./brushes/brush_.bmp"; //+2 for brush number and null char
	for(i = 0; i < BRUSH_COUNT; i++){
		toOpen[15] = i+49;
		brush = bmp_open(toOpen, 'r');
		//if error opening, close previously opened brushes and return
		if(brush == -1){
			int iReverse;
			for(iReverse = i-1; iReverse >= 0; iReverse--){
				bmp_close(brushList[iReverse]);
			}
			return -1;
		}
		brushList[i] = brush;
	}
	return 0;
}

//bmp_get_brush
//function to return the bmp index for the given brush number
//
//@ brushNum: the brush to get
//
//Return: the bmp index of the given brush number, -1 if it is not open or if
//	  brushNum is out of range
brush_t bmp_get_brush(const int brushNum){
	if(brushNum < 0 || brushNum > BRUSH_COUNT)
		return -1;

	if(bmp_is_open(brushList[brushNum])){
		return brushList[brushNum];
	}
	else
		return -1;
}


//bmp_close_brushes
//closes all open brushes
//
//Return: 0 if success, -1 otherwise
int bmp_close_brushes(void){
	int i;
	for(i = 0; i < BRUSH_COUNT; i++){
		if(bmp_close(brushList[i]) == -1){
			printf("ERROR: BMP_CLOSE_BRUSHES: failed to close brushes\n");
			return -1;
		}
		brushList[i] = -1;
	}
	
	return 0;

}

//bmp_RGB_to_HSV
//function to convert RGB to HSV
//
//@ rgb: rgb value to convert
//
//Return: HSV struct with resulting values
RGB_t bmp_HSV_to_RGB(HSV_t hsv){
	RGB_t rgb;

	if(hsv.s == 0){
		BYTE v = (BYTE)(hsv.v * 255);
		rgb = {v, v, v};
		return rgb;
	}
	hsv.h = (int)hsv.h % 360;

	int hi = hsv.h/60;
	float f = hsv.h/60.0f - hi;
	float p = hsv.v * (1.0f - hsv.s);
	float q = hsv.v * (1.0f - hsv.s * f);
	float t = hsv.v * (1.0f - hsv.s * (1.0f - f));

	switch (hi){
	
		case 0:
			rgb = {(BYTE)(hsv.v * 255), (BYTE)(t * 255), (BYTE)(p * 255)};
			break;
		case 1:
			rgb = {(BYTE)(q * 255), (BYTE)(hsv.v * 255), (BYTE)(p * 255)};
			break;
		case 2:
			rgb = {(BYTE)(p * 255), (BYTE)(hsv.v * 255), (BYTE)(t * 255)};
			break;
		case 3:
			rgb = {(BYTE)(p * 255), (BYTE)(q * 255), (BYTE)(hsv.v * 255)};
			break;
		case 4:
			rgb = {(BYTE)(t * 255), (BYTE)(p * 255), (BYTE)(hsv.v * 255)};
			break;
		case 5:
			rgb = {(BYTE)(hsv.v * 255), (BYTE)(p * 255), (BYTE)(q * 255)};
			break;
	}

	return rgb;

}

//returns the min of a and b
float minf(float a, float b){
	return a < b ? a : b;
}

//returns the max of a and b
float maxf(float a, float b){
	return a > b ? a : b;
}


//bmp_RGB_to_HSV
//function to convert rgb color space to hsv
//
//
HSV_t bmp_RGB_to_HSV(const RGB_t rgb){

	//rgbf to hold the normalized values of rgb
	v3f rgbf = {rgb.R/255.0f, rgb.G/255.0f, rgb.B/255.0f};
	float min = minf(rgbf.x, minf(rgbf.y, rgbf.z));
	float max = maxf(rgbf.x, maxf(rgbf.y, rgbf.z));
	HSV_t hsv;

	if(max == min)
		 hsv.h = 0;

	else if(max == rgbf.x){
	 	hsv.h = 60.0f * ((rgbf.y - rgbf.z)/(max - min));
	}
	else if(max == rgbf.y){
	 	hsv.h = 60.0f * (2 + (rgbf.z - rgbf.x)/(max - min));
	}
	else if(max == rgbf.z){ 
		hsv.h = 60.0f * (4 + (rgbf.x - rgbf.y)/(max - min));
	}

	if(hsv.h < 0){
	 	hsv.h += 360;
	}

	if(max == 0){
		hsv.s = 0;
	}
	else{
		hsv.s = (max-min)/max;
	}

	hsv.v = max;

	 return hsv;
}

int bmp_set_pixHSV(const bmp_t bmpNum, const int x, const int y, const HSV_t HSV){
	RGB_t RGB = bmp_HSV_to_RGB(HSV);
	return(bmp_set_pixRGB(bmpNum, x, y, RGB));
}

void bmp_grayscale(const bmp_t bmpNum){

	if(!bmp_is_open(bmpNum)){
		return;
	}

	int width = bmp_get_width(bmpNum);
	int height = bmp_get_height(bmpNum);
	RGB_t pixbuffrgb;
	HSV_t pixbuffhsv;

	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++){
			pixbuffrgb = bmp_get_avg_RGB(bmpNum,i,j, 3);
//			bmp_get_pixRGB(bmpNum, i, j, &pixbuffrgb);
			pixbuffhsv = bmp_RGB_to_HSV(pixbuffrgb);
			pixbuffhsv.h = 0;
			pixbuffhsv.s = 0;
			bmp_set_pixRGB(bmpNum, i, j, bmp_HSV_to_RGB(pixbuffhsv));
		}
	}
}


void bmp_twotone(const bmp_t bmpNum, const int lineSize){

	if(!bmp_is_open(bmpNum)){
		return;
	}

	bmp_grayscale(bmpNum);
	
	int width = bmp_get_width(bmpNum);
	int height = bmp_get_height(bmpNum);
	RGB_t pixbuff;
	float radius;

	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j+=(lineSize*2)){
			bmp_get_pixRGB(bmpNum, i, j, &pixbuff);
			radius = (1-((float)pixbuff.R/255))*lineSize;
			bmp_draw_circle(i, j, radius, bmpNum, BLACKRGB);
		}
	}
}

void bmp_twotone(const bmp_t src, const bmp_t dest, const int lineSize){

	if(!bmp_is_open(src) || !bmp_is_open(dest)){
		return;
	}



	bmp_grayscale(src);
	
	int width = bmp_get_width(src);
	int height = bmp_get_height(src);
	RGB_t pixbuff;
	float radius;

	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j+=(lineSize*2)-3){
			bmp_get_pixRGB(src, i, j, &pixbuff);
			radius = (1-((float)pixbuff.R/255))*lineSize;
			bmp_draw_circle(i, j, radius, dest, BLACKRGB);
		}
	}

}

//returns the variance between two color, higher return value = more different
float bmp_variance(const HSV_t src, const HSV_t other){
	float hv, sv, vv;

	hv = absf(src.h - other.h)/360.0f;
	sv = absf(src.s - other.s);
	vv = absf(src.v - other.v);
	return((hv+sv+vv)/3);

}


void bmp_random_init(void){
	RANDOMINIT = true;
	srand(time(0));
}

//returns a random integer from 0 to range-1
int bmp_random(int range){
	return(rand()%range);
}

//returns a random integer from [min,max]
int bmp_random(int min, int max){
	return(rand()%(max-min+1) + min);
}

float bmp_rand_normal(float mean, float stddev){
	//Box muller method
	static float n2 = 0.0;
	static int n2_cached = 0;
	if (!n2_cached){
		float x, y, r;
		do
		{
			x = 2.0*rand()/RAND_MAX - 1;
			y = 2.0*rand()/RAND_MAX - 1;
	
			r = x*x + y*y;
		}
		while (r == 0.0 || r > 1.0);
		{
			float d = sqrt(-2.0*log(r)/r);
			float n1 = x*d;
			n2 = y*d;
			float result = n1*stddev + mean;
			n2_cached = 1;
			return result;
		}
	}
	else
	{
		n2_cached = 0;
		return n2*stddev + mean;
	}
}


void bmp_mask(const bmp_t src, const bmp_t dest, const bmp_t mask){
	if(!bmp_is_open(src) || !bmp_is_open(dest) || !bmp_is_open(mask)){
		return;
	}

	int width = bmp_get_width(mask);
	int height = bmp_get_height(mask);

//	bmp_grayscale(mask);

	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			RGB_t buffMask, buffDest, buffSrc;
			bmp_get_pixRGB(mask, x, y, &buffMask);
			bmp_get_pixRGB(dest, x, y, &buffDest);
			bmp_get_pixRGB(src, x, y, &buffSrc);

			float alpha = 1 - ((float)buffMask.R/255.0f);
			RGB_t newPix = bmp_interpolateRGB(buffSrc, buffDest, alpha);
			bmp_set_pixRGB(dest, x, y, newPix);
		}
	}

}


template <typename T>
int count(T container[]){
	return(sizeof(container)/sizeof(*container[0]));
}

float csum(float arr[], const int len){
	int i;
	float sum = 0;
	for(i = 0; i < len; i++){
		sum += arr[i];
	}

	return sum;
}

v2i v2fTov2i(const v2f v){
	return {(int)v.x, (int)v.y};
}


///////CLASS SOURCE/////////
//

polygon::polygon(std::vector<v2f> verts, HSV_t col){
	vertices = verts;
	color = col;
	buffUpdate();
}

polygon::polygon(){
	color = BLACKHSV;
}

void polygon::buffUpdate(){
	if(buff != -1){
		bmp_close(buff);
	}	

	int xmin = vertices[0].x;
	int xmax = xmin;
	int ymin = vertices[0].y;
	int ymax = ymin;
	for(int i = 1; i < size(); i++){
		v2f vbuff = vertices[i];
		if(vbuff.x < xmin){
			xmin = vbuff.x;
		}
		if(vbuff.x > xmax){
			xmax = vbuff.x;
		}
		if(vbuff.y < ymin){
			ymin = vbuff.y;
		}
		if(vbuff.y > ymax){
			ymax = vbuff.y;
		}
	}

	if(ymin != 0){
		for(int i = 0; i < size(); i++){
			vertices[i].y -= ymin;
		}
		ymax -= ymin;
		ymin = 0;
	}

	if(xmin != 0){
		for(int i = 0; i < size(); i++){
			vertices[i].x -= xmin;
		}
		xmax -= xmin;
		xmin = 0;		
	}

	buff = bmp_new("./Images/shapes/shape.bmp", xmax, ymax);
}

v2f polygon::getCentroid(){
	v2f c;

	float x = 0;
	float y = 0;

	for(int i = 0; i < size(); i++){
		x += vertices[i].x;
		y += vertices[i].y;
	}

	c.x = x / size();
	c.y = y / size();

	return c;

}

void polygon::outline(){
	
	for(int i = 0; i < size() - 1; i++){
		bmp_draw_line(buff, v2fTov2i(vertices[i]), v2fTov2i(vertices[i+1]), bmp_HSV_to_RGB(color));
	}

	bmp_draw_line(buff, v2fTov2i(vertices[vertices.size()-1]), v2fTov2i(vertices[0]), bmp_HSV_to_RGB(color));
	bmp_write_out(buff);


}

void polygon::draw(bmp_t img, int x, int y){

	RGB_t colorBuff;
	v2f centroid = getCentroid();

	for(int i = 0; i < bmp_get_height(buff); i++){
		for(int j = 0; j < bmp_get_width(buff); j++){
			bmp_get_pixRGB(buff, j, i, &colorBuff);
			if(colorBuff.R == BLACKRGB.R){
				bmp_set_pixRGB(img, x - centroid.x + j, y - centroid.y + i, bmp_HSV_to_RGB(color));
			}
		}
	}


}


void polygon::fill(){
	
	outline();

	int scanline;

	int j;
	for(scanline = 0; scanline < bmp_get_height(buff); scanline++){
		std::vector<v2f> activeLines;
		std::vector<int> intersections;

		for(j = 0; j < size() - 1; j++){
			if((vertices[j].y > scanline && vertices[j+1].y <= scanline) ||
			   (vertices[j].y <= scanline && vertices[j+1].y > scanline)){
				activeLines.push_back(vertices[j]);
				activeLines.push_back(vertices[j+1]);
			}
		}
		if((vertices[size() - 1].y > scanline && vertices[0].y <= scanline)||
		   (vertices[size() - 1].y <= scanline && vertices[0].y > scanline)){
			activeLines.push_back(vertices[size() - 1]);
			activeLines.push_back(vertices[0]);
		}
	
		for(j = 0; j < (int)activeLines.size(); j+=2){
			float m = (activeLines[j+1].x - activeLines[j].x)/(activeLines[j+1].y - activeLines[j].y);
			float inter = ((float)scanline - activeLines[j].y) * m + activeLines[j].x;
			intersections.push_back((int)roundf(inter));
		}

		std::sort(intersections.begin(), intersections.end());

		std::vector<int>::iterator itr = intersections.begin();

		if(itr == intersections.end()){
			return;
		}

		for(j = *itr; (int)j < bmp_get_width(buff) && itr != intersections.end(); j++){
			//entering shape
			if(j == *itr){
				itr++;

				while(itr + 1 != intersections.end() && ( *(itr + 1) - *itr) == 1){
					itr++;
				}
				
				while(j < *itr){
					bmp_set_pixRGB(buff, j, scanline, BLACKRGB);
					j++;				
				}

				itr++;
			}
		}
	}

	bmp_write_out(buff);
}
void polygon::setColor(HSV_t col){
	color = col;
}

void polygon::scale(float factor){

	v2f centroid = getCentroid();

	for(int i = 0; i < size(); i++){
		vertices[i].x -= centroid.x;
		vertices[i].y -= centroid.y;
		vertices[i].x *= factor;
		vertices[i].y *= factor;
	}

	buffUpdate();

}
void polygon::rotate(float angle){
	
	v2f centroid = getCentroid();

	for(int i = 0; i < size(); i++){
		vertices[i].x -= centroid.x;
		vertices[i].y -= centroid.y;
		float prevx = vertices[i].x;
		float prevy = vertices[i].y;

		vertices[i].x = prevx * cos(angle) - prevy * sin(angle);
		vertices[i].y = prevx * sin(angle) + prevy * cos(angle);
	}

	buffUpdate();
};

int polygon::size(){
	return (int)vertices.size();
}
void polygon::addVertex(v2f v, int idx){
	vertices.insert(vertices.begin() + idx, v);
}
void polygon::removeVertex(int idx){
	vertices.erase(vertices.begin() + idx);
}
v2f polygon::getVertex(int idx){
	return(vertices[idx]);
}
void moveVertex(int idx, int x, int y);


//COLOR//


palette_t bmp_generate_palette(const int _numColors, paletteType _type){

	palette_t pal;
	pal.numColors = 0;
	if(_numColors <= 0){
		return pal;
	}

	if(!RANDOMINIT){
		bmp_random_init();
	}

	HSV_t *_colors = (HSV_t*)malloc(sizeof(HSV_t) * _numColors);
	if (_colors == NULL){
		return pal;	
	}

	int hue;
	float sat = bmp_rand_normal(.6, .1);
	float val = bmp_rand_normal(.6, .1);
	int i = 2;
	switch(_type){
		case mono:
			hue = bmp_random(360);
			_colors[0] = {(float)hue, .31, .18};
			_colors[1] = {(float)hue, .14, .98};
			for(; i < _numColors; i++){
				_colors[i].h = bmp_rand_normal(hue, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			break;

		case comp:
			hue = bmp_random(360);
			_colors[0] = {(float)hue, .31, .18};
			_colors[1] = {(float)hue, .14, .98};
			for(; i < _numColors/2; i++){	
				_colors[i].h = bmp_rand_normal(hue, 2);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}

			for(; i < _numColors; i++){	
				_colors[i].h = bmp_rand_normal((hue + 180) % 360, 2);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			break;

		case splitcomp:
			hue = bmp_random(360);
			_colors[0] = {(float)hue, .31, .18};
			_colors[1] = {(float)hue, .14, .98};
			for(; i < _numColors/3; i++){
				_colors[i].h = bmp_rand_normal(hue, 1);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors*2/3; i++){
				_colors[i].h = bmp_rand_normal((hue + 150) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors; i++){		
				_colors[i].h = bmp_rand_normal((hue + 210) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			break;

		case trio:
			hue = bmp_random(360);
			_colors[0] = {(float)hue, .31, .18};
			_colors[1] = {(float)hue, .14, .98};
			for(; i < _numColors/3; i++){
				_colors[i].h = bmp_rand_normal(hue, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors*2/3; i++){
				_colors[i].h = bmp_rand_normal((hue + 120) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors; i++){		
				_colors[i].h = bmp_rand_normal((hue + 240) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			break;

		case tetra:
			hue = bmp_random(360);
			_colors[0] = {(float)hue, .31, .18};
			_colors[1] = {(float)hue, .14, .98};
			for(; i < _numColors/4; i++){
				_colors[i].h = bmp_rand_normal(hue, 1);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors*2/4; i++){
				_colors[i].h = bmp_rand_normal((hue + 90) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors*3/4; i++){		
				_colors[i].h = bmp_rand_normal((hue + 180) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors; i++){		
				_colors[i].h = bmp_rand_normal((hue + 270) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}

			break;

		case analogous:
			hue = bmp_random(360);
			_colors[0] = {(float)hue, .31, .18};
			_colors[1] = {(float)hue, .14, .98};
			for(; i < _numColors/3; i++){
				_colors[i].h = bmp_rand_normal(hue, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors*2/3; i++){
				_colors[i].h = bmp_rand_normal((hue + 30) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			for(; i < _numColors; i++){		
				_colors[i].h = bmp_rand_normal((hue + -30) % 360, 5);
				_colors[i].s = bmp_rand_normal(sat, .1);
				_colors[i].v = bmp_rand_normal(val, .1);
			}
			break;

		default:
			break;	
	}



	pal.numColors = _numColors;
	pal.type = _type;
	pal.colors = _colors;

	return pal;

}

#include <math.h>
#include <algorithm>
#include <iostream>
#include "JMAP.h"


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

	buff = bmp_new("./Images/shapes/shape.bmp", xmax + 1, ymax + 1);
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
		bmp_draw_line(buff, v2fTov2i(vertices[i]), v2fTov2i(vertices[i+1]), BLACKRGB);
	}

	bmp_draw_line(buff, v2fTov2i(vertices[vertices.size()-1]), v2fTov2i(vertices[0]), BLACKRGB);
	bmp_write_out(buff);


}

void polygon::draw(bmp_t img, int x, int y){

	RGB_t colorBuff;
	v2f centroid = getCentroid();

	for(int i = 0; i < bmp_get_height(buff); i++){
		for(int j = 0; j < bmp_get_width(buff); j++){
			bmp_get_pixRGB(buff, j, i, &colorBuff);
			if(colorBuff.R != WHITERGB.R || colorBuff.G != WHITERGB.G || colorBuff.B != WHITERGB.B){
				bmp_set_pixRGB(img, x - centroid.x + j, y - centroid.y + i, colorBuff);
			}
		}
	}


}


void polygon::fill(){


	int scanline;
	RGB_t rgbCol = bmp_HSV_to_RGB(color);

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

		for(j = 0; j < (int)activeLines.size() - 1; j+=2){
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
					bmp_set_pixRGB(buff, j, scanline, rgbCol);
					j++;
				}

				itr++;
			}
		}
	}

	outline();

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
void polygon::moveVertex(int idx, int x, int y){
	vertices[idx].x += x;
	vertices[idx].y += y;
};

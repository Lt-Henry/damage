/*
	damage
	
	Copyright (C) 2016  Enrique Medina Gremaldos <quiqueiii@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "raster.hpp"
#include "math.hpp"

#include <iostream>
#include <cstring>

using namespace damage;
using namespace damage::math;
using namespace std;


Raster::Raster()
{
	frameBuffer=nullptr;
	depthBuffer=nullptr;
}


Raster::~Raster()
{
}


void Raster::Resize(int width,int height)
{
	this->width=width;
	this->height=height;
	
	frameBuffer=new uint32_t[width*height];
	depthBuffer=new uint16_t[width*height];
}


void Raster::Clear()
{
	memset(frameBuffer,0x00000000,width*height*sizeof(uint32_t));
	memset(depthBuffer,0x0000,width*height*sizeof(uint16_t));	
}


void Raster::Draw()
{
	float triangle[12];
	
	triangle[0]=5.0f;
	triangle[1]=5.0f;
	triangle[2]=5.0f;
	
	triangle[4]=width-5.0f;
	triangle[5]=5.0f;
	triangle[6]=5.0f;
	
	triangle[8]=5.0f;
	triangle[9]=height-5.0f;
	triangle[10]=5.0f;
	
	DrawTriangle(triangle);
	
	triangle[0]=5.0f;
	triangle[1]=height-5.0f;
	triangle[2]=5.0f;
	
	triangle[4]=width-5.0f;
	triangle[5]=5.0f;
	triangle[6]=5.0f;
	
	triangle[8]=width-5.0f;
	triangle[9]=height-5.0f;
	triangle[10]=5.0f;
	
	DrawTriangle(triangle);
	
}


void Raster::DrawTriangle(const float* data)
{

	int v0[2];
	int v1[2];
	int v2[2];
	
	v0[0]=data[0];
	v1[0]=data[4];
	v2[0]=data[8];
	
	v0[1]=data[1];
	v1[1]=data[5];
	v2[1]=data[9];

	int minx,miny;
	int maxx,maxy;
	
	minx=std::min(v0[0],v1[0]);
	minx=std::min(minx,v2[0]);
	
	miny=std::min(v0[1],v1[1]);
	miny=std::min(miny,v2[1]);
	
	maxx=std::max(v0[0],v1[0]);
	maxx=std::max(maxx,v2[0]);
	
	maxy=std::max(v0[1],v1[1]);
	maxy=std::max(maxy,v2[1]);
	
	int area=v2i::orient(v0,v1,v2);
	
	for (int y=miny;y<=maxy;y++) {
		for (int x=minx;x<=maxx;x++) {
			
			int c[2];
			
			c[0]=x;
			c[1]=y;
			
			int w0,w1,w2;
			
			w0=v2i::orient(v1,v2,c);
			w1=v2i::orient(v2,v0,c);
			w2=v2i::orient(v0,v1,c);
			
			if (w0>0 and w1>0 and w2>0) {
				uint16_t z = data[2];
				uint16_t Z = depthBuffer[x+y*width];
				
				if (z>Z) {
					depthBuffer[x+y*width]=z;
					frameBuffer[x+y*width]=0xff0000ff;
				}
			}
		}
	}

}

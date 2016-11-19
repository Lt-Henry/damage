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
	
	frameBuffer=new uint8_t[width*height*4];
	depthBuffer=new uint16_t[width*height*4];
}


void Raster::Clear()
{
	memset(frameBuffer,0x00,width*height*4);
	memset(depthBuffer,0x0000,width*height*2);	
}


void Raster::Draw()
{
	for (int j=0;j<height;j++) {
		for (int i=0;i<width;i++) {
	
			frameBuffer[(i*4)+j*width*4]=0xffAA55ff;
		}
	}
}



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

#include "texture.hpp"

#include <iostream>

using namespace damage;
using namespace std;

Texture::Texture(int width,int height)
{
	owner=true;
	
	size=width*height;

	this->width=width;
	this->height=height;
	
	data = new uint32_t[size];
}


Texture::~Texture()
{
	if (owner) {
		cout<<"destroying texture..."<<endl;
		delete data;
	}
}


uint32_t Texture::Pixel(int x,int y)
{
	return data[x+y*width];
}

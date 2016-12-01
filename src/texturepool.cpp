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

#include "texturepool.hpp"

#include <stdexcept>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace damage;
using namespace std;


Texture* TexturePool::Load(string filename)
{
	SDL_Surface* surface=nullptr;
	Texture* texture;
	
	string path = basedir+"/"+filename;
	
	surface = IMG_Load(path.c_str());
	
	if (surface==nullptr) {
		throw runtime_error("IMG_Load error");
	}
	
	texture = new Texture(surface->w,surface->h);
	
	for (int j=0;j<surface->h;j++) {
		for (int i=0;i<surface->w;i++) {
			uint32_t* source = (uint32_t*)surface->pixels;
			
			uint32_t pixel = source[i+j*surface->w];
			
			uint8_t r,g,b,a;
			
			SDL_GetRGBA(pixel,surface->format,&r,&g,&b,&a);
			
			/*
			texture->data[((i*4)+(j*surface->w*4))+0]=a;
			texture->data[((i*4)+(j*surface->w*4))+1]=r;
			texture->data[((i*4)+(j*surface->w*4))+2]=g;
			texture->data[((i*4)+(j*surface->w*4))+3]=b;
			*/
			
			pixel = (a<<24) | (r<<16) | (g<<8) | b;
			texture->data[i+j*surface->w]=pixel;
		}
	}
	
	
	SDL_FreeSurface(surface);
	
	return texture;
}


TexturePool::TexturePool(string basedir)
{
	this->basedir=basedir;
}


TexturePool::~TexturePool()
{
}


Texture* TexturePool::Get(string name)
{
	if (textures.find(name)==textures.end()) {
		cout<<"Loading: "<<name<<endl;
		textures[name] = Load(name);
	}
	
	return textures[name];
}

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
#include "mesh.hpp"

#include <iostream>

#include <SDL2/SDL.h>

using namespace damage;
using namespace std;

#define WIDTH TILE_SIZE*6
#define HEIGHT TILE_SIZE*5


int main(int argc,char* argv[])
{

	cout<<"damage project"<<endl;
	
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	
	// Simple SDL2 initialization
	
	cout<<"* resolution: "<<WIDTH<<"x"<<HEIGHT<<endl;
	
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("damage", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH,HEIGHT);
	
	Raster raster(3);
	
	raster.Resize(texture,WIDTH/TILE_SIZE,HEIGHT/TILE_SIZE);
	raster.Frustum(-0.012f,0.012f,-0.01f,0.01f,0.1f,1000.0f);
	
	Mesh mesh("crate.mesh");
	
	//main loop
	
	bool quit_request=false;
	uint32_t t1,t2,dt;
	
	int fpcount = 0;
	int fps = 0;
	
	t1=SDL_GetTicks();
	
	while (!quit_request) {
	
	
		SDL_Event event;
		
		while(SDL_PollEvent(&event)) {
		
			switch (event.type) {
				case SDL_QUIT:
					cout<<"Quit request"<<endl;
					quit_request=true;
				break;
				
			} // switch
		} // while
		
		// pixel drawing
		
		
		raster.Update();
		
		//SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		
		fpcount++;
		
		t2=SDL_GetTicks();
		
		dt = t2-t1;
		
		if (dt>1000) {
			t1=t2;
			cout<<"fps: "<<fpcount<<endl;
			fpcount=0;
		}

		
	} // while
	
	return 0;
}

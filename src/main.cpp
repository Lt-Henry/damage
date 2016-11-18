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

#include <iostream>

#include <SDL2/SDL.h>

using namespace std;

#define WIDTH 1280
#define HEIGHT 720


int main(int argc,char* argv[])
{

	cout<<"damage project"<<endl;
	
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture * texture;
	
	// Simple SDL2 initialization
	
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("damage", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH,HEIGHT);
	
	uint8_t* buffer = new uint8_t [WIDTH*HEIGHT*4];
	
	//main loop
	
	bool quit_request=false;
	uint32_t t1,t2,dt;
	
	uint32_t begin,end;
	uint32_t ms_z1=0;
	uint32_t ms_z2=0;
	
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
		begin=SDL_GetTicks();
		
		for (int j=0;j<HEIGHT;j++) {
			for (int i=0;i<WIDTH;i++) {
			
				buffer[(i*4)+j*WIDTH*4]=0xffAA55ff & t1;
			}
		}
		
		end=SDL_GetTicks();
		
		ms_z1=end-begin;
		
		begin=SDL_GetTicks();
		SDL_UpdateTexture(texture,NULL,(void*)buffer,WIDTH*4);
		end=SDL_GetTicks();
		
		ms_z2=end-begin;
		
		//SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		
		fpcount++;
		
		t2=SDL_GetTicks();
		
		dt = t2-t1;
		
		if (dt>1000) {
			t1=t2;
			cout<<"fps: "<<fpcount<<" z1: "<<ms_z1<<" z2: "<<ms_z2<<endl;
			fpcount=0;
			ms_z1=0;
			ms_z2=0;
		}

		
	} // while
	
	return 0;
}

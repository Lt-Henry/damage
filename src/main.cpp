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
#include "math.hpp"
#include "texturepool.hpp"

#include <iostream>
#include <chrono>

#include <SDL2/SDL.h>

using namespace damage;
using namespace damage::math;
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
	
	Raster raster(1);
	
	raster.Resize(texture,WIDTH/TILE_SIZE,HEIGHT/TILE_SIZE);
	raster.Frustum(-1.0f,1.0f,-1.0f,1.0f,1.00f,100.0f);
	
	TexturePool pool(".");
	Mesh mesh("bolt.mesh",&pool);
	
	//main loop
	
	bool quit_request=false;
	uint32_t t1,t2,dt;
	std::chrono::steady_clock::time_point begin,end;
	uint32_t ns[5]={0,0,0,0,0};
	
	int fpcount = 0;
	int fps = 0;
	
	float mR[16];
	float mT[16];
	
	float phi=0.0f;
	
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
		
		phi+=0.005f;
		
		// move mesh
		m4f::Translation(mT,0.0f,0.0f,3.0f);
		m4f::RotationY(mR,phi);
		
		m4f::Mult(mesh.matrix,mR,mT);
		
		begin = std::chrono::steady_clock::now();
		raster.Draw(&mesh);
		end = std::chrono::steady_clock::now();
		ns[0]+=std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		
		begin = std::chrono::steady_clock::now();
		raster.Update();
		end = std::chrono::steady_clock::now();
		ns[1]+=std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		
		begin = std::chrono::steady_clock::now();
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		end = std::chrono::steady_clock::now();
		ns[2]+=std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		
		fpcount++;
		
		t2=SDL_GetTicks();
		
		dt = t2-t1;
		
		if (dt>1000) {
			t1=t2;
			cout<<"fps: "<<fpcount<<endl;
			cout<<"Average time per frame"<<endl;
			uint32_t factor=1000000*fpcount;
			cout<<"* Draw():       "<<ns[0]/factor<<" ms"<<endl;
			cout<<"* Update():     "<<ns[1]/factor<<" ms"<<endl;
			cout<<"* RenderCopy(): "<<ns[2]/factor<<" ms"<<endl;
			
			fpcount=0;
			ns[0]=0;
			ns[1]=0;
			ns[2]=0;
		}

		
	} // while
	
	return 0;
}

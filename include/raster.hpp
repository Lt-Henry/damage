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


#ifndef _DAMAGE_RASTER_
#define _DAMAGE_RASTER_

#include <cstdint>
#include <thread>
#include <mutex>
#include <queue>


#define TILE_SIZE 64

namespace damage
{


	enum class CommandType {
		None,
		Draw,
		Clear,
		Exit
	};
	
	
	class Command {
	public:
		int tx;
		int ty;
		CommandType type;
	};
	
	
	class Tile {
	public:
		int x;
		int y;
	
		uint32_t* frameBuffer;
		uint16_t* depthBuffer;
		
		Tile(int x,int y);
		~Tile();
	};
	
	
	class Raster {
	public:
	
		std::mutex cmdMutex;
		std::queue<Command> cmdQueue;
		std::thread workers[16];
		
		/*! screen dimensions in pixels */
		int width;
		int height;
		
		/*! screen dimensions in tiles */
		int tilesW;
		int tilesH;
		
		Tile* tiles;
	
		uint32_t** frameBuffer;
		uint16_t** depthBuffer;
	
		Raster();
		~Raster();
		
		void Resize(int tilesW,int tilesH);
		
		void Clear();
		
		void Worker();
		void Draw();
		
		void DrawTriangle(const float* data,int tx,int ty);
	};
}

#endif

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

#include "mesh.hpp"
#include "buffer.hpp"

#include <cstdint>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>

#include <SDL2/SDL.h>

/*! tile size */
#define TILE_SIZE 128

/*! number of triangles */
#define VBO_SIZE 960000

namespace damage
{



	class Tile {
	public:
		int x;
		int y;
	
		uint32_t* frameBuffer;
		uint16_t* depthBuffer;
		
		Buffer<uint32_t>* bin;
		uint32_t triangles;
		
		Tile(int x,int y);
		~Tile();
	};
	
	
	enum class CommandType {
		None,
		Draw,
		Clear,
		Upload,
		Exit
	};
	
	
	class Command {
	public:
		Tile* tile;
		CommandType type;
		
		Command();
		Command(CommandType type,Tile* tile);
	};
	
	
	class Raster {
	private:
		
		std::mutex cmdMutex;
		std::queue<Command> cmdQueue;
		std::mutex commitMutex;
		std::queue<Command> commitQueue;
		
		std::vector<std::thread> workers;
		
		/*! thread worker */
		void Worker();
		
		/*! vbo */
		int numTriangles;
		Buffer<float>* vertices;
		Buffer<float>* normals;
		Buffer<float>* uvs;
		Buffer<Texture*>* textures;
	
	public:
	
		/*! frustum */
		float left,right,top,bottom,near,far;
		float mProjection[16];
		float mViewport[16];
		
		/*! target texture */
		SDL_Texture* renderTarget;
		
		/*! screen dimensions in pixels */
		int screenWidth;
		int screenHeight;
		
		/*! screen dimensions in tiles */
		int numTilesWidth;
		int numTilesHeight;
		
		/*! tiles vector */
		std::vector<Tile*> tiles;
	
		Raster(int numThreads=1);
		~Raster();
		
		void Resize(SDL_Texture* texture,int numTilesWidth,int numTilesHeight);
		
		void Frustum(float left,float right,float top,float bottom,float near,float far);
		
		void Clear();
		
		void Draw(Mesh* mesh);
		
		void Update();
		
		void DrawTriangle(int index,Tile* tile);
	};
}

#endif

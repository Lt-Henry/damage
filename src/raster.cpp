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


Tile::Tile(int x,int y)
{
	this->x=x;
	this->y=y;
	
	frameBuffer=new uint32_t[TILE_SIZE*TILE_SIZE];
	depthBuffer=new uint16_t[TILE_SIZE*TILE_SIZE];
}


Tile::~Tile()
{
	delete frameBuffer;
	delete depthBuffer;
}


Command::Command()
{
}


Command::Command(CommandType type,Tile* tile)
{
	this->type=type;
	this->tile=tile;
}


Raster::Raster(int numThreads)
{
	// spawn workers
	for (int n=0;n<numThreads;n++) {
		workers.push_back(thread(&Raster::Worker,this));
	}
}


Raster::~Raster()
{
	Command cmd(CommandType::Exit,nullptr);

	cmdMutex.lock();
	
	// broadcast exit signal to workers
	for (int n=0;n<workers.size();n++) {
		cmdQueue.push(cmd);
		cmdQueue.push(cmd);
	}
	
	cmdMutex.unlock();
	
	// wait for workers
	for (int n=0;n<workers.size();n++) {
		workers[n].join();
	}
}


void Raster::Resize(SDL_Texture* texture,int numTilesWidth,int numTilesHeight)
{
	this->texture=texture;
	this->numTilesWidth=numTilesWidth;
	this->numTilesHeight=numTilesHeight;
	this->screenWidth=numTilesWidth*TILE_SIZE;
	this->screenHeight=numTilesHeight*TILE_SIZE;
	
	// create tiles
	// TODO: erase previously created tiles
	for (int ty=0;ty<numTilesHeight;ty++) {
		for (int tx=0;tx<numTilesWidth;tx++) {

			tiles.push_back(new Tile(tx*TILE_SIZE,ty*TILE_SIZE));
		}
	}
	
}


void Raster::Frustum(float left,float right,float top,float bottom,float near,float far)
{
	this->left=left;
	this->right=right;
	this->top=top;
	this->bottom=bottom;
	this->near=near;
	this->far=far;
}


void Raster::Clear()
{
	// queue clear commands for each tile
	cmdMutex.lock();
	for (Tile* tile : tiles) {
		cmdQueue.push(Command(CommandType::Clear,tile));
	}
	cmdMutex.unlock();
	
}


void Raster::Worker()
{
	cout<<"* Worker enter"<<endl;
	
	float triangle[12];
	
	bool quitRequest=false;
	
	while (!quitRequest) {

		Command cmd(CommandType::None,nullptr);
		
		cmdMutex.lock();
		
		if (cmdQueue.size()>0) {
			cmd = cmdQueue.front();
			cmdQueue.pop();
		}
		
		cmdMutex.unlock();
		
		switch (cmd.type) {
			case CommandType::Draw:
				
				triangle[0]=5.0f;
				triangle[1]=5.0f;
				triangle[2]=5.0f;
	
				triangle[4]=screenWidth-5.0f;
				triangle[5]=5.0f;
				triangle[6]=5.0f;
	
				triangle[8]=5.0f;
				triangle[9]=screenHeight-5.0f;
				triangle[10]=5.0f;
	
				DrawTriangle(triangle,cmd.tile);
	
				triangle[0]=5.0f;
				triangle[1]=screenHeight-5.0f;
				triangle[2]=5.0f;
	
				triangle[4]=screenWidth-5.0f;
				triangle[5]=5.0f;
				triangle[6]=5.0f;
	
				triangle[8]=screenWidth-5.0f;
				triangle[9]=screenHeight-5.0f;
				triangle[10]=5.0f;
	
				DrawTriangle(triangle,cmd.tile);
				
				commitMutex.lock();
				commitQueue.push(cmd);
				commitMutex.unlock();
				
			break;
			
			case CommandType::Clear:
				memset(cmd.tile->frameBuffer,0xff,TILE_SIZE*TILE_SIZE*sizeof(uint32_t));
				memset(cmd.tile->depthBuffer,0x00,TILE_SIZE*TILE_SIZE*sizeof(uint16_t));
				
				commitMutex.lock();
				commitQueue.push(cmd);
				commitMutex.unlock();
				
			break;
			
			case CommandType::Exit:
				quitRequest=true;
				
				commitMutex.lock();
				commitQueue.push(cmd);
				commitMutex.unlock();
				
			break;
		}
	}
	
	cout<<"* Worker exit"<<endl;
}


void Raster::Draw()
{
	// broadcast draw command
	cmdMutex.lock();
	for (Tile* tile : tiles) {
		cmdQueue.push(Command(CommandType::Draw,tile));
	}
	cmdMutex.unlock();
	
}


void Raster::Update()
{

	int uploaded=0;
	
	Clear();
	
	while (uploaded<tiles.size()) {
	
		Command cmd(CommandType::None,nullptr);
		
		commitMutex.lock();
		if (commitQueue.size()>0) {
			cmd=commitQueue.front();
			commitQueue.pop();
		}
		commitMutex.unlock();
		
		switch (cmd.type) {
		
			case CommandType::Clear:
				cmdMutex.lock();
				cmdQueue.push(Command(CommandType::Draw,cmd.tile));
				cmdMutex.unlock();
			break;
			
			case CommandType::Draw:
				SDL_Rect rect;
				
				rect.x=cmd.tile->x;
				rect.y=cmd.tile->y;
				rect.w=TILE_SIZE;
				rect.h=TILE_SIZE;
				
				SDL_UpdateTexture(texture,&rect,(void*)cmd.tile->frameBuffer,TILE_SIZE*sizeof(uint32_t));
				uploaded++;
			break;
		}
	}

}


static int orient(const int* a,const int* b,const int* c)
{
	return (b[0]-a[0])*(c[1]-a[1]) - (b[1]-a[1])*(c[0]-a[0]);
}


void Raster::DrawTriangle(const float* data,Tile* tile)
{

	int screenLeft = tile->x;
	int screenRight = tile->x+TILE_SIZE;
	int screenTop = tile->y;
	int screenBottom = tile->y+TILE_SIZE;

	int v0[2];
	int v1[2];
	int v2[2];
	
	v0[0]=data[0] - screenLeft;
	v1[0]=data[4] - screenLeft;
	v2[0]=data[8] - screenLeft;
	
	v0[1]=data[1] - screenTop;
	v1[1]=data[5] - screenTop;
	v2[1]=data[9] - screenTop;

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
	
	minx=std::max(minx,0);
	maxx=std::min(maxx,TILE_SIZE-1);
	
	miny=std::max(miny,0);
	maxy=std::min(maxy,TILE_SIZE-1);
	
	if (maxx < minx or maxy < miny) {
		return;
	}
	
	int area=orient(v0,v1,v2);
	
	for (int y=miny;y<=maxy;y++) {
		for (int x=minx;x<=maxx;x++) {
			
			int c[2];
			
			c[0]=x;
			c[1]=y;
			
			int w0,w1,w2;
			
			w0=orient(v1,v2,c);
			w1=orient(v2,v0,c);
			w2=orient(v0,v1,c);
			
			if (w0>=0 and w1>=0 and w2>=0) {
				uint16_t z = data[2];
				uint16_t Z = tile->depthBuffer[x+y*TILE_SIZE];
				
				if (z>Z) {
					tile->depthBuffer[x+y*TILE_SIZE]=z;
					tile->frameBuffer[x+y*TILE_SIZE]=0xff99aa11;
				}
			}
		}
	}

}

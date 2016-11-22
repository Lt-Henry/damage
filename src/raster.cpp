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


Command::Command(CommandType type,Tile* tile)
{
	this->type=type;
	this->tile=tile;
}


Raster::Raster(int numThreads)
{
	
	for (int n=0;n<numThreads;n++) {
		workers.push_back(thread(&Raster::Worker,this));
	}
}


Raster::~Raster()
{
	Command cmd;

	cmd.type=CommandType::Exit;
	cmdMutex.lock();
	cmdQueue.push(cmd);
	cmdQueue.push(cmd);
	cmdMutex.unlock();
}


void Raster::Resize(int tilesW,int tilesH)
{
	this->tilesW=tilesW;
	this->tilesH=tilesH;
	this->width=tilesW*64;
	this->height=tilesH*64;
	
	frameBuffer=new uint32_t*[tilesW*tilesH];
	depthBuffer=new uint16_t*[tilesW*tilesH];
	
	for (int n=0;n<(tilesW*tilesH);n++) {
		frameBuffer[n]=new uint32_t[64*64];
		depthBuffer[n]=new uint16_t[64*64];
	}
}


void Raster::Clear()
{

	
	for (int ty=0;ty<tilesH;ty++) {
		for (int tx=0;tx<tilesW;tx++) {
			Command cmd;
			
			cmd.tx=tx;
			cmd.ty=ty;
			cmd.type=CommandType::Clear;
			cmdMutex.lock();
			cmdQueue.push(cmd);
			cmdMutex.unlock();
		}
	}

}


void Raster::Worker()
{
	cout<<"* Worker enter"<<endl;
	
	float triangle[12];
	
	bool quitRequest=false;
	
	while (!quitRequest) {

		Command cmd;
		
		cmdMutex.lock();
		
		if (cmdQueue.size()>0) {
			cmd = cmdQueue.front();
			cmdQueue.pop();
		}
		else {
			cmd.type=CommandType::None;
		}
		
		cmdMutex.unlock();
		
		switch (cmd.type) {
			case CommandType::Draw:
				
	
				triangle[0]=5.0f;
				triangle[1]=5.0f;
				triangle[2]=5.0f;
	
				triangle[4]=width-5.0f;
				triangle[5]=5.0f;
				triangle[6]=5.0f;
	
				triangle[8]=5.0f;
				triangle[9]=height-5.0f;
				triangle[10]=5.0f;
	
				DrawTriangle(triangle,cmd.tx,cmd.ty);
	
				triangle[0]=5.0f;
				triangle[1]=height-5.0f;
				triangle[2]=5.0f;
	
				triangle[4]=width-5.0f;
				triangle[5]=5.0f;
				triangle[6]=5.0f;
	
				triangle[8]=width-5.0f;
				triangle[9]=height-5.0f;
				triangle[10]=5.0f;
	
				DrawTriangle(triangle,cmd.tx,cmd.ty);
			break;
			
			case CommandType::Clear:
				memset(frameBuffer[cmd.tx+cmd.ty*tilesW],0x00000000,64*64*sizeof(uint32_t));
				memset(depthBuffer[cmd.tx+cmd.ty*tilesW],0x0000,64*64*sizeof(uint16_t));
			break;
			
			case CommandType::Exit:
				quitRequest=true;
			break;
		}
	}
	
	cout<<"* Worker exit"<<endl;
}


void Raster::Draw()
{

	for (int ty=0;ty<tilesH;ty++) {
		for (int tx=0;tx<tilesW;tx++) {
		
			Command cmd;
			
			cmd.tx=tx;
			cmd.ty=ty;
			cmd.type=CommandType::Draw;
			cmdMutex.lock();
			cmdQueue.push(cmd);
			cmdMutex.unlock();

		}
	
	}
	
	bool ready=false;
	
	while (!ready) {
		cmdMutex.lock();
		ready=cmdQueue.size()==0;
		cmdMutex.unlock();
	}
	
}


static int orient(const int* a,const int* b,const int* c)
{
	return (b[0]-a[0])*(c[1]-a[1]) - (b[1]-a[1])*(c[0]-a[0]);
}


void Raster::DrawTriangle(const float* data,int tx,int ty)
{

	int screenLeft = tx*64;
	int screenRight = screenLeft+64;
	int screenTop = ty*64;
	int screenBottom = screenTop+64;

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
	maxx=std::min(maxx,63);
	
	miny=std::max(miny,0);
	maxy=std::min(maxy,63);
	
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
				uint16_t Z = depthBuffer[tx+ty*tilesW][x+y*64];
				
				if (z>Z) {
					depthBuffer[tx+ty*tilesW][x+y*64]=z;
					frameBuffer[tx+ty*tilesW][x+y*64]=0xffaa3300;
				}
			}
		}
	}

}

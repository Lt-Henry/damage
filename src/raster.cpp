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

	m4f::Identity(mProjection);
	m4f::Identity(mViewport);
	
	// vbo
	const int vboSize=100000;
	
	vertices=new float[12*vboSize];
	normals=new float[12*vboSize];
	uvs=new float[6*vboSize];
	textures=new Texture*[vboSize];
	
	numTriangles=0;

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
	
	delete vertices;
	delete normals;
	delete uvs;
	delete textures;
}


void Raster::Resize(SDL_Texture* texture,int numTilesWidth,int numTilesHeight)
{
	this->renderTarget=texture;
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
	
	// viewport matrix
	mViewport[0]=screenWidth/2.0f;
	mViewport[1]=0.0f;
	mViewport[2]=0.0f;
	mViewport[3]=screenWidth/2.0f;
	
	mViewport[4]=0.0f;
	mViewport[5]=-screenHeight/2.0f;
	mViewport[6]=0.0f;
	mViewport[7]=screenHeight/2.0f;
	
	mViewport[8]=0.0f;
	mViewport[9]=0.0f;
	mViewport[10]=1.0f;
	mViewport[11]=0.0f;
	
	mViewport[12]=0.0f;
	mViewport[13]=0.0f;
	mViewport[14]=0.0f;
	mViewport[15]=1.0f;
}


void Raster::Frustum(float left,float right,float top,float bottom,float near,float far)
{
	this->left=left;
	this->right=right;
	this->top=top;
	this->bottom=bottom;
	this->near=near;
	this->far=far;

	mProjection[0]=(2.0f*near)/(right-left);
	mProjection[1]=0.0f;
	mProjection[2]=(right+left)/(right-left);
	mProjection[3]=0.0f;
	
	mProjection[4]=0.0f;
	mProjection[5]=(2.0f*near)/(top-bottom);
	mProjection[6]=(top+bottom)/(top-bottom);
	mProjection[7]=0.0f;

	mProjection[8]=0.0f;
	mProjection[9]=0.0f;
	mProjection[10]=-(far+near)/(far-near);
	mProjection[11]=-(2.0f*far*near)/(far-near);

	mProjection[12]=0.0f;
	mProjection[13]=0.0f;
	mProjection[14]=-1.0f;
	mProjection[15]=0.0f;
	
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
	
	float* source;
	
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
				
				
				for (int n=0;n<this->numTriangles;n++) {
				
					DrawTriangle(n,cmd.tile);
				}
				
				
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


void Raster::Draw(Mesh* mesh)
{

	float matrix[16];
	float m1[16];
	float m2[16];
	
	m4f::Set(m1,mesh->matrix);
	m4f::Mult(m2,m1,mProjection);
	m4f::Mult(matrix,m2,mViewport);
	
	float* vDest=this->vertices;
	float* vSource=mesh->vertices;
	float* uvDest=this->uvs;
	float* uvSource=mesh->uvs;
	Texture** tDest=this->textures;
	Texture** tSource=mesh->textures;
	
	
	
	for (int n=0;n<mesh->size;n++) {
	
		for (int m=0;m<3;m++) {
		
			v4f::Mult(vDest,vSource,matrix);
		
			vDest[0]=vDest[0]/vDest[3];
			vDest[1]=vDest[1]/vDest[3];
		
			uvDest[0]=uvSource[0];
			uvDest[1]=uvSource[1];
		
			vDest+=4;
			vSource+=4;
			uvDest+=2;
			uvSource+=2;
		}
		
		tDest[0]=tSource[0];
		tDest++;
		tSource++;
		
		this->numTriangles++;
	
	}
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
				
				SDL_UpdateTexture(renderTarget,&rect,(void*)cmd.tile->frameBuffer,TILE_SIZE*sizeof(uint32_t));
				uploaded++;
			break;
		}
	}
	
	
	// reset vbo
	this->numTriangles=0;

}


static int orient(const int* a,const int* b,const int* c)
{
	return (b[0]-a[0])*(c[1]-a[1]) - (b[1]-a[1])*(c[0]-a[0]);
}


void Raster::DrawTriangle(int index,Tile* tile)
{

	int screenLeft = tile->x;
	int screenRight = tile->x+TILE_SIZE;
	int screenTop = tile->y;
	int screenBottom = tile->y+TILE_SIZE;

	int v0[2];
	int v1[2];
	int v2[2];
	
	float* vData=vertices+(index*12);
	float* uvData=uvs+(index*6);
	Texture** tData=textures+index;
	
	v0[0]=vData[0] - screenLeft;
	v1[0]=vData[4] - screenLeft;
	v2[0]=vData[8] - screenLeft;
	
	v0[1]=vData[1] - screenTop;
	v1[1]=vData[5] - screenTop;
	v2[1]=vData[9] - screenTop;

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
			
			if ((w0 | w1 | w2)>=0) {
				uint16_t z = 0xffff-((vData[2]-near)/(far-near))*0xffff;
				uint16_t Z = 0xffff-tile->depthBuffer[x+y*TILE_SIZE];
				
				
				// slow as hell!
				float b0,b1,b2;
				
				b0=w0/(float)area;
				b1=w1/(float)area;
				b2=w2/(float)area;
				
				float u = uvData[0]*b0 + uvData[2]*b1 + uvData[4]*b2;
				float v = uvData[1]*b0 + uvData[3]*b1 + uvData[5]*b2;
				
				int tx=u*tData[0]->width;
				int ty=v*tData[0]->height;
				
				tx=tx%tData[0]->width;
				ty=ty%tData[0]->height;
				
				
				uint32_t pixel = tData[0]->Pixel(tx,ty);
				//uint32_t pixel=0xffff0000;
				
				if (z<Z) {
					tile->depthBuffer[x+y*TILE_SIZE]=z;
					tile->frameBuffer[x+y*TILE_SIZE]=pixel;
				}
			}
		}
	}

}

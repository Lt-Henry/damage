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
#include <algorithm>

using namespace damage;
using namespace damage::math;
using namespace std;


Tile::Tile(int x,int y)
{
	this->x=x;
	this->y=y;
	
	frameBuffer=new uint32_t[TILE_SIZE*TILE_SIZE];
	depthBuffer=new uint16_t[TILE_SIZE*TILE_SIZE];
	
	bin = new Buffer<uint32_t>(VBO_SIZE);
	triangles=0;
}


Tile::~Tile()
{
	delete frameBuffer;
	delete depthBuffer;
	
	delete bin;
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
	vertices=new Buffer<float>(12*VBO_SIZE);
	normals=new Buffer<float>(12*VBO_SIZE);
	uvs=new Buffer<float>(6*VBO_SIZE);
	textures=new Buffer<Texture*>(VBO_SIZE);
	
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
				
				
				for (int n=0;n<cmd.tile->triangles;n++) {
					DrawTriangle(cmd.tile->bin->data[n],cmd.tile);
				}
				cmd.tile->triangles=0;
				
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
	
	float* vDest=vertices->data;
	float* vSource=mesh->vertices->data;
	float* uvDest=uvs->data;
	float* uvSource=mesh->uvs->data;
	Texture** tDest=textures->data;
	Texture* tSource=mesh->texture;
	
	
	
	for (int n=0;n<mesh->size;n++) {
	
		v4f::Mult(vDest,vSource,matrix);
		v4f::Mult(vDest+4,vSource+4,matrix);
		v4f::Mult(vDest+8,vSource+8,matrix);
		
		float rz0,rz1,rz2;
		
		rz0=1.0f/vDest[3];
		rz1=1.0f/vDest[7];
		rz2=1.0f/vDest[11];
		
		vDest[0]*=rz0;
		vDest[1]*=rz0;
		vDest[4]*=rz1;
		vDest[5]*=rz1;
		vDest[8]*=rz2;
		vDest[9]*=rz2;
		
		uvDest[0]=uvSource[0]*rz0;
		uvDest[1]=uvSource[2]*rz1;
		uvDest[2]=uvSource[4]*rz2;
		uvDest[3]=uvSource[1]*rz0;
		uvDest[4]=uvSource[3]*rz1;
		uvDest[5]=uvSource[5]*rz2;
		
		vDest+=12;
		vSource+=12;
		uvDest+=6;
		uvSource+=6;
		
		/*
			TODO: rethink this part
		*/
		tDest[0]=tSource;
		tDest++;
		
		this->numTriangles++;
	
	}
	
	
	vDest=vertices->data;
	
	for (int n=0;n<numTriangles;n++) {
	
		float minx,miny,maxx,maxy;
		
		minx = std::min({vDest[0],vDest[4],vDest[8]});
		maxx = std::max({vDest[0],vDest[4],vDest[8]});
		miny = std::min({vDest[1],vDest[5],vDest[9]});
		maxy = std::max({vDest[1],vDest[5],vDest[9]});
		
		minx=std::max(minx,0.0f);
		miny=std::max(miny,0.0f);
		
		maxx=std::min(maxx,(float)screenWidth-1);
		maxy=std::min(maxy,(float)screenHeight-1);
		
		int sx,sy;
		int ex,ey;
		
		sx=minx/TILE_SIZE;
		sy=miny/TILE_SIZE;
		
		ex=maxx/TILE_SIZE;
		ey=maxy/TILE_SIZE;
		
		for (int j=sy;j<=ey;j++) {
			for (int i=sx;i<=ex;i++) {
				Tile* tile =tiles[i+j*numTilesWidth];
				
				tile->bin->data[tile->triangles]=n;
				tile->triangles++;
			}
		}
		
	
	
		vDest+=12;
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

	/*
		Credits:
	
		https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
	*/
	
	int32_t screenLeft = tile->x;
	int32_t screenTop = tile->y;

	int32_t v0[2];
	int32_t v1[2];
	int32_t v2[2];
	
	float* vData=vertices->data+(index*12);
	float* uvData=uvs->data+(index*6);
	Texture** tData=textures->data+(index);
	
	v0[0]=vData[0] - screenLeft;
	v1[0]=vData[4] - screenLeft;
	v2[0]=vData[8] - screenLeft;
	
	v0[1]=vData[1] - screenTop;
	v1[1]=vData[5] - screenTop;
	v2[1]=vData[9] - screenTop;

	int32_t minx,miny;
	int32_t maxx,maxy;
	
	minx=std::min({v0[0],v1[0],v2[0]});
	
	miny=std::min({v0[1],v1[1],v2[1]});
	
	maxx=std::max({v0[0],v1[0],v2[0]});
	
	maxy=std::max({v0[1],v1[1],v2[1]});

	
	minx=std::max(minx,0);
	maxx=std::min(maxx,TILE_SIZE-1);
	
	miny=std::max(miny,0);
	maxy=std::min(maxy,TILE_SIZE-1);
	/*
	if (maxx < minx or maxy < miny) {
		return;
	}
	*/
	
	int32_t A[4];
	int32_t B[4];
	
	// A01 B01
	A[2] = v0[1] - v1[1], B[2] = v1[0] - v0[0];
	// A12 B12
	A[0] = v1[1] - v2[1], B[0] = v2[0] - v1[0];
	// A20 B20
	A[1] = v2[1] - v0[1], B[1] = v0[0] - v2[0];
	
	int32_t p[2] = { minx, miny };

	int32_t wrow[4];
	
	wrow[0] = orient(v1, v2, p);
	wrow[1] = orient(v2, v0, p);
	wrow[2] = orient(v0, v1, p);
	
	
	// this can be pre-computed
	float rz[4];
	
	rz[0]=1.0f/vData[3];
	rz[1]=1.0f/vData[7];
	rz[2]=1.0f/vData[11];
	
	float area=1.0f/orient(v0,v1,v2);
	
	
	for (p[1]=miny;p[1]<=maxy;p[1]++) {
		int32_t w[4];
		
		w[0] = wrow[0];
		w[1] = wrow[1];
		w[2] = wrow[2];
	
		for (p[0]=minx;p[0]<=maxx;p[0]++) {
		
			if (w[0]>=0 and w[1]>=0 and w[2]>=0) {
			
				float fw[4]={w[0],w[1],w[2],0.0f};
				
				fw[0]*=area;
				fw[1]*=area;
				fw[2]*=area;
				
				float wz = rz[0]*fw[0] + rz[1]*fw[1] + rz[2]*fw[2];
				wz=1.0f/wz;
				
				uint16_t z = 0xffff-((-wz-near)/(far-near))*0xffff;
				uint16_t Z = tile->depthBuffer[p[0]+p[1]*TILE_SIZE];
				
				if (z>Z) {
				
					float u = uvData[0]*fw[0] + uvData[1]*fw[1] + uvData[2]*fw[2];
					float v = uvData[3]*fw[0] + uvData[4]*fw[1] + uvData[5]*fw[2];

					u*=wz;
					v*=wz;

					uint32_t tx=u*(tData[0]->width-1);
					uint32_t ty=v*(tData[0]->height-1);

					//tx=tx%tData[0]->width;
					//ty=ty%tData[0]->height;

					uint32_t pixel = tData[0]->data[tx+ty*tData[0]->width];
					
					tile->depthBuffer[p[0]+p[1]*TILE_SIZE]=z;
					tile->frameBuffer[p[0]+p[1]*TILE_SIZE]=pixel;

				}
				
			}
			
			w[0] += A[0];
			w[1] += A[1];
			w[2] += A[2];
		}
		
		wrow[0] += B[0];
		wrow[1] += B[1];
		wrow[2] += B[2];
	}
	
	
}

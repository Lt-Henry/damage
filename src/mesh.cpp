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


#include "mesh.hpp"
#include "math.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>


using namespace damage;
using namespace damage::math;
using namespace std;


Mesh::Mesh()
{
}


Mesh::Mesh(string filename,TexturePool* pool)
{

	
	m4f::Identity(matrix);

	char buffer [128];
	
	ifstream file(filename,ifstream::binary);
	
	if (file) {
	
		// header
		file.read(buffer,4);
		buffer[4]='\0';
		
		string header(buffer);
		
		// version
		file.read(buffer,1);
		
		// texture name
		file.read(buffer,2);
		
		uint16_t len;
		
		len=*((uint16_t*)buffer);
		
		// texture name
		file.read(buffer,len);
		buffer[len]='\0';
		
		string textureName(buffer);
		
		cout<<"* "<<textureName<<endl;
		
		texture=pool->Get(textureName);
		
		// num of vertices
		file.read(buffer,4);

		uint32_t numVertices;
		
		numVertices=*((uint32_t*)buffer);
		
		cout<<"Mesh vertices: "<<numVertices<<endl;
		
		vector<float> verticesData;
		vector<float> normalsData;
		vector<float> uvsData;
		
		for (int n=0;n<numVertices;n++) {
			
			// coords float x3
			file.read(buffer,12);
			
			float* ptr=(float*)buffer;
			verticesData.push_back(ptr[0]);
			verticesData.push_back(ptr[1]);
			verticesData.push_back(ptr[2]);
			
			// normal float x3
			file.read(buffer,12);
			
			ptr=(float*)buffer;
			normalsData.push_back(ptr[0]);
			normalsData.push_back(ptr[1]);
			normalsData.push_back(ptr[2]);
			
			
		}
		
		// num of triangles
		file.read(buffer,4);
		
		uint32_t numTriangles;
		
		numTriangles=*((uint32_t*)buffer);
		
		cout<<"Mesh triangles: "<<numTriangles<<endl;
		
		vector<uint32_t> trianglesData;
		
		for (int n=0;n<numTriangles;n++) {
			
			// triangle uint32 x3
			file.read(buffer,12);
			uint32_t* ptr = (uint32_t*)buffer;
			trianglesData.push_back(ptr[0]);
			trianglesData.push_back(ptr[1]);
			trianglesData.push_back(ptr[2]);
			
			// uv float x6
			file.read(buffer,24);
			
			float* fptr=(float*)buffer;
			uvsData.push_back(fptr[0]);
			uvsData.push_back(fptr[1]);
			uvsData.push_back(fptr[2]);
			uvsData.push_back(fptr[3]);
			uvsData.push_back(fptr[4]);
			uvsData.push_back(fptr[5]);
		}
		
		
		file.close();
		
		// build mesh structure
		this->size=numTriangles;
		
		this->vertices=new Buffer<float>(this->size * 12);
		this->normals=new Buffer<float>(this->size * 12);
		this->uvs=new Buffer<float>(this->size * 6);
		
		for (int n=0;n<numTriangles;n++) {
			int i0 = trianglesData[(n*3)+0];
			int i1 = trianglesData[(n*3)+1];
			int i2 = trianglesData[(n*3)+2];
			
			this->vertices->data[(n*12)+0]=verticesData[(i0*3)+0];
			this->vertices->data[(n*12)+1]=verticesData[(i0*3)+1];
			this->vertices->data[(n*12)+2]=verticesData[(i0*3)+2];
			this->vertices->data[(n*12)+3]=1.0f;
			
			this->vertices->data[(n*12)+4]=verticesData[(i1*3)+0];
			this->vertices->data[(n*12)+5]=verticesData[(i1*3)+1];
			this->vertices->data[(n*12)+6]=verticesData[(i1*3)+2];
			this->vertices->data[(n*12)+7]=1.0f;
			
			this->vertices->data[(n*12)+8]=verticesData[(i2*3)+0];
			this->vertices->data[(n*12)+9]=verticesData[(i2*3)+1];
			this->vertices->data[(n*12)+10]=verticesData[(i2*3)+2];
			this->vertices->data[(n*12)+11]=1.0f;
			
			this->normals->data[(n*12)+0]=normalsData[(i0*3)+0];
			this->normals->data[(n*12)+1]=normalsData[(i0*3)+1];
			this->normals->data[(n*12)+2]=normalsData[(i0*3)+2];
			this->normals->data[(n*12)+3]=0.0f;
			
			this->normals->data[(n*12)+4]=normalsData[(i1*3)+0];
			this->normals->data[(n*12)+5]=normalsData[(i1*3)+1];
			this->normals->data[(n*12)+6]=normalsData[(i1*3)+2];
			this->normals->data[(n*12)+7]=0.0f;
			
			this->normals->data[(n*12)+0]=normalsData[(i2*3)+0];
			this->normals->data[(n*12)+1]=normalsData[(i2*3)+1];
			this->normals->data[(n*12)+2]=normalsData[(i2*3)+2];
			this->normals->data[(n*12)+3]=0.0f;
			
			this->uvs->data[(n*6)+0]=uvsData[(n*6)+0];
			this->uvs->data[(n*6)+1]=uvsData[(n*6)+1];
			
			this->uvs->data[(n*6)+2]=uvsData[(n*6)+2];
			this->uvs->data[(n*6)+3]=uvsData[(n*6)+3];
			
			this->uvs->data[(n*6)+4]=uvsData[(n*6)+4];
			this->uvs->data[(n*6)+5]=uvsData[(n*6)+5];
			
		}
		
	
	}
		
}


Mesh::~Mesh()
{
	delete vertices;
	delete normals;
	delete uvs;
}

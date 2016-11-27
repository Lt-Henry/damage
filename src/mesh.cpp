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
#include <fstream>
#include <vector>


using namespace damage;
using namespace damage::math;
using namespace std;


Mesh::Mesh()
{
}


Mesh::Mesh(string filename)
{
	char buffer [128];
	
	ifstream file(filename,ifstream::binary);
	
	if (file) {
	
		// header
		file.read(buffer,4);
		buffer[4]='\0';
		
		string header(buffer);
		
		cout<<"header:"<<header<<endl;
		
		// version
		file.read(buffer,1);
		
		cout<<"version:"<<(int)buffer[0]<<endl;
		
		// num of vertices
		file.read(buffer,4);

		uint32_t numVertices;
		
		numVertices=*((uint32_t*)buffer);
		
		cout<<"vertices:"<<numVertices<<endl;
		
		vector<float> verticesData;
		vector<float> normalsData;
		
		for (int n=0;n<numVertices;n++) {
			
			// coords float x3
			file.read(buffer,12);
			verticesData.push_back(*((float*)buffer));
			verticesData.push_back(*((float*)buffer+4));
			verticesData.push_back(*((float*)buffer+8));
			
			// normal float x3
			file.read(buffer,12);
			normalsData.push_back(*((float*)buffer));
			normalsData.push_back(*((float*)buffer+4));
			normalsData.push_back(*((float*)buffer+8));
			
			// color uint8 x3 (rgb)
			file.read(buffer,3);
			
			// uv float x2
			file.read(buffer,8);
			
			// tag
			file.read(buffer,1);
		}
		
		// num of triangles
		file.read(buffer,4);
		
		uint32_t numTriangles;
		
		numTriangles=*((uint32_t*)buffer);
		
		cout<<"triangles:"<<numTriangles<<endl;
		
		vector<uint32_t> trianglesData;
		
		for (int n=0;n<numTriangles;n++) {
			
			// triangle uint32 x4
			file.read(buffer,16);
			trianglesData.push_back(*(uint32_t*)buffer);
			trianglesData.push_back(*(uint32_t*)buffer+4);
			trianglesData.push_back(*(uint32_t*)buffer+8);
		}
		
		// num of textures
		file.read(buffer,4);
		
		int numTextures;
		
		numTextures=*((uint32_t*)buffer);
		
		cout<<"textures:"<<numTextures<<endl;
		
		for (int n=0;n<numTextures;n++) {
		
			// texture name len
			file.read(buffer,2);
			
			uint16_t len;
			
			len=*((uint16_t*)buffer);
			
			// texture name
			file.read(buffer,len);
			buffer[len]='\0';
			
			string textureName(buffer);
			
			cout<<"* "<<textureName<<endl;
			
		}
		
		file.close();
		
		// build mesh structure
		this->size=numTriangles;
		this->vertices=new float[this->size * 12];
		this->normals=new float[this->size * 12];
		
		for (int n=0;n<numTriangles;n++) {
			int i0 = trianglesData[(n*3)+0];
			int i1 = trianglesData[(n*3)+1];
			int i2 = trianglesData[(n*3)+2];
			
			this->vertices[(n*12)+0]=verticesData[(i0*3)+0];
			this->vertices[(n*12)+1]=verticesData[(i0*3)+1];
			this->vertices[(n*12)+2]=verticesData[(i0*3)+2];
			this->vertices[(n*12)+3]=0.0f;
			
			this->vertices[(n*12)+4]=verticesData[(i1*3)+0];
			this->vertices[(n*12)+5]=verticesData[(i1*3)+1];
			this->vertices[(n*12)+6]=verticesData[(i1*3)+2];
			this->vertices[(n*12)+7]=0.0f;
			
			this->vertices[(n*12)+8]=verticesData[(i2*3)+0];
			this->vertices[(n*12)+9]=verticesData[(i2*3)+1];
			this->vertices[(n*12)+10]=verticesData[(i2*3)+2];
			this->vertices[(n*12)+11]=0.0f;
			
			
		}
	}
		
}


Mesh::~Mesh()
{
}

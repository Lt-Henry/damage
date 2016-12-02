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
	vertices=nullptr;
	normals=nullptr;
	uvs=nullptr;
	textures=nullptr;
}


Mesh::Mesh(string filename,TexturePool* pool)
{

	vertices=nullptr;
	normals=nullptr;
	uvs=nullptr;
	textures=nullptr;
	
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
		
		// num of vertices
		file.read(buffer,4);

		uint32_t numVertices;
		
		numVertices=*((uint32_t*)buffer);
		
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
			
			// color uint8 x3 (rgb)
			file.read(buffer,3);
			
			// uv float x2
			file.read(buffer,8);
			
			ptr=(float*)buffer;
			uvsData.push_back(ptr[0]);
			uvsData.push_back(ptr[1]);
			
			// tag
			file.read(buffer,1);
		}
		
		// num of triangles
		file.read(buffer,4);
		
		uint32_t numTriangles;
		
		numTriangles=*((uint32_t*)buffer);
		
		cout<<"Mesh triangles: "<<numTriangles<<endl;
		
		vector<uint32_t> trianglesData;
		
		for (int n=0;n<numTriangles;n++) {
			
			// triangle uint32 x4
			file.read(buffer,16);
			uint32_t* ptr = (uint32_t*)buffer;
			trianglesData.push_back(ptr[0]);
			trianglesData.push_back(ptr[1]);
			trianglesData.push_back(ptr[2]);
			trianglesData.push_back(ptr[3]);
		}
		
		// num of textures
		file.read(buffer,4);
		
		int numTextures;
		
		numTextures=*((uint32_t*)buffer);
		
		vector<string> textureData;
		
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
			
			textureData.push_back(textureName);
			
		}
		
		file.close();
		
		// build mesh structure
		this->size=numTriangles;
		this->vertices=new float[this->size * 12];
		this->normals=new float[this->size * 12];
		this->uvs=new float[this->size*6];
		this->textures=new Texture*[this->size];
		
		for (int n=0;n<numTriangles;n++) {
			int i0 = trianglesData[(n*4)+0];
			int i1 = trianglesData[(n*4)+1];
			int i2 = trianglesData[(n*4)+2];
			int tx = trianglesData[(n*4)+3];
			
			this->vertices[(n*12)+0]=verticesData[(i0*3)+0];
			this->vertices[(n*12)+1]=verticesData[(i0*3)+1];
			this->vertices[(n*12)+2]=verticesData[(i0*3)+2];
			this->vertices[(n*12)+3]=1.0f;
			
			this->vertices[(n*12)+4]=verticesData[(i1*3)+0];
			this->vertices[(n*12)+5]=verticesData[(i1*3)+1];
			this->vertices[(n*12)+6]=verticesData[(i1*3)+2];
			this->vertices[(n*12)+7]=1.0f;
			
			this->vertices[(n*12)+8]=verticesData[(i2*3)+0];
			this->vertices[(n*12)+9]=verticesData[(i2*3)+1];
			this->vertices[(n*12)+10]=verticesData[(i2*3)+2];
			this->vertices[(n*12)+11]=1.0f;
			
			this->normals[(n*12)+0]=normalsData[(i0*3)+0];
			this->normals[(n*12)+1]=normalsData[(i0*3)+1];
			this->normals[(n*12)+2]=normalsData[(i0*3)+2];
			this->normals[(n*12)+3]=0.0f;
			
			this->normals[(n*12)+4]=normalsData[(i1*3)+0];
			this->normals[(n*12)+5]=normalsData[(i1*3)+1];
			this->normals[(n*12)+6]=normalsData[(i1*3)+2];
			this->normals[(n*12)+7]=0.0f;
			
			this->normals[(n*12)+0]=normalsData[(i2*3)+0];
			this->normals[(n*12)+1]=normalsData[(i2*3)+1];
			this->normals[(n*12)+2]=normalsData[(i2*3)+2];
			this->normals[(n*12)+3]=0.0f;
			
			this->uvs[(n*6)+0]=uvsData[(i0*2)+0];
			this->uvs[(n*6)+1]=uvsData[(i0*2)+1];
			
			this->uvs[(n*6)+2]=uvsData[(i1*2)+0];
			this->uvs[(n*6)+3]=uvsData[(i1*2)+1];
			
			this->uvs[(n*6)+4]=uvsData[(i2*2)+0];
			this->uvs[(n*6)+5]=uvsData[(i2*2)+1];
			
			this->textures[n]=pool->Get(textureData[tx]);
		}
		
		std::setprecision(2);
		cout.setf(ios::fixed);
		for (int n=0;n<this->size;n++) {
			cout<<"["<<n<<"]:"<<uvs[(n*6)+0]<<","<<uvs[(n*6)+1]<<endl;
			cout<<"["<<n<<"]:"<<uvs[(n*6)+2]<<","<<uvs[(n*6)+3]<<endl;
			cout<<"["<<n<<"]:"<<uvs[(n*6)+4]<<","<<uvs[(n*6)+5]<<endl;
		}
	}
		
}


Mesh::~Mesh()
{
	if (vertices!=nullptr) {
		delete vertices;
	}
	
	if (normals!=nullptr) {
		delete normals;
	}
	
	if (uvs!=nullptr) {
		delete uvs;
	}
	
	if (textures!=nullptr) {
		delete textures;
	}
}

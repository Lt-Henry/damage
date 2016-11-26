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
		
		for (int n=0;n<numVertices;n++) {
			
			// coords float x3
			file.read(buffer,12);
			
			// normal float x3
			file.read(buffer,12);
			
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
		
		for (int n=0;n<numTriangles;n++) {
			
			// triangle uint32 x4
			file.read(buffer,16);
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
	}
		
}


Mesh::~Mesh()
{
}

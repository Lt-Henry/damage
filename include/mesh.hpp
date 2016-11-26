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


#ifndef _DAMAGE_MESH_
#define _DAMAGE_MESH_

#include <string>
#include <cstdint>

namespace damage
{
	class Mesh {
	public:
	
		uint32_t size;
		
		float* vertices;
		
		float* uvs;
		
		uint16_t* materials;
		
		Mesh();
		Mesh(std::string filename);
		
		~Mesh();
		
	};
}

#endif

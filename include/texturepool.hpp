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

#ifndef _DAMAGE_TEXTUREPOOL_
#define _DAMAGE_TEXTUREPOOL_

#include "texture.hpp"

#include <string>
#include <map>


namespace damage
{
	
	class TexturePool {
	private:
		
		std::string basedir;
	
		std::map<std::string,Texture*> textures;
		
		Texture* Load(std::string filename);
	
	public:
	
		TexturePool(std::string basedir);
		~TexturePool();
	
		Texture* Get(std::string name);
	
	};
}

#endif

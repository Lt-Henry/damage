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

#ifndef _DAMAGE_BUFFER_
#define _DAMAGE_BUFFER_

#include <cstdint>
#include <memory>

namespace damage
{
	template <typename T>
	class Buffer {
	private:
	
		size_t size;
		size_t rawSize;

		
		T* raw;
	
	public:
	
		T* data;
		
		
		
		Buffer(size_t size)
		{
			
			rawSize=size+1;
			raw = new T[rawSize];
			void* tmp=raw;
			
			std::align(alignof(T),sizeof(T)*rawSize,tmp,size);
			data=static_cast<T*>(tmp);
			this->size=size;
		}
		
		
		~Buffer()
		{
			if (raw!=nullptr) {
				delete raw;
			}
		}
		
		
		size_t Size()
		{
			return size;
		}
		
	};
}

#endif

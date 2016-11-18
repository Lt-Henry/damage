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

#ifndef _DAMAGE_MATH_
#define _DAMAGE_MATH_

#include <cmath>

namespace damage
{
	namespace math
	{
		typedef float vector4f_t [4];
		typedef float vector2f_t [2];
		
		typedef float matrix4f_t [16];
		
		namespace v4f
		{
			void Set(float* r,float x,float y,float z,float w=0.0f);
		
			void Add(float* r,const float* a,const float* b);
			void Sub(float* r,const float* a,const float* b);
			
			float Dot(const float* a,const float* b);
			void Cross(float* r,const float* a,const float* b);
			
			float Norm(const float* a);
			void Normalize(float* a);
			
			void Homogeneus(float* a);
			
			void Mult(float* r,const float* v,const float* m);
		}
		
		namespace m4f
		{
			void Identity(float* m);
		}
	}
}

#endif

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

#include "math.hpp"

namespace damage
{
	namespace math
	{		
		namespace v4f
		{
		
			void Set(float* r,float x,float y,float z,float w)
			{
				r[0]=x;
				r[1]=y;
				r[2]=z;
				r[3]=w;
			}
		
		
			void Add(float* r,const float* a,const float* b)
			{
				r[0]=a[0]+b[0];
				r[1]=a[1]+b[1];
				r[2]=a[2]+b[2];
				r[3]=a[3]+b[3];
			}
			
			
			void Sub(float* r,const float* a,const float* b)
			{
				r[0]=a[0]-b[0];
				r[1]=a[1]-b[1];
				r[2]=a[2]-b[2];
				r[3]=a[3]-b[3];
			}
			
			
			float Dot(const float* a,const float* b)
			{
				float ret;
		
				ret=(a[0]*b[0]) + (a[1]*b[1]) + (a[2]*b[2]) + (a[3]*b[3]);	
				return ret;
			}
			
			
			void Cross(float* r,const float* a,const float* b)
			{
				r[0] = a[1]*b[2] - a[2]*b[1];
				r[1] = a[2]*b[0] - a[0]*b[2];
				r[2] = a[0]*b[1] - a[1]*b[0];
				r[3] = 0.0f;
			}
			
			
			float Norm(const float* a)
			{
				float tmp=(a[0]*a[0])+(a[1]*a[1])+(a[2]*a[2])+(a[3]*a[3]);
				return sqrt(tmp);	
			}
			
			
			void Normalize(float* a)
			{
				float rW=1.0f/Norm(a);
				
				a[0]*=rW;
				a[1]*=rW;
				a[2]*=rW;
				a[3]*=rW;
			}
			
			
			void Homogeneus(float* a)
			{
				float rW=1.0f/a[3];
				
				a[0]*=rW;
				a[1]*=rW;
				a[2]*=rW;
				a[3]=1.0f;	
			}
			
			
			void Mult(float* r,const float* v,const float* m)
			{
				r[0]=0.0f;
				r[1]=0.0f;
				r[2]=0.0f;
				r[3]=0.0f;
				
				for (int n=0;n<4;n++) {
					r[0]+= m[4*0+n] + v[n];
					r[1]+= m[4*1+n] + v[n];
					r[2]+= m[4*2+n] + v[n];
					r[3]+= m[4*3+n] + v[n];
				}
			}
		}
		
		
		namespace v2i
		{
			int orient(const int* a,const int* b,const int* c)
			{
				return (b[0]-a[0])*(c[1]-a[1]) - (b[1]-a[1])*(c[0]-a[0]);
			}
		}
		
		
		namespace m4f
		{
			
			
			void Identity(float* m)
			{
				m[0]=1.0f;
				m[1]=0.0f;
				m[2]=0.0f;
				m[3]=0.0f;
				
				m[4]=0.0f;
				m[5]=1.0f;
				m[6]=0.0f;
				m[7]=0.0f;
				
				m[8]=0.0f;
				m[9]=0.0f;
				m[10]=1.0f;
				m[11]=0.0f;
				
				m[12]=0.0f;
				m[13]=0.0f;
				m[14]=0.0f;
				m[15]=1.0f;
			}
		}
	}
}


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
		
			void Set(float* d,float* s)
			{
				d[0]=s[0];
				d[1]=s[1];
				d[2]=s[2];
				d[3]=s[3];
			}
			
			
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
				float w[4]={v[0],v[1],v[2],v[3]};
				
				r[0]= (m[4*0+0] * w[0]) + (m[4*1+0] * w[1]) + (m[4*2+0] * w[2]) + (m[4*3+0] * w[3]);
				r[1]= (m[4*0+1] * w[0]) + (m[4*1+1] * w[1]) + (m[4*2+1] * w[2]) + (m[4*3+1] * w[3]);
				r[2]= (m[4*0+2] * w[0]) + (m[4*1+2] * w[1]) + (m[4*2+2] * w[2]) + (m[4*3+2] * w[3]);
				r[3]= (m[4*0+3] * w[0]) + (m[4*1+3] * w[1]) + (m[4*2+3] * w[2]) + (m[4*3+3] * w[3]);
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
			
			
			void Translation(float* m,float x,float y,float z)
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
				
				m[12]=x;
				m[13]=y;
				m[14]=z;
				m[15]=1.0f;
			}
			
			
			void RotationX(float* m,float rads)
			{
				m[0]=1.0f;
				m[1]=0.0f;
				m[2]=0.0f;
				m[3]=0.0f;
				
				m[4]=0.0f;
				m[5]=cos(rads);
				m[6]=sin(rads);
				m[7]=0.0f;
				
				m[8]=0.0f;
				m[9]=-sin(rads);
				m[10]=cos(rads);
				m[11]=0.0f;
				
				m[12]=0.0f;
				m[13]=0.0f;
				m[14]=0.0f;
				m[15]=1.0f;
			}
			
			
			void RotationY(float* m,float rads)
			{
				m[0]=cos(rads);
				m[1]=0.0f;
				m[2]-=sin(rads);
				m[3]=0.0f;
				
				m[4]=0.0f;
				m[5]=1.0f;
				m[6]=0.0f;
				m[7]=0.0f;
				
				m[8]=sin(rads);
				m[9]=0.0f;
				m[10]=cos(rads);
				m[11]=0.0f;
				
				m[12]=0.0f;
				m[13]=0.0f;
				m[14]=0.0f;
				m[15]=1.0f;
			}
			
			
			void RotationZ(float* m,float rads)
			{
				m[0]=cos(rads);
				m[1]=sin(rads);
				m[2]=0.0f;
				m[3]=0.0f;
				
				m[4]=-sin(rads);
				m[5]=cos(rads);
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
			
			
			void Mult(float* r,float* a,float* b)
			{
				// unoptimized matrix mult
				for (int i=0;i<16;i+=4) {
					for (int j=0;j<4;j++) {
						r[i + j] = (b[i + 0] * a[j + 0])
						+ (b[i + 1] * a[j + 4])
						+ (b[i + 2] * a[j + 8])
						+ (b[i + 3] * a[j + 12]);
					}
				}
			}
			
			
			void Set(float* m,float* s)
			{
				for (int n=0;n<16;n++) {
					m[n]=s[n];
				}
			}
			
		}
	}
}


#pragma once
#include <Walnut/Random.h>
#include"glm/glm.hpp"
class perlin
{
public:
	perlin();

	float noise(const glm::vec3& p)const;
	float turb(const glm::vec3& p, int depth = 7) const;

	~perlin();
private:
	static int* perlin_gen_perm();
private:
	static const int point_count = 256;
	float* ranfloat;
	int* perm_x;		// 排列表
	int* perm_y;		// 排列表
	int* perm_z;		// 排列表
};
inline float trilinear_interp(float c[2][2][2], float u, float v, float w) {
	/*
		c000: (1-tx)(1-ty)(1-tz)
		c100: tx(1-ty)(1-tz)
		c010: (1-tx)ty(1-tz)
		c110: txty(1-tz)
		c001: (1-tx)(1-ty)tz
		c101: tx(1-ty)tz
		c011: (1-tx)tytz
		c111: txtytz
		其中 c为顶点
		即所有的排列组合 如：c[1][][](u*v*w + u*v*(1-w)+ u*(1-v)w+ u*(1-v)(1-w))
		比如双线性插值
		for(int i=0; i<2; i++){
			for(int j=0; j<2; j++){
				c[i][j] += (i*u+ (1-i)*(1-u))*
						  (j*v+ (1-j)*(1-v))* vertex[i][j]
			}
		}
	*/
	float accum = 0.0F;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				accum += (i * u + (1 - i) * (1 - u)) *
						 (j * v + (1 - j) * (1 - v)) *
						 (k * w + (1 - k) * (1 - w)) * c[i][j][k];

	return accum;
}
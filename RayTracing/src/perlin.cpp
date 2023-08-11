#include "perlin.h"

perlin::perlin()
{
	ranfloat = new float[point_count];
	for (int i = 0; i < point_count; i++) {
		ranfloat[i] = Walnut::Random::random_float();
	}
	perm_x = perlin_gen_perm();
	perm_y = perlin_gen_perm();
	perm_z = perlin_gen_perm();
}

float perlin::noise(const glm::vec3& p)const
{
	int i = glm::floor(p.x);
	int j = glm::floor(p.y);
	int k = glm::floor(p.z);

	auto u = p.x - i;
	auto v = p.y - j;
	auto w = p.z - k;
	//u = u * u * (3 - 2 * u);
	//v = v * v * (3 - 2 * v);
	//w = w * w * (3 - 2 * w);

	float c[2][2][2];

	//得到着色点周围8个点的噪声（正方体）
	for (int di = 0; di < 2; di++)
		for (int dj = 0; dj < 2; dj++)
			for (int dk = 0; dk < 2; dk++)
				c[di][dj][dk] = ranfloat[
										perm_x[(i + di) & 255] ^
										perm_y[(j + dj) & 255] ^
										perm_z[(k + dk) & 255] 
									    ];

	//return ranfloat[perm_x[i & 255] ^ perm_y[j & 255]  ^ perm_z[k & 255]];
	return trilinear_interp(c, u, v, w);
}

perlin::~perlin() {
	delete ranfloat;
	delete perm_z;
	delete perm_y;
	delete perm_x;
}

int* perlin::perlin_gen_perm() {
	auto p = new int[point_count];

	for (int i = 0; i < point_count; i++) {
		p[i] = i;
	}

	for (int i = point_count - 1; i > 0; i--) {
		int target = Walnut::Random::UInt() % (i + 1);
		std::swap(target, p[i]);
	}

	return p;
}

float perlin::turb(const glm::vec3& p, int depth) const {
	auto accum = 0.0;
	glm::vec3 temp_p = p;
	auto weight = 1.0;

	for (int i = 0; i < depth; i++) {
		accum += weight * noise(temp_p);
		weight *= 0.5;
		temp_p *= 2;
	}

	return fabs(accum);
}

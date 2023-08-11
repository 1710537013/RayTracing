#include "texture.h"

glm::vec3 checker_texture::value(float u, float v, const glm::vec3& p) const
{
	auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
	//auto sines = sin(p.x) * sin(p.y) * sin(p.z);
	if (sines < 0)
		return odd->value(u, v, p);
	else
		return even->value(u, v, p);
}

glm::vec3 noise_texture::value(float u, float v, const glm::vec3& p) const
{
	return glm::vec3(1.0f) * 0.5f * (per.noise(scale * p) + 1.0f);
	//return glm::vec3(1, 1, 1) * 0.5F * (1 + sin(scale * p.z+ 10 * per.noise(p)));
}

image_texture::image_texture(const char* filename)
{
	int nchannel;
	picData = stbi_load(
		filename, &nx, &ny, &nchannel, 3);

	if (!picData) {
		std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
		nx = ny = 0;
	}
}

glm::vec3 image_texture::value(float u, float v, const glm::vec3& p) const
{
	if (!picData)
		return glm::vec3(1, 0, 1);

	u = glm::clamp(u, 0.0f, 1.0f);
	v = 1.0F- glm::clamp(v, 0.0f, 1.0f);  // -> [1, 0] y reverse

	auto i = static_cast<int>(u * nx);
	auto j = static_cast<int>(v * ny);

	i = glm::clamp(i, 0, nx - 1);
	j = glm::clamp(j, 0, ny - 1);

	float r = int(picData[j * nx * 3 + 3 * i + 0]) / 255.0f;
	float g = int(picData[j * nx * 3 + 3 * i + 1]) / 255.0f;
	float b = int(picData[j * nx * 3 + 3 * i + 2]) / 255.0f;

	return { r,g,b };
}

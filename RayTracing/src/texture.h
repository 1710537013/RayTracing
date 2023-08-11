#pragma once
#include"glm/glm.hpp"
#include<memory>
#include "perlin.h"
#include <iostream>
#include "stb_image.h"
class texture
{
public:
	virtual glm::vec3 value(float u, float v, const glm::vec3& p)const = 0;
};

class solid_color : public texture {
public:
    solid_color() {}
    solid_color(glm::vec3 c) : color_value(c) {}

    solid_color(double red, double green, double blue)
        : solid_color(glm::vec3(red, green, blue)) {}

    virtual glm::vec3 value(float u, float v, const glm::vec3& p) const override {return color_value;}

private:
    glm::vec3 color_value;
};

class checker_texture : public texture {
public:
    checker_texture() {}

    checker_texture(std::shared_ptr<texture> _even, std::shared_ptr<texture> _odd)
        : even(_even), odd(_odd) {}

    checker_texture(glm::vec3 c1, glm::vec3 c2)
        : even(std::make_shared<solid_color>(c1)), odd(std::make_shared<solid_color>(c2)) {}

    virtual glm::vec3 value(float u, float v, const glm::vec3& p) const override;

public:
    std::shared_ptr<texture> odd;
    std::shared_ptr<texture> even;
};

class noise_texture : public texture {
public:
    noise_texture(float _scale) :scale(_scale) {};
    virtual glm::vec3 value(float u, float v, const glm::vec3& p)const override;

private:
    float scale;
    perlin per;
};


class image_texture:public texture {
public:
    image_texture(const char* filename);
    virtual glm::vec3 value(float u, float v, const glm::vec3& p)const;

    ~image_texture() { delete picData; }

private:
    unsigned char* picData;   
    int nx, ny;
};
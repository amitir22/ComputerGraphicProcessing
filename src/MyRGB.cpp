#include "MyRGB.h"

MyRGB::MyRGB()
    : r(0), g(0), b(0) // Default constructor definition
{}

MyRGB::MyRGB(unsigned char r, unsigned char g, unsigned char b)
    : r(r), g(g), b(b) // Parameterized constructor definition
{}

MyRGB::MyRGB(vec3 color)
{
    color = color.cwiseMin(1.0);
    this->r = (unsigned char)(unsigned int)(color[0] * 255);
    this->g = (unsigned char)(unsigned int)(color[1] * 255);
    this->b = (unsigned char)(unsigned int)(color[2] * 255);
}

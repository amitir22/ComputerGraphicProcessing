#include "MyRGB.h"

MyRGB::MyRGB()
    : r(0), g(0), b(0) // Default constructor definition
{}

MyRGB::MyRGB(unsigned char r, unsigned char g, unsigned char b)
    : r(r), g(g), b(b) // Parameterized constructor definition
{}

MyRGB::MyRGB(float r, float g, float b)
{
    this->r = (unsigned char)(unsigned int)(r * 255);
    this->g = (unsigned char)(unsigned int)(g * 255);
    this->b = (unsigned char)(unsigned int)(b * 255);
}

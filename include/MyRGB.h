#pragma once
#include "Geometry.h"

class MyRGB {
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;

    MyRGB(); // Default constructor declaration
    MyRGB(unsigned char r, unsigned char g, unsigned char b); // Parameterized constructor declaration
    MyRGB(float r, float g, float b); // Parameterized constructor declaration
    MyRGB(vec3 color) : MyRGB(color.x(), color.y(), color.z()) {} // Parameterized constructor declaration
};
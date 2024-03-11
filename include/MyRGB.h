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

// define a few colors
const MyRGB RED((unsigned char)255, (unsigned char)0, (unsigned char)0);
const MyRGB GREEN((unsigned char)0, (unsigned char)255, (unsigned char)0);
const MyRGB BLUE((unsigned char)0, (unsigned char)0, (unsigned char)255);
const MyRGB WHITE((unsigned char)255, (unsigned char)255, (unsigned char)255);
const MyRGB BLACK((unsigned char)0, (unsigned char)0, (unsigned char)0);
const MyRGB YELLOW((unsigned char)255, (unsigned char)255, (unsigned char)0);
const MyRGB CYAN((unsigned char)0, (unsigned char)255, (unsigned char)255);
const MyRGB MAGENTA((unsigned char)255, (unsigned char)0, (unsigned char)255);
const MyRGB ORANGE((unsigned char)255, (unsigned char)165, (unsigned char)0);
const MyRGB GRAY((unsigned char)128, (unsigned char)128, (unsigned char)128);
const MyRGB DARK_GRAY((unsigned char)64, (unsigned char)64, (unsigned char)64);
const MyRGB LIGHT_GRAY((unsigned char)192, (unsigned char)192, (unsigned char)192);
const MyRGB BROWN((unsigned char)165, (unsigned char)42, (unsigned char)42);
const MyRGB PINK((unsigned char)255, (unsigned char)192, (unsigned char)203);
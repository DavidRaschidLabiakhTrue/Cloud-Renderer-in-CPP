#pragma once

#include "../Include/stb/stb_image.h"
#include <string>
#include <iostream>
#include <vector>


#include <glad/glad.h>

using namespace std;


unsigned int generateTexture2D(int w, int h);
unsigned int generateTexture3D(int w, int h, int d);
void bindTexture2D(unsigned int tex, int unit = 0);
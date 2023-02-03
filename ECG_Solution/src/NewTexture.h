#pragma once

#include <string>
#include <GL/glew.h>
#include "Utils.h"
#include <stb/stb_image.h>

class NewTexture
{
private:
	
	int  width, height, nrChan;
	bool _init;

public:
	GLuint _handle;
	NewTexture(std::string file);
	~NewTexture();

	void bind(unsigned int unit);
};


/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#pragma once

#include <memory>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <iostream>
#include "Texture.h"
#include "Shader.h"



class Postp
{
public:
	std::shared_ptr<Shader> postShader;
	GLuint width, height, coltex, normtex;

	Postp(std::shared_ptr<Shader> shader, GLuint width, GLuint height);
	~Postp();

	// Prepares the postprocessor's framebuffer operations before rendering the game
	void BeginRender();
	//  stores all the rendered data into a texture object
	void EndRender();
	// Renders the PostProcessor texture (as a screen-encompassing large sprite)
	void Render();

private:
	GLuint msfbo, fbo;
	GLuint rbo;
	GLuint vao, vbo;
	void init();
};

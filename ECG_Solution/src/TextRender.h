#pragma once

#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H  
#include <iostream>
#include <map>
#include <glm/glm.hpp>
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <stb/stb_image.h>

struct Character {
	GLuint textureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint Advance;    // Offset to advance to next glyph
};

struct Quad {
	GLuint textureID;  // ID handle of the glyph texture
	int width, height, nrChannels;
};

class TextRender {
public:
	TextRender(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shader2, int width, int height);
	~TextRender();
	// Text, X-Offs, Y-Offs, Scale, Color    ->offset 0 is lower left corner
	void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	// sprite name, offset, size (0 = original pic size), Color    -> 0  = screen lower left corner/ sprite upper left corner
	void renderquad(std::string name, glm::vec2 offset, glm::vec2 size, glm::vec4 color);
	//path, name
	void createTex(std::string path, std::string name);
private:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> shader2;
	GLuint vao, vbo, vao2, vbo2;
	int width, height;

	std::map<std::string, Quad> quads;
	std::map<GLchar, Character> Characters;
	void init();
	
};
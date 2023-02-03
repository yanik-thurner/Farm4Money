#pragma once

#include "Utils.h"
#include "ModelManager.h"
#include "gameObjects/SceneObject.h"
#include <memory>
#include <glm/ext.hpp>

struct Part {
	glm::vec3 pos, speed;
	float size, angle, weight;
	float life; // in sec, 0> is dead
	float cameradistance; // dead : -1.0f

	// Sort in reverse order -> first draw far away particles
	bool operator<(const Part& that) const {
		return this->cameradistance > that.cameradistance;
	}
};

class Particle
{
public:
	Particle(std::string path, std::shared_ptr<Shader> shader, glm::vec3 pos);
	~Particle();
	void generate(double dt);
	void update(double dt, glm::vec3 cam);
	void draw();

private:
	const int max = 1000;
	const float size = 0.5;
	int partcount;
	int lastP = 0;

	std::shared_ptr<Shader> shader;
	std::vector<Part> particles;
	glm::vec3 pos;
	NewTexture tex;
	GLuint vbo_texture, vbo_position, vbo_color;
	GLuint vao;
	GLfloat* pos_size = new GLfloat[max * 4];
	
	int findUnused(); //find unused Particle or override first
	void sortP();
};


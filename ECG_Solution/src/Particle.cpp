#include "Particle.h"



Particle::Particle(std::string path, std::shared_ptr<Shader> shader, glm::vec3 pos) : tex(NewTexture(path)), shader(shader), pos(pos)
{
	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};

	//bind vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//generate vbo containing quad data
	glGenBuffers(1, &vbo_texture);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//generate vbo containing position and size data (data is updated each frame)
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, max * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	//unbind vao
	glBindVertexArray(0);

	//fill particle vector with standard data
	for (int i = 0; i< max; i++) {
		particles.push_back(Part());

		particles[i].life = -1.0f;
		particles[i].cameradistance = -1.0f;
	}
}


Particle::~Particle()
{
	glDeleteBuffers(1, &vbo_position);
	glDeleteBuffers(1, &vbo_texture);
}

void Particle::update(double dt, glm::vec3 cam)
{
	partcount = 0;
	for (int i = 0; i < max; i++) {

		Part& p = particles[i];
		if (p.life > 0.0f) {
			// Decrease life
			p.life -= dt;
			if (p.life > 0.0f) {
				p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)dt * 0.5f;
				p.pos += p.speed * (float)dt;
				p.cameradistance = glm::length2(p.pos - cam);

				// Fill the GPU buffer
				pos_size[4 * partcount + 0] = p.pos.x;
				pos_size[4 * partcount + 1] = p.pos.y;
				pos_size[4 * partcount + 2] = p.pos.z;
				pos_size[4 * partcount + 3] = p.size;
			}
			else {
				p.cameradistance = -1.0f;
			}
			partcount++;
		}
	}
	sortP();
}

void Particle::draw()
{
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, max * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, partcount * sizeof(GLfloat) * 4, pos_size);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->use();

	tex.bind(0);
	shader->setUniform("parttexture",0);
	
	// vert
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// pos
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glVertexAttribDivisor(0, 0); // vert
	glVertexAttribDivisor(1, 1); // pos

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, partcount);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void Particle::generate(double dt)
{
	//generate new particles with cap on slow frame and 60fps
	int newparticles = (int)(dt*100.0);
	if (newparticles > (int)(0.016f*100.0))
		newparticles = (int)(0.016f*100.0);

	for (int i = 0; i<newparticles; i++) {
		int particleIndex = findUnused();
		//set life time in secounds
		particles[particleIndex].life = 2.0f;
		particles[particleIndex].pos = pos;

		float spread = 1.5f;
		glm::vec3 maindir = glm::vec3(0.0f, 5.0f, 0.0f);
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);

		particles[particleIndex].speed = maindir + randomdir * spread * size;
		particles[particleIndex].size = ((rand() % 1000) / 2000.0f + 0.1f )* size;
	}
}

int Particle::findUnused() {

	for (int i = lastP; i<max; i++) {
		if (particles[i].life < 0) {
			lastP = i;
			return i;
		}
	}

	for (int i = 0; i<lastP; i++) {
		if (particles[i].life < 0) {
			lastP = i;
			return i;
		}
	}

	return 0; 
}

void Particle::sortP() {
	std::sort(&particles[0], &particles[max-1]);
}
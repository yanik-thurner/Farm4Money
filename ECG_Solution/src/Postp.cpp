#include "Postp.h"


Postp::Postp(std::shared_ptr<Shader> shader, GLuint width, GLuint height)
	: postShader(shader), width(width), height(height)
{
	//create framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// create a color attachment texture
	glGenTextures(1, &coltex);
	glBindTexture(GL_TEXTURE_2D, coltex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, coltex, 0);

	// create a normal attachment texture
	glGenTextures(1, &normtex);
	glBindTexture(GL_TEXTURE_2D, normtex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normtex, 0);

	//set draw buffers
	GLenum drawbuf[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawbuf);

	// create a singe renderbuffer object for depth and stencil attachment
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); 

	//attach rbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
						
	// check for framebuffer error
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//initialise vao and abo
	init();
}

Postp::~Postp()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glDeleteTextures(1, &coltex);
	glDeleteTextures(1, &normtex);
}

void Postp::BeginRender() {
	// bind to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);

	// clear framebuffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Postp::EndRender() {
	// bind default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// disable depth test so screen-space quad isn't discarded due to depth test.
	glDisable(GL_DEPTH_TEST);		
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT);
}

void Postp::Render() {
	postShader->use();
	glBindVertexArray(vao);

	//set the rendered color texture as shader uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, coltex);	
	postShader->setUniform("screenTexture", 0);

	//set the rendered normal texture as shader uniform
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normtex);
	postShader->setUniform("normTexture", 1);

	//draw quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//glBindVertexArray(0);
}

void Postp::init() {
	// coords for screen filling quad
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	// screen quad VAO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

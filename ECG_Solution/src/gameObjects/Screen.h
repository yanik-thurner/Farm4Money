#pragma once
#include "Interactable.h"
#include "../NewTexture.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2\video.hpp>

class Screen : public SceneObject
{
private:
	float total_time = 0;
public:
	Screen(glm::vec2 size, glm::vec3 position);
	~Screen();


	std::vector<std::unique_ptr<GLuint>> texs;
	GLuint vao, vbo_position;

	
	int frame_now = 0;
	void next_frame();
	Shader *shader;

	// Inherited via SceneObject

	virtual void apply_model() override;
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;
};


#include "Screen.h"

Screen::Screen(glm::vec2 size, glm::vec3 position) : SceneObject(SceneObject::model_manager->get_model(SceneObject::model_manager->SCREEN), glm::mat4(1.0f), position)
{
	CvCapture* capture;
	IplImage *image;

	capture = cvCaptureFromFile("./assets/sprites/gameplay.mp4");

	while (true) {
		image = cvQueryFrame(capture);
		if (!image) {
			break;
		}

		cvCvtColor(image, image, CV_BGR2RGB);
		
		texs.push_back(std::move(std::make_unique<GLuint>(-1)));

		glGenTextures(1, texs.back().get());
		glBindTexture(GL_TEXTURE_2D, *texs.back().get());

		glPixelStorei(GL_UNPACK_ALIGNMENT, image->align);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, image->widthStep / image->nChannels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	}
	
	transform_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x * 2, position.y * 2, position.z * 2));
	current_model.transform(transform_matrix);
}

void Screen::next_frame() {
	frame_now += 1;
	if (frame_now >= texs.size()) {
		frame_now = 0;
	}
}
Screen::~Screen()
{
}

void Screen::apply_model()
{
}

void Screen::update(double timediff, glm::vec3 dir)
{
	total_time += timediff;
	if (total_time >= 0.02) {
		next_frame();
		total_time = 0;
	}
	
}

void Screen::draw(glm::vec3 cam_pos, float view_frustum)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texs[frame_now]);
	
	shader->use();
	shader->setUniform("tex", 0);

	current_model.draw(cam_pos, view_frustum);
	glBindTexture(GL_TEXTURE_2D, 0);

}

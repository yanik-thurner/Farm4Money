#include "PlayerCamera.h"



PlayerCamera::PlayerCamera(float fov, float aspect, float near, float far)
{
	_projMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
	_viewMatrix = glm::mat4(1.0f);
	_position = glm::vec3(0, 0, 0);
	alpha = 0;
	beta = 0;
	_mouseX = 0;
	_mouseY = 0;
}


PlayerCamera::~PlayerCamera()
{
}

glm::vec3 PlayerCamera::getViewDir()
{
	glm::mat4 iview = glm::inverse(_viewMatrix);
	//std::cout << iview[0][2] << iview[1][2] << iview[2][2] << std::endl;
	//std::cout << "------------------" << std::endl;
	//std::cout << glm::to_string(iview) << std::endl;
	//std::cout << "------------------" << std::endl;
	return glm::vec3(iview[0][2], iview[1][2], iview[2][2]);
}

glm::vec3 PlayerCamera::getPosition()
{
	return _position;
}

glm::mat4 PlayerCamera::getViewProjectionMatrix()
{
	return _projMatrix * glm::inverse(_viewMatrix);
}

glm::mat4 PlayerCamera::getViewMatrix()
{
	return glm::inverse(_viewMatrix);
}

void PlayerCamera::update(int x, int y, glm::vec3 pos, float dt, bool drag = true)
{	
	glm::mat4 cam = glm::mat4(1.0f);
	//std::cout << "x: " + std::to_string(x) << std::endl;
	//std::cout << "y: " + std::to_string(y) << std::endl;

	if (drag) {
		float deltax = (_mouseX - x) * 4, deltay = (_mouseY - y) * 4;
		alpha = alpha + deltay *dt;
		if (alpha > 15) alpha = 15;
		if (alpha < -80) alpha = -80;
		beta = beta + deltax * dt;
		if (beta > 360) beta = beta -360;
		if (beta < -360) beta = beta + 360;
	}
	float radius = 4;
	float tempR = glm::cos(glm::radians(alpha)) * radius;

	cam = glm::translate(cam, glm::vec3(pos.x, pos.y, pos.z ));
	cam = glm::translate(cam, glm::vec3(glm::sin(glm::radians(beta)) * tempR, -glm::sin(glm::radians(alpha)) * radius, glm::cos(glm::radians(beta)) * tempR));
	cam = glm::rotate(cam, glm::radians(beta), glm::vec3(0.0f, 1.0f, 0.0f));
	cam = glm::rotate(cam, glm::radians(alpha), glm::vec3(1.0f, 0.0f, 0.0f));

	_mouseY = y;
	_mouseX = x;
	_position = pos;
	_viewMatrix = cam;
}

float PlayerCamera::getBeta()
{
	return beta;
}

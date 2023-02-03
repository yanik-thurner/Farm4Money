#pragma once
#include <memory>

#include <glm\glm.hpp>
#include <glm\gtc/matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include "glm/gtx/string_cast.hpp"

class PlayerCamera
{
public:
	PlayerCamera(float fov, float aspect, float near, float far);
	~PlayerCamera();

	glm::vec3 getViewDir();
	glm::vec3 getPosition();
	glm::mat4 getViewProjectionMatrix();
	glm::mat4 getViewMatrix();
	void update(int x, int y, glm::vec3 pos, float dt, bool drag);
	float getBeta();

private:
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	int _mouseX, _mouseY;
	float alpha, beta;

	glm::vec3 _position;
	glm::vec3 _strafe;
};


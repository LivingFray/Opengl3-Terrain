#pragma once
#include "glm\glm.hpp"
#include "loaders.h"

class Camera {
private:
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 dir;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 pos = glm::vec3(0, 0, 0);
	float FOV = 75.0;
	float mouseSpeed = 0.1f;
	double camSpeed = 8.0;
	float theta = 0;
	float phi = 0;

public:
	inline glm::mat4 getView() {
		return view;
	}
	inline glm::mat4 getProjection() {
		return projection;
	}
	inline glm::vec3 getDirection() {
		return dir;
	}
	inline glm::vec3 getCameraPos() {
		return pos;
	}
	void calculateCamera(GLFWwindow* win, double time);
	void setFOV(float fov);
	void setMouseSpeed(float speed);
	void setCameraPos(glm::vec3 pos);
};
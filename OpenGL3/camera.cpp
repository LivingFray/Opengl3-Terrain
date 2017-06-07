#include "camera.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

void Camera::calculateCamera(GLFWwindow* win, double time) {
	//Get information from glfw
	int w, h;
	glfwGetWindowSize(win, &w, &h);
	double mx, my;
	glfwGetCursorPos(win, &mx, &my);
	float dx = static_cast<float>(w/2) - static_cast<float>(mx);
	float dy = static_cast<float>(h/2)- static_cast<float>(my);
	//Centre mouse
	glfwSetCursorPos(win, static_cast<double>(w) / 2.0, static_cast<double>(h) / 2.0);
	//Calculate new angle
	theta += static_cast<float>(time) * dx * mouseSpeed;
	phi += static_cast<float>(time) * dy * mouseSpeed;
	//Range checks
	if (theta < 0) {
		theta += glm::two_pi<float>();
	}
	if (phi < 0) {
		phi += glm::two_pi<float>();
	}
	if (theta > glm::two_pi<float>()) {
		theta -= glm::two_pi<float>();
	}
	if (phi > glm::two_pi<float>()) {
		phi -= glm::two_pi<float>();
	}
	if (phi < glm::pi<float>() && phi > glm::half_pi<float>()) {
		phi = glm::half_pi<float>();
	} else if (phi < glm::three_over_two_pi<float>() && phi >= glm::pi<float>()) {
		phi = glm::three_over_two_pi<float>();
	}
	//Create direction vector
	dir = glm::vec3(cos(phi) * sin(theta), sin(phi), cos(phi) * cos(theta));
	//Create right vector
	right = glm::vec3(sin(theta - glm::half_pi<double>()), 0, cos(theta - glm::half_pi<double>()));
	//Create up vector
	up = glm::cross(right, dir);
	//Move camera
	float speed = glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) ? 10.0f : 1.0f;
	if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS) {
		pos += dir * static_cast<float>(speed * camSpeed * time);
	}
	if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS) {
		pos -= dir * static_cast<float>(speed * camSpeed * time);
	}
	if (glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS) {
		pos -= right * static_cast<float>(speed * camSpeed * time);
	}
	if (glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		pos += right * static_cast<float>(speed * camSpeed * time);
	}
	projection = glm::perspective(glm::radians(FOV), static_cast<float>(w) / static_cast<float>(h), 0.1f, 400.0f);
	view = glm::lookAt(pos, pos + dir, up);
}

void Camera::setFOV(float fov) {
	if (fov > 0) {
		FOV = fov;
	}
}

void Camera::setMouseSpeed(float speed) {
	if (speed > 0) {
		mouseSpeed = speed;
	}
}

void Camera::setCameraPos(glm::vec3 pos) {
	this->pos = pos;
}

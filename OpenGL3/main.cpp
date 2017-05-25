#include "loadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtx\euler_angles.hpp"
#include <iostream>

#define FULLSCREEN 0

#define FOV 75.0f

#define ROT_SPEED 3.14

using namespace std;

GLuint VertexArrayID;
static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};
// This will identify our vertex buffer
GLuint vertexbuffer;

GLuint programID;

glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 mvp = glm::mat4(1.0f);
GLuint matrixID;

glm::vec3 camPos;
double ang;

void init() {
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	programID = loadShaders("vertex.glsl", "fragment.glsl");
	matrixID = glGetUniformLocation(programID, "MVP");
	ang = 0;
}


void draw(GLFWwindow* window) {
	//Model transformation here
	model = glm::mat4(1.0f);
	view = glm::lookAt(
		camPos,
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	mvp = projection * view * model;
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}


void update(double elapsed) {
	ang += elapsed * ROT_SPEED;
	if (ang > 2 * glm::pi<float>()) {
		ang -= glm::pi<float>() * 2;
	}
	camPos = glm::eulerAngleXYZ(0.0, ang, 0.0) * glm::vec4(0, 1, -1, 1);
}


void reshape(GLFWwindow* win, int width, int height) {
	glViewport(0, 0, width, height);
	projection = glm::perspective(glm::radians(FOV), (float)width / (float)height, 0.1f, 100.0f);
}


int main() {
	if (!glfwInit()) {
		cerr << "Cannot initialise GLFW" << endl;
		return 1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if FULLSCREEN==0
	GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGL 3.3", NULL, NULL);
#else
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGL 3.3", monitor, NULL);
#endif
	if (!window) {
		cerr << "Cannot run opengl 3 on this potato" << endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		reshape(window, w, h);
	}
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		cerr << "Cannot create GLEW" << endl;
		glfwTerminate();
		return 1;
	}
	init();
	glfwSetWindowSizeCallback(window, reshape);
	bool running = true;
	double time = glfwGetTime();
	while (running) {
		double nTime = glfwGetTime();
		double elapsed = nTime - time;
		time = nTime;
		glfwPollEvents();
		//update
		update(elapsed);
		//draw
		draw(window);
		glfwSwapBuffers(window);
		//CLOSE WITH ESC
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window)) {
			running = false;
		}
	}
	return 0;
}
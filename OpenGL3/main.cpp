#include "loadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtx\euler_angles.hpp"
#include <iostream>

#define FULLSCREEN 0

#define FOV 75.0f

#define ROT_SPEED (3.1415926535/4)

using namespace std;

GLuint VertexArrayID;
static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};
GLuint cubeArrayID;
static const GLfloat cube_data[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};
// This will identify our vertex buffer
GLuint vertexbuffer;
GLuint cubeBuffer;

GLuint programID;

glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 mvp = glm::mat4(1.0f);
GLuint matrixID;
GLuint colorID;

glm::vec3 camPos;
double ang;

template <typename T, int N>
void genArray(GLuint* vertId, GLuint* vertBuffer, const T(&vertArray)[N]) {
	glGenVertexArrays(1, vertId);
	glBindVertexArray(*vertId);
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, vertBuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, *vertBuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, N * sizeof(GLfloat), vertArray, GL_STATIC_DRAW);
}


void init() {
	genArray(&VertexArrayID, &vertexbuffer, g_vertex_buffer_data);
	genArray(&cubeArrayID, &cubeBuffer, cube_data);
	programID = loadShaders("vertex.glsl", "fragment.glsl");
	matrixID = glGetUniformLocation(programID, "MVP");
	colorID = glGetUniformLocation(programID, "col");
	ang = 0;
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
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
	glUniform3f(colorID, 1.0f, 0.0f, 0.0f);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	model = glm::translate(glm::vec3(0, -2, 0));
	mvp = projection * view * model;
	glBindBuffer(GL_ARRAY_BUFFER, cubeBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniform3f(colorID, 0.0f, 1.0f, 1.0f);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableVertexAttribArray(0);
}


void update(double elapsed) {
	ang += elapsed * ROT_SPEED;
	if (ang > 2 * glm::pi<float>()) {
		ang -= glm::pi<float>() * 2;
	}
	camPos = glm::eulerAngleXYZ(0.0, ang, 0.0) * glm::vec4(0, 2, -2, 1);
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
#include "loaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtx\euler_angles.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "camera.h"
#include "Object.h"
#include <iostream>
#include <string>

#define FULLSCREEN 0

#define FOV 75.0f

using namespace std;

GLuint programID;
GLuint texture;
GLuint matrixID;

glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 mvp = glm::mat4(1.0f);

Camera cam;

Object cube;

Object monkey;

GLFWwindow* window;

int frames;
int fps;
double fps_time;

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
	int w, h, c;
	//Load texture
	stbi_uc* img = stbi_load("texture.png", &w, &h, &c, 4);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//Hook into shaders
	programID = loadShaders("vertex.glsl", "fragment.glsl");
	//Set up opengl
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	//Create cube
	cube = Object(texture, programID);
	cube.generateBuffers();
	monkey = Object(texture, programID, "monkey.obj");
	monkey.generateBuffers();
	monkey.setModel(glm::translate(glm::vec3(0, 2, 0)));
}


void draw(GLFWwindow* window) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	cube.draw(cam);
	monkey.draw(cam);
}


void update(double elapsed) {
	cam.calculateCamera(window, elapsed);
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
	window = glfwCreateWindow(1024, 768, "OpenGL 3.3", NULL, NULL);
#else
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	window = glfwCreateWindow(mode->width, mode->height, "OpenGL 3.3", monitor, NULL);
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
	glfwSwapInterval(1);
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
		frames++;
		fps_time += elapsed;
		if (fps_time > 1) {
			fps = frames;
			frames = 0;
			fps_time -= 1;
		}
		glfwSetWindowTitle(window, (std::to_string(fps) + " fps").c_str());
		glfwSwapBuffers(window);
		//CLOSE WITH ESC
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window)) {
			running = false;
		}
	}
	return 0;
}
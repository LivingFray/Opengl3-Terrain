#pragma once
#include "Camera.h"
#include <vector>

using std::vector;

class Object {
public:
	Object();
	Object(GLuint texture, GLuint matID, const char* add = "cube.obj");
	~Object();
	// Creates the buffers for the loaded object
	void generateBuffers();
	// Draws the object
	void draw(Camera cam);
protected:
	//List of properties for verticies
	vector<glm::vec3> verticies;
	vector<glm::vec2> uv;
	vector<glm::vec3> normals;
	//Transformations applied to object
	glm::mat4 model;
	//Texture to use for object
	GLuint texture;
	//Buffers for properties
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalsBuffer;
	//Position of matrix in shader
	GLuint matrixID;
};


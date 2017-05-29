#pragma once
#include "Camera.h"
#include <vector>

using std::vector;

class Object {
public:
	Object();
	Object(GLuint texture, GLuint progID, const char* add = "cube.obj");
	~Object();
	// Creates the buffers for the loaded object
	void generateBuffers();
	// Draws the object
	void draw(Camera cam);
	// Sets the model matrix of the object
	void setModel(glm::mat4 m);
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
	//The program ID for the shaders
	GLuint programID;
	GLuint matrixID;
	GLuint viewID;
	GLuint modelID;
	GLuint lightID;
	GLuint textureID;
	GLuint lightPowerID;
	GLuint lightColorID;
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	float lightPower;
};


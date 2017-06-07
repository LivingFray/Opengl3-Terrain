#pragma once
#include "Camera.h"
#include <vector>

using std::vector;

class Object {
public:
	Object();
	Object(GLuint texture, GLuint progID, const char* add = "cube.obj", GLuint normal = 0);
	~Object();
	// Draws the object
	virtual void draw(Camera cam);
	// Sets the model matrix of the object
	void setModel(glm::mat4 m);
protected:
	void sharedInit();
	// Creates the buffers for the loaded object
	virtual void generateBuffers();
	//List of properties for verticies
	vector<glm::vec3> vertices;
	vector<glm::vec2> uvs;
	vector<glm::vec3> normals;
	vector<unsigned int> indices;
	vector<glm::vec3> tangents;
	vector<glm::vec3> bitangents;
	//Transformations applied to object
	glm::mat4 model;
	//Texture to use for object
	GLuint texture;
	GLuint normal;
	//Buffers for properties
	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalsBuffer;
	GLuint indiciesBuffer;
	GLuint tangentBuffer;
	GLuint bitangentBuffer;
	//The program ID for the shaders
	GLuint programID;
	GLuint matrixID;
	GLuint viewID;
	GLuint modelID;
	GLuint modelViewID;
	GLuint useNormalID;
	GLuint lightID;
	GLuint textureID;
	GLuint normalID;
	GLuint lightPowerID;
	GLuint lightColorID;
	GLuint fogID;
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	glm::vec3 fogColor;
	float lightPower;
	bool cull;
	bool useNormal;
};


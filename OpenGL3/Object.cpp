#include "Object.h"
#include "objloader.h"

Object::Object() {
	model = glm::mat4(1.0f);
}

Object::Object(GLuint texture, GLuint matID, const char* add) {
	model = glm::mat4(1.0f);
	loadOBJ(add, verticies, uv, normals);
	this->texture = texture;
	matrixID = matID;
}


Object::~Object() {
}


// Creates the buffers for the loaded object
void Object::generateBuffers() {
	GLuint id;
	//Verticies
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(glm::vec3), &verticies[0], GL_STATIC_DRAW);
	//UV
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
	//Normals
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
}


// Draws the object
void Object::draw(Camera cam) {
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, texture);
	//model = glm::translate(glm::vec3(0, -2, 0));
	glm::mat4 mvp = cam.getProjection() * cam.getView() * model;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, verticies.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

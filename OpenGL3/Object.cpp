#include "Object.h"
#include "objloader.h"

Object::Object() {
	model = glm::mat4(1.0f);
}

Object::Object(GLuint texture, GLuint programID, const char* add) {
	model = glm::mat4(1.0f);
	loadOBJ(add, verticies, uv, normals);
	this->texture = texture;
	this->programID = programID;
	matrixID = glGetUniformLocation(programID, "MVP");
	viewID = glGetUniformLocation(programID, "V");
	modelID = glGetUniformLocation(programID, "M");
	lightID = glGetUniformLocation(programID, "lightPos");
	lightPos = glm::vec3(2, 2, 0);
	textureID = glGetUniformLocation(programID, "tex");
	lightColorID = glGetUniformLocation(programID, "lightColor");
	lightPowerID = glGetUniformLocation(programID, "lightPower");
	lightPower = 50;
	lightColor = glm::vec3(1, 1, 1);
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
	lightPos = cam.getCameraPos();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);
	//(textureID, texture);
	//model = glm::translate(glm::vec3(0, -2, 0));
	glm::mat4 mvp = cam.getProjection() * cam.getView() * model;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &cam.getView()[0][0]);
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform3fv(lightID, 1, &lightPos[0]);
	glUniform3fv(lightColorID, 1, &lightColor[0]);
	glUniform1fv(lightPowerID, 1, &lightPower);
	glDrawArrays(GL_TRIANGLES, 0, verticies.size());
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Object::setModel(glm::mat4 m) {
	model = m;
}

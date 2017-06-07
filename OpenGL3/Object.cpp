#include "Object.h"
#include "objloader.h"
#include "vboindexer.h"

Object::Object() {
}

Object::Object(GLuint texture, GLuint programID, const char* add){
	vector<glm::vec3> _vertices;
	vector<glm::vec2> _uvs;
	vector<glm::vec3> _normals;
	loadOBJ(add, _vertices, _uvs, _normals);
	indexVBO(_vertices, _uvs, _normals, indices, vertices, uvs, normals);
	this->texture = texture;
	this->programID = programID;
	sharedInit();
	generateBuffers();
	cull = true;
}

void Object::sharedInit() {
	model = glm::mat4(1.0f);
	matrixID = glGetUniformLocation(programID, "MVP");
	viewID = glGetUniformLocation(programID, "V");
	modelID = glGetUniformLocation(programID, "M");
	lightID = glGetUniformLocation(programID, "lightPos");
	lightPos = glm::vec3(0, 5000, 0);
	textureID = glGetUniformLocation(programID, "tex");
	lightColorID = glGetUniformLocation(programID, "lightColor");
	lightPowerID = glGetUniformLocation(programID, "lightPower");
	lightPower = 1;
	lightColor = glm::vec3(1, 1, 1);
	fogID = glGetUniformLocation(programID, "fogCol");
	fogColor = glm::vec3(135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f);
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	//UV
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	//Normals
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	//Indicies
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	glGenBuffers(1, &indiciesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}


// Draws the object
void Object::draw(Camera cam) {
	if(cull) {
		glEnable(GL_CULL_FACE);
	}else {
		glDisable(GL_CULL_FACE);
	}
	//lightPos = cam.getCameraPos();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureID, 0);
	glm::mat4 mvp = cam.getProjection() * cam.getView() * model;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)nullptr);
	glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &cam.getView()[0][0]);
	glUniformMatrix4fv(modelID, 1, GL_FALSE, &model[0][0]);
	glUniform3fv(lightID, 1, &lightPos[0]);
	glUniform3fv(lightColorID, 1, &lightColor[0]);
	glUniform1fv(lightPowerID, 1, &lightPower);
	glUniform3fv(fogID, 1, &fogColor[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)nullptr);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Object::setModel(glm::mat4 m) {
	model = m;
}

#pragma once
#include "Object.h"
class Terrain :
	public Object {
public:
	Terrain();
	explicit Terrain(GLuint texture, GLuint programID);
	~Terrain();
	void draw(Camera cam);
protected:
	void generateTerrain();
	void generateBuffers();
	vector<unsigned int> indices;
};


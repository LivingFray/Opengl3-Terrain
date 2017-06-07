#pragma once
#include "Object.h"
class Terrain :
	public Object {
public:
	Terrain();
	explicit Terrain(GLuint texture, GLuint programID);
	~Terrain();
protected:
	void generateTerrain();
};


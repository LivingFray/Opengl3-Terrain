#include "Terrain.h"
#include <vector>
#include <random>

#define N 8
#define NODES_X ((1 << N) + 1)
#define NODES_Z NODES_X

#define WIDTH 800.0
#define DEPTH 800.0
#define MIN_Y (-100.0)
#define MAX_Y (200.0)
#define REPEAT 20.0

#define ROUGHNESS 1.0

#define getNode(x, z) (nodes[(x) * NODES_X + (z)])


using namespace std;

Terrain::Terrain() {
}

Terrain::Terrain(GLuint texture, GLuint programID) {
	this->programID = programID;
	this->texture = texture;
	Object::sharedInit();
	generateTerrain();
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	cull = false;
}


Terrain::~Terrain() {
}



void Terrain::generateTerrain() {
	uniform_real_distribution<double> uni(MIN_Y, MAX_Y);
	default_random_engine rng;
	vector<glm::vec3> nodes;
	vector<glm::vec2> uv;
	//Generate nodes
	for (int x = 0; x < NODES_X; x++) {
		for (int z = 0; z < NODES_Z; z++) {
			nodes.push_back(glm::vec3(0,0,0));
			uv.push_back(glm::vec2(0, 0));
		}
	}
	for (int x = 0; x < NODES_X; x++) {
		for (int z = 0; z < NODES_Z; z++) {
			getNode(x, z) =	glm::vec3(-WIDTH / 2.0f + x * (WIDTH / NODES_X), MIN_Y, -DEPTH / 2.0f + z * (DEPTH / NODES_Z));
		}
	}
	//Create corners
	for(int x=0;x<2;x++) {
		for(int z=0;z<2;z++) {
			getNode(x * (NODES_X - 1), z * (NODES_Z-1)).y = static_cast<float>(uni(rng));
		}
	}
	//Perform iterative steps
	int size = NODES_X - 1;
	float rand_var = (MAX_Y - MIN_Y) / 2;
	while(size>1) {
		//Diamond stages
		uniform_real_distribution<float> u(-rand_var, rand_var);
		for (int x = 0; x < (NODES_X - 1); x+=size) {
			for (int z = 0; z < (NODES_Z - 1); z+=size) {
				float p1, p2, p3, p4;
				p1 = getNode(x, z).y;
				p2 = getNode(x, z + size).y;
				p3 = getNode(x + size, z + size).y;
				p4 = getNode(x + size, z).y;
				getNode(x + size / 2, z + size / 2).y = (p1 + p2 + p3 + p4) / 4.0f + u(rng);

			}
		}
		//Square stages
		//(x + y - 1) ÷ y
		int s = (NODES_X - 1 + size / 2)/(size / 2);
		for (int x = 0; x < s; x++) {
			for (int z = 0; z < s; z++) {
				if(x%2 == z%2) {
					continue;
				}
				int px = x * (size / 2);
				int pz = z * (size / 2);
				float h = 0;
				int n = 0;
				if(x>0) {
					n++;
					h += getNode(px - (size / 2), pz).y;
				}
				if(x<s-1) {
					n++;
					h += getNode(px + (size / 2), pz).y;
				}
				if (z>0) {
					n++;
					h += getNode(px, pz - (size / 2)).y;
				}
				if (z<s - 1) {
					n++;
					h += getNode(px, pz + (size / 2)).y;
				}
				if (n > 0) {
					h /= n;
				}
				getNode(px, pz).y = h + u(rng);
			}
		}
		rand_var *= static_cast<float>(pow(2,-ROUGHNESS));
		size = size / 2;
	}
	//Convert to buffers
	//Vertices: Node list can be used directly
	//UV list: made to be related to position in grid
	for (int x = 0; x < NODES_X; x++) {
		for (int z = 0; z < NODES_Z; z++) {
			int pos = x * NODES_X + z;
			uv[pos] = glm::vec2(REPEAT * static_cast<double>(x) / NODES_X, REPEAT * static_cast<double>(z) / NODES_Z);
		}
	}
	//Normals: Calculate connected edges and average
	vector<glm::vec3> norm;
	for (int x = 0; x < NODES_X; x++) {
		for (int z = 0; z < NODES_Z; z++) {
			int pos = x * NODES_X + z;
			glm::vec3 node = nodes[pos];
			int n = 0;
			glm::vec3 normal = glm::vec3(0,0,0);
			//Neighbours to the -x
			if(x>0) {
				glm::vec3 negX = nodes[(x - 1) * NODES_X + z] - node;
				//Neighbours to the -z
				if(z>0) {
					glm::vec3 negZ = nodes[pos - 1] - node;
					n++;
					normal -= glm::normalize(glm::cross(negX, negZ));
				}
				//Neighbours to the +z
				if(z<NODES_Z-1) {
					glm::vec3 posZ = nodes[pos + 1] - node;
					n++;
					normal -= glm::normalize(glm::cross(posZ, negX));
				}
			}
			//Neighbours to the +x
			if (x<NODES_X - 1) {
				glm::vec3 posX = nodes[(x + 1) * NODES_X + z] - node;
				//Neighbours to the -z
				if (z>0) {
					glm::vec3 negZ = nodes[pos - 1] - node;
					n++;
					normal -= glm::normalize(glm::cross(negZ, posX));
				}
				//Neighbours to the +z
				if (z<NODES_Z - 1) {
					glm::vec3 posZ = nodes[pos + 1] - node;
					n++;
					normal -= glm::normalize(glm::cross(posX, posZ));
				}
			}
			if(n==0) {
				normal = glm::vec3(0, 1, 0);
			}
			normal = glm::normalize(normal);
			norm.push_back(normal);
		}
	}
	//Generate indices
	vector<unsigned int> ind;
	//Each node has up to 4 associated faces
	//For even z: start at beginning, for odd start 1 in
	//Add "diamond" centred on every other position
	for(int z = 0; z < NODES_Z; z++) {
		for(int x = z%2; x < NODES_X; x+=2) {
			int pos = x * NODES_X + z;
			//Neighbours to the -x
			if (x>0) {
				//Neighbours to the -z
				if (z>0) {
					//Pos
					ind.push_back(pos);
					//Negative Z
					ind.push_back(x * NODES_X + z - 1);
					//Negative X
					ind.push_back((x - 1) * NODES_X + z);
				}
				//Neighbours to the +z
				if (z<NODES_Z - 1) {
					//Pos
					ind.push_back(pos);
					//Negative X
					ind.push_back((x - 1) * NODES_X + z);
					//Positive Z
					ind.push_back(x * NODES_X + z + 1);
				}
			}
			//Neighbours to the +x
			if (x<NODES_X - 1) {
				//Neighbours to the -z
				if (z>0) {
					//Pos
					ind.push_back(pos);
					//Positive X
					ind.push_back((x + 1) * NODES_X + z);
					//Negative Z
					ind.push_back(x * NODES_X + z - 1);
				}
				//Neighbours to the +z
				if (z<NODES_Z - 1) {
					//Pos
					ind.push_back(pos);
					//Positive Z
					ind.push_back(x * NODES_X + z + 1);
					//Positive X
					ind.push_back((x + 1) * NODES_X + z);
				}
			}
		}
	}
	//Convert and pass on
	vertices = nodes;
	uvs = uv;
	normals = norm;
	indices = ind;
	generateBuffers();
}



// Creates the buffers for the loaded object
void Terrain::generateBuffers() {
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}


// Draws the object
void Terrain::draw(Camera cam) {
	if (cull) {
		glEnable(GL_CULL_FACE);
	} else {
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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesBuffer);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)nullptr);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindTexture(GL_TEXTURE_2D, 0);
}

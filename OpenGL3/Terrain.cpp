#include "Terrain.h"
#include <vector>
#include <random>

#define WIDTH 40
#define DEPTH 40
#define NODES_X 40
#define NODES_Z 40
#define MIN_Y (-12)
#define MAX_Y (-2)
#define REPEAT 5


using namespace std;

Terrain::Terrain() {
}

Terrain::Terrain(GLuint texture, GLuint programID) {
	this->programID = programID;
	this->texture = texture;
	Object::sharedInit();
	generateTerrain();
}


Terrain::~Terrain() {
}



void Terrain::generateTerrain() {
	uniform_real_distribution<double> uni(MIN_Y, MAX_Y);
	default_random_engine rng;
	//Temp to test conversion to opengl buffers
	vector<glm::vec3> nodes;
	vector<glm::vec2> uv;
	for(int i = 0; i < NODES_X * NODES_Z; i++) {
		nodes.push_back(glm::vec3(0, 0, 0));
		uv.push_back(glm::vec2(0,0));
	}
	for (int x = 0; x < NODES_X; x++) {
		for (int z = 0; z < NODES_Z; z++) {
			int pos = x * NODES_X + z;
			nodes[pos] = glm::vec3(-WIDTH / 2 + x * (WIDTH / NODES_X), uni(rng), -DEPTH / 2 + z * (DEPTH / NODES_Z));
			uv[pos] = glm::vec2(static_cast<double>(x % REPEAT) / NODES_X, static_cast<double>(z % REPEAT) / NODES_Z);
		}
	}
	//Convert to buffers
	//Vertices: Node list can be used directly
	//UV list: made to be related to position in grid
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
					normal += glm::normalize(glm::cross(negX, negZ));
				}
				//Neighbours to the +z
				if(z<NODES_Z-1) {
					glm::vec3 posZ = nodes[pos + 1] - node;
					n++;
					normal += glm::normalize(glm::cross(posZ, negX));
				}
			}
			//Neighbours to the +x
			if (x<NODES_X - 1) {
				glm::vec3 posX = nodes[(x + 1) * NODES_X + z] - node;
				//Neighbours to the -z
				if (z>0) {
					glm::vec3 negZ = nodes[pos - 1] - node;
					n++;
					normal += glm::normalize(glm::cross(negZ, posX));
				}
				//Neighbours to the +z
				if (z<NODES_Z - 1) {
					glm::vec3 posZ = nodes[pos + 1] - node;
					n++;
					normal += glm::normalize(glm::cross(posX, posZ));
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
	vector<unsigned short> ind;
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

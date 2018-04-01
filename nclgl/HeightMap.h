#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "../../nclgl/Mesh.h"

class HeightMap : public Mesh {
public:
	HeightMap(std::string name, int rw = 257, int rh = 257, float hx = 16.0f, float hy = 16.0f, float hz = 1.25f, float htx = 1.0f / 16.0f, float htz = 1.0f / 16.0f);
	~HeightMap(void) {};

	int RAW_WIDTH = 257;
	int RAW_HEIGHT = 257;

	float HEIGHTMAP_X = 16.0f;
	float HEIGHTMAP_Z = 16.0f;
	float HEIGHTMAP_Y = 1.25f;
	float HEIGHTMAP_TEX_X = 1.0f / 16.0f;
	float HEIGHTMAP_TEX_Z = 1.0f / 16.0f;
protected:
};
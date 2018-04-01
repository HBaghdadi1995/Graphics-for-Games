#pragma once
#include "../../nclgl/OBJMesh.h"
#include "../../nclgl/SceneNode.h"

class CubeRobot : public SceneNode {
public:
	CubeRobot(Vector3 pos = Vector3(0,0,0));
	~CubeRobot(void) {};
	virtual void Update(float msec);

	static void CreateCube() {
		OBJMesh *m = new OBJMesh();
		m->LoadOBJMesh("../../Meshes/cube.obj");
		cube = m;
	}
	static void DeleteCube() { delete cube; }

protected:
	static Mesh* cube;
	SceneNode* head;
	SceneNode* leftArm;
	SceneNode* rightArm;
	Vector3 pos;
};
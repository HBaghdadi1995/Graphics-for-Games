#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/heightmap.h"

#define SHADOWSIZE 2048

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void DrawMesh();
	void DrawFloor();
	void DrawShadowScene();
	void DrawCombinedScene();
	void DrawForest();
	void DrawSkyBox();

	Shader* sceneShader;
	Shader* shadowShader;
	Shader * skyboxShader;

	GLuint shadowTex;
	GLuint shadowFBO;

	OBJMesh* hellData;
	SceneNode* hellNode;
	Mesh* floor;
	Camera* camera;
	Light* light;

	HeightMap* forest;

	GLuint cubeMesh;
};
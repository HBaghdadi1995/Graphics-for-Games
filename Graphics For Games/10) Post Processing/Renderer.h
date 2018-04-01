#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Camera.h"

#define POST_PASSES 10

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	void PresentScene();
	void DrawPostProcess();
	void DrawScene();
	void DrawSobel();

	Shader* sceneShader;
	Shader* processShader;
	Shader* sobelShader;

	Camera* camera;

	Mesh* quad;
	HeightMap* heightMap;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
};
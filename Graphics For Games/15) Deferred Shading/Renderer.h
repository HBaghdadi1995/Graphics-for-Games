#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/heightmap.h"

#define LIGHTNUM 8

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	static const int SCENENUMS = 3;

	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();

	void GenerateScreenTexture(GLuint &into, bool depth = false);

	void RenderScene0();

	void DrawSkyBox();

	Shader* processShader;
	Shader* skyboxShader;
	Shader* textShader;
	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;

	Light* pointLights;

	HeightMap* mars;
	HeightMap* forest;
	HeightMap* pond;
	
	OBJMesh* sphere;
	Mesh* quad;
	//Camera* camera;

	float rotation;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;

	GLuint pointLightFBO;
	GLuint lightEmissiveTex;
	GLuint lightSpecularTex;

	GLuint cubeMesh;

	Camera* camera[SCENENUMS];

	int currentScene = 0;

};
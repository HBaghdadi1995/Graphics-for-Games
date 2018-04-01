#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"
#include "../../nclgl/heightmap.h"
#include "textmesh.h"
#include "ParticleEmitter.h"
#include <string> 
#include "../../nclgl/MD5Mesh.h"
#include "../../nclgl/MD5Node.h"

#define LIGHTNUM 8
#define SHADOWSIZE 2048

class Renderer : public OGLRenderer {
public:
	Renderer(Window &paren);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void lastScene();
	void nextScene();

	void TogglePause() { pause = !pause; }
	void ToggleFrameRate() { frameTrue = !frameTrue; }
	void ToggleAutocam() { autocam = !autocam; }

protected:
	static const int SCENENUMS = 3;

	void DrawMesh();
	void DrawForest();
	void DrawWater();

	void RenderScene0();
	void RenderScene1();
	void RenderScene2();

	void DrawScene0();
	void DrawScene1();
	void DrawScene2();

	void DrawPointLights();
	void CombineBuffers();

	void DrawShadowScene();
	void DrawCombinedScene();

	void PrintFrames();
	void	DrawText(const std::string &text, const Vector3 &position, const float size = 10.0f, const bool perspective = false);

	void InitialiseScene();

	void GenerateScreenTexture(GLuint &into, bool depth = false);

	void DrawTree();
	void DrawFloor();
	void RenderSnow();
	void DrawScreen();

	void PresentScene();
	void DrawSobel();

	void	SetShaderParticleSize(float f);	//And a new setter

	Camera* camera[SCENENUMS];
	Light* light;
	Light* light2;
	//Mesh* heightMap[SCENENUMS];
	HeightMap* mars;
	HeightMap* forest;
	HeightMap* pond;

	//GLuint cubeMap;

	Mesh* quad;
	Mesh* freeQuad;
	OBJMesh* sphere;

	ParticleEmitter*	emitter;

	int currentScene = 0;

	void DrawSkyBox();
	void DrawHellKnight();
	//void DrawSkyBox1();
	//void DrawSkyBox2();

	Shader* textureShader;
	Shader* skyboxShader;
	Shader* textShader;
	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;
	Shader* treeShader;
	Shader* shadowSceneShader;
	Shader* shadowShader;
	Shader* reflectShader;
	Shader* lightShader;
	Shader* particleShader;
	Shader* sobelShader;
	Shader* sobelCombineShader;
	Shader* animShader;

	float refresh;
	float rotation;
	float snowPileup = 0.0f;

	Font*	basicFont;
	Light* pointLights;

	//Light* light0;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	//GLuint bufferColourTex2[2];
	GLuint bufferProcessTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;
	GLuint processFBO;

	GLuint pointLightFBO;
	GLuint lightEmissiveTex;
	GLuint lightSpecularTex;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint cubeMesh;

	OBJMesh * tree;

	bool autocam = true;
	bool pause = false;
	bool frameTrue = true;

	MD5FileData*hellData;
	MD5Node*	hellNode;
};
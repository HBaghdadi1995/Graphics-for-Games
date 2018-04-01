#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	heightMap = new HeightMap(TEXTUREDIR"MarsTerrain.raw");
	camera = new Camera();

	currentShader = new Shader(SHADERDIR"TexturedVertex.glsl",SHADERDIR"TexturedFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"XantheTerra_v5.minimap.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!heightMap->GetTexture()) {
		return;
	}
	SetTextureRepeating(heightMap->GetTexture(), false);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	init = true;
}

Renderer::~Renderer(void) {
	delete heightMap;
	delete camera;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	textureMatrix = Matrix4::Scale(Vector3(1.0f/16.0f, -1.0f / 16.0f, -1.0f / 16.0f)) * Matrix4::Rotation(-90.0f, Vector3(0.0f, 0.0f, 1.0f));

	heightMap->Draw();

	glUseProgram(0);
	SwapBuffers();
}
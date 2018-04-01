#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {

	hellData = new OBJMesh(MESHDIR"lowpolytree.obj", 100);
	hellNode = new SceneNode(hellData);
	//treeShader = new Shader(SHADERDIR"treeVert.glsl", SHADERDIR"shadowFrag.glsl");

	/*hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);


	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");*/

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");

	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram()) {
		return;
	}
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");
	if (!skyboxShader->LinkProgram()) {
		return;
	}
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	forest = new HeightMap(TEXTUREDIR"forest.raw");
	forest->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"tileable-img_0062-verydark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!forest->GetTexture()) {
		return;
	}
	SetTextureRepeating(forest->GetTexture(), true);
	//light = new Light(Vector3((forest->RAW_HEIGHT*forest->HEIGHTMAP_X / 2.0f), 2000.0f, (forest->RAW_HEIGHT*forest->HEIGHTMAP_Z / 2.0f)), Vector4(1, 1, 1, 1), 5500.0f);
	light = new Light(Vector3(2327.16577f, 674.887451f, 5170.17090), Vector4(1, 1, 1, 1), 5500.0f);
	camera = new Camera(0.0f, 0.0f, Vector3(forest->RAW_WIDTH*forest->HEIGHTMAP_X / 2.0f, 500, forest->RAW_WIDTH*forest->HEIGHTMAP_X));
	//camera = new Camera(-8.0f, 40.0f, light->GetPosition());
	

	if (!floor->GetTexture()) {
		return;
	}
	if (!floor->GetBumpMap()) {
		return;
	}

	glEnable(GL_DEPTH_TEST);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	cubeMesh = SOIL_load_OGL_cubemap(TEXTUREDIR"starfield_ft.tga", TEXTUREDIR"starfield_bk.tga", TEXTUREDIR"starfield_up.tga", TEXTUREDIR"starfield_dn.tga", TEXTUREDIR"starfield_rt.tga", TEXTUREDIR"starfield_lf.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	init = true;
}

Renderer::~Renderer(void) {
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
	delete camera;
	delete light;
	delete hellData;
	delete hellNode;
	delete floor;

	delete sceneShader;
	delete shadowShader;
	currentShader = NULL;
}

void Renderer::UpdateScene(float msec){
	camera->UpdateCamera(msec);
	hellNode->Update(msec);
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawSkyBox();
	DrawShadowScene();
	DrawCombinedScene();

	SwapBuffers();
}

void Renderer::DrawShadowScene() {
	projMatrix = Matrix4::Perspective(1.0f, 1000.0f, (float)width / (float)height, 45.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	//light->SetPosition(camera->GetPosition());
	//viewMatrix = camera->BuildViewMatrix();
	viewMatrix  = Matrix4::Rotation(-4.99000549, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-3.04000902, Vector3(0, 1, 0)) *
		Matrix4::Translation(-light->GetPosition());
	textureMatrix = biasMatrix *(projMatrix * viewMatrix);


	UpdateShaderMatrices();

	DrawFloor();
	DrawForest();
	DrawMesh();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"),0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1,(float*)&camera->GetPosition());

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	//viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(2560, 0, 2560));
	viewMatrix = camera -> BuildViewMatrix ();
	//textureMatrix = biasMatrix*(projMatrix*viewMatrix);
	UpdateShaderMatrices();

	//DrawFloor();
	DrawForest();
	DrawMesh();

	glUseProgram(0);
}

void Renderer::DrawMesh() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			modelMatrix.ToIdentity();
			modelMatrix = Matrix4::Translation(Vector3(100 + 900 * i, 300, 100 + 900 * j));

			Matrix4 tempMatrix = textureMatrix * modelMatrix;

			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

			hellNode->Draw(*this);
		}
	}
}

void Renderer::DrawFloor() {
	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(450, 450, 1));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	floor->Draw();
}

void Renderer::DrawForest() {
	modelMatrix.ToIdentity();
	//modelMatrix = Matrix4::Translation(Vector3(-500, -200, -500));



	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	forest->Draw();
}
void Renderer::DrawSkyBox() {

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	/*glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, spaceSkyBox);*/

	UpdateShaderMatrices();
	floor->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}
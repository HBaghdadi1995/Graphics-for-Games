#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	rotation = 0.0f;

	/*for (int i = 0; i < SCENENUMS; i++) {
		camera[i] = new Camera(0.0f, 0.0f, Vector3(RAW_WIDTH*HEIGHTMAP_X / 2.0f, 500, RAW_WIDTH*HEIGHTMAP_X));
	}*/

	//Create Shader Programmes
	textureShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!textureShader->LinkProgram()) {
		return;
	}
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TextFragment.glsl");
	if (!textShader->LinkProgram()) {
		return;
	}
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");
	if (!skyboxShader->LinkProgram()) {
		return;
	}
	sceneShader = new Shader(SHADERDIR"bumpvertex.glsl", SHADERDIR"bufferfragment.glsl");
	if (!sceneShader->LinkProgram()) {
		return;
	}
		
	combineShader = new Shader(SHADERDIR"combinevert.glsl", SHADERDIR"combinefrag.glsl");
	if (!combineShader->LinkProgram()) {
		return;
	}
	pointlightShader = new Shader(SHADERDIR"pointlightvertex.glsl", SHADERDIR"pointlightfragment.glsl");
	if (!pointlightShader->LinkProgram()) {
		return;
	}
	treeShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TextFragment.glsl");
	if (!treeShader->LinkProgram()) {
		return;
	}
	shadowSceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");
	if (!shadowSceneShader->LinkProgram()) {
		return;
	}
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
	if (!shadowShader->LinkProgram()) {
		return;
	}
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	if (!reflectShader->LinkProgram()) {
		return;
	}
	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");
	if (!lightShader->LinkProgram()) {
		return;
	}
	particleShader = new Shader(SHADERDIR"vertex.glsl", SHADERDIR"fragment.glsl", SHADERDIR"geometry.glsl");
	if (!particleShader->LinkProgram()) {
		return;
	}
	sobelShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"sobel.glsl");
	if (!sobelShader->LinkProgram()) {
		return;
	}
	sobelShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"sobel.glsl");
	if (!sobelShader->LinkProgram()) {
		return;
	}
	sobelCombineShader = new Shader(SHADERDIR"CombineVert.glsl", SHADERDIR"sobelCombineFrag.glsl");
	if (!sobelCombineShader->LinkProgram()) {
		return;
	}
	animShader = new Shader(SHADERDIR"skeletonvertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!animShader->LinkProgram()) {
		return;
	}


	//****Create Meshes****
	
	quad = Mesh::GenerateQuad();
	//http://www.interlopers.net/tutorials/8062
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(quad->GetTexture(), true);


	freeQuad = Mesh::GenerateQuad();

	//**Scene One**

	//https://zero-k.info/Maps/DetailName?name=XantheTerra_v5
	mars = new HeightMap(TEXTUREDIR"marsTerrain.raw", 257, 257, 16.0f, 16.0f, 1.25f, 1.0f/257.0f, 1.0f/257.0f);

	mars->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"XantheTerra_v5_minimap.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	mars->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!mars->GetBumpMap()) {
		return;
	}

	if (!mars->GetTexture()) {
		return;
	}
	SetTextureRepeating(mars->GetTexture(), true);
	SetTextureRepeating(mars->GetBumpMap(), true);

	//http://www.custommapmakers.org/skyboxes.php

	cubeMesh = SOIL_load_OGL_cubemap(TEXTUREDIR"starfield_ft.tga", TEXTUREDIR"starfield_bk.tga", TEXTUREDIR"starfield_up.tga", TEXTUREDIR"starfield_dn.tga", TEXTUREDIR"starfield_rt.tga", TEXTUREDIR"starfield_lf.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	camera[0] = new Camera(0.0f, 0.0f, Vector3(mars->RAW_WIDTH*mars->HEIGHTMAP_X / 2.0f, 500, mars->RAW_WIDTH*mars->HEIGHTMAP_X));

	/*if (!cubeMap) {
		return;
	}*/

	//**Scene Two**
	forest = new HeightMap(TEXTUREDIR"forest.raw");

	forest->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"tileable-img_0062-verydark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!forest->GetTexture()) {
		return;
	}
	SetTextureRepeating(forest->GetTexture(), true);

	camera[1] = new Camera(0.0f, 0.0f, Vector3(forest->RAW_WIDTH*forest->HEIGHTMAP_X / 2.0f, 500, forest->RAW_WIDTH*forest->HEIGHTMAP_X));
	light = new Light(Vector3(2327.16577f, 674.887451f, 5170.17090), Vector4(1, 1, 1, 1), 5500.0f);


	tree = new OBJMesh();
	tree->LoadOBJMesh(MESHDIR"lowpolytree.obj",100);
	tree->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SetTextureRepeating(tree->GetTexture(), true);

	//A new particle emitter!
	emitter = new ParticleEmitter();

	/*forestSkyBox = SOIL_load_OGL_cubemap(TEXTUREDIR"skyLeft.png", TEXTUREDIR"skyRight.png", TEXTUREDIR"skyTop.png", TEXTUREDIR"skyBottom.png", TEXTUREDIR"skyBack.png", TEXTUREDIR"skyFront.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!forestSkyBox) {
		return;
	}*/

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellData->setTexture(SOIL_load_OGL_texture(TEXTUREDIR"hellknight.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	//**Scene Three**
	pond = new HeightMap(TEXTUREDIR"pond.raw", 257, 257, 16.0f,16.0f, 5.0f);

	pond->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"tileable-img_0062-verydark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!pond->GetTexture()) {
		return;
	}
	SetTextureRepeating(pond->GetTexture(), true);

	camera[2] = new Camera(0.0f, 0.0f, Vector3(pond->RAW_WIDTH*pond->HEIGHTMAP_X / 2.0f, 500, pond->RAW_WIDTH*pond->HEIGHTMAP_X));
	light2 = new Light(Vector3((pond->RAW_HEIGHT*pond->HEIGHTMAP_X / 2.0f), 500.0f, (pond->RAW_HEIGHT*pond->HEIGHTMAP_Z / 2.0f)), Vector4(0.9f, 0.9f, 1.0f, 1), (pond->RAW_WIDTH*pond->HEIGHTMAP_X) / 2.0f);

	/*marshSkyBox = SOIL_load_OGL_cubemap(TEXTUREDIR"greenhaze_lf.tga", TEXTUREDIR"greenhaze_rt.tga", TEXTUREDIR"greenhaze_up.tga", TEXTUREDIR"greenhaze_dn.tga", TEXTUREDIR"greenhaze_bk.tga", TEXTUREDIR"greenhaze_ft.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!marshSkyBox) {
		return;
	}*/


	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	
	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj")) {
		return;
	}
	//Create Lights

	pointLights = new Light[LIGHTNUM*LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light &l = pointLights[(x*LIGHTNUM) + z];

			float xPos = (mars->RAW_WIDTH*mars->HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (mars->RAW_HEIGHT*mars->HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (mars->RAW_WIDTH*mars->HEIGHTMAP_X / LIGHTNUM);
			l.SetRadius(radius);
		}
	}

	//light0 = new Light(Vector3((RAW_HEIGHT*HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)), Vector4(1, 1, 1, 1), (RAW_WIDTH*HEIGHTMAP_X) / 2.0f);

	//Buffers

	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferProcessTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	/*for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourTex2[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourTex2[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}*/

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex2[0], 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
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
	glGenFramebuffers(1, &processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	//Enables
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	//init
	init = true;
}

Renderer::~Renderer(void) {
	for (int i = 0; i < SCENENUMS; i++) {
		delete camera[i];
	}
	delete light;
	delete light2;

	delete mars;
	delete forest;
	delete pond;

	delete quad;
	delete freeQuad;
	delete sphere;

	delete emitter;

	delete textureShader;
	delete skyboxShader;
	delete textShader;
	delete sceneShader;
	delete pointlightShader;
	delete combineShader;
	delete treeShader;
	delete shadowSceneShader;
	delete shadowShader;
	delete reflectShader;
	delete lightShader;
	delete particleShader;
	delete sobelShader;
	delete sobelCombineShader;
	delete animShader;

	delete basicFont;
	delete[] pointLights;

	delete tree;

	delete hellData;
	delete hellNode;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferProcessTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	currentShader = 0;
}

void Renderer::UpdateScene(float msec) {
	if (!pause) {
		rotation = msec * 0.01f;
		light->SetPosition(light->GetPosition() + Vector3(msec, 0, 0) / 5);
		emitter->Update(msec);
		snowPileup += msec * 0.025;
		hellNode->Update(msec);
		if (autocam) {
			camera[currentScene]->SetPosition(camera[currentScene]->GetPosition() + Vector3(0, 0, -msec) / 5);
			if (camera[currentScene]->GetPosition().z < 500) {
				nextScene();
			}
		}
	}
	else {
		rotation = 0;
	}
	camera[currentScene]->UpdateCamera(msec);
	viewMatrix = camera[currentScene]->BuildViewMatrix();
	refresh = 1000 / msec;
}

void Renderer::RenderScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	switch (currentScene) {
	case 0:
		RenderScene0();
		break;
	case 1:
		RenderScene1();
		break;
	case 2:
		RenderScene2();
		break;
	}
	if (frameTrue) {
		PrintFrames();
	}
	SwapBuffers();
}

void Renderer::nextScene() {
	currentScene = (currentScene + 1) % 3;
	InitialiseScene();
}
void Renderer::lastScene() {
	currentScene = (currentScene - 1) % 3;
	if (currentScene == -1) currentScene = 2;
	InitialiseScene();
}

void Renderer::RenderScene0() {
	Matrix4 tempTexM = textureMatrix;
	Matrix4 tempModM = modelMatrix;
	Matrix4 tempProM = projMatrix;
	DrawScene0();
	//DrawSkyBox();
	DrawPointLights();
	CombineBuffers();

	textureMatrix = tempTexM;
	modelMatrix = tempModM;
	projMatrix = tempProM;

}

void Renderer::DrawScene0() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkyBox();

	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	modelMatrix.ToIdentity();
	UpdateShaderMatrices();

	mars->Draw();

	glUseProgram(0);
	//PrintFrames();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawSkyBox() {

	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	viewMatrix = camera[currentScene]->BuildViewMatrix();
	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::RenderScene1() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//PrintFrames();
	DrawShadowScene();

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawSkyBox();
	DrawCombinedScene();
	RenderSnow();
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	DrawSobel();
	/*DrawShadowScene();
	DrawCombinedScene();*/
	PresentScene();

	//SwapBuffers();
}

void Renderer::DrawScene1() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix*(projMatrix*viewMatrix);

	UpdateShaderMatrices();

	DrawFloor();
	DrawTree();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderScene2() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawHellKnight();
	DrawSkyBox();
	DrawScene2();
	DrawWater();
	DrawScreen();

	//SwapBuffers();
}

void Renderer::DrawScene2() {
	SetCurrentShader(lightShader);
	SetShaderLight(*light2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera[2]->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	modelMatrix = Matrix4::Translation(Vector3(0, 125, 0));

	UpdateShaderMatrices();

	pond->Draw();

	glUseProgram(0);
	modelMatrix.ToIdentity();
}

void Renderer::InitialiseScene() {
	switch (currentScene) {
	case 0:
		glDisable(GL_CULL_FACE);
		viewMatrix = camera[currentScene]->BuildViewMatrix();
		textureMatrix.ToIdentity();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		modelMatrix.ToIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//http://www.custommapmakers.org/skyboxes.php
		SOIL_load_OGL_cubemap(TEXTUREDIR"starfield_ft.tga", TEXTUREDIR"starfield_bk.tga", TEXTUREDIR"starfield_up.tga", TEXTUREDIR"starfield_dn.tga", TEXTUREDIR"starfield_rt.tga", TEXTUREDIR"starfield_lf.tga", SOIL_LOAD_RGB,cubeMesh, 0);
		camera[0] = new Camera(0.0f, 0.0f, Vector3(mars->RAW_WIDTH*mars->HEIGHTMAP_X / 2.0f, 500, mars->RAW_WIDTH*mars->HEIGHTMAP_X));
		camera[0]->SetPitch(0.0f);
		camera[0]->SetYaw(0.0f);
		camera[0]->SetPosition(Vector3(mars->RAW_WIDTH*mars->HEIGHTMAP_X / 2.0f, 500, mars->RAW_WIDTH*mars->HEIGHTMAP_X));
		break;
	case 1:
		glDisable(GL_CULL_FACE);
		viewMatrix = camera[currentScene]->BuildViewMatrix();
		textureMatrix.ToIdentity();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		modelMatrix.ToIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//http://www.custommapmakers.org/skyboxes.php
		SOIL_load_OGL_cubemap(TEXTUREDIR"rockq3_ft.tga", TEXTUREDIR"rockq3_bk.tga", TEXTUREDIR"rockq3_up.tga", TEXTUREDIR"rockq3_dn.tga", TEXTUREDIR"rockq3_rt.tga", TEXTUREDIR"rockq3_lf.tga", SOIL_LOAD_RGB, cubeMesh, 0);
		//camera[1] = new Camera(0.0f, 0.0f, Vector3(forest->RAW_WIDTH*forest->HEIGHTMAP_X / 2.0f, 500, forest->RAW_WIDTH*forest->HEIGHTMAP_X));
		camera[1]->SetPitch(0.0f);
		camera[1]->SetYaw(0.0f);
		camera[1]->SetPosition(Vector3(forest->RAW_WIDTH*forest->HEIGHTMAP_X / 2.0f, 500, forest->RAW_WIDTH*forest->HEIGHTMAP_X));
		light->SetPosition(Vector3(2327.16577f, 674.887451f, 5170.17090));
		snowPileup = 0.0f;
		emitter->clearParticles();
		break;
	case 2:
		glDisable(GL_CULL_FACE);
		viewMatrix = camera[currentScene]->BuildViewMatrix();
		textureMatrix.ToIdentity();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
		modelMatrix.ToIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//http://www.custommapmakers.org/skyboxes.php
		SOIL_load_OGL_cubemap(TEXTUREDIR"greenhaze_ft.tga", TEXTUREDIR"greenhaze_bk.tga", TEXTUREDIR"greenhaze_up.tga", TEXTUREDIR"greenhaze_dn.tga", TEXTUREDIR"greenhaze_rt.tga", TEXTUREDIR"greenhaze_lf.tga", SOIL_LOAD_RGB, cubeMesh, 0);
		//camera[2] = new Camera(0.0f, 0.0f, Vector3(pond->RAW_WIDTH*pond->HEIGHTMAP_X / 2.0f, 500, pond->RAW_WIDTH*pond->HEIGHTMAP_X));
		camera[2]->SetPitch(0.0f);
		camera[2]->SetYaw(0.0f);
		camera[2]->SetPosition(Vector3(pond->RAW_WIDTH*pond->HEIGHTMAP_X / 2.0f, 500, pond->RAW_WIDTH*pond->HEIGHTMAP_X));
		break;
	}
	//SwapBuffers();
}

void Renderer::PrintFrames() {

	SetCurrentShader(textShader);
	glUseProgram(currentShader->GetProgram());	//Enable the shader...
												//And turn on texture unit 0
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 3);

	//Render function to encapsulate our font rendering!
	DrawText(std::to_string(refresh), Vector3(0, 0, 1.0f), 16.0f);

	glUseProgram(0);	//That's everything!
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	Matrix4 rModel = modelMatrix;
	Matrix4 rView = viewMatrix;
	Matrix4 rProj = projMatrix;

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera[currentScene]->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	textureMatrix.ToIdentity();
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	modelMatrix = rModel;
	viewMatrix = rView;
	projMatrix = rProj;

	UpdateShaderMatrices();

	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::GenerateScreenTexture(GLuint &into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, width, height, 0, depth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void  Renderer::DrawPointLights() {
	SetCurrentShader(pointlightShader);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera[0]->GetPosition());

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Vector3 translate = Vector3((mars->RAW_HEIGHT*mars->HEIGHTMAP_X / 2.0f), 500, (mars->RAW_HEIGHT*mars->HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < LIGHTNUM; ++x) {
		for (int z = 0; z < LIGHTNUM; ++z) {
			Light &l = pointLights[(x*LIGHTNUM) + z];
			float radius = l.GetRadius();

			modelMatrix = pushMatrix * Matrix4::Rotation(rotation, Vector3(0, 1, 0)) * popMatrix * Matrix4::Translation(l.GetPosition()) * Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(modelMatrix.GetPositionVector());

			SetShaderLight(l);

			UpdateShaderMatrices();

			float dist = (l.GetPosition() - camera[0]->GetPosition()).Length();
			if (dist < radius) {
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}

			sphere->Draw();
		}
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::CombineBuffers() {
	SetCurrentShader(combineShader);

	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "specularTex"), 4);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	freeQuad->Draw();

	glUseProgram(0);
}

void Renderer::DrawTree() {
	modelMatrix.ToIdentity();

	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	tree->Draw();
}

void Renderer::DrawFloor() {
	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(450, 450, 1));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	forest->Draw();
}

void Renderer::DrawCombinedScene() {

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	SetCurrentShader(shadowSceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera[1]->GetPosition());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "snowPileup"), snowPileup);

	SetShaderLight(*light);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	//viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(2560, 0, 2560));
	viewMatrix = camera[1]->BuildViewMatrix();
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

			tree->Draw();
		}
	}
	
}

void Renderer::DrawForest() {
	modelMatrix.ToIdentity();
	//modelMatrix = Matrix4::Translation(Vector3(-500, -200, -500));



	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);

	forest->Draw();
}

void Renderer::DrawShadowScene() {
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	SetCurrentShader(shadowShader);

	//light->SetPosition(camera->GetPosition());
	//viewMatrix = camera->BuildViewMatrix();
	viewMatrix = Matrix4::Rotation(-4.99000549, Vector3(1, 0, 0)) *
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
}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera[2]->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMesh);

	float heightX = (pond->RAW_WIDTH*pond->HEIGHTMAP_X / 2.0f);
	float heightY = 256 * pond->HEIGHTMAP_Y / 3.0f;
	float heightZ = (pond->RAW_WIDTH*pond->HEIGHTMAP_Z / 2.0f);
	
	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) * Matrix4::Scale(Vector3(heightX, 1, heightZ)) * Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f));

	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);
}

void Renderer::RenderSnow() {

	SetCurrentShader(particleShader);
	//glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleSize(8.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(8.0f);
	emitter->SetParticleLifetime(10000.0f);
	emitter->SetParticleSpeed(0.1f);
	emitter->SetParticleRate(20.0f);
	UpdateShaderMatrices();

	emitter->Draw();

	glUseProgram(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void	Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

void Renderer::PresentScene() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sobelCombineShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	//viewMatrix = camera[1]->BuildViewMatrix();
	UpdateShaderMatrices();
	//freeQuad->SetTexture(bufferColourTex);
	//freeQuad->Draw();


	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "sobelTex"), 3);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferProcessTex);


	freeQuad->Draw();
	/*modelMatrix = Matrix4::Translation(Vector3(0, 0, 1.0f));
	UpdateShaderMatrices();
	freeQuad->SetTexture(bufferProcessTex);
	freeQuad->Draw();*/
	//glUseProgram(0);
}

void Renderer::DrawSobel() {
	modelMatrix.ToIdentity();
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferProcessTex, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(sobelShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	//for (int i = 0; i < POST_PASSES; ++i) {
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);

	freeQuad->SetTexture(bufferColourTex);
	freeQuad->Draw();

	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);

	//freeQuad->SetTexture(bufferProcessTex);
	//freeQuad->Draw();
	//}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawScreen() {
	SetCurrentShader(textureShader);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera[2]->GetPosition());

	float heightX = (pond->RAW_WIDTH*pond->HEIGHTMAP_X / 2.0f);
	float heightY = 256 * pond->HEIGHTMAP_Y / 3.0f;
	float heightZ = (0);

	modelMatrix.ToIdentity();
	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY + 500, heightZ)) * Matrix4::Scale(Vector3(heightX, heightY, heightZ)) * Matrix4::Rotation(180, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Rotation(0,Vector3(0,0,0));

	UpdateShaderMatrices();
	freeQuad->SetTexture(bufferColourTex);
	freeQuad->Draw();

	glUseProgram(0);
}

void Renderer::DrawHellKnight() {
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(animShader);
	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();

	for (int y = 0; y < 1; ++y) {
		for (int x = 0; x < 1; ++x) {
			modelMatrix = Matrix4::Translation(Vector3(x * 100, 0, y * 100)) * Matrix4::Scale(Vector3(10,10,10));
			UpdateShaderMatrices();
			hellNode->Draw(*this);
		}
	}

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
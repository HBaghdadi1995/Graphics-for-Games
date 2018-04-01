#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	rotation = 0.0f;

	mars = new HeightMap(TEXTUREDIR"marsTerrain.raw", 257, 257, 16.0f, 16.0f, 1.25f, 256,256);
	mars->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"XantheTerra_v5.minimap.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!mars->GetTexture()) {
		return;
	}
	cubeMesh = SOIL_load_OGL_cubemap(TEXTUREDIR"starfield_ft.tga", TEXTUREDIR"starfield_bk.tga", TEXTUREDIR"starfield_up.tga", TEXTUREDIR"starfield_dn.tga", TEXTUREDIR"starfield_rt.tga", TEXTUREDIR"starfield_lf.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	
	forest = new HeightMap(TEXTUREDIR"forest.raw");
	forest->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"tileable-img_0062-verydark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!forest->GetTexture()) {
		return;
	}

	pond = new HeightMap(TEXTUREDIR"pond.raw");
	pond->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"tileable-img_0062-verydark.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!pond->GetTexture()) {
		return;
	}

	camera[0] = new Camera(0.0f, 0.0f, Vector3(mars->RAW_WIDTH*mars->HEIGHTMAP_X / 2.0f, 500, mars->RAW_WIDTH*mars->HEIGHTMAP_X));
	camera[1] = new Camera(0.0f, 0.0f, Vector3(forest->RAW_WIDTH*mars->HEIGHTMAP_X / 2.0f, 500, mars->RAW_WIDTH*mars->HEIGHTMAP_X));
	camera[2] = new Camera(0.0f, 0.0f, Vector3(pond->RAW_WIDTH*mars->HEIGHTMAP_X / 2.0f, 500, mars->RAW_WIDTH*mars->HEIGHTMAP_X));

	quad = Mesh::GenerateQuad();

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
	mars->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!mars->GetBumpMap()) {
		return;
	}

	SetTextureRepeating(mars->GetTexture(), true);
	SetTextureRepeating(mars->GetBumpMap(), true);

	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj")) {
		return;
	}
	processShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!processShader->LinkProgram()) {
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
	sceneShader = new Shader(SHADERDIR"bumpvertex.glsl",SHADERDIR"bufferfragment.glsl");
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
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferColourTex);
	GenerateScreenTexture(bufferNormalTex);
	GenerateScreenTexture(lightEmissiveTex);
	GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);


	init = true;
}

Renderer::~Renderer(void) {
	delete sceneShader;
	delete combineShader;
	delete pointlightShader;

	delete mars;
	for (int i = 0; i < SCENENUMS; i++) {
		delete camera[i];
	}
	delete sphere;
	delete quad;
	delete[] pointLights;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	currentShader = 0;
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

void Renderer::UpdateScene(float msec) {
	camera[currentScene]->UpdateCamera(msec);
	viewMatrix = camera[currentScene]->BuildViewMatrix();
	rotation = msec * 0.01f;
}

void Renderer::RenderScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	RenderScene0();

	//FillBuffers();
	//DrawPointLights();
	//CombineBuffers();
	SwapBuffers();
}

void Renderer::FillBuffers() {
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera[currentScene]->GetPosition());

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

			float dist = (l.GetPosition() - camera[currentScene]->GetPosition()).Length();
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

	quad->Draw();

	glUseProgram(0);
}

void Renderer::RenderScene0() {
	Matrix4 tempTexM = textureMatrix;
	Matrix4 tempModM = modelMatrix;
	Matrix4 tempProM = projMatrix;
	FillBuffers();
	//DrawSkyBox();
	DrawPointLights();
	CombineBuffers();

	textureMatrix = tempTexM;
	modelMatrix = tempModM;
	projMatrix = tempProM;

}

void Renderer::DrawSkyBox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	/*glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, spaceSkyBox);*/

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}
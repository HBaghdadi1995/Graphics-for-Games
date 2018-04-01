#pragma once
#pragma once
#include "OGLRenderer.h"
#include <time.h>

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MAX_BUFFER
};
class Mesh {
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	virtual void SDraw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

	void SetTexture(GLuint tex) { texture = tex; }
	void SetTexture2(GLuint tex) { texture2 = tex; }
	GLuint  GetTexture() { return texture; }
	GLuint  GetTexture2() { return texture2; }

	void SetVerticies(Vector3* v) { vertices = v; }

	void SetTextureCoords(Vector2* t) { textureCoords = t; }

	void SetNumIndices(GLuint n) { numIndices = n; }
	GLuint getNumIndices() { return numIndices; }

	void SetNumVertices(GLuint n) { numVertices = n; };

	void setIndices(unsigned int* i) { indices = i; }

	void SetBumpMap(GLuint tex) { bumpTexture = tex; }
	GLuint GetBumpMap(void){ return bumpTexture; }

protected:
	void BufferData();
	void GenerateNormals();

	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector2 &ta, const Vector2 &tb, const Vector2 &tc);

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;

	Vector3* vertices;
	Vector4* colours;

	GLuint texture;
	GLuint texture2;
	Vector2* textureCoords;

	GLuint numIndices;
	unsigned int* indices;

	Vector3* normals;

	Vector3* tangents;
	GLuint bumpTexture;
};
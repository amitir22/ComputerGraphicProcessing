// Renderer.h
#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#include <memory>

using namespace std;
class Renderer
{
	std::unique_ptr<float[]> m_outBuffer;
	std::unique_ptr<float[]> m_zbuffer;
	int m_width, m_height;

	void CreateBuffers(int width, int height);
	void CreateLocalBuffer();

	//////////////////////////////
	// openGL stuff. Don't touch.

	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	
	//////////////////////////////
public:
	// Transformation Matrices
	mat4 m_modelTransform;
	mat4 m_viewTransform;
	mat4 m_projectionTransform;	
	bool m_isPerspective;
	mat4 m_mvp; // ModelViewProjection
	mat4 m_viewPortTransform;
	mat3 m_normalTransform;

	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals=NULL);
	void SetViewTransform(const mat4& viewTransform);
	void SetProjection(const mat4& projection, bool isPerspective);
	void SetModelMatrices(const mat4& modelTransform, const mat3& nTransform);
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void handleWindowReshape(int newWidth, int newHeight);
	// Our functions
	void DrawLine(int x0, int y0, int x1, int y1);
	void DrawLine3D(float x0, float y0, float z0, float x1, float y1, float z1);
	void DrawPixel(int x, int y, int z);
	void DrawPixel(int x, int y);
	bool LineCompletelyInsideRectangle(int x0, int y0, int x1, int y1) const noexcept;
	bool LineCompletlyOutsideRectangle(int x0, int y0, int x1, int y1) const noexcept;
	void computeViewPortMatrix();
private:
};

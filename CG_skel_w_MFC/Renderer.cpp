// Renderer.cpp
#include "StdAfx.h"
#include <algorithm> 
#include <cmath>   
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"
#include "constants.h"


#define INDEX(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() :m_width(CG::DEFAULT_WIDTH), m_height(CG::DEFAULT_HEIGHT)
{
	InitOpenGLRendering();
	CreateBuffers(m_width, m_height);
	Init();
}
Renderer::Renderer(int width, int height) :m_width(width), m_height(height)
{
	InitOpenGLRendering();
	CreateBuffers(width,height);
	Init();
}
Renderer::~Renderer(void)
{
	glDeleteTextures(1, &gScreenTex);
	glDeleteVertexArrays(1, &gScreenVtc);
}

void Renderer::Init()
{ // Init transformation matrices to default values
	m_modelTransform = mat4(1.0);
	m_viewTransform = mat4(1.0);
	m_projectionTransform = mat4(1.0);
	m_mvp = m_projectionTransform * m_viewTransform * m_modelTransform;
	computeViewPortMatrix();
	m_normalTransform = mat3(1.0);
}


void Renderer::CreateBuffers(int width, int height)
{
	m_width=width;
	m_height=height;	
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = std::make_unique<float[]>(3 * m_width * m_height);
	std::fill(m_outBuffer.get(), m_outBuffer.get() + 3 * m_width * m_height, 0);
}

void Renderer::SetDemoBuffer()
{
	//vertical line
	for(int i=0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width,256,i,0)]=1;	m_outBuffer[INDEX(m_width,256,i,1)]=0;	m_outBuffer[INDEX(m_width,256,i,2)]=0;

	}
	//horizontal line
	for(int i=0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width,i,256,0)]=1;	m_outBuffer[INDEX(m_width,i,256,1)]=0;	m_outBuffer[INDEX(m_width,i,256,2)]=1;

	}
}

void Renderer::handleWindowReshape(int newWidth, int newHeight) {
	m_width = newWidth;
	m_height = newHeight;
	computeViewPortMatrix(); // Recalculate viewport transformation matrix
	CreateBuffers(m_width, m_height); // Recreate buffers with new size
}
/////////////////////////////////////////////////////
//				DRAW FUNCTIONS
///////////////////////////////////////////////////
void Renderer::DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals) {
	updateMVP();
	if (vertices == nullptr || vertices->size() % 3 != 0) {
		throw std::runtime_error("Invalid vertices input.");
	}
	// Transform vertices from world space to screen space, save them in a new vector
	// create a vector of screen vertices initialized to zero of size vertices->size()
	vector<vec4> ndcVertices(vertices->size());
	for (int i = 0; i < vertices->size(); i++) {
		vec4 vertex = vec4((*vertices)[i].x, (*vertices)[i].y, (*vertices)[i].z, 1);
		vec4 clipVertex = m_mvp * vertex; // aka normalized device coordinates
		if (m_isPerspective)
			ndcVertices[i] = vec4(clipVertex.x / clipVertex.w, clipVertex.y / clipVertex.w, clipVertex.z / clipVertex.w, 1);
	}
	// Create vector of faces
	vector<Geometry::Face> faces;
	for (int i = 0; i < ndcVertices.size(); i += 3) {
		// Check all three vertices are inside normalized viewing volume [-1,1]^3
		if (ndcVertices[i].x < -1 || ndcVertices[i].x > 1 || ndcVertices[i].y < -1 || ndcVertices[i].y > 1 || ndcVertices[i].z < -1 || ndcVertices[i].z > 1) {
			continue;
		}
		Geometry::Face face;
		face.v[0] = ndcVertices[i];
		face.v[1] = ndcVertices[i + 1];
		face.v[2] = ndcVertices[i + 2];
		faces.push_back(face);
	}

	
	// Draw each triangle
	for (int i = 0; i < faces.size(); i++) {
		vec3 v1 = vec3(faces[i].v[0].x, faces[i].v[0].y, faces[i].v[0].z);
		vec3 v2 = vec3(faces[i].v[1].x, faces[i].v[1].y, faces[i].v[1].z);
		vec3 v3 = vec3(faces[i].v[2].x, faces[i].v[2].y, faces[i].v[2].z);
		v1 = m_viewPortTransform * v1;
		v2 = m_viewPortTransform * v2;
		v3 = m_viewPortTransform * v3;

		// call drawLine and cast the vertices to int
		DrawLine((int)v1.x, (int)v1.y,  (int)v2.x, (int)v2.y);
		DrawLine((int)v2.x, (int)v2.y,  (int)v3.x, (int)v3.y);
		DrawLine((int)v3.x, (int)v3.y,  (int)v1.x, (int)v1.y);
	}
}

void Renderer::SetViewTransform(const mat4& viewTransform)
{
	m_viewTransform = viewTransform;
	m_mvp = m_projectionTransform * m_viewTransform * m_modelTransform;
}

void Renderer::SetProjection(const mat4& projection, bool isPerspective)
{
	m_projectionTransform = projection;
	m_isPerspective = isPerspective;
	m_mvp = m_projectionTransform * m_viewTransform * m_modelTransform;
}

bool Renderer::LineCompletelyInsideRectangle(int x0, int y0, int x1, int y1) const noexcept {
	if ( (x0 >= 0 && x1 >= 0) && (y0 >= 0 && y1 >= 0) && (x0 < m_width && x1 < m_width) && (y0 < m_height && y1 < m_height) ) {
		return true;
	}
	return false;
}

bool Renderer::LineCompletlyOutsideRectangle(int x0, int y0, int x1, int y1) const noexcept {
	if ((x0 < 0 && x1 < 0) || (y0 < 0 && y1 < 0) || (x0 >= m_width && x1 >= m_width) || (y0 >= m_height && y1 >= m_height)) {
		return true;
	}
	return false;
}

 // We want to map the normalized device coordinates to the screen coordinates
void Renderer::computeViewPortMatrix()
{
	m_viewPortTransform = mat4(1.0);
	m_viewPortTransform[0][0] = m_width / 2.0;
	m_viewPortTransform[1][1] = m_height / 2.0;
	m_viewPortTransform[0][3] = (m_width) / 2.0; 
	m_viewPortTransform[1][3] = (m_height) / 2.0;
	// Keeping z values between [-1,1]
	m_viewPortTransform[2][2] = 1;
}

void Renderer::DrawLine(int x0, int y0, int x1, int y1) {
	// Check if line is not completely inside the rectangle

	const bool steep = abs(y1 - y0) > abs(x1 - x0);
	// If slope (in absolute value) is larger than 1, we switch roles of x and y 
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	
	// Make sure we draw from left to right
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	const int dx = x1 - x0;
	const int dy = std::abs(y1 - y0); // Also handle negative slopes
	const int ystep = (y0 < y1) ? 1 : -1;
	int D = 2 * dy - dx;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			DrawPixel(y, x);
		}
		else {
			DrawPixel(x, y);
		}
		if (D > 0) { // If D > 0, we should move one step in the y direction
			y += ystep;
			D += 2 * (dy - dx);
		}
		else { // Else, don't increment
			D += 2 * dy;
		}
	}
}

void Renderer::DrawPixel(int x, int y, int z) {
	if (x < 0 || x >= m_width || y < 0 || y >= m_height || z < -1 || z > 1 ) {
		return;
	}
	m_outBuffer[INDEX(m_width, x, y, 0)] = 1;
	m_outBuffer[INDEX(m_width, x, y, 1)] = 1;
	m_outBuffer[INDEX(m_width, x, y, 2)] = 1;

}

void Renderer::DrawPixel(int x, int y) {
	if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
		return;
	}
	m_outBuffer[INDEX(m_width, x, y, 0)] = 1;
	m_outBuffer[INDEX(m_width, x, y, 1)] = 1;
	m_outBuffer[INDEX(m_width, x, y, 2)] = 1;
}

/////////////////////////////////////////////////////
//			OpenGL stuff. Don't touch.
/////////////////////////////////////////////////////

void Renderer::InitOpenGLRendering()
{
	glGenTextures(1, &gScreenTex);
	glGenVertexArrays(1, &gScreenVtc);
	GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &buffer);
	const GLfloat vtc[]={
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );

	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0,
		0 );

	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		(GLvoid *) sizeof(vtc) );
	glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_width, m_height);
}

void Renderer::SetModelMatrices(const mat4& modelTransform, const mat3& nTransform)
{
	m_modelTransform = modelTransform;
	m_normalTransform = nTransform;
	m_mvp = m_projectionTransform * m_viewTransform * m_modelTransform;
}

void Renderer::SwapBuffers()
{

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindVertexArray(gScreenVtc);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glutSwapBuffers();
}

void Renderer::ClearColorBuffer()
{
	std::fill(m_outBuffer.get(), m_outBuffer.get() + 3 * m_width * m_height, 0);
}

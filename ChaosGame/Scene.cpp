#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene(HDC hDC, GLfloat aspectRatio, GLuint program, EShape shape, EVertexRestrictions restrictions)
	: m_hDC(hDC), m_program(program), m_vao{}, m_vbo{}, m_pointCount{}, m_unMvp(-1)
{
	if (!m_hDC)
	{
		assert(false); throw EXCEPTION(L"Window device context is NULL");
	}
	else if (!m_program)
	{
		assert(false); throw EXCEPTION(L"Invalid GLSL program ID");
	}

	// Initial scale factor for the camera.
	const GLfloat CameraScaleFactor = 1.0f;

	m_spCamera = std::make_unique<Camera>(aspectRatio, CameraScaleFactor, FieldOfView, FrustumNear, FrustumFar);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Select shape for the chaos game.
	switch (shape)
	{
	case EShape::Triangle:
		m_spShape = std::make_unique<Triangle>();
		break;
	case EShape::Square:
		m_spShape = std::make_unique<Square>();
		break;
	case EShape::Pentagon:
		m_spShape = std::make_unique<Pentagon>();
		break;
	case EShape::Hexagon:
		m_spShape = std::make_unique<Hexagon>();
		break;
	case EShape::Tetrahedron:
		m_spShape = std::make_unique<Tetrahedron>();
		break;
	default:
		assert(false); throw EXCEPTION(L"Undefined or unknown shape");
	}

	// Set vertex restrictions.
	m_spShape->setVertexRestrictions(restrictions);

	// Generate vertex coordinates for the chaos game.

	const GLfloat DistanceFraction = 0.5;
	const size_t Iterations = 25000;    // 100,000 is too many

	glm::vec3 vertices[Iterations];
	size_t vertexOffset = {};

	// Current point: a random point inside the polygon - e.g. center of one of the polygon's sides.
	glm::vec3 p = m_spShape->getRandomPointInside();
	vertices[vertexOffset++] = p;

	for (size_t i = 1; i < Iterations; ++i)
	{
		// Select the current vertex by randomly choosing its index.
		glm::vec3 v = m_spShape->selectVertex();

		p = (v + p) * DistanceFraction;

		vertices[vertexOffset++] = p;
	}

#ifndef DISPLAY_BOUNDING_SHAPE
	// Display the chaos game vertices.
	m_pointCount = Iterations;
#else
	// Display the bounding shape.
	m_pointCount = m_spShape->VertexCount;
#endif

	// Generate VBO and fill it with the data.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

#ifndef DISPLAY_BOUNDING_SHAPE
	// Display the chaos game vertices.
	glBufferData(GL_ARRAY_BUFFER, _countof(vertices) * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);
#else
	// Display the bounding shape.
	glBufferData(GL_ARRAY_BUFFER, m_spShape->m_vertices.size() * sizeof(vertices[0]), &m_spShape->m_vertices[0], GL_STATIC_DRAW);
#endif

	// Fill in the vertex position attribute.
	const GLuint attrVertexPosition = 0;
	glVertexAttribPointer(attrVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrVertexPosition);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_unMvp = glGetUniformLocation(m_program, "mvp");
	if (-1 == m_unMvp)
	{
		assert(false); throw EXCEPTION(L"Failed to get uniform location: mvp");
	}

#if DISPLAY_BOUNDING_SHAPE
	// Increase size of the points.
	glPointSize(4.0f);
#endif
}

Scene::~Scene()
{
	if (0 != m_vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_vbo);
	}

	if (0 != m_vao)
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_vao);
	}
}

void Scene::resize(int clientWidth, int clientHeight)
{
	if (0 == clientHeight)    // prevent dividing by zero
	{
		clientHeight = 1;
	}

	// Resize the viewport.
	glViewport(0, 0, clientWidth, clientHeight);

	// Calculate aspect ratio of the window.
	gluPerspective(FieldOfView, clientWidth / (GLfloat)clientHeight, FrustumNear, FrustumFar);
}

void Scene::translateCameraX(GLfloat diff)
{
	m_spCamera->translateX(diff);
}

void Scene::translateCameraY(GLfloat diff)
{
	m_spCamera->translateY(diff);
}

void Scene::translateCameraZ(GLfloat diff)
{
	m_spCamera->translateZ(diff);
}

void Scene::rotateCameraX(GLfloat angle)
{
	m_spCamera->rotateX(angle);
}

void Scene::rotateCameraY(GLfloat angle)
{
	m_spCamera->rotateY(angle);
}

void Scene::rotateCameraZ(GLfloat angle)
{
	m_spCamera->rotateZ(angle);
}

void Scene::scaleCamera(GLfloat amount)
{
	m_spCamera->scale(amount);
}

void Scene::updateViewMatrices(const std::unique_ptr<Camera>& spCamera) const
{
	assert(-1 != m_unMvp);

	glUseProgram(m_program);

	glm::mat4 mvp = spCamera->getModelViewProjectionMatrix();

	glUniformMatrix4fv(m_unMvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glUseProgram(0);
}

void Scene::render() const
{
	assert(m_program);

	updateViewMatrices(m_spCamera);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glDrawArrays(GL_POINTS, 0, m_pointCount);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(m_hDC);
}

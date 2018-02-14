#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene(HWND hWnd, HDC hDC, GLfloat aspectRatio)
	: m_hWnd(hWnd), m_hDC(hDC), m_hRC(nullptr), m_vao{}, m_vbo{}, m_pointCount{}, m_unMvp(-1)
{
	if (!m_hWnd)
	{
		assert(false); throw EXCEPTION(L"Window handle is NULL");
	}
	else if (!m_hDC)
	{
		assert(false); throw EXCEPTION(L"Window device context is NULL");
	}

	// Set up OpenGL context for our window.

	const int OpenGlMajor = 4;
	const int OpenGlMinor = 4;

	if (!OpenGLHelpers::setupOpenGlContext(OpenGlMajor, OpenGlMinor, m_hDC, m_hRC))
	{
		assert(false); throw EXCEPTION_FMT(L"Failed to set up OpenGL context (version %d.%d)", OpenGlMajor, OpenGlMinor);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.8f, 0.93f, 0.96f, 1.0f);    // very light blue
	//glClearColor(0.0f, 0.64f, 0.91f, 1.0f);    // light blue

	// Initial scale factor for the camera.
	const GLfloat CameraScaleFactor = 1.0f; /*3.5f;*/

	m_spCamera = std::make_unique<Camera>(aspectRatio, CameraScaleFactor, FieldOfView, FrustumNear, FrustumFar);

#if 0
	m_spCamera->translateZ(-1.5f);
#endif

	// Initialize the program wrapper.

	const ShaderCollection shaders = {
		{ GL_VERTEX_SHADER,   "shaders\\chaos.vert" },
		{ GL_FRAGMENT_SHADER, "shaders\\chaos.frag" }
	};

	m_spProgram = std::make_unique<ProgramGLSL>(shaders);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

#if 0
	std::vector<GLfloat> vertices = {

#if 0    // a quad
		0.5f, 0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f, 0.5f, 0.0f   // top left  
#else
		// an equilateral triangle
		0.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
#endif
	};

	m_pointCount = vertices.size() / 3;    // 3 coordinates per vertex
#else

	// Generate vertex coordinates for the chaos game.

	// V - number of polygon vertices
	// F - fraction of the distance between the current point and one of the polygon vertices (F < 1.0)
	// Iterations - number of iterations of the algorithm.

	// TODO: hard-coded values
	const size_t PolygonVertexCount = 3;    // a triangle
	const size_t Iterations = 5000;
	const GLfloat DistanceFraction = 0.5;

	glm::vec3 polygonVertices[] = {
		{  0.0f,  1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.0f },
		{  1.0f, -1.0f, 0.0f }
	};

	glm::vec3 vertices[Iterations];
	size_t vertexOffset = {};

	// Current point: a random point inside the polygon - e.g. center of one of the polygon's sides.
	glm::vec3 p = { (polygonVertices[0] + polygonVertices[1]) / 2.0f };
	vertices[vertexOffset++] = p;

	// Random numbers generator to select the polygon vertices.
	std::mt19937 mt(std::random_device{}());
	std::uniform_int_distribution<int> distr(0, PolygonVertexCount - 1);

	for (size_t i = 1; i < Iterations; ++i)
	{
		// Select the current vertex by randomly choosing its index.
		glm::vec3 v = polygonVertices[distr(mt)];

		p = (v + p) * DistanceFraction;

		vertices[vertexOffset++] = p;
	}

	m_pointCount = Iterations;
#endif

	// Generate VBO and fill it with the data.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, _countof(vertices) * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	// Fill in the vertex position attribute.
	const GLuint attrVertexPosition = 0;
	glVertexAttribPointer(attrVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(attrVertexPosition);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	const GLuint program = m_spProgram->getProgram();

	m_unMvp = glGetUniformLocation(program, "mvp");
	if (-1 == m_unMvp)
	{
		assert(false); throw EXCEPTION(L"Failed to get uniform location: mvp");
	}

	// TODO: temp. Increase size of the points
#if 0
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
	assert(m_spProgram);
	assert(-1 != m_unMvp);

	// TODO: turn on
#if 0
	assert(-1 != m_unNormal);
#endif

	glUseProgram(m_spProgram->getProgram());

	glm::mat4 mvp = spCamera->getModelViewProjectionMatrix();

	glUniformMatrix4fv(m_unMvp, 1, GL_FALSE, glm::value_ptr(mvp));

	glm::mat4 modelView = spCamera->getModelViewMatrix();

	// WARNING: we are using the fact that there are no non-uniform scaling. If this will change, use the entire 4x4 matrix.
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelView)));

	//Normal = mat3(transpose(inverse(model))) * normal;

	// TODO: turn on
#if 0
	glUniformMatrix4fv(m_unNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));
#endif

	glUseProgram(0);
}

void Scene::render() const
{
	updateViewMatrices(m_spCamera);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assert(m_spProgram);

	glUseProgram(m_spProgram->getProgram());
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glDrawArrays(GL_POINTS, 0, m_pointCount);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(m_hDC);
}

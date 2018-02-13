#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


Scene::Scene(HWND hWnd, HDC hDC, GLfloat aspectRatio)
	: m_hWnd(hWnd), m_hDC(hDC), m_hRC(nullptr), m_vao{}, m_vbo{}, m_index{}, m_indexCount{}, m_unMvp(-1)
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

	if (!setupOpenGlContext(OpenGlMajor, OpenGlMinor))
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

	// Fill in vertex indices.

	std::vector<GLint> indices = { 
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	m_indexCount = indices.size();

	glGenBuffers(1, &m_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Fill in vertex coordinates.

	std::vector<GLfloat> vertices = {
		0.5f, 0.5f, 0.0f,  // top right
		0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f, 0.5f, 0.0f   // top left  
	};

	// Generate VBO and fill it with the data.

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), &vertices[0], GL_STATIC_DRAW);

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
#if 1
	glPointSize(8.0f);
#endif
}

Scene::~Scene()
{
	if (0 != m_index)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &m_index);
	}

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

bool Scene::setupOpenGlContext(int versionMajor, int versionMinor)
{
	if (versionMajor < 1)
	{
		std::cerr << __FUNCTION__ << ": invalid OpenGL major version: " << versionMajor << '\n';
		assert(false); return false;
	}

	// Step 1. Set pixel format for the Windows DC.

	PIXELFORMATDESCRIPTOR pfd = {};

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags =
		PFD_SUPPORT_OPENGL |    // OpenGL window
		PFD_DRAW_TO_WINDOW |    // render to window
		PFD_DOUBLEBUFFER;       // support double-buffering
	pfd.iPixelType = PFD_TYPE_RGBA;       // color type: red, green, blue, and alpha
	pfd.cColorBits = 32;                  // preferred color depth (bits per pixel for each color buffer): 8, 16, 24, or 32
	pfd.cDepthBits = 24;                  // depth of the depth (z-axis) buffer
	//pfd.iLayerType = PFD_MAIN_PLANE;    // main layer. Ignored in OpenGL 3.0 and later

	// The OS/driver will try to find the matching pixel format.
	// If some value cannot be set, it will be replaced by the highest possible value (e.g. 24-bit colors instead of 32-bit).
	// Returns an integer ID of the pixel format.
	int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (0 == pixelFormat)
	{
		std::cerr << __FUNCTION__ << ": ChoosePixelFormat() failed: " << ::GetLastError() << '\n';
		assert(false); return false;
	}

	// Set the pixel format for the device context and the associated window.
	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))
	{
		std::cerr << __FUNCTION__ << ": SetPixelFormat() failed: " << ::GetLastError() << '\n';
		assert(false); return false;
	}

	// Step 2. Create a temporary OpenGL rendering context to try to get the latest one - see below.
	HGLRC hRcTmp = wglCreateContext(m_hDC);
	if (!hRcTmp)
	{
		std::cerr << __FUNCTION__ << ": wglCreateContext() failed: " << ::GetLastError() << '\n';
		assert(false); return false;
	}

	// Step 3. Make the temporary rendering context current for our thread.
	if (!wglMakeCurrent(m_hDC, hRcTmp))
	{
		std::cerr << __FUNCTION__ << ": wglMakeCurrent() failed: " << ::GetLastError() << '\n';
		assert(false); return false;
	}

	// Step 4. Initialize GLEW (in particular, to be able to conveniently use the wglCreateContextAttribsARB extension.

	glewExperimental = true;

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << __FUNCTION__ << "glewInit() failed: " << (char *)glewGetErrorString(err) << '\n';
		assert(false); return false;
	}

	// Step 5. Set up the modern OpenGL rendering context.

	// Set the OpenGL version required.
	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, versionMajor,
		WGL_CONTEXT_MINOR_VERSION_ARB, versionMinor,

#if _DEBUG
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
#endif

		0 };    // zero indicates the end of the array

	// If the pointer to this extension is NULL, the OpenGL version is not supported.
	if (!wglCreateContextAttribsARB)
	{
		std::cerr << __FUNCTION__ << ": OpenGL version " << versionMajor << "." << versionMinor << " not supported\n";
		assert(false); return false;
	}

	// Create a modern OpenGL context.
	m_hRC = wglCreateContextAttribsARB(m_hDC, 0, attribs);
	if (!m_hRC)
	{
		std::cerr << __FUNCTION__ << "wglCreateContextAttribsARB() failed\n";
		assert(false); return false;
	}

	// On success, delete the temporary context.
	wglDeleteContext(hRcTmp);

	// Step 6. Make the final rendering context current for our thread.
	if (!wglMakeCurrent(m_hDC, m_hRC))
	{
		std::cerr << __FUNCTION__ << ": wglMakeCurrent() failed (2): " << ::GetLastError() << '\n';
		assert(false); return false;
	}

	glDebugMessageCallback(openGlDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	return true;
}

void APIENTRY Scene::openGlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
	GLsizei length, const GLchar* message, const void* param)
{
	UNREFERENCED_PARAMETER(length);
	UNREFERENCED_PARAMETER(param);

	std::cout << "Debug message from the ";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:
		std::cout << "OpenGL API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		std::cout << "window system";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		std::cout << "shader compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		std::cout << "third party tools or libraries";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		std::cout << "application (explicit)";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		std::cout << "other source";
		break;
	default:    // unknown source?
		assert(false); break;
	}

	std::cout << "\nMessage text: " << message;

	std::cout << "\nType: ";
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		std::cout << "OTHER";
		break;
	}

	std::cout << "\nID: " << id;

	std::cout << "\nSeverity: ";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	}
	std::cout << std::endl;
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
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index);

	glDrawElements(GL_POINTS, m_indexCount, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(m_hDC);
}

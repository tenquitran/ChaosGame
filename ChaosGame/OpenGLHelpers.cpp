#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


GLuint OpenGLHelpers::buildShaders(const std::map<GLenum, std::string>& shaderFiles)
{
	if (shaderFiles.empty())
	{
		std::cerr << "No shader files to build\n";
		return 0;
	}

	GLuint programId = {};

	// Create GLSL program and shader objects.

	programId = glCreateProgram();
	if (0 == programId)
	{
		std::cerr << "glCreateProgram() failed\n";
		return 0;
	}

	// Process the list of shaders.

	std::vector<GLuint> shaderIds = {};

	for (const auto& itrShader : shaderFiles)
	{
		// Read the shader's source.

		const std::string fileName = itrShader.second;

		std::ifstream shaderFile(fileName);
		if (!shaderFile.good())
		{
			std::cerr << "Failed to load shader file \"" << fileName << "\"\n";
			return 0;
		}

		std::istreambuf_iterator<char> itrStream(shaderFile);

		std::string src;
		src.assign(itrStream, std::istreambuf_iterator<char>());

		shaderFile.close();

		// Create shader.

		GLuint shader = glCreateShader(itrShader.first);
		if (0 == shader)
		{
			std::cerr << "glCreateShader() failed\n";
			return 0;
		}

		shaderIds.push_back(shader);

		// Compile the shader and add it to the GLSL program.

		const GLchar *pBuff = static_cast<const GLchar*>(src.c_str());

		glShaderSource(shader, 1, (const GLchar**)&pBuff, nullptr);

		if (!compileShader(shader, fileName))
		{
			std::cerr << "Failed to compile shader \"" << fileName << "\"\n";
			return 0;
		}

		glAttachShader(programId, shader);
	}

	// Link the program and check the results.

	glLinkProgram(programId);

	GLint resLink = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &resLink);

	if (GL_TRUE != resLink)
	{
		GLint logLen = {};
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			std::unique_ptr<char[]> spLog = std::make_unique<char[]>(logLen);

			GLsizei written;
			glGetProgramInfoLog(programId, logLen, &written, spLog.get());

			std::cerr << "Failed to link the program: " << spLog.get() << '\n';

#if _DEBUG
			CAtlString msg;
			msg.Format(L"Failed to link the program:\n%S\n", spLog.get());
			OutputDebugStringW(msg);
#endif
		}

		return 0;
	}

	// Validate the program and check the results.

	glValidateProgram(programId);

	GLint resValidation = GL_FALSE;
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &resValidation);

	if (GL_TRUE != resValidation)
	{
		GLint logLen = {};
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			std::unique_ptr<char[]> spLog = std::make_unique<char[]>(logLen);

			GLsizei written;
			glGetProgramInfoLog(programId, logLen, &written, spLog.get());

			std::cerr << "Failed to validate the program: " << spLog.get() << '\n';

#if _DEBUG
			CAtlString msg;
			msg.Format(L"Failed to validate the program:\n%S\n", spLog.get());
			OutputDebugStringW(msg);
#endif
		}

		return 0;
	}

	// Clean up the shaders data.
	for (const auto& itr : shaderIds)
	{
		glDetachShader(programId, itr);
		glDeleteShader(itr);
	}

	return programId;
}

bool OpenGLHelpers::compileShader(GLuint shader, const std::string& fileName)
{
	glCompileShader(shader);

	GLint res = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);

	if (GL_TRUE != res)
	{
		GLint logLen = {};
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			std::unique_ptr<char[]> spLog = std::make_unique<char[]>(logLen);

			GLsizei written;
			glGetShaderInfoLog(shader, logLen, &written, spLog.get());

			std::cerr << "Failed to compile shader \"" << fileName << "\":\n" << spLog.get() << '\n';

#if _DEBUG
			CAtlString msg;
			msg.Format(L"Failed to compile shader %S:\n%S\n", fileName.c_str(), spLog.get());
			OutputDebugStringW(msg);
#endif
		}

		return false;
	}

	return true;
}

bool OpenGLHelpers::setupOpenGlContext(int versionMajor, int versionMinor, HDC hDC, HGLRC& hRC)
{
	hRC = nullptr;

	if (versionMajor < 1)
	{
		std::cerr << __FUNCTION__ << ": invalid OpenGL major version: " << versionMajor << '\n';
		assert(false); return false;
	}
	else if (!hDC)
	{
		std::cerr << __FUNCTION__ << ": windows device context is NULL\n";
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
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (0 == pixelFormat)
	{
		std::cerr << __FUNCTION__ << ": ChoosePixelFormat() failed: " << GetLastError() << '\n';
		assert(false); return false;
	}

	// Set the pixel format for the device context and the associated window.
	if (!SetPixelFormat(hDC, pixelFormat, &pfd))
	{
		std::cerr << __FUNCTION__ << ": SetPixelFormat() failed: " << GetLastError() << '\n';
		assert(false); return false;
	}

	// Step 2. Create a temporary OpenGL rendering context to try to get the latest one - see below.
	HGLRC hRcTmp = wglCreateContext(hDC);
	if (!hRcTmp)
	{
		std::cerr << __FUNCTION__ << ": wglCreateContext() failed: " << GetLastError() << '\n';
		assert(false); return false;
	}

	// Step 3. Make the temporary rendering context current for our thread.
	if (!wglMakeCurrent(hDC, hRcTmp))
	{
		std::cerr << __FUNCTION__ << ": wglMakeCurrent() failed: " << GetLastError() << '\n';
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
	hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
	if (!hRC)
	{
		std::cerr << __FUNCTION__ << "wglCreateContextAttribsARB() failed\n";
		assert(false); return false;
	}

	// On success, delete the temporary context.
	wglDeleteContext(hRcTmp);

	// Step 6. Make the final rendering context current for our thread.
	if (!wglMakeCurrent(hDC, hRC))
	{
		std::cerr << __FUNCTION__ << ": wglMakeCurrent() failed (2): " << GetLastError() << '\n';
		assert(false); return false;
	}

	std::cout << "Set up OpenGL context: version " << versionMajor << "." << versionMinor << '\n';

	glDebugMessageCallback(openGlDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	return true;
}

void APIENTRY OpenGLHelpers::openGlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
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

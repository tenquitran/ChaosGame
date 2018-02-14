#pragma once


namespace ChaosGameApp
{
	class OpenGLHelpers
	{
	public:
		// Build the shaders.
		// Parameters: shaderFiles - list of shaders to build (key: shader type; value: shader file name or relative path).
		// Returns: program handle (a positive integer) on success, zero otherwise.
		// Throws: std::bad_alloc
		static GLuint buildShaders(const std::map<GLenum, std::string>& shaderFiles);

		// Create and set up OpenGL context for the window.
		// Parameters: versionMajor - major version number of OpenGL;
		//             versionMinor - minor version number of OpenGL;
		//             hDC - device context of the window;
		//             hRC - OpenGL rendering context for the window.
		static bool setupOpenGlContext(int versionMajor, int versionMinor, HDC hDC, HGLRC& hRC);

	private:
		// Compile the shader.
		// Parameters: shader - OpenGL shader object;
		//             fileName - shader file name (for logging).
		static bool compileShader(GLuint shader, const std::string& fileName);

		// Debug callback for OpenGL-related errors.
		static void APIENTRY openGlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
			GLsizei length, const GLchar* message, const void* param);
	};
}

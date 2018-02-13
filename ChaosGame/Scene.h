#pragma once


namespace ChaosGameApp
{
	// OpenGL scene for the chaos game.
	class Scene
	{
	public:
		// Parameters: hWnd - main window handle;
		//             hDC - main window device context;
		//             aspectRatio - main window aspect ratio.
		// Throws: Exception, std::bad_alloc
		Scene(HWND hWnd, HDC hDC, GLfloat aspectRatio);

		virtual ~Scene();

		void render() const;

		void resize(int clientWidth, int clientHeight);

		void translateCameraX(GLfloat diff);
		void translateCameraY(GLfloat diff);
		void translateCameraZ(GLfloat diff);

		void rotateCameraX(GLfloat angle);
		void rotateCameraY(GLfloat angle);
		void rotateCameraZ(GLfloat angle);

		void scaleCamera(GLfloat amount);

		// Update Model-View-Projection (MVP) and other matrices in the GLSL program.
		void updateViewMatrices(const std::unique_ptr<Camera>& spCamera) const;

	private:
		// Create and set up OpenGL context for the window.
		// Parameters: versionMajor - major version number of OpenGL;
		//             versionMinor - minor version number of OpenGL.
		bool setupOpenGlContext(int versionMajor, int versionMinor);

		// Debug callback for OpenGL-related errors.
		static void APIENTRY openGlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, 
			GLsizei length, const GLchar* message, const void* param);

	private:
		// Field of view angle.
		const GLfloat FieldOfView = 45.0f;

		// Frustum boundaries.
		const GLfloat FrustumNear = 0.1f;
		const GLfloat FrustumFar = 1000.0f;

		// Handle of the main window.
		HWND m_hWnd;

		// Device context of the main window.
		HDC m_hDC;

		// OpenGL rendering context.
		HGLRC m_hRC;

		std::unique_ptr<Camera> m_spCamera;

		std::unique_ptr<ProgramGLSL> m_spProgram;

		GLuint m_vao;
		GLuint m_vbo;

		GLuint m_index;         // index buffer
		size_t m_indexCount;    // number of indices

		GLint m_unMvp;          // MVP matrix uniform
	};
}

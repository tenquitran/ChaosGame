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
		// Field of view angle.
		const GLfloat FieldOfView = 45.0f;

		// Frustum boundaries.
		const GLfloat FrustumNear = 0.1f;
		const GLfloat FrustumFar = 1000.0f;

		// Handle of the main window.
		HWND m_hWnd;

		// Device context of the main window.
		HDC m_hDC;

		// OpenGL rendering context for the main window.
		HGLRC m_hRC;

		std::unique_ptr<Camera> m_spCamera;

		std::unique_ptr<ProgramGLSL> m_spProgram;

		GLuint m_vao;
		GLuint m_vbo;

		size_t m_pointCount;    // number of points to render

		GLint m_unMvp;          // MVP matrix uniform

		// Bounding shape for chaos game.
		std::unique_ptr<Shape> m_spShape;
	};
}

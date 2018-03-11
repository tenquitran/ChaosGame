#pragma once


namespace ChaosGameApp
{
	// OpenGL scene for the chaos game.
	class Scene
	{
	public:
		// Parameters: hDC - main window device context;
		//             aspectRatio - main window aspect ratio;
		//             program - GLSL program identifier;
		//             shape - shape to use for the chaos game;
		//             restrictions - vertex restrictions for the chaos game.
		// Throws: Exception, std::bad_alloc
		Scene(HDC hDC, GLfloat aspectRatio, GLuint program, EShape shape, EVertexRestrictions restrictions);

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

	private:
		// Update Model-View-Projection (MVP) and other matrices in the GLSL program.
		void updateViewMatrices(const std::unique_ptr<Camera>& spCamera) const;

	private:
		// Field of view angle.
		const GLfloat FieldOfView = 45.0f;

		// Frustum boundaries.
		const GLfloat FrustumNear = 0.1f;
		const GLfloat FrustumFar = 1000.0f;

		// Device context of the main window.
		HDC m_hDC;

		// GLSL program identifier.
		GLuint m_program;

		GLuint m_vao;
		GLuint m_vbo;

		size_t m_pointCount;    // number of points to render

		GLint m_unMvp;          // MVP matrix uniform

		std::unique_ptr<Camera> m_spCamera;

		// Bounding shape for chaos game.
		std::unique_ptr<Shape> m_spShape;
	};
}

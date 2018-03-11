#pragma once


namespace ChaosGameApp
{
	class Tetrahedron
		: public Shape
	{
	public:
		Tetrahedron()
			: Shape(getVertexCount())
		{
			m_vertices = {
				{  1.0f,  0.0f, -0.707f },
				{ -1.0f,  0.0f, -0.707f },
				{  0.0f,  1.0f,  0.707f },
				{  0.0f, -1.0f,  0.707f } };
		}

		// Get number of vertices defining the shape (at least three).
		virtual size_t getVertexCount() const override
		{
			return 4;
		}
	};
}

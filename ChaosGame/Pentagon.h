#pragma once


namespace ChaosGameApp
{
	struct Pentagon
		: public Shape
	{
	public:
		Pentagon()
			: Shape(getVertexCount())
		{
			m_vertices = {
				{  0.0f,    1.0f,   0.0f },
				{ -0.951f,  0.309f, 0.0f },
				{  0.951f,  0.309f, 0.0f },
				{ -0.588f, -0.809f, 0.0f },
				{  0.588f, -0.809f, 0.0f },
			};
		}

		// Get number of vertices defining the shape (at least three).
		virtual size_t getVertexCount() const override
		{
			return 5;
		}
	};
}

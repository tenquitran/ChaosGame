#pragma once


namespace ChaosGameApp
{
	struct Hexagon
		: public Shape
	{
	public:
		Hexagon()
			: Shape(getVertexCount())
		{
			m_vertices = {
				{ -0.5f, -0.866f, 0.0f },
				{  0.5f, -0.866f, 0.0f },
				{ -1.0f,  0.0f,   0.0f },
				{  1.0f,  0.0f,   0.0f },
				{ -0.5f,  0.866f, 0.0f },
				{  0.5f,  0.866f, 0.0f }
			};
		}

		// Get number of vertices defining the shape (at least three).
		virtual size_t getVertexCount() const override
		{
			return 6;
		}
	};
}

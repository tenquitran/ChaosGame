#pragma once


namespace ChaosGameApp
{
	struct Triangle
		: public Shape
	{
	public:
		Triangle()
			: Shape(getVertexCount())
		{
			m_vertices = {
				{  0.0f,  0.866f, 0.0f },
				{ -1.0f, -1.0f,   0.0f },
				{  1.0f, -1.0f,   0.0f }
			};
		}

		// Get number of vertices defining the shape (at least three).
		virtual size_t getVertexCount() const override
		{
			return 3;
		}
	};
}

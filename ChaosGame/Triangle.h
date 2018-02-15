#pragma once


namespace ChaosGameApp
{
	struct Triangle
		: public Shape
	{
	public:
		Triangle();

	private:
		// Number of vertices defining the shape.
		static const size_t VertexCount = 3;
	};
}

#pragma once


namespace ChaosGameApp
{
	class Tetrahedron
		: public Shape
	{
	public:
		Tetrahedron();

	private:
		// Number of vertices defining the shape.
		static const size_t VertexCount = 4;
	};
}

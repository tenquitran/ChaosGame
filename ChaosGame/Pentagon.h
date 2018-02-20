#pragma once


namespace ChaosGameApp
{
	struct Pentagon
		: public Shape
	{
	public:
		Pentagon();

	private:
		// Number of vertices defining the shape.
		static const size_t VertexCount = 5;
	};
}

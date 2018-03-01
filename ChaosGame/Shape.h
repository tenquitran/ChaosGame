#pragma once


namespace ChaosGameApp
{
	// 2D or 3D shape.
	struct Shape
	{
	public:
		// Parameters: vertexCount - number of vertices defining the shape (at least three).
		explicit Shape(size_t vertexCount);

		// Get number of vertices defining the shape (at least three).
		virtual size_t getVertexCount() const = 0;

		// Select a random point inside the shape.
		glm::vec3 getRandomPointInside() const;

		// Randomly select a vertex.
		glm::vec3 selectVertex() const;

		// Set restrictions on vertex selection.
		// Usually called during shape initialization.
		void setVertexRestrictions(EVertexRestrictions restrictions);

#if DISPLAY_BOUNDING_SHAPE
	public:
#else
	protected:
#endif
		// Number of vertices defining the shape.
		const size_t VertexCount;

		// Vertex coordinates.
		std::vector<glm::vec3> m_vertices;

		// Random number engine.
		mutable std::mt19937 m_randomEngine;

		// Random number generator.
		std::uniform_int_distribution<size_t> m_rng;

		// Restrictions on vertex selection.
		EVertexRestrictions m_selectionRestrictions;

		// Previously chosen vertex indices 
		// (front - the previous one, next - the one before the previous, etc.).
		mutable std::deque<size_t> m_previousVertices;
	};
}

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


Shape::Shape(size_t vertexCount)
	: VertexCount(vertexCount), 
	  m_randomEngine(std::random_device{}()), 
	  m_rng(0, vertexCount - 1)
{
	if (VertexCount < 3)
	{
		assert(false); throw EXCEPTION(L"A shape should consist of at least 3 vertices");
	}
}

glm::vec3 Shape::getRandomPointInside() const
{
	// A point inside the shape can be, e.g. the center of one of the shape's sides.
	return { (m_vertices[0] + m_vertices[1]) / 2.0f };
}

glm::vec3 Shape::selectRandomVertex() const
{
	return m_vertices[m_rng(m_randomEngine)];
}

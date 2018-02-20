#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////

// Maximum number of previous vertex indices stored.
const size_t MaxPreviousVertices = 2;

//////////////////////////////////////////////////////////////////////////


Shape::Shape(size_t vertexCount)
	: VertexCount(vertexCount), 
	  m_randomEngine(std::random_device{}()), 
	  m_rng(0, vertexCount - 1),
	  m_selectionRestrictions(VertexRestrictions::None)
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

glm::vec3 Shape::selectVertex() const
{
	size_t index = m_rng(m_randomEngine);

	switch (m_selectionRestrictions)
	{
	case VertexRestrictions::None:
		break;
	case VertexRestrictions::NotTheSame:
		if (!m_previousVertices.empty())
		{
			while (m_previousVertices.front() == index)
			{
				index = m_rng(m_randomEngine);
			}
		}
		break;
	case VertexRestrictions::CannotHaveOffset_2:
		assert(false);    // TODO: implement
		break;
	case VertexRestrictions::CannotHaveOffsets_1_and_3:
		assert(false);    // TODO: implement
		break;
	case VertexRestrictions::CannotHaveOffsets_1_and_4:
		assert(false);    // TODO: implement
		break;
	default:    // new restriction?
		assert(false); break;
	}

	// Remove the excessive info on previously selected vertices.
	while (m_previousVertices.size() >= MaxPreviousVertices)
	{
		m_previousVertices.pop_back();
	}

	// Add the currently selected vertex info.
	m_previousVertices.push_front(index);

	return m_vertices[index];
}

void Shape::setVertexRestrictions(VertexRestrictions restrictions)
{
	m_selectionRestrictions = restrictions;
}

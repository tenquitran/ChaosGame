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
	  m_selectionRestrictions(EVertexRestrictions::None)
{
	if (VertexCount < 3)
	{
		assert(false); throw EXCEPTION(L"A shape should consist of at least 3 vertices");
	}

	if (   EVertexRestrictions::NotOffsets_1_and_3 == m_selectionRestrictions
		&& VertexCount < 4)
	{
		assert(false); throw EXCEPTION_FMT(L"Offset 3 requires a shape with at least 4 vertices");
	}
	else if (   EVertexRestrictions::NotOffsets_1_and_4 == m_selectionRestrictions
		     && VertexCount < 5)
	{
		assert(false); throw EXCEPTION_FMT(L"Offset 4 requires a shape with at least 5 vertices");
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
	case EVertexRestrictions::None:
		break;
	case EVertexRestrictions::NotTheSame:
		if (!m_previousVertices.empty())
		{
			while (m_previousVertices.front() == index)
			{
				index = m_rng(m_randomEngine);
			}
		}
		break;

		// TODO: check
#if 1
	case EVertexRestrictions::NotOffset_1:
		if (!m_previousVertices.empty())
		{
			while (   1 == abs((int)(m_previousVertices.front() - index))
				   || VertexCount - 1 == abs((int)(m_previousVertices.front() - index)))
			{
				index = m_rng(m_randomEngine);
			}
		}
		break;
#endif

	case EVertexRestrictions::NotOffset_1_Anticlockwise:
		if (!m_previousVertices.empty())
		{
			while (   -1 == index - m_previousVertices.front()
				   || VertexCount - 1 == index - m_previousVertices.front())
			{
				index = m_rng(m_randomEngine);
			}
		}
		break;

	// TODO: Correct for a square; check at least for a pentagon
	case EVertexRestrictions::NotOffset_2:
		if (!m_previousVertices.empty())
		{
			while (
				   2 == abs((int)(m_previousVertices.front() - index))
				|| VertexCount - 2 == abs((int)(m_previousVertices.front() - index))

				//2 == abs((int)(m_previousVertices.front() - index))
				)
			{
				index = m_rng(m_randomEngine);
			}
		}
		break;

	// TODO: wrong
#if 1
	case EVertexRestrictions::NotOffsets_1_and_3:
		if (!m_previousVertices.empty())
		{
			while (

				(
				1 == abs((int)(m_previousVertices.front() - index))
				|| VertexCount - 1 == abs((int)(m_previousVertices.front() - index))
				)

				|| 
				  (3 == abs((int)(m_previousVertices.back()  - index))
				  || VertexCount - 3 == abs((int)(m_previousVertices.back() - index))
				  )
				
#if 0
				      1 == abs((int)(m_previousVertices.front() - index))
				   || 3 == abs((int)(m_previousVertices.back()  - index))
#endif
				   
				   )
			{
				index = m_rng(m_randomEngine);
			}
		}
		break;
#endif

		// TODO: wrong
#if 1
	case EVertexRestrictions::NotOffsets_1_and_4:
		if (!m_previousVertices.empty())
		{
			while (  
		
				(
				1 == abs((int)(m_previousVertices.front() - index))
				|| VertexCount - 1 == abs((int)(m_previousVertices.front() - index))
				)

				||
				(4 == abs((int)(m_previousVertices.back() - index))
				|| VertexCount - 4 == abs((int)(m_previousVertices.back() - index))
				)
#if 0
				1 == abs((int)(m_previousVertices.front() - index))
				   || 4 == abs((int)(m_previousVertices.back()  - index))
#endif
				   )
			{
				index = m_rng(m_randomEngine);
			}
		}
		break;
#endif
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

void Shape::setVertexRestrictions(EVertexRestrictions restrictions)
{
	m_selectionRestrictions = restrictions;
}

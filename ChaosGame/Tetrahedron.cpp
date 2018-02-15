#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


Tetrahedron::Tetrahedron()
	: Shape(VertexCount)
{
	// TODO: fix the coordinates - the shape is somewhat skewed
	m_vertices = {
		// base
		{ -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f },
		{  0.0f, -1.0f, -1.0f },
		// top
		{  0.0f,  1.0f,  0.0f },
	};
}

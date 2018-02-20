#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


Tetrahedron::Tetrahedron()
	: Shape(VertexCount)
{
	// TODO: rotate
	m_vertices = {
		{  1.0f,  0.0f, -0.707f },
		{ -1.0f,  0.0f, -0.707f },
		{  0.0f,  1.0f,  0.707f },
		{  0.0f, -1.0f,  0.707f },

#if 0
		// TODO: fix the coordinates - the shape is somewhat skewed
		// base
		{ -1.0f, -1.0f,  1.0f   },
		{  1.0f, -1.0f,  1.0f   },
		//{  0.0f, -1.0f,  -0.866f },
		{ 0.0f, -1.0f, -1.0f },
		// top
		//{ 0.0f, 0.866f, 1.433f },
		{ 0.0f, 1.0f, 0.0f },
#endif
	};
}

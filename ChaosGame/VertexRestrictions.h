#pragma once


namespace ChaosGameApp
{
	// Restrictions on selecting the next vertex of the shape.
	enum class EVertexRestrictions {
		None,                         // No restrictions
		NotTheSame,                   // The vertex cannot have the same index as the previous one
		NotOffset_1,                  // The vertex cannot be 1 place away from the previously chosen vertex
		NotOffset_1_Anticlockwise,    // The vertex cannot be 1 place away (anti-clockwise) from the previous vertex
		NotOffset_2,                  // The vertex cannot be 2 places away from the previously chosen vertex
		NotOffsets_1_and_3,           // The vertex cannot be 1 or 3 places away, respectively, from the two previous vertices
		NotOffsets_1_and_4,           // The vertex cannot be 1 or 4 places away, respectively, from the two previous vertices
	};
}

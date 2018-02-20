#pragma once


namespace ChaosGameApp
{
	// Restrictions on selecting the next vertex of the shape.
	enum class VertexRestrictions {
		None,                         // No restrictions
		NotTheSame,                   // The vertex cannot have the same index as the previous one
		CannotHaveOffset_2,           // The vertex cannot be 2 places away from the previously chosen vertex
		CannotHaveOffsets_1_and_3,    // The vertex cannot be 1 or 3 places away, respectively, from the two previously chosen vertices
		CannotHaveOffsets_1_and_4,    // The vertex cannot be 1 or 4 places away, respectively, from the two previously chosen vertices
	};
}

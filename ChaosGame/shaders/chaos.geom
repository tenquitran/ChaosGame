#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout (points) in;
layout (triangle_strip, max_vertices = 10) out;

void main(void)
{
	// TODO: hard-coded; make a uniform
	// Half of the cube diagonal.
	float cubeHalfDiag = 0.01;

	// TODO: we have only one incoming vertex, so the cycle may be unnecessary
	int i;
	for (i = 0; i < gl_in.length(); i++)
	{
		// Sides of the cube.

		// Forward side.

		gl_Position = vec4(gl_in[i].gl_Position.x - cubeHalfDiag, gl_in[i].gl_Position.y + cubeHalfDiag, gl_in[i].gl_Position.z + cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		gl_Position = vec4(gl_in[i].gl_Position.x - cubeHalfDiag, gl_in[i].gl_Position.y - cubeHalfDiag, gl_in[i].gl_Position.z + cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		gl_Position = vec4(gl_in[i].gl_Position.x + cubeHalfDiag, gl_in[i].gl_Position.y + cubeHalfDiag, gl_in[i].gl_Position.z + cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		gl_Position = vec4(gl_in[i].gl_Position.x + cubeHalfDiag, gl_in[i].gl_Position.y - cubeHalfDiag, gl_in[i].gl_Position.z + cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		// Right side.

		gl_Position = vec4(gl_in[i].gl_Position.x + cubeHalfDiag, gl_in[i].gl_Position.y + cubeHalfDiag, gl_in[i].gl_Position.z - cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		gl_Position = vec4(gl_in[i].gl_Position.x + cubeHalfDiag, gl_in[i].gl_Position.y - cubeHalfDiag, gl_in[i].gl_Position.z - cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();
		
		// Backward side.
		
		gl_Position = vec4(gl_in[i].gl_Position.x - cubeHalfDiag, gl_in[i].gl_Position.y + cubeHalfDiag, gl_in[i].gl_Position.z - cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		gl_Position = vec4(gl_in[i].gl_Position.x - cubeHalfDiag, gl_in[i].gl_Position.y - cubeHalfDiag, gl_in[i].gl_Position.z - cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		// Left side.

		gl_Position = vec4(gl_in[i].gl_Position.x - cubeHalfDiag, gl_in[i].gl_Position.y + cubeHalfDiag, gl_in[i].gl_Position.z + cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();

		gl_Position = vec4(gl_in[i].gl_Position.x - cubeHalfDiag, gl_in[i].gl_Position.y - cubeHalfDiag, gl_in[i].gl_Position.z + cubeHalfDiag, gl_in[i].gl_Position.w);
		EmitVertex();
	}
	EndPrimitive();
}

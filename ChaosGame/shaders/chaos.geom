#version 440 core
#pragma debug(on)
#pragma optimize(off)

layout (points) in;
layout (triangle_strip, max_vertices = 10) out;

void main(void)
{
	// TODO: hard-coded; make a uniform
	// Half-diagonal of the cube.
	float cubeHalfDiag = 0.01;
	//float cubeHalfDiag = 0.01;
	//float cubeHalfDiag = 0.2;

	float x = gl_in[0].gl_Position.x;
	float y = gl_in[0].gl_Position.y;
	float z = gl_in[0].gl_Position.z;
	float w = gl_in[0].gl_Position.w;

	// Sides of the cube.

	// Forward side.

	gl_Position = vec4(x - cubeHalfDiag, y + cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x - cubeHalfDiag, y - cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x + cubeHalfDiag, y + cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x + cubeHalfDiag, y - cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	// Right side.

	gl_Position = vec4(x + cubeHalfDiag, y + cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x + cubeHalfDiag, y - cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();
		
	// Backward side.
		
	gl_Position = vec4(x - cubeHalfDiag, y + cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x - cubeHalfDiag, y - cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();

	// Left side.

	gl_Position = vec4(x - cubeHalfDiag, y + cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x - cubeHalfDiag, y - cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	EndPrimitive();

	// Top of the cube.

	gl_Position = vec4(x + cubeHalfDiag, y + cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x - cubeHalfDiag, y + cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x + cubeHalfDiag, y + cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x + cubeHalfDiag, y + cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	EndPrimitive();

	// Bottom of the cube.

	gl_Position = vec4(x - cubeHalfDiag, y - cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x - cubeHalfDiag, y - cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x + cubeHalfDiag, y - cubeHalfDiag, z + cubeHalfDiag, w);
	EmitVertex();

	gl_Position = vec4(x + cubeHalfDiag, y - cubeHalfDiag, z - cubeHalfDiag, w);
	EmitVertex();

	EndPrimitive();
}

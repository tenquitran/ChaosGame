#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////

using namespace ChaosGameApp;

//////////////////////////////////////////////////////////////////////////


ProgramGLSL::ProgramGLSL(const ShaderCollection& shaders)
	: m_shaderFiles(shaders)
{
	m_program = OpenGLHelpers::buildShaders(m_shaderFiles);
	if (0 == m_program)
	{
		throw EXCEPTION(L"Failed to build the program");
	}
}

ProgramGLSL::~ProgramGLSL()
{
	if (0 != m_program)
	{
		glDeleteProgram(m_program);
	}
}

GLuint ProgramGLSL::getProgram() const
{
	return m_program;
}

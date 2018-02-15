// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Additional headers our program requires.

#include <cassert>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <map>
#include <random>

#include <windows.h>
#include <atlstr.h>

// GLEW library (http://glew.sourceforge.net/)
#include <GL/glew.h>
#include <GL/wglew.h>
#pragma comment(lib, "glew32.lib")

// GLM library (http://glm.g-truc.net/)
#define GLM_FORCE_RADIANS    // enforce all angles to be in radians, not in degrees
#include <glm/glm.hpp>       // the most common GLM headers
#include <glm/gtc/matrix_transform.hpp>    // matrix transformations: glm::perspective(), etc.
#include <glm/gtc/type_ptr.hpp>            // glm::value_ptr() to pass matrices to shaders, etc.

// WGL (Windows OpenGL)
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


namespace ChaosGameApp
{
	// For OpenGL code.
#define BUFFER_OFFSET(offset) ((void *)(offset))
}


#include "Exception.h"

#include "OpenGLHelpers.h"
#include "ProgramGLSL.h"
#include "Camera.h"

#include "Shape.h"
#include "Triangle.h"
#include "Tetrahedron.h"

#include "Scene.h"
#include "MainWindow.h"

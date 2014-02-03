#include "misc.h"

#include "gl.h"

#include <iostream>

namespace base
{
	std::string glErrorString( int glErrorCode )
	{
		switch(glErrorCode)
		{
		case GL_NO_ERROR:
				return "No error has been recorded.";break;
		case GL_INVALID_ENUM:
				return "An unacceptable value is specified for an enumerated argument.";break;
		case GL_INVALID_VALUE:
				return "A numeric argument is out of range.";break;
		case GL_INVALID_OPERATION:
			return "The specified operation is not allowed in the current state.";break;
		case GL_STACK_OVERFLOW:
			return "This command would cause a stack overflow.";break;
		case GL_STACK_UNDERFLOW:
			return "This command would cause a stack underflow.";break;
		case GL_OUT_OF_MEMORY:
			return "There is not enough memory left to execute the command.";break;
		case GL_TABLE_TOO_LARGE:
			return "GL_TABLE_TOO_LARGE";break;
		default:
			return "";
		};
		return "";
	}
	bool checkGlError(const char *file, int line)
	{
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error at " << file << " line " << line << ":" << glErrorString(err) << std::endl;
			return true;
		}
		return false;
	}
}

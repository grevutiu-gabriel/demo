#pragma once
#include <string>

#define CHECKGLERROR() ::base::checkGlError(__FILE__, __LINE__)

namespace base
{
	std::string                           glErrorString( int glErrorCode );
	bool                          checkGlError(const char *file, int line); // returns true if there was an error
}







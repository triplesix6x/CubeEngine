#include "../includes/CubeException.h"
#include <sstream>



CubeException::CubeException(int line, const char* file) : line(line), file(file)
{

}

const char* CubeException::whatEx() const
{
	std::ostringstream oss;
	oss << getType() << std::endl << getOriginString();
	whatExBuffer = oss.str();
	return whatExBuffer.c_str();
}

const char* CubeException::getType() const
{
	return "Cube Exception";
}

int CubeException::getLine() const
{
	return line;
}

const std::string& CubeException::getFile() const
{
	return file;
}

std::string CubeException::getOriginString() const
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl << "[Line] " << line;
	return oss.str();
}
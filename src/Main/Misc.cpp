/*
 * Misc.cpp
 *
 *  Created on: Oct 4, 2016
 *      Author: evert
 */
#include "../Main/Misc.hpp"

string val2str(const char* fmt, double val)
{
	char   valstr [50];

	sprintf(valstr, fmt, val);
	return valstr;
}

std::vector<std::string> split(const std::string text, char sep)
{
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;

	while ((end = text.find(sep, start)) != std::string::npos)
	{
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}

static RMS_IQ ob1j;
static RMS obj2;


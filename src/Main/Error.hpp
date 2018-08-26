/*
 * Error.hpp
 *
 *  Created on: Jan 18, 2017
 *      Author: evert
 */

#ifndef ERROR_HPP_
#define ERROR_HPP_

#include <string>
#include <sstream>

using namespace std;

class Error
{
	string _err;
	static  int _line;
public:
	Error(string err) : _err(err) {}

	static void setLineNum(int n) { _line = n; }

	operator const char* ()
	{
		stringstream ret;

		ret << _line << " : " << _err;
		return ret.str().c_str();
	}
};

#endif /* ERROR_HPP_ */

/*
 * tVar.hpp
 *
 *  Created on: Jan 17, 2017
 *      Author: evert
 */

#ifndef TVAR_HPP_
#define TVAR_HPP_

#include <string>
#include <sstream>

#include "../Main/Error.hpp"

using namespace std;

struct tVar
{
	string _str;
	double _dbl;
	int    _int;
	enum {eDbl, eInt, eStr } _type;

	tVar(string str) : _str(str), _dbl(0.0), _int(0),   _type(eStr) {}
	tVar(double val) : _str(""),  _dbl(val), _int(0),   _type(eDbl) {}
	tVar(int val) :    _str(""),  _dbl(0.0), _int(val), _type(eInt) {}
	tVar(bool val) :   _str(""),  _dbl(0.0), _int(val), _type(eInt) {}

	operator string()
	{
		if (_type != eStr)
			throw Error("Type mismatch.");
		return _str;
	}

	operator double()
	{
		if (_type != eDbl)
			throw Error("Type mismatch.");
		return _dbl;
	}

	operator int()
	{
		if (_type != eInt)
			throw Error("Type mismatch.");
		return _int;
	}

	operator bool()
	{
		if (_type != eInt)
			throw Error("Type mismatch.");
		return _int != 0;
	}

	const string getStr()
	{
		string ret;
		stringstream t;

		if (_type == eInt) { t <<_int; ret = t.str(); return ret; }
		if (_type == eDbl) { t <<_dbl; ret = t.str(); return ret; }
		return string("'") + _str + "'";
	}
};




#endif /* TVAR_HPP_ */

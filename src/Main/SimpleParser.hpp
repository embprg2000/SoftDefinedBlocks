/*
 * SimpleParser.hpp
 *
 *  Created on: Jan 16, 2017
 *      Author: evert
 */

#ifndef SIMPLEPARSER_HPP_
#define SIMPLEPARSER_HPP_

// http://www.technical-recipes.com/2011/a-mathematical-expression-parser-in-java-and-cpp/

#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>

#include "Error.hpp"

using namespace std;

class SimpleParser
{
	enum { _maxTmpVars = 20 };
	double _tmpVariable[_maxTmpVars];
	map<string,double> _variables;

	void Tokenize(const string& str,
			vector<string>& tokens, string& delim,
					const string& delimiters = " ");
	double getVal(string exp);
	double eval(double l, double r, char op);
	double evalL2(string exp);
	void assignVar(string name, const string& exp);

public:

	SimpleParser() {}
	virtual ~SimpleParser() {}

	bool processLine(string str);
	double evalL1(string exp, int level = 0);
};

#endif /* SIMPLEPARSER_HPP_ */

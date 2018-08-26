//============================================================================
// Name        : SimpleParser.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// http://www.technical-recipes.com/2011/a-mathematical-expression-parser-in-java-and-cpp/

#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>     // std::cout
#include <vector>
#include <fstream>
#include <sstream>
#include <stack>
#include <map>
#include <math.h>

#include "SimpleParser.hpp"

int Error::_line;

void SimpleParser::Tokenize(const string& str,
                      vector<string>& tokens, string& delim,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    if (pos > 0)
    {
    	// Skip delimiter in numbers like 123e-45
		if (str[pos-1] == 'e' || str[pos-1] == 'E')
		{
			if (str[pos] == '+' || str[pos] == '-')
			{
				pos = str.find_first_of(delimiters, pos+1);
			}
		}
    }

    while (string::npos != pos || string::npos != lastPos)
    {
    	delim += str[ pos ];

    	int match;
    	do {
    		// When delimiter is found check next character
    		match = 0;
    		for_each(delimiters.begin(), delimiters.end(), [=,&match](char c)
    				{ match += str[pos+1] == c && c != ' ';});
    		if (match > 0)
    			delim += str[ ++pos ];
    	} while (match > 0);

    	string token;
    	if (pos == string::npos)
    		pos = str.length();
    	for (string::size_type i=lastPos; i<pos; ++i)
    	{
    		if (str[i] != ' ')
    			token += str[i];
    	}
    	tokens.push_back(token);

        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);

        // Skip delimiter in numbers like 123e-45
        if (str[pos-1] == 'e' || str[pos-1] == 'E')
        {
        	if (str[pos] == '+' || str[pos] == '-')
        	{
        		pos = str.find_first_of(delimiters, pos+1);
        	}
        }
    }
}

double SimpleParser::getVal(string exp)
{
	double ret = 0;

	if ( isdigit( exp[0] ) )
		stringstream(exp) >> ret;

	else if (exp.substr(0,2) == "#t")
	{
		int idx;

		stringstream(exp.substr(2)) >> idx;

		return _tmpVariable[idx];
	}
	else
	{
		if (exp == "true" || exp == "TRUE")
			return 1.0;
		else if (exp == "false" || exp == "FALSE")
			return 0.0;
		else
		{
			auto it = _variables.find(exp);

			if (it != _variables.end())
				ret = it->second;
			else
			{
				string::size_type idx = exp.find("#t");
				if (idx != string::npos)
				{
					int varIdx;
					string func = exp.substr(0,idx);
					stringstream(exp.substr(idx+2)) >> varIdx;
					double var = _tmpVariable[varIdx];

					if (func == "sin") 	      return sin( var );
					else if (func == "cos")   return cos( var );
					else if (func == "tan")   return tan( var );
					else if (func == "log")   return log( var );
					else if (func == "log10") return log10( var );
					else
						throw Error(string("Function: ") + func + " not supported.");
				}
				else
					throw Error(string("variable: ") + exp + " not found.");
			}
		}
	}
	return ret;
}

double SimpleParser::eval(double l, double r, char op)
{
	double ret;

	switch (op)
	{
		case '+': ret = l + r; break;
		case '-': ret = l - r; break;
		case '*': ret = l * r; break;
		case '/': ret = l / r; break;
	}
	return ret;
}

double SimpleParser::evalL2(string exp)
{
	double ret = 0;
    string  delim;
    vector<string> tokens;
    vector<double> vals;

    Tokenize(exp, tokens, delim, "+-*/");

    for (unsigned int i=0; i<tokens.size(); ++i)
    	vals.push_back( getVal( tokens[i] ) );

    // If exp only contains one number of variable
    if (tokens.size() == 1)
    	return vals[0];

	string::size_type i = 0;
    while (delim.length() > 1 && i < delim.length())
    {
    	char c = delim[i];

    	if (c == '*' || c == '/')
    	{
    		if (c == '*')
    			ret = vals[i] * vals[i+1];
    		else
    			ret = vals[i] / vals[i+1];

    		//cout << vals[i] << c << vals[i+1] << endl;
    		vals.erase(vals.begin() + i);
    		vals[i] = ret;
    		delim.erase(delim.begin() + i);
    	}
    	else
    		++i;
    }

    i = 0;
    while (delim.length() > 1)
    {
    	char c = delim[i];

    	if (c == '+' || c == '-')
    	{
    		if (c == '+')
    			ret = vals[i] + vals[i+1];
    		else
    			ret = vals[i] - vals[i+1];

    		vals.erase(vals.begin() + i);
    		vals[i] = ret;
    		delim.erase(delim.begin() + i);
    	}
    	else
    	{
    		throw Error(string("Error: operator ") + c + " not expected!");
    		break;
    	}
    }
	return ret;
}

double SimpleParser::evalL1(string exp, int level)
{
	static double ret = 0;

	string::size_type p1 = exp.find_last_of("(");
	if (p1 != string::npos)
	{
		string::size_type p2 = exp.find(")");
		if (p2 != string::npos)
		{
			stringstream var;
			string str = exp.substr(p1+1,p2-p1-1);

			ret = evalL2(str);
			if (level < _maxTmpVars)
				_tmpVariable[level] = ret;
			else
				throw Error("To many temp. Variables");

			var << "#t" << level++;
			str = exp.substr(0,p1);
			str += var.str();
			str += exp.substr(p2+1);
			evalL1( str, level );
		}
	}
	else
		ret = evalL2(exp);

	return ret;
}

void SimpleParser::assignVar(string name, const string& exp)
{
	_variables[name] = evalL1(exp);
	cout << name << "=" << _variables.find(name)->second << endl;
}

bool SimpleParser::processLine(string str)
{
    string  delim;
    vector<string> tokens;

    if (str.length() == 0) // Return on empty line
    	return true;

    // Remove spaces tabs etc.
    str.erase( remove_if(str.begin(), str.end(),
                    [](char x){return isspace(x);}), str.end());

    // Skip remarks
    if (str.substr(0,2) == "//")
    	return true;

    if (str[str.length()-1] != ';')
    	throw Error(string("Expected ';' : ") + str);
    else
    	str = str.substr(0, str.length() - 1);

    for (auto c : str)
    {
     	if (c == '{' || c == '}' || c == '<' || c == '>')
     	{
			throw Error("Characters {,},<,> are not supported.");
			return false;
     	}
    }

    Tokenize(str, tokens, delim, "+-*/(),=");

    int match = 0;
    for_each(delim.begin(), delim.end(), [=,&match](char c)
        				{ match += c == '('; match -= c == ')';});

    if (match != 0)
    {
    	throw Error(string("Parenthesis mismatch: ") + str);
    	return false;
    }

    if (delim[0] == '=')
    {
    	string exp = str.substr( str.find("=")+1);

    	assignVar(tokens[0], exp);
    }
    else
    {
    	return false;
    	//throw Error("Not and assignment statement");
    }

//    cout << "---------------------------" << endl;
//    copy(tokens.begin(), tokens.end(), ostream_iterator<string>(cout, ", "));
//    cout << endl;
//    cout << delim << endl;

    return true;
}


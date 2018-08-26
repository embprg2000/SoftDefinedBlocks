/*
 * Variables.hpp
 *
 *  Created on: Sep 17, 2016
 *      Author: evert
 */

#ifndef VARIABLES_HPP_
#define VARIABLES_HPP_

#include <typeinfo>
#include <iostream>
#include <string>     // std::string, std::stoi
#include <stdlib.h>   // atof
#include <list>
#include <vector>
#include <sstream>
#include <atomic>
#include <thread>
#include <mutex>

#include "../Main/IBlock.hpp"

#include "../Server/ClientConnect.hpp"

using namespace std;

class Variables;

static mutex   mutexV;

//std::vector<std::string> split(const std::string text, char sep);

class IVar
{
public:
	IVar()
	{
		m_vars.push_back( this );
	}
	virtual ~IVar();

	virtual bool setVal(string str) = 0;

	virtual string getBlock() = 0;
	virtual string getBlkName()  = 0;
	virtual string getVal()   = 0;
	virtual string getInfo()  = 0;

	friend Variables;

private:
	static	list<IVar*> m_vars;
};

template<class tVar>
class VarVal : public IVar
{
public:
	VarVal(IBlock* block, string name, tVar& val, tVar min, tVar max, string fmt, atomic_bool& dirty )
				: m_block(block), m_name(name), m_val(val), m_min(min), m_max(max), m_fmt(fmt), m_isDirty(dirty)
	{
		if (typeid(tVar) == typeid(int))
		{
			if (m_fmt == "")
				m_fmt = "%d";
		}
		else
		{
			if (typeid(tVar) == typeid(double))
			{
				if (m_fmt == "")
					m_fmt = "%f";
			}
			else
				DBG_ERR(Err::eErrWrongCast, "Only (int) and (double) types are supported.");
		}
	}

	virtual ~VarVal()
	{
		//cout << "Destructor VarVal: " <<  m_name << endl;
	}

	virtual string getBlock() { return m_block->getBlkName(); }
	virtual string getBlkName()  { return m_name; }

	virtual string getVal()
	{
	    char   valstr [50];

	    sprintf(valstr, m_fmt.c_str(), m_val);
	    m_isDirty = false;
	    return getBlkName() + "," + valstr;
	}

	bool setVal(tVar val)
	{
		if (checkVal(val) == true)
		{
			m_val = val;
			m_isDirty = true;
			return true;
		}
		return false;
	}

	virtual bool setVal(string str)
	{
		if (typeid(tVar) == typeid(int))
		{
			m_isDirty = true;
			return setVal(atoi(str.c_str()));
		}

		if (typeid(tVar) == typeid(double))
		{
			m_isDirty = true;
			return setVal(atof(str.c_str()));
		}
		return false;
	}


	virtual string getInfo()
	{
		ostringstream info;

		info << getBlock() << "," << m_name << "," << m_min << "," << m_max << "," << m_val;
		return info.str();
	}

	tVar& operator==(const tVar& arg)
	{
		if (this->m_val != arg.m_val)
			m_isDirty = true;
		return this->m_val == arg.m_val;
	}

private:
	IBlock* m_block;
	string m_name;
	tVar& m_val;
	tVar  m_min;
	tVar  m_max;
	string m_fmt;
	atomic_bool& m_isDirty;

	bool checkVal(tVar val) { return val >= m_min && val <= m_max; }
};

class VarStr : public IVar
{
public:
	VarStr(IBlock* block, string name, string& val, atomic_bool& dirty )
				: m_block(block), m_name(name), m_val(val), m_isDirty(dirty) {}

	virtual ~VarStr()
	{
		//cout << "Destructor VarVal: " <<  m_name << endl;
	}

	virtual string getBlock() { return m_block->getBlkName(); }
	virtual string getBlkName()  { return m_name; }
	virtual string getVal()   { m_isDirty = false; return m_val; }

	virtual bool setVal(string val)
	{
		if (m_val != val)
		{
			m_val = val;
			m_isDirty = true;
		}
		return true;
	}

	virtual string getInfo()
	{
		ostringstream info;

		info << getBlock() << "," << m_name << "," << m_val;
		return info.str();
	}

	virtual bool isDirty() { return m_isDirty; }

private:
	IBlock* m_block;;
	string  m_name;
	string& m_val;
	atomic_bool& m_isDirty;
};


template<class T>
class mutexVar
{
public:
	mutexVar(IBlock* block, const string name, T val, T min, T max, const string fmt="") : m_val(val)
	{
		new VarVal<T>(block, name, m_val, min, max, fmt, m_dirty);
	}

	T& operator = (const T val)
	{
		unique_lock<mutex> lock(m_mutex);
		m_val = val;
		return m_val;
	}

	operator double()
	{
		unique_lock<mutex> lock(m_mutex);
		m_dirty = false;
		return m_val;
	}

	bool isDirty() { return m_dirty; }

private:
	T m_val;
	mutex   m_mutex;
	atomic_bool m_dirty;
};

class mutexStr
{
public:
	mutexStr(IBlock* block, string name, string val) : m_val(val)
	{
		new VarStr(block, name, m_val, m_dirty);
	}

	string& operator = (const string val)
	{
		unique_lock<mutex> lock(m_mutex);
		m_val = val;
		return m_val;
	}

	operator string()
	{
		unique_lock<mutex> lock(m_mutex);
		return m_val;
	}

	bool isDirty() { return m_dirty; }

private:
	string m_val;
	mutex   m_mutex;
	atomic_bool m_dirty;
};

class Variables
{
public:
	typedef list<IVar*>::iterator iter;

	Variables();
	virtual ~Variables();

	virtual string getBlkName() { return "Variables"; }

	string getInfo();
	string getValues();

private:

	iter   getIter(string block, string name);
	string getVal(string block, string name);
	string getInfo(string block, string name);
	string command(string cmd);
};

typedef mutexVar<double> mDbl;
typedef mutexVar<int>    mInt;
typedef mutexStr         mStr;

/*
int main()
{
	mDouble freq("SDR", "Freq", 99.1e6, 40e6, 1.5e9);
	mDouble IFrate("SDR", "IFrate", 1e6, 2e5, 20e6);
	mStr    fName("SDR", "fName", "Lekker nat");

	Variables vars = Variables::getInst();

	cout << vars.getInfo();

	cout << vars.getVal("SDR", "Freq") << endl;
	cout << vars.getVal("SDR", "fName") << endl;

	cout << vars.command("SDR:Freq::55e7");
	//freq = 69.1e6;
	fName = "Suck it";

	cout << "Val=" << freq + 2e6 << endl;

	cout << vars.getVal("SDR", "Freq") << endl;
	cout << vars.getVal("SDR", "fName") << endl;

	return 0;
}
*/



#endif /* VARIABLES_HPP_ */

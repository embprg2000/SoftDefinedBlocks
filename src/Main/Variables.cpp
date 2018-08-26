/*
 * Variables.cpp
 *
 *  Created on: Sep 19, 2016
 *      Author: evert
 */

#include <unistd.h>
#include <iostream>

#include "../Main/Misc.hpp"
#include "../Main/Variables.hpp"

#define CRLF "\r\n"

list<IVar*> IVar::m_vars;

IVar::~IVar()
{
}

Variables::Variables()
{
}

Variables::~Variables()
{
	for (auto it = IVar::m_vars.begin(); it != IVar::m_vars.end(); ++it)
		delete *it;
	IVar::m_vars.clear();
}

Variables::iter Variables::getIter(std::string block, std::string name)
{
	for (auto it = IVar::m_vars.begin(); it != IVar::m_vars.end(); ++it)
	{
		if ((*it)->getBlock() == block && (*it)->getBlkName() == name)
		{
			return it;
		}
	}
	return (Variables::iter) 0;
}

std::string Variables::getVal(std::string block, std::string name)
{
	iter it = getIter(block, name);

	if (it != (Variables::iter) 0)
		return (*it)->getVal();
	else
		return "";
}

std::string Variables::getInfo(std::string block, std::string name)
{
	iter it = getIter(block, name);

	if (it != (Variables::iter) 0)
		return (*it)->getInfo();
	else
		return "";
}

std::string Variables::getValues()
{
	string val;

	for (auto it = IVar::m_vars.begin(); it != IVar::m_vars.end(); ++it)
		val += (*it)->getVal() + ",";

	return val;
}

std::string Variables::getInfo()
{
	ostringstream info;

	for (auto it = IVar::m_vars.begin(); it != IVar::m_vars.end(); ++it)
		info << (*it)->getInfo() << ",";
	return info.str();
}

std::string Variables::command(std::string cmd)
{
	ostringstream ret;
	std::vector<std::string> args = split(cmd, ':');

	if (args.size() == 4)
	{
		for (auto it = IVar::m_vars.begin(); it != IVar::m_vars.end(); ++it)
		{
			if ((*it)->getBlock() == args[0] && (*it)->getBlkName() == args[1])
			{
				if ((*it)->setVal(args[3]) == true)
					ret << "OK" << CRLF;
				else
					ret << "Out of limits :" << (*it)->getInfo();
				return ret.str();
			}
		}
		ret << "Not found:" << args[0] << ":" << args[1] << CRLF;
	}
	else
		ret << "Wrong format: " << cmd << CRLF;

	return ret.str();
}

/*
Err Variables::init()
{
	return Err::eOK;
}

Err Variables::start()
{
	// This is called before all other functions
	cout << "*** Start ***" << endl;
	cout << getInfo() << endl;
	m_sse.init();
	return Err::eOK;
}

Err Variables::update()
{
	return Err::eOK;
}

Err Variables::process()
{
	//cout << getValues() << endl;

	string tmp = m_sse.recvFromBrowser();

	if (tmp.compare(0,5,"BUTT,") == 0)
	{
		tmp = tmp.substr(5);
		if (tmp == "Exit")
		{
			bTerminate.store(true);
			m_sse.stop();
		}
		else if (tmp == "Test1")
			m_sse.sendCommand("STR,divTest1,<h2>Lekker</h2>");
		else if (tmp == "Test2")
		{
			ostringstream tmp;

		    tmp << "GRP" << "," << "canv1" <<  "," << 0 <<  "," << 15 <<  "," << -1 <<  "," << 1;
		    tmp <<  "," << "#875050" <<  "," << 2;
		    for (int i=0; i<200; ++i)
		    	tmp << "," << sin((double)i/10.0);
		    m_sse.sendCommand(tmp.str());
		}
	}
	else
	{
		m_sse.sendCommand("VAL," + getValues());
	}

	bTerminate.store(true);
	return Err::eOK;
}

Err Variables::stop()
{
	cout << "*** Stop ***" << endl;
	m_sse.stop();

	DBG_LOG("Stopped Variables thread.\n");
	return Err(Err::eOK);
}
*/

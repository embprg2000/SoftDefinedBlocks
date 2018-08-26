/*
 * WidgetVarList.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetVarList.hpp"


WidgetVarList::WidgetVarList(string divId, string clss, list<string> arr)
	: WidgetCommon(divId)
	, _clss(clss)
	, _varList(arr) { _currVals.resize( _varList.size() ); }

WidgetVarList::~WidgetVarList()
{
}

void WidgetVarList::init()
{
	ostringstream html;

	html << "div," << _divId << ",";
	html << "<table class=\"" << _clss << "\">";
	html << "<tr>";
	for (auto element : _varList)
		html << "<th style=\"width: 80px\">" << element << "</th>";
	html << "</tr><tr>";
	for (list<string>::size_type i=0; i<_varList.size(); ++i)
		html << "<td class=\"" << _clss << "\"><div id=\"" << _divId << i << "\">-</div></td>";
	html << "</tr></table>";
	_BrwConn.sendCommand(html.str());
}

bool WidgetVarList::process(const string& msg)
{
	//if (_bIsDirty == true)
	{
		int i = 0;
		ostringstream html;

		html << "VAL,";
		for (auto val : _currVals)
			html << _divId << i++ << "," << val << ",";
		_BrwConn.sendCommand(html.str());

		_bIsDirty = false;
	}
	return false;
}

int WidgetVarList::getSelected()
{
	return 0;
}

void WidgetVarList::setValues(vector<double> vals)
{
	if (_varList.size() != vals.size())
		return;

	int i = 0;
	for (auto val : vals)
	{
		_bIsDirty = false;
		if (_currVals[i] != val)
		{
			_bIsDirty = true;
			_currVals[i] = val;
		}
		++i;
	}
}



/*
 * WidgetSelect.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetSelect.hpp"


WidgetSelect::WidgetSelect(string divId, int sel, vector<string> selections)
	: WidgetCommon(divId)
	, _sel(sel)
	, _selections(selections) {}
WidgetSelect::~WidgetSelect() {}

void WidgetSelect::init()
{
	ostringstream html;

	html << "div,id" << _divId << ",";
	html << "<select id=\"sel" << _divId << "\" onchange=\"onChangeSel(this.id)\">";
	int i = 0;
	for (auto sel : _selections)
	{
		if (_sel == i)
			html << "<option value=\"" << sel << "\" selected>" + sel;
		else
			html << "<option value=\"" << sel << "\">" + sel;
		++i;
	}

	html << "</select>";
	_BrwConn.sendCommand(html.str());
}

bool WidgetSelect::process(const string& msg)
{
	if (msg.compare(0, 7, "SEL,sel") == 0)
	{
		string tmp = msg.substr(7);
		ostringstream html;
		int idx = tmp.find_first_of(",");

		if (idx > 0)
		{
			if (tmp.substr(0,idx) == _divId)
			{
				int num = 0;
				for_each(tmp.begin()+idx+1, tmp.end(), [&num](char c) { if (isdigit(c)) { num *= 10; num += c - '0';} });

				_sel = num;
				html << "SEL,sel" << _divId << "," << _sel << ",";
				_BrwConn.sendCommand(html.str());
				return true;
			}
		}
	}
	return false;
}

int WidgetSelect::getSelected()
{
	return _sel;
}




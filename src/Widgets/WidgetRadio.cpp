/*
 * WidgetRadio.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetRadio.hpp"


WidgetRadio::WidgetRadio(string divId, int sel, vector<string> selections)
	: WidgetCommon(divId)
	, _sel(sel)
	, _selections(selections) {}

WidgetRadio::~WidgetRadio() {}

void WidgetRadio::init()
{
	ostringstream html;

	html << "div,id" << _divId << ",";
	html << "<form><table border=1>";

	int i = 0;
	for (auto sel : _selections)
	{
		html << "<tr><td><input id=\"rdo" << _divId << i << "\" type=\"radio\" name=\"" << _divId;
		html << "\" value=" << i << " onclick=\"onChangeRadio(this.id)\"";
		if (_sel == i)
			html << "\" checked" + sel;
		html << ">" << sel << "</td></tr>";
		++i;
	}

	html << "</table></form>";
	_BrwConn.sendCommand(html.str());
}

bool WidgetRadio::process(const string& msg)
{
	if (msg.compare(0, 7, "RDO,rdo") == 0)
	{
		string tmp = msg.substr(7);
		ostringstream html;
		int idx = tmp.find_first_of(",");

		if (idx > 0)
		{
			if (tmp.substr(0,_divId.size()) == _divId)
			{
				int num = 0;
				for_each(tmp.begin()+idx+1, tmp.end(), [&num](char c) { if (isdigit(c)) { num *= 10; num += c - '0';} });

				_sel = num;
				//					html << "RDO,rdo" << _divId << "," << _sel << ",";
				//					_BrwConn.sendCommand(html.str());
				return true;
			}
		}
	}
	return false;
}

int WidgetRadio::getSelected()
{
	return _sel;
}



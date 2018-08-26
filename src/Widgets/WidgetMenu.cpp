/*
 * WidgetMenu.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetMenu.hpp"

#include <fstream>


WidgetMenu::WidgetMenu(string divId, vector<string> items)
	: WidgetCommon(divId)
	, _menu(items)
	, _selected(0) {}

WidgetMenu::WidgetMenu(WidgetMenu&& mv)
	: WidgetCommon(mv._divId)
	, _menu(0)
	, _selected(-1)
{
	swap(_menu, mv._menu);
	swap(_selected,mv._selected);
}


void WidgetMenu::init()
{
	int i = 0;
	ostringstream html;

	html << "div," << _divId << ",";
	for (auto m : _menu)
		html << "<a href=\"javascript:void(0)\" onclick=\"menuSel(" << i++ << ")\">" << m << "</a>";
	_BrwConn.sendCommand(html.str());
}

bool WidgetMenu::process(const string& msg)
{
	if (msg.compare(0, 4, "Menu") == 0)
	{
		int idx = 0;

		for_each(msg.begin()+5, msg.end(), [&idx](char c) { if (isdigit(c)) { idx *= 10; idx += c - '0';} });

		_file = _menu[idx];

		size_t i = _file.find_first_of(' ');
		while (i != string::npos) {
			_file[i] = '_';
			i = _file.find_first_of(' ', i+1);
		}

		ifstream fs(string("html/") + _file + ".div");
		if (fs.is_open() == true)
		{
			fs.close();
			_bIsDirty = true;
			_selected = idx;
		}
		else
		{
			_BrwConn.sendCommand( string("div,idMain,Couldn't open file: ") + string("html/") + _file + ".div");
		}
		return true;
	}
	return false;
}

int WidgetMenu::getSelected()
{
	_bIsDirty = false;
	return _selected;
}

bool WidgetMenu::file2client()
{
	if (_bIsDirty == false)
		return true;

	string line;
	ifstream fs(string("html/") + _file + ".div");
	string data("div,idMain,");

	if (fs.is_open() == true)
	{
		while(!fs.eof())
		{
			getline(fs,line);
			data += line + '\n';
		}
		data.erase( remove_if(data.begin(), data.end(),
				[](char x){return x=='\n' || x=='\r';}), data.end());
		_BrwConn.sendCommand( data );
		fs.close();
		_bIsDirty = false;
		return true;
	}
	else
		_BrwConn.sendCommand( string("div,idMain,Couldn't open file: ") + string("html/") + _file + ".div");

	return false;
}



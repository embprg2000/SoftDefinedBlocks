/*
 * widgetMenu.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETMENU_HPP_
#define WIDGETMENU_HPP_

#include "WidgetCommon.hpp"

class WidgetMenu: public WidgetCommon {
public:
	vector<string>   _menu;
	string           _file;
	int              _selected = 0;

	int getFile(const string file, string& data);
public:
	WidgetMenu(string divId, vector<string> items);
	WidgetMenu(WidgetMenu&& mv);

	void init() override;
	bool process(const string& msg) override;
	int  getSelected() override;

	bool file2client();

	string& getFileName() { return _file; }
};

#endif /* WIDGETMENU_HPP_ */

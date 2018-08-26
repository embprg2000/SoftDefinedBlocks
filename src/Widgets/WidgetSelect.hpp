/*
 * WidgetSelect.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETSELECT_HPP_
#define WIDGETSELECT_HPP_

#include "WidgetCommon.hpp"

class WidgetSelect : public WidgetCommon
{
	short			_sel;
	vector<string>  _selections;

public:
	WidgetSelect(string divId, int sel, vector<string> selections);
	virtual ~WidgetSelect();

	void init() override;
	bool process(const string& msg) override;
	int getSelected() override;
};


#endif /* WIDGETSELECT_HPP_ */

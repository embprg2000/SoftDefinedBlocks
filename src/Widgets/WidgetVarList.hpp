/*
 * WidgetVarList.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETVARLIST_HPP_
#define WIDGETVARLIST_HPP_

#include <list>

#include "WidgetCommon.hpp"

class WidgetVarList: public WidgetCommon {
public:
	string _clss;
	list<string>   _varList;
	vector<double> _currVals;

public:

	WidgetVarList(string divId, string clss, list<string> arr);
	virtual ~WidgetVarList();

	void init() override;
	bool process(const string& msg) override;
	int  getSelected() override;
	void setValues(vector<double> vals);
};

#endif /* WIDGETVARLIST_HPP_ */

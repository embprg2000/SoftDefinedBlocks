/*
 * WidgetRadio.h
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETRADIO_HPP_
#define WIDGETRADIO_HPP_

#include "WidgetCommon.hpp"

class WidgetRadio : public WidgetCommon
{
	short			_sel;
	vector<string>  _selections;

public:
	WidgetRadio(string divId, int sel, vector<string> selections);
	virtual ~WidgetRadio();

	void init() override;
	bool process(const string& msg) override;
	int getSelected() override;
};

#endif /* WIDGETRADIO_HPP_ */

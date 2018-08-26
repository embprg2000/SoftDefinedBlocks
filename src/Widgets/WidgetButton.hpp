/*
 * WidgetButton.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETBUTTON_HPP_
#define WIDGETBUTTON_HPP_

#include "WidgetCommon.hpp"

//  http://css3buttongenerator.com/
class WidgetButton : public WidgetCommon
{
	string			_text;
	atomic_bool		_bOn;
public:
	WidgetButton(string divId, string text, bool bOn = false);
	virtual ~WidgetButton();

	void init() override;
	bool process(const string& msg) override;
	int getSelected() override;
};


#endif /* WIDGETBUTTON_HPP_ */

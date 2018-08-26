/*
 * WidgetText.hpp
 *
 *  Created on: Feb 9, 2017
 *      Author: evert
 */

#ifndef WIDGETTEXT_HPP_
#define WIDGETTEXT_HPP_

#include "WidgetCommon.hpp"

class WidgetText  : public WidgetCommon{
	string _text;
	string _css;
public:
	WidgetText(string divId, string text, string css = "InfoText");
	virtual ~WidgetText();

	void init() override;
	bool process(const string& msg) override;
	int getSelected() override;

	void setText(string text);
};

#endif /* WIDGETTEXT_HPP_ */

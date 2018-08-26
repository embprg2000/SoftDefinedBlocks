/*
 * WidgetText.cpp
 *
 *  Created on: Feb 9, 2017
 *      Author: evert
 */

#include "WidgetText.hpp"


WidgetText::WidgetText(string divId, string text, string css)
	: WidgetCommon(divId)
	, _text(text)
	, _css(css) {}

WidgetText::~WidgetText() {}

void WidgetText::init()
{
	setText(_text);
}

bool WidgetText::process(const string& msg)
{
	return false;
}

int WidgetText::getSelected()
{
	return 0;
}


void WidgetText::setText(string text)
{
	ostringstream html;

	_text = text;
	html << "div,id" << _divId << ",";
	html << "<p class=\"" << _css << "\">" << _text << "</p>";
	_BrwConn.sendCommand(html.str());
}

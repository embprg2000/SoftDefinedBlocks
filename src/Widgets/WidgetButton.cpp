/*
 * WidgetButton.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetButton.hpp"

WidgetButton::WidgetButton(string divId, string text, bool bOn)
	: WidgetCommon(divId)
	, _text(text)
	, _bOn(bOn) {}

WidgetButton::~WidgetButton() {}

void WidgetButton::init()
{
	ostringstream html;

	html << "div,id" << _divId << ",";
	html << "<button id=\"butt" << _divId << "\" class=\"buttonPush\" onclick=\"onclick_button(this.id)\">";
	html << _text << "</button>";
	_BrwConn.sendCommand(html.str());
}

bool WidgetButton::process(const string& msg)
{
	if (msg.compare(0, 9, "BUTT,butt") == 0) {
		ostringstream html;

		if (msg.substr(9) == _divId)
		{
			_bOn = (_bOn == true) ? false : true;
			html << "BUTT,butt" << _divId << "," << _bOn << ",";
			_BrwConn.sendCommand(html.str());
			_bIsDirty = true;
			return true;
		}
	}
	return false;
}

int WidgetButton::getSelected()
{
	_bIsDirty = false;
	return _bOn;
}


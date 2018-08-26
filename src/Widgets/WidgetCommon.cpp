/*
 * WidgetCommon.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetCommon.hpp"

WidgetCommon::WidgetCommon(string divId)
	: _BrwConn(ClientConnect::getInst())
	, _divId(divId)
	, _bIsDirty(false)
	, _bIsActive(true) {}

WidgetCommon::~WidgetCommon() {
}


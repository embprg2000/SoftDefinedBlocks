/*
 * WidgetCommon.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETCOMMON_HPP_
#define WIDGETCOMMON_HPP_

#include <vector>
#include <atomic>
#include <sstream>
#include <algorithm>

#include "../Widgets/iWidget.hpp"
#include "../Server/ClientConnect.hpp"

class WidgetCommon: public iWidget {
public:

	ClientConnect&  _BrwConn;
	string			 _divId;
	atomic_bool      _bIsDirty;
	atomic_bool      _bIsActive;

	WidgetCommon(string divId);
	virtual ~WidgetCommon();

	bool isActive() override { return _bIsActive; }
	bool isDirty()  override { return _bIsDirty;  }

	const string& getDivId() { return _divId; }
	void  setActive(bool bActive) { _bIsActive = bActive; }
};

#endif /* WIDGETCOMMON_HPP_ */

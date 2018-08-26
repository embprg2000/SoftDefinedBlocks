/*
 * iWidget.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef IWIDGET_HPP_
#define IWIDGET_HPP_

#include <string>

using namespace std;

class iWidget {
public:
	iWidget();
	virtual ~iWidget();

	virtual void init() = 0;
	virtual bool process(const string& msg) = 0;

	// If division is not found in HTLM code the widget is deactivated.
	virtual bool isActive() = 0;
	virtual bool isDirty() = 0;
	virtual int  getSelected() = 0;

	virtual const string& getDivId() = 0;
	virtual void  setActive(bool bActive) = 0;
};

#endif /* IWIDGET_HPP_ */

/*
 * WidgetContainer.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETCONTAINER_HPP_
#define WIDGETCONTAINER_HPP_

#include "WidgetCommon.hpp"

class WidgetContainer: private WidgetCommon {
	WidgetContainer();
	vector<iWidget* > _widgets;
public:
	static WidgetContainer& getInst();
	virtual ~WidgetContainer();

	void init() override;
	bool process(const string& msg) override;
	int  getSelected() override; // No sense in calling this

	void add(iWidget* widget);
	void deactivate(string divId);

	bool   startServer(short port = 80);
	bool   isServerConnected();
	bool   isServerTerminated();
	string msgFromClient();

	void msecSleep(int msec);
	bool isSendBuffEmpty();
};

#endif /* WIDGETCONTAINER_HPP_ */

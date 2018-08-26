/*
 * WidgetContainer.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetContainer.hpp"

WidgetContainer::WidgetContainer() : WidgetCommon("Container") {
}

WidgetContainer::~WidgetContainer() {
}

WidgetContainer& WidgetContainer::getInst()
{
	static WidgetContainer obj;

	return obj;
}


void WidgetContainer::init()
{
	for(auto it : _widgets)
	{
		it->init();
		it->setActive(true);
	}
}

bool WidgetContainer::process(const string& msg)
{
	for(auto it : _widgets)
	{
		if (it->isActive() == true)
		{
			if (it->process(msg) == true)
				return true;
		}
	}
	return false;
}

void WidgetContainer::deactivate(string divId)
{
	for(auto it : _widgets)
	{
		if (it->getDivId() == divId)
			it->setActive( false );
	}
}

// No sense in calling this
int WidgetContainer::getSelected()
{
	return 0;
}

void WidgetContainer::add(iWidget* widget)
{
	_widgets.push_back(widget);
}

bool WidgetContainer::startServer(short port)
{
	_BrwConn.setPort(port);
	_BrwConn.start();
	return true;
}

bool WidgetContainer::isServerTerminated()
{
	return _BrwConn.isTerminated();
}

bool WidgetContainer::isServerConnected()
{
	while (_BrwConn.isRunning() == false)
		_BrwConn.msecSleep(100);

	//init();
	return true;
}

string WidgetContainer::msgFromClient()
{
	string msg = _BrwConn.msgFromClient();

	if (msg.substr(0,6) == "U_DIV,")
	{
		// If division was not found in HTML code deactivate widget.
		deactivate(msg.substr(6));
	}
	return msg;
}

void WidgetContainer::msecSleep(int msec)
{
	_BrwConn.msecSleep(msec);
}

bool WidgetContainer::isSendBuffEmpty()
{
	return _BrwConn.isSendBuffEmpty();
}

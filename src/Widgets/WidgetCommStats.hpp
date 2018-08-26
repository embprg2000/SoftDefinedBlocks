/*
 * WidgetCommStats.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETCOMMSTATS_HPP_
#define WIDGETCOMMSTATS_HPP_

#include "WidgetCommon.hpp"

class WidgetCommStats: public WidgetCommon {
public:
	WidgetCommStats(string divId);
	virtual ~WidgetCommStats();

	void init() override;
	bool process(const string& msg) override;
	int  getSelected() override;
};

#endif /* WIDGETCOMMSTATS_HPP_ */

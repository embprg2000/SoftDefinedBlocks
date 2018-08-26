/*
 * WidgetSlider.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETSLIDER_HPP_
#define WIDGETSLIDER_HPP_

#include "WidgetCommon.hpp"

class WidgetSlider : public WidgetCommon
{
	typedef struct {
		string name;
		string unit;
		double min;
		double max;
		double step;
		double def;
	} tSldFld;

	double			_value;
	vector<tSldFld>  _sliderTab;

public:
	WidgetSlider(string divId);
	virtual ~WidgetSlider();

	int addSlider(tSldFld slider);

	void init() override;

	bool process(const string& msg) override;
	int getSelected() override;

	double getValue(int idx);

};


#endif /* WIDGETSLIDER_HPP_ */

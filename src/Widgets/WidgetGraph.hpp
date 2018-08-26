/*
 * WidgetGraph.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#ifndef WIDGETGRAPH_HPP_
#define WIDGETGRAPH_HPP_

#include "WidgetCommon.hpp"

class WidgetGraph : public WidgetCommon
{
	typedef struct {
		double x1;
		double y1;
		double x2;
		double y2;
		short dpH;
		short dpV;
	} tAxis;

	short			_idx;
	short			_width;
	short			_height1;
	short			_height2;
	short			_height3;
	static	short   _idxCnt;
	tAxis*          _axis;

	void sendAxis();

public:
	WidgetGraph(string divId, short width, short height1 = -1, short height2 = -1, short height3 = -1);
	virtual ~WidgetGraph();

	void init() override;
	int  getSelected() override;
	bool process(const string& msg) override;

	bool axis(double x1, double y1, double x2, double y2, short dpH = 0, short dpV = 0);
	bool graphArr(string col, short width, vector<double> arr);
	bool graphArr(string col, short width, vector<float> arr);
};


#endif /* WIDGETGRAPH_HPP_ */

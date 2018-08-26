/*
 * WidgetGraph.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetGraph.hpp"

short WidgetGraph::_idxCnt = 0;

WidgetGraph::WidgetGraph(string divId, short width, short height1, short height2, short height3)
	: WidgetCommon(divId)
	, _idx(-1)
	, _width(width)
	, _height1(height1)
	, _height2(height2)
	, _height3(height3)
	, _axis(nullptr) {}

WidgetGraph::~WidgetGraph() {}

void WidgetGraph::sendAxis()
{
	if (_axis != nullptr)
	{
		ostringstream html;
		html << "GRP,AXIS," << _idx << "," << _axis->x1 << "," << _axis->y1 << ",";
		html << _axis->x2 << "," << _axis->y2 << "," << _axis->dpH << "," << _axis->dpV << ",";
		_BrwConn.sendCommand(html.str());
		delete _axis;
		_axis = nullptr;
	}
}

void WidgetGraph::init()
{
	ostringstream obj, dummy;
	ostringstream html;
	short totHeight = (_height2 != -1) ? _height1 + _height2 + _height3 : _height1;

	_idx = _idxCnt++;
	html << "div,id" << _divId << ",";
	html << "<canvas id=\"idCanvas" << _divId << "\" width=\"" << _width << "\" height=\"";
	html << totHeight << "\" style=\"border: 1px solid #000000;\"></canvas>";
	_BrwConn.sendCommand(html.str());

	html.str("");
	html.clear();

	html << "GRP,NEW,idCanvas" << _divId << "," << _idx << ",";
	if (_height2 != -1)
	{
		html << _height1 << "," << _height2 << "," << _height3 << ",";
	}
	_BrwConn.sendCommand(html.str());
	sendAxis();
}

bool WidgetGraph::process(const string& msg)
{
	_bIsDirty = false;
	return false;
}

int WidgetGraph::getSelected()
{
	return false;
}


bool WidgetGraph::axis(double x1, double y1, double x2, double y2, short dpH, short dpV)
{
	_axis = new tAxis;
	_axis->x1  = x1;
	_axis->y1  = y1;
	_axis->x2  = x2;
	_axis->y2  = y2;
	_axis->dpH = dpH;
	_axis->dpV = dpV;
	return true;
}

bool WidgetGraph::graphArr(string col, short width, vector<double> arr)
{
	ostringstream html;
	html << "GRP,ARR," << _idx << "," << col << "," << width;

	for(auto val : arr)
		html << "," << val;
	html << ",";
	_BrwConn.sendCommand(html.str());
	_bIsDirty = true;
	return true;
}

bool WidgetGraph::graphArr(string col, short width, vector<float> arr)
{
	ostringstream html;
	html << "GRP,ARR," << _idx << "," << col << "," << width;

	for(auto val : arr)
		html << "," << val;
	html << ",";
	_BrwConn.sendCommand(html.str());
	_bIsDirty = true;
	return true;
}



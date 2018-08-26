/*
 * WidgetSlider.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */
#include <math.h>
#include "WidgetSlider.hpp"

WidgetSlider::WidgetSlider(string divId)
	: WidgetCommon(divId)
	, _value(0) {}

WidgetSlider::~WidgetSlider() {}

int WidgetSlider::addSlider(tSldFld slider)
{
	_sliderTab.push_back( slider );
	return _sliderTab.size() - 1; // Return index.
}

void WidgetSlider::init()
{
	ostringstream obj, dummy;
	ostringstream html;

	html << "div,id" << _divId << ",";
	html << "<table class=\"slider\">";

	int i = 0;
	for (auto sld : _sliderTab)
	{
		obj << "slObj[" << i << "]";

		html << "<tr><td class=\"slider\">" << sld.name << ":</td>";
		html << "<td class=\"slider\">" << sld.min << " " << sld.unit << "</td>";
		html << "<td class=\"slider\"><input type=\"range\" id=\"id" << sld.name << "\" min=" << sld.min
				<< " max=" << sld.max << " step=" << sld.step << " value=" << sld.def
				<< " onclick=\"" << obj.str() << ".update('" << sld.name << "')\"></td>";
		html << "<td class=\"slider\">" << sld.max << " " << sld.unit << "&nbsp;</td>";
		html << "<td class=\"slider\"><button onclick=" << obj.str() << ".valInc(-1,\"" << sld.name << "\")>-</button></td>";
		html << "<td class=\"slider\"><button onclick=" << obj.str() << ".valInc(1,\"" << sld.name << "\")>+</button></td>";
		html << "<td class=\"slider\"><input id=\"idInp"  << sld.name << "\" size=\"6\" value=" << sld.def
				<< " onchange=" << obj.str() << ".valChange(\"" << sld.name << "\")></td></tr>";
		obj.str("");
		obj.clear();
	}
	html << "</table>";
	_BrwConn.sendCommand(html.str());
}

bool WidgetSlider::process(const string& msg)
{
	if (msg.compare(0, 7, "SLI,sli") == 0)
	{
		string tmp = msg.substr(7);
		ostringstream html;
		int idx = tmp.find_first_of(",");

		if (idx > 0)
		{
			string name = tmp.substr(0,idx);

			for (auto& sld : _sliderTab)
			{
				if (name == sld.name)
				{
					double num = 0.0;
					string fld = tmp.substr(idx+1);

					for_each(fld.begin(), fld.end(), [&num](char c) { if (isdigit(c)) { num *= 10; num += c - '0';} });
					idx = fld.find_last_of(".");
					if (idx > 0)
					{
						num /= pow(10, fld.size() - idx - 1);
					}
					if (sld.def != num)
					{
						sld.def = num;
						_bIsDirty = true;
					}
				}
			}
		}
	}
	return false;
}

int WidgetSlider::getSelected()
{
	return _value;
}

double WidgetSlider::getValue(int idx)
{
	_bIsDirty = false;
	return _sliderTab[idx].def;
}


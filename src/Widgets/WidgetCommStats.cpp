/*
 * WidgetCommStats.cpp
 *
 *  Created on: Feb 6, 2017
 *      Author: evert
 */

#include "WidgetCommStats.hpp"

WidgetCommStats::WidgetCommStats(string divId) : WidgetCommon(divId) {}
WidgetCommStats::~WidgetCommStats() {}

void WidgetCommStats::init()
{
	ostringstream html;

	html << "div," << _divId << ",";
	html << "<table border=\"0px\" style=\"width: 100px; border-radius: 10px; font-family: arial; font-size: 8pt; background-color: #818181; color: white;\">";
	html << "<tr>";
	html << "<td align=\"right\">Tx:</td>";
	html << "<td align=\"center\"><div id=\"connTx1\">0</div></td>";
	html << "<td align=\"center\"><div id=\"connTx2\">0</div></td>";
	html << "</tr>";
	html << "<tr>";
	html << "<td align=\"right\">Rx:</td>";
	html << "<td align=\"center\"><div id=\"connRx1\">0</div></td>";
	html << "<td align=\"center\"><div id=\"connRx2\">0</div></td>";
	html << "</tr>";
	html << "<tr>";
	html << "<td align=\"right\">St:</td>";
	html << "<td colspan=\"2\" align=\"center\"><div id=\"connSt\">connected</div></td>";
	html << "</tr>";
	html << "</table>";
	_BrwConn.sendCommand(html.str());
}

bool WidgetCommStats::process(const string& msg)
{
	ClientConnect::tStats cStats;

	_BrwConn.getCommstats( cStats );

	if (cStats.rx2 < 50)
	{
		ostringstream tmp;

		tmp << "VAL,connTx1," << cStats.tx1 << ",connTx2," << cStats.tx2 <<
				",connRx1," << cStats.rx1 << ",connRx2,"  << cStats.rx2 << ",";

		_BrwConn.sendCommand( tmp.str() );
	}
	return false;
}

int WidgetCommStats::getSelected()
{
	return 0;
}



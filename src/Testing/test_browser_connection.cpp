#include <math.h>
#include <iostream>
#include <complex>

#include "../Widgets/WidgetMenu.hpp"
#include "../Widgets/WidgetGraph.hpp"
#include "../Widgets/WidgetRadio.hpp"
#include "../Widgets/WidgetButton.hpp"
#include "../Widgets/WidgetSelect.hpp"
#include "../Widgets/WidgetSlider.hpp"
#include "../Widgets/WidgetVarList.hpp"
#include "../Widgets/WidgetContainer.hpp"
#include "../Widgets/WidgetCommStats.hpp"

#include "../Server/simpleFFT.hpp"
#include "../Server/ClientConnect.hpp"

using namespace std;

int main()
{
	WidgetContainer& widgets = WidgetContainer::getInst();

	widgets.add( new WidgetCommStats("idCommStats") );
	widgets.add( new WidgetMenu("idSideMenu", {"Home", "FM Radio", "ADM", "Morse", "Pager"}) );

	WidgetVarList varList("idVars", "clssVars", {"baseband", "IF", "Sample"} );
	widgets.add( &varList );

	WidgetButton autoButton("ButtAuto", "Auto");
	widgets.add( &autoButton );

	WidgetSelect sel("Sel", 2, {"10 dB", "20 dB", "40 dB", "60 dB", "80 dB"} );
	widgets.add( &sel );

	WidgetRadio radio("Rdo", 2, {"10 dB", "20 dB", "40 dB", "60 dB", "80 dB"} );
	widgets.add( &radio );

	WidgetSlider sliders("Sliders");
	int idxFreq = sliders.addSlider( {"Frequency", "MHz", 87, 109, 0.2, 107.5} );
	int idxVol  = sliders.addSlider( {"Volume",    "",    0, 100, 1, 10} );
	widgets.add( &sliders );

	WidgetGraph graph("FFTBase", 600, 150, 0, 200);
	graph.axis(0,0,128,150, 1,1);
	widgets.add( &graph );

	widgets.startServer(1234);

	while (widgets.isServerConnected() == false)
		;

	double val = 0.1;
	while (widgets.isServerTerminated() == false) {
		string msgFromBrowser = widgets.msgFromBrowser();

		if (widgets.process(msgFromBrowser) == true) {
			cout << "Msg:" << msgFromBrowser << endl;
		}
		else
		{
			widgets.msecSleep(500);

			val += 0.345;

			varList.setValues( { val, val+10, val+20 } );

			//cout << "Frequency:" << sliders.getValue(idxFreq) << endl;
			//cout << "Volume:"    << sliders.getValue(idxVol) << endl;

			vector<double> arr( pow(2,9) );

			for (unsigned int i=0; i<arr.size(); i += 2)
				arr[i] = sin(((double)(i + val)/arr.size()) * 100.0 * M_PI);

			double* ptr = arr.data();
			simpleFFT(ptr, arr.size()/2, -1);

			for (unsigned int i=0; i<arr.size()/4; ++i)
				ptr[i] = abs(complex<double>(ptr[i*2],ptr[i*2+1]));
			arr.resize(arr.size()/4);

			graph.graphArr("#F00000", 3, arr);
		}
	}
}

#include "SeqControl.hpp"
#include "ProcessScript.hpp"

//int main()
void the_main_function()  // To be started from the Testing directory.
{
	SeqControl& seqCtrl = SeqControl::getInstance();

	seqCtrl.run("FM_Radio");
}

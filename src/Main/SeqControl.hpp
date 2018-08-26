/*
 * SeqControl.hpp
 *
 *  Created on: Sep 17, 2016
 *      Author: evert
 */

#ifndef SEQCONTROL_HPP_
#define SEQCONTROL_HPP_

#include <map>
#include <vector>

#include "../Main/IBlock.hpp"
//#include "../Main/Variables.hpp"

#include "../Widgets/WidgetMenu.hpp"
#include "../Widgets/WidgetVarList.hpp"
#include "../Widgets/WidgetCommStats.hpp"
#include "../Widgets/WidgetContainer.hpp"


class SeqControl : public IBlock
{
public:
	virtual ~SeqControl();

	static SeqControl& getInstance();

	virtual Err init(vector<tVar>& args);
	virtual Err update();
	virtual Err start();
	virtual Err process();
	virtual Err stop();

	virtual Err setInputBlk(IBlock* ptr) { return Err::eOK; }

	virtual double getSampleRate() { return 1.0; } // Should never call this.

	virtual void getOutBuff(SampleVector*& vec)  { vec = 0; }
	virtual void getOutBuff(IQSampleVector*& vec) { vec = 0; }

	virtual string getBlkName() { return "SeqControl"; }

	void run(string file);

	bool    loadScript();
	IBlock* addBlock(string name, IBlock* blk);
	bool    removeBlock(string name);

	virtual IBlock* copyBlock()
	{
		SeqControl* obj = new SeqControl;
		//*obj = *this;
		return obj;
	}

private:

	double  m_sampleRate;
	double  m_sseTime;        // Time between updates.
	bool    m_scriptLoaded;
	WidgetContainer& m_widget;
	WidgetMenu*  	 m_menu;
	WidgetCommStats* m_commStats;
	WidgetVarList*   m_varList;
	vector<IBlock*>	 m_blkList;
	vector<IBlock*>::iterator m_itr;

	SeqControl();

	void deleteBlocks();

	/** Handle Ctrl-C and SIGTERM. */
	static void handle_sigterm(int sig);

//	IBlock* findBlock(string name);
//	double time2double();
};

#endif /* SEQCONTROL_HPP_ */

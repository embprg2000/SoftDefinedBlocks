/*
 * lowPassRC.hpp
 *
 *  Created on: Sep 19, 2016
 *      Author: evert
 */

#ifndef LOWPASSRC_HPP_
#define LOWPASSRC_HPP_

#include "../Main/IBlock.hpp"

class lowPassRC : public blkCommon<SampleVector,SampleVector>
{
public:
	lowPassRC();
	virtual ~lowPassRC() {}

	virtual Err init(vector<tVar>& args);

	virtual Err process();

	virtual std::string getBlkName() { return "lowPassRC"; }

	virtual IBlock* copyBlock()
	{
		lowPassRC* obj = new lowPassRC;
		*obj = *this;
		return obj;
	}

private:

    double  m_timeconst;
    SampleType  m_y1;
};

#endif /* LOWPASSRC_HPP_ */

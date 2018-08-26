
/*
 * DECODE_X10.hpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

// https://fccid.io/B4SKR19A#download
// https://fccid.io/pdf.php?id=20435
// https://ecfr.io/Title-47/pt47.1.15#sp47.1.15.c


#ifndef DECODE_X10_HPP_
#define DECODE_X10_HPP_

#include <map>

#include "IBlock.hpp"

class DecodeX10 : public blkSink<IQSampleVector>
{
public:
	DecodeX10(IBlock *blkIn);
	virtual ~DecodeX10() {}

	virtual Err init(vector<tVar>& args);

	virtual Err process();
	virtual Err stop();

	virtual std::string getBlkName() { return "DecodeX10"; }

private:
	double 			m_sum;
	int				m_idx;
	uint16_t		m_taps;
	uint16_t		m_sampleCnt;
	int16_t			m_dnSampleCnt;
	double			m_seq;
	bool			m_bStartMsg;
	double			m_x1;
	double			m_y1;
	double			m_alpha;
	double			m_max;
	bool			m_levelHigh;
	SampleVector 	m_vec;
	string			m_message;
	string			m_morse;
	vector<int>     m_vecTime;

	Err		calcTwoUnits();
	Err		decode();
};

#endif /* DECODE_X10_HPP_ */

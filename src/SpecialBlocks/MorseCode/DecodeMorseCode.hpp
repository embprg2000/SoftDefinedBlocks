/*
 * DECODE_MORSECODE.hpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

// http://www.arrl.org/40-wpm-code-archive
// https://coderwall.com/p/kotsja/generate-morse-code-wav-files-on-the-linux-cli

#ifndef DECODE_MORSECODE_HPP_
#define DECODE_MORSECODE_HPP_

#include <map>

#include "IBlock.hpp"

class DecodeMorseCode : public blkSink<SampleVector>
{
public:
	DecodeMorseCode(IBlock *blkIn);
	virtual ~DecodeMorseCode() {}

	virtual Err init(vector<tVar>& args);

	virtual Err process();
	virtual Err stop();

	virtual std::string getBlkName() { return "DecodeMorseCode"; }

private:
	double 			m_sum;
	int				m_idx;
	uint16_t		m_taps;
	uint16_t		m_sampleCnt;
	double			m_seq;
	int				m_twoUnits;
	double			m_x1;
	double			m_y1;
	double			m_alpha;
	double			m_max;
	bool			m_levelHigh;
	SampleVector 	m_vec;
	string			m_message;
	string			m_morse;
	vector<int>     m_vecTime;
	map<uint64_t,string> mCodes;

	Err		calcTwoUnits();
	Err		decode();
};

#endif /* DECODE_MORSECODE_HPP_ */

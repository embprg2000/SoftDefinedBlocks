/*
 * DecodeX10.cpp
 *
 *  Created on: Oct 26, 2016
 *      Author: evert
 */

/*
 * DecodeX10.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

// http://www.embedded.com/design/configurable-systems/4007653/DSP-Tricks-DC-Removal

#include <algorithm>    // std::sort
#include <string>

#include "DecodeX10.hpp"

template<class T>
inline short sign(T a) { return (a >= 0) ? 1 : -1; }

DecodeX10::DecodeX10(IBlock *blkIn)
	: blkSink(blkIn)
	, m_sum(0.0)
	, m_idx(0)
	, m_taps(6)
	, m_sampleCnt(0)
	, m_dnSampleCnt(0)
	, m_seq(0)
	, m_bStartMsg(false)
	, m_x1(0.0)
	, m_y1(0.0)
	, m_alpha(0.98)
	, m_max(0.0)
	, m_levelHigh(false)
{
}


FILE* fp_x10;
Err DecodeX10::init(vector<tVar>& args)
{
	m_sum   = 0.0;
	m_idx   = 0;
	m_taps  = 8;
	m_seq   = 0;
	m_x1    = 0.0;
	m_y1    = 0.0;
	m_alpha = 0.92;
	m_max   = 0.0;
	m_bStartMsg  = false;
	m_sampleCnt = 0;
	m_dnSampleCnt = -300;
	m_levelHigh   = false;

	m_vecTime.clear();
	m_vec.resize(m_taps);
	m_vec.clear();
	m_message = "";
	m_morse = "";
fp_x10 = fopen("test.bin","wb");
	return blkSink::init();
}


Err DecodeX10::process()
{
    for (auto it = m_in->begin() + 1; it != m_in->end(); ++it)
    {
    	// Remove DC and take absolute value.
    	double x = abs(*it);
    	double y = x - m_x1 + m_alpha * m_y1;
    	double mag = abs( y );

    	m_x1 = x;
    	m_y1 = y;

    	m_sum += mag;
    	m_vec[m_idx++] = mag;
    	if (m_idx >= m_taps)
    		m_idx = 0;
    	m_sum -= m_vec[m_idx];

    	//if (m_dnSampleCnt++ < 10)
    	//	continue;  // Let filter settle

    	m_dnSampleCnt = 0;

//    	double yy = m_sum; //y * m_taps;
//    	if (yy > m_max)
//    		m_max = yy;
//    	else
//    		m_max *= 0.99999;

    	m_max = 40 / 0.33;

    	double tmp = m_sum; fwrite((void *) &tmp, sizeof(double), 1, fp_x10);


    	m_sampleCnt++;

    	if (m_sum > (m_max * 0.33))
    	{
    		if (m_levelHigh == false)
    		{
    			if (m_bStartMsg == true)
    				m_vecTime.push_back(-m_sampleCnt);
    			m_levelHigh = true;
    			m_sampleCnt = 0;
    		}
    	}
    	else
    	{
    		if (m_levelHigh == true)
    		{
				if (m_sampleCnt > 50)
				{
					m_bStartMsg = true;
				}
				if (m_bStartMsg == true)
					m_vecTime.push_back(m_sampleCnt);
    			m_levelHigh = false;
    			m_sampleCnt = 0;
    		}
    	}
        if (m_vecTime.size() > 66)
        {
        	decode();
        	m_vecTime.clear();
        	m_bStartMsg = false;
        }
    }
	return Err::eOK;
}

Err DecodeX10::decode()
{
	uint8_t nShifts = 0;
	uint64_t code = 0;
	const float	per = 5.4;

	for (auto it=m_vecTime.begin() + 2; it != m_vecTime.end(); ++it)
	{
		int nPer = round((float) *it / per);

		while (nPer > 0)
		{
			code <<= 1;
			code |= 0x00000001;
			nPer--;
			nShifts++;
		}

		while (nPer < -2)
		{
			code <<= 1;
			nPer += 2;
			nShifts++;
		}
	}

	cout << hex << code << endl;

	return Err::eOK;
}

Err DecodeX10::stop()
{
	fclose(fp_x10);
	return Err::eOK;
}





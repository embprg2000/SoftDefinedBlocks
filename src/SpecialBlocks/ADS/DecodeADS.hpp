/*
 * DECODE_ADS.hpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

#ifndef DECODE_ADS_HPP_
#define DECODE_ADS_HPP_

#include "IBlock.hpp"

template <typename T, int NUM>
class bitVector
{
public:
	bitVector() : idx(0)
	{
		vec = vector<T>(NUM);
	}

	void push_back(T elt)
	{
		vec[ (idx++ + 120) & NUM ] = elt;
	}

	bool preamble()
	{
		bool levels[] = {1,0,1,0,0,0,0,1,0,1};
		for (unsigned short i=0; i<sizeof(levels); ++i)
		{
			T a = vec[(idx+i) & NUM];
			T b = vec[(idx+i+1) & NUM];
			T m = max(a,b);

			//if (abs(a-b) < (0.03 * m))
			//		return false;

			if ((a > b) != levels[i])
				return false;
		}
		return true;
	}

	vector<T> getElements(int n)
	{
		vector<T> ret;

		for (unsigned short i=0; i<n; ++i)
			ret.push_back( vec[(idx+i) & NUM] );
		return ret;
	}

	vector<bool> getBits(int n)
	{
		vector<bool> ret;

		short samplesPreamble = 16;
		for (unsigned short i=samplesPreamble; i<(n*2)+samplesPreamble; i += 2)
		{
			T a = vec[(idx+i) & NUM];
			T b = vec[(idx+i+1) & NUM];

			ret.push_back( a > b);
		}
		return ret;
	}

private:
	int idx;
	vector<T> vec;
};

class DecodeADS : public blkSink<IQSampleVector>
{
public:
	DecodeADS(IBlock *blkIn) : blkSink(blkIn) {}
	virtual ~DecodeADS() {}

	virtual Err process();

	virtual std::string getBlkName() { return "DecodeADS"; }

private:
	bitVector<float, 0xFF>  m_samples;
	uint32_t modesChecksum(vector<bool> bs, int bits);
	bool 	getBit(short idx, unsigned char in[]);
	unsigned long getBits(vector<bool>& msg, short b1, short b2);
	Err 	decodeVec(vector<bool>& bitsVec);

	void	test();
};

#endif /* DECODE_ADS_HPP_ */

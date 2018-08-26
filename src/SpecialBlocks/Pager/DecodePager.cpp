/*
 * DecodePager.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

#include "DecodePager.hpp"


Err DecodePager::process()
{
/*
	double threshold = 0.04;

	//cout << "Samples read:" << m_in->size() << endl;

	int cnt = 1;
	int err = 0;
	for (auto it : *m_in)
	{
		float mag = abs(it);

		m_samples.push_back( (mag > threshold) ? mag : 0.0 );

		if (m_samples.preamble() == true)
		{
			short frameLength = m_samples.getBits(1)[0] ? 112 : 56;
			vector<float> msg = m_samples.getElements(20);
			vector<bool> bits = m_samples.getBits(frameLength);

			//for (auto ll : msg)
			//	cout << ll << endl;

			if (cnt < 500)
			{
				uint32_t crc = modesChecksum(bits, frameLength);

				if (crc == 0)
				{
					for (auto ll : bits)
						cout << ll;

					cout << endl;
					decodeVec(bits);
					cout << endl;
				}
				else
					err++;
			}
			cnt++;
		}
	}
*/
	//cout << "Total: " << cnt << "  Errors: " << err <<  "   ratio:" << ((double) err / (double) cnt) << endl;
	return Err::eOK;
}

uint32_t DecodePager::modesChecksum(vector<bool> bs, int bits) // bs means bits stream :)
{
	return 1;
}

bool DecodePager::getBit(short idx, unsigned char in[])
{
	static unsigned char mask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	short b = (idx - 1) % 8;
	short j = (idx - 1) / 8;
	unsigned char c = in[j];

	return (c & mask[b]) > 0;
}

unsigned long DecodePager::getBits(vector<bool>& msg, short b1, short b2)
{
	unsigned long ret = 0;

	--b1;
	--b2;
	for (int i=b1, j=0; i<=b2; ++i, ++j)
	{
		ret <<= 1;
		if (msg[i] == true)
			ret |= 1;
	}
	return ret;
}


Err DecodePager::decodeVec(vector<bool>& bitMsg)
{

	return Err::eOK;
}



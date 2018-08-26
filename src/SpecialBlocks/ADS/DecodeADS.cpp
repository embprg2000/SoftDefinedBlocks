/*
 * DecodeADS.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 *
 * http://www.aerotransport.org/html/ICAO_hex_decode.html
 */

#include "DecodeADS.hpp"
#include <algorithm>    // std::reverse

/*
unsigned char tmsg0[] = {
        // Test data: first ADS-B message from testfiles/modes1.bin
        0x8f, 0x4d, 0x20, 0x23, 0x58, 0x7f, 0x34, 0x5e,
        0x35, 0x83, 0x7e, 0x22, 0x18, 0xb2
};

static unsigned char tmsg100[] = {0x8D, 0x48, 0x40, 0xD6, 0x20, 0x2C, 0xC3, 0x71, 0xC3, 0x2C, 0xE0, 0x57, 0x60, 0x98};
*/

static uint32_t modes_checksum_table[112] = {
0x3935ea, 0x1c9af5, 0xf1b77e, 0x78dbbf, 0xc397db, 0x9e31e9, 0xb0e2f0, 0x587178,
0x2c38bc, 0x161c5e, 0x0b0e2f, 0xfa7d13, 0x82c48d, 0xbe9842, 0x5f4c21, 0xd05c14,
0x682e0a, 0x341705, 0xe5f186, 0x72f8c3, 0xc68665, 0x9cb936, 0x4e5c9b, 0xd8d449,
0x939020, 0x49c810, 0x24e408, 0x127204, 0x093902, 0x049c81, 0xfdb444, 0x7eda22,
0x3f6d11, 0xe04c8c, 0x702646, 0x381323, 0xe3f395, 0x8e03ce, 0x4701e7, 0xdc7af7,
0x91c77f, 0xb719bb, 0xa476d9, 0xadc168, 0x56e0b4, 0x2b705a, 0x15b82d, 0xf52612,
0x7a9309, 0xc2b380, 0x6159c0, 0x30ace0, 0x185670, 0x0c2b38, 0x06159c, 0x030ace,
0x018567, 0xff38b7, 0x80665f, 0xbfc92b, 0xa01e91, 0xaff54c, 0x57faa6, 0x2bfd53,
0xea04ad, 0x8af852, 0x457c29, 0xdd4410, 0x6ea208, 0x375104, 0x1ba882, 0x0dd441,
0xf91024, 0x7c8812, 0x3e4409, 0xe0d800, 0x706c00, 0x383600, 0x1c1b00, 0x0e0d80,
0x0706c0, 0x038360, 0x01c1b0, 0x00e0d8, 0x00706c, 0x003836, 0x001c1b, 0xfff409,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};

Err DecodeADS::process()
{
	double threshold = 0.02;
	bitVector<float, 0x1FF>  samples;

	cout << "Samples read:" << m_in->size() << endl;

	int cnt = 0;
	int err = 0;
	for (auto it : *m_in)
	{
		float mag = abs(it);

		samples.push_back( (mag > threshold) ? mag : 0.0 );
		//samples.push_back( mag );

		if (samples.preamble() == true)
		{
			short frameLength = samples.getBits(1)[0] ? 112 : 56;
			vector<float> msg = samples.getElements(20);
			vector<bool> bits = samples.getBits(frameLength);

			uint32_t crc = modesChecksum(bits, frameLength);

			//if (crc == 0)
			if (1)
			{
				uint8_t map[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

				cout << "\n\r*";
				for (uint8_t k=0; k<frameLength; k += 4)
				{
					uint8_t nibble = bits[k] << 3 | bits[k+1] << 2 | bits[k+2] << 1 | bits[k+3];
					cout << map[nibble];
				}
				cout << ";";

				cout << endl;
				decodeVec(bits);
			}
			else
				err++;

			cnt++;
		}
	}
	cout << "Total: " << cnt << "  Errors: " << err <<  "   ratio:" << ((double) err / (double) cnt) << endl;
	return Err::eOK;
}

uint32_t DecodeADS::modesChecksum(vector<bool> bs, int bits) // bs means bits stream :)
{
    uint32_t   crc = 0;
    uint32_t   rem = 0;
    int        offset = (bits == 112) ? 0 : (112-56);
    uint32_t * pCRCTable = &modes_checksum_table[offset];
    int j;

    // We don't really need to include the checksum itself
    bits -= 24;
    for(j = 0; j < bits; j++)
    {
        // If bit is set, xor with corresponding table entry.
        if (bs[j])
        	crc ^= *pCRCTable;
        pCRCTable++;
    }

	for (int i=0; i<24; i++)
	{
		if (bs[bits+i] == true)
			rem += 1 << (23-i);
	}
    return ((crc ^ rem) & 0x00FFFFFF); // 24 bit checksum syndrome.
}

bool DecodeADS::getBit(short idx, unsigned char in[])
{
	static unsigned char mask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	short b = (idx - 1) % 8;
	short j = (idx - 1) / 8;
	unsigned char c = in[j];

	return (c & mask[b]) > 0;
}

unsigned long DecodeADS::getBits(vector<bool>& msg, short b1, short b2)
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
/*
Bit from	Bit to	Abbr.	   Name
   1	       5	  DF	   Downlink Format
   6	       8	  CA	   Message Subtype
   9	       32	  ICAO24   ICAO aircraft address
   33	       88	  DATA	   Data frame
   89	       112	  PC	   Parity check
*/
Err DecodeADS::decodeVec(vector<bool>& bitMsg)
{
	char map[] = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ#####_###############0123456789######";
	unsigned int  DF     = getBits(bitMsg, 1,5);
	unsigned int  CA     = getBits(bitMsg, 6,8);
	unsigned long ICA024 = getBits(bitMsg, 9,32);   // http://www.airframes.org/
	unsigned int  TC     = getBits(bitMsg, 33,37);
	//unsigned long DATA   = getBits(bitMsg, 33,88);
	//unsigned long PC     = getBits(bitMsg, 89,112);

	char aircraftID[10];
	//stringstream ss;
	//ss << std::hex << ICA024;

	//for (int i=1; i<100; ++i)
	//	cout << getBit(i);

	cout << "ICA=" << hex << ICA024 << dec << "  DF=" << DF << "   TC=" << TC << endl;

	if (DF == 17)
	{
		cout << "DF 17: ADS-B message." << endl;
		if (TC >= 1 && TC <= 4)
		{
			for (short i=0; i<8; ++i)
			{
				short b1  = 41+i*6;
				unsigned int idx = getBits(bitMsg, b1, b1+5);

				//cout << std::bitset<6>(idx) << " : " << idx << endl;
				aircraftID[i] = map[idx];
			}
			aircraftID[8] = 0;

			cout << "  aircraftID=" << aircraftID << endl;
		}

		if (TC >= 9 && TC <= 18)
		{
			//MSG bits | # bits | Abbr | Content
			//1-5	      5	       DF    Downlink format
			//33-37       5	       TC	 Type code
			//38-39       2        SS    Surveillance status
			//40          1       NICsb  NIC supplement-B
			//41-52       12       ALT   Altitude
			//53          1         T    Time
			//54          1         F    CPR odd/even frame flag
			//55-71       17     LAT-CPR Latitude in CPR format
			//72-88       17     LON-CPR Longitude in CPR format

			unsigned int SS      = getBits(bitMsg, 38,39);
			unsigned int NICsb   = getBits(bitMsg, 40,40);
			unsigned int ALT     = getBits(bitMsg, 41,52);
			bool T               = getBits(bitMsg, 53,53);
			bool F               = getBits(bitMsg, 54,54);
			unsigned int LAT_CPR = getBits(bitMsg, 55,71);
			unsigned int LON_CPR = getBits(bitMsg, 72,88);

			double LAT_CPR_EVEN;
			double LON_CPR_EVEN;
			double LAT_CPR_ODD;
			double LON_CPR_ODD;

			if (F == true)
			{
				LAT_CPR_EVEN = (double)LAT_CPR / 131072.0;
				LON_CPR_EVEN = (double)LON_CPR / 131072.0;
			}
			else
			{
				LAT_CPR_ODD = (double)LAT_CPR / 131072.0;
				LON_CPR_ODD = (double)LON_CPR / 131072.0;
			}

			//1100001 1 1000
			int mul = ((ALT & 0x00010) > 0) ? 25 : 100;
			int N = ((ALT & 0x0FE0) >> 1) | (ALT & 0x000F);
			cout << "  Altitude: " << (N * mul - 1000) << " (ft)" << endl;

			//int j = floor(59 * LAT_CPR_EVEN − 60 * LAT_CPR_ODD + 12);
		}
		if (TC >= 19)
		{
			/*
			MSG Bits  N bits	Abbr	Content
			33-37	   5        TC      Type code
			38-40	   3        ST      Subtype
			41	       1        IC      Intent change flag
			42	       1        RESV_A	Reserved-A
			43-45	   3        NAC     Velocity uncertainty (NAC)
			46	       1        S-WE	East-West velocity sign
			47-56	   10       V-WE	East-West velocity
			57	       1        S-NS	North-South velocity sign
			58-67      10       V-NS	North-South velocity
			68	       1        VrSrc	Vertical rate source
			69	       1        S-Vr	Vertical rate sign
			70-78      9        Vr      Vertical rate
			79-80      2        RESV_B	Reserved-B
			81         1        S-Dif	Diff from baro alt, sign
			82-88      7        Dif     Diff from baro alt
			*/

			unsigned short ST = getBits(bitMsg, 38,40);

			if (ST == 1 || ST == 2)
			{
				bool  bEW     =  getBits(bitMsg, 46,46);
				float vEW     = (getBits(bitMsg, 47,56) - 1) * (bEW ? -1.0 : 1.0);
				bool  bNS     =  getBits(bitMsg, 57,57);
				float vNS     = (getBits(bitMsg, 58,67) - 1) * (bNS ? -1.0 : 1.0);
				bool  bDN     =  getBits(bitMsg, 69,69);
				bool  vDN     =  getBits(bitMsg, 70,78);
				float heading = atan(vEW / vNS) * (360 / 2 * M_PI);

				if (heading < 0)
					heading += 360;
				cout << "    EW velocity: " << vEW << " (kn)  -  "<< (bEW ? "Flying East to West" : "Flying West to East") << endl;
				cout << "    NS velocity: " << vNS << " (kn)  -  "<< (bNS ? "Flying North to South" : "Flying South to North") << endl;
				cout << "       velocity: " << sqrt(vEW * vEW + vNS * vNS) << " (kn) -  heading:" << heading << " deg." << endl;
				cout << "       Vertical: " << vDN << " (ft/min)  " << (bDN ? "Down" : "UP") << endl;
			}
		}
	}
	return Err::eOK;
}


#ifdef __KUTJE__
/*
unsigned char tmsg0[] = {
        // Test data: first ADS-B message from testfiles/modes1.bin
        0x8f, 0x4d, 0x20, 0x23, 0x58, 0x7f, 0x34, 0x5e,
        0x35, 0x83, 0x7e, 0x22, 0x18, 0xb2
};

static unsigned char tmsg100[] = {0x8D, 0x48, 0x40, 0xD6, 0x20, 0x2C, 0xC3, 0x71, 0xC3, 0x2C, 0xE0, 0x57, 0x60, 0x98};
*/

static uint32_t modes_checksum_table[112] = {
	0x3935ea, 0x1c9af5, 0xf1b77e, 0x78dbbf, 0xc397db, 0x9e31e9, 0xb0e2f0, 0x587178,
	0x2c38bc, 0x161c5e, 0x0b0e2f, 0xfa7d13, 0x82c48d, 0xbe9842, 0x5f4c21, 0xd05c14,
	0x682e0a, 0x341705, 0xe5f186, 0x72f8c3, 0xc68665, 0x9cb936, 0x4e5c9b, 0xd8d449,
	0x939020, 0x49c810, 0x24e408, 0x127204, 0x093902, 0x049c81, 0xfdb444, 0x7eda22,
	0x3f6d11, 0xe04c8c, 0x702646, 0x381323, 0xe3f395, 0x8e03ce, 0x4701e7, 0xdc7af7,
	0x91c77f, 0xb719bb, 0xa476d9, 0xadc168, 0x56e0b4, 0x2b705a, 0x15b82d, 0xf52612,
	0x7a9309, 0xc2b380, 0x6159c0, 0x30ace0, 0x185670, 0x0c2b38, 0x06159c, 0x030ace,
	0x018567, 0xff38b7, 0x80665f, 0xbfc92b, 0xa01e91, 0xaff54c, 0x57faa6, 0x2bfd53,
	0xea04ad, 0x8af852, 0x457c29, 0xdd4410, 0x6ea208, 0x375104, 0x1ba882, 0x0dd441,
	0xf91024, 0x7c8812, 0x3e4409, 0xe0d800, 0x706c00, 0x383600, 0x1c1b00, 0x0e0d80,
	0x0706c0, 0x038360, 0x01c1b0, 0x00e0d8, 0x00706c, 0x003836, 0x001c1b, 0xfff409,
	0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
	0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
	0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};


void DecodeADS::test()
{
	uint8_t tmsg0[] = {
	        // Test data: first ADS-B message from testfiles/modes1.bin
	        0x8f, 0x4d, 0x20, 0x23, 0x58, 0x7f, 0x34, 0x5e,
	        0x35, 0x83, 0x7e, 0x22, 0x18, 0xb2
	};
	bool preamble[] = {1,0,1,0,0,0,0,1,0,1,0,0,0,0,0,0};

	m_in->clear();

	for (uint8_t i=0; i<sizeof(preamble)/sizeof(bool); ++i)
	{
		m_in->push_back( IQSample(preamble[i] ? 0.1 : 0.0, 0.0) );
	}

	for (uint8_t i=0; i<sizeof(tmsg0)/sizeof(uint8_t); ++i)
	{
		uint8_t ch = tmsg0[i];
		for (uint8_t j=0; j<8; ++j)
		{
			if ((ch & 0x80) > 0)
				m_in->push_back( IQSample(0.1, 0.0) );
			else
				m_in->push_back( IQSample(0.0, 0.0) );
			m_in->push_back( IQSample(0.0, 0.0) );
			ch <<= 1;
		}
	}
}

#define preamble_len		16

short quality = 0;
#define BADSAMPLE 65535

inline uint16_t single_manchester(uint16_t a, uint16_t b, uint16_t c, uint16_t d)
/* takes 4 consecutive real samples, return 0 or 1, BADSAMPLE on error */
{
	int bit, bit_p;
	bit_p = a > b;
	bit   = c > d;

	if (quality == 0) {
		return bit;}

	if (quality == 5) {
		if ( bit &&  bit_p && b > c) {
			return BADSAMPLE;}
		if (!bit && !bit_p && b < c) {
			return BADSAMPLE;}
		return bit;
	}

	if (quality == 10) {
		if ( bit &&  bit_p && c > b) {
			return 1;}
		if ( bit && !bit_p && d < b) {
			return 1;}
		if (!bit &&  bit_p && d > b) {
			return 0;}
		if (!bit && !bit_p && c < b) {
			return 0;}
		return BADSAMPLE;
	}

	if ( bit &&  bit_p && c > b && d < a) {
		return 1;}
	if ( bit && !bit_p && c > a && d < b) {
		return 1;}
	if (!bit &&  bit_p && c < a && d > b) {
		return 0;}
	if (!bit && !bit_p && c < b && d > a) {
		return 0;}
	return BADSAMPLE;
}

inline int preamble(uint16_t *buf, int i)
/* returns 0/1 for preamble at index i */
{
	int i2;
	uint16_t low  = 0;
	uint16_t high = 65535;
	for (i2=0; i2<preamble_len; i2++)
	{
		switch (i2) {
			case 0:
			case 2:
			case 7:
			case 9:
				//high = min16(high, buf[i+i2]);
				high = buf[i+i2];
				break;
			default:
				//low  = max16(low,  buf[i+i2]);
				low = buf[i+i2];
				break;
		}
		if (high <= low) {
			return 0;}
	}
	return 1;
}

Err DecodeADS::process()
{
	uint16_t buf[10000];
	//uint8_t  bits[200];
	uint16_t cnt = 0;

	//test();

	for (auto it : *m_in)
	{
		float mag = abs(it) * 1e2;

		buf[cnt++] = mag;
	}
	uint8_t	frameLength = 112;
	vector<bool> bits;
	bool bFirst = true;
	uint8_t bCnt = 0;
	uint16_t a, b = 0;
	uint16_t len = m_in->size();
	uint16_t i=0;
	while (i < len)
	{
		if (bFirst == true)
		{
			if (!preamble(buf, i))
			{
				++i;
				continue;
			}

			a = buf[i];
			b = buf[i+1];
			bCnt = 0;
			bits.clear();
			bFirst = false;
			i += preamble_len;

			uint16_t tmp = single_manchester(a, b, buf[i], buf[i+1]);
			if (tmp == BADSAMPLE)
			{
				bFirst = true;
				continue;
			}
			frameLength = tmp > 0 ? 112 : 56;
			bits.push_back(tmp);
			++bCnt;
			i += 2;
		}
		else
		{
			uint16_t tmp = single_manchester(a, b, buf[i], buf[i+1]);
			if (tmp == BADSAMPLE)
			{
				bFirst = true;
				continue;
			}
			bits.push_back(tmp);
			++bCnt;
			a = buf[i];
			b = buf[i+1];

			if (bCnt == frameLength)
			{
				uint32_t crc = modesChecksum(bits, frameLength);

				//if (crc == 0)
				{
					uint8_t map[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

					cout << "\n\r*";
					for (uint8_t k=0; k<112; k += 4)
					{
						uint8_t nibble = bits[k] << 3 | bits[k+1] << 2 | bits[k+2] << 1 | bits[k+3];
						cout << map[nibble];
					}
					cout << ";";

					decodeVec(bits);
				}

				bFirst = true;
				continue;
			}
			i += 2;
		}
	}


	return Err::eOK;
}

#ifdef __NONO__
Err DecodeADS::process()
{
	double threshold = 0.04;

	//cout << "Samples read:" << m_in->size() << endl;

	test();

	int cnt = 0;
	int err = 0;
	for (auto it : *m_in)
	{
		float mag = abs(it);

		cout << mag << endl;

		cout << "=======================================" << endl;

		m_samples.push_back( (mag > threshold) ? mag : 0.0 );

		if (cnt == 0)
		{
			if (m_samples.preamble() == true)
				++cnt;
		}
		else
		{
			++cnt;
		}

		if (cnt == 56)
		{
			short frameLength = m_samples.getBits(1)[0] ? 112 : 56;
			vector<float> msg = m_samples.getElements(20);
			vector<bool> bits = m_samples.getBits(frameLength);

			for (auto ll : msg)
				cout << ll << endl;

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
	//cout << "Total: " << cnt << "  Errors: " << err <<  "   ratio:" << ((double) err / (double) cnt) << endl;
	return Err::eOK;
}
#endif

uint32_t DecodeADS::modesChecksum(vector<bool> bs, int bits) // bs means bits stream :)
{
    uint32_t   crc = 0;
    uint32_t   rem = 0;
    int        offset = (bits == 112) ? 0 : (112-56);
    uint32_t * pCRCTable = &modes_checksum_table[offset];
    int j;

    // We don't really need to include the checksum itself
    bits -= 24;
    for(j = 0; j < bits; j++)
    {
        // If bit is set, xor with corresponding table entry.
        if (bs[j])
        	crc ^= *pCRCTable;
        pCRCTable++;
    }

	for (int i=0; i<24; i++)
	{
		if (bs[bits+i] == true)
			rem += 1 << (23-i);
	}
    return ((crc ^ rem) & 0x00FFFFFF); // 24 bit checksum syndrome.
}

bool DecodeADS::getBit(short idx, unsigned char in[])
{
	static unsigned char mask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	short b = (idx - 1) % 8;
	short j = (idx - 1) / 8;
	unsigned char c = in[j];

	return (c & mask[b]) > 0;
}

unsigned long DecodeADS::getBits(vector<bool>& msg, short b1, short b2)
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
/*
Bit from	Bit to	Abbr.	   Name
   1	       5	  DF	   Downlink Format
   6	       8	  CA	   Message Subtype
   9	       32	  ICAO24   ICAO aircraft address
   33	       88	  DATA	   Data frame
   89	       112	  PC	   Parity check
*/
Err DecodeADS::decodeVec(vector<bool>& bitMsg)
{
	char map[] = "#ABCDEFGHIJKLMNOPQRSTUVWXYZ#####_###############0123456789######";
	unsigned int  DF     = getBits(bitMsg, 1,5);
	unsigned int  CA     = getBits(bitMsg, 6,8);
	unsigned long ICA024 = getBits(bitMsg, 9,32);   // http://www.airframes.org/
	unsigned int  TC     = getBits(bitMsg, 33,37);
	//unsigned long DATA   = getBits(bitMsg, 33,88);
	//unsigned long PC     = getBits(bitMsg, 89,112);

	char aircraftID[10];
	//stringstream ss;
	//ss << std::hex << ICA024;

	//for (int i=1; i<100; ++i)
	//	cout << getBit(i);

	cout << "ICA=" << hex << ICA024 << dec << "  DF=" << DF << "   CA=" << CA << "   TC=" << TC << endl;

	if (DF == 17)
	{
		cout << "DF 17: ADS-B message." << endl;
		if (TC >= 1 && TC <= 4)
		{
			for (short i=0; i<8; ++i)
			{
				short b1  = 41+i*6;
				unsigned int idx = getBits(bitMsg, b1, b1+5);

				//cout << std::bitset<6>(idx) << " : " << idx << endl;
				aircraftID[i] = map[idx];
			}
			aircraftID[8] = 0;

			cout << "  aircraftID=" << aircraftID << endl;
		}

		if (TC >= 9 && TC <= 18)
		{
			//MSG bits | # bits | Abbr | Content
			//1-5	      5	       DF    Downlink format
			//33-37       5	       TC	 Type code
			//38-39       2        SS    Surveillance status
			//40          1       NICsb  NIC supplement-B
			//41-52       12       ALT   Altitude
			//53          1         T    Time
			//54          1         F    CPR odd/even frame flag
			//55-71       17     LAT-CPR Latitude in CPR format
			//72-88       17     LON-CPR Longitude in CPR format

			unsigned int SS      = getBits(bitMsg, 38,39);
			unsigned int NICsb   = getBits(bitMsg, 40,40);
			unsigned int ALT     = getBits(bitMsg, 41,52);
			bool T               = getBits(bitMsg, 53,53);
			bool F               = getBits(bitMsg, 54,54);
			unsigned int LAT_CPR = getBits(bitMsg, 55,71);
			unsigned int LON_CPR = getBits(bitMsg, 72,88);

			double LAT_CPR_EVEN;
			double LON_CPR_EVEN;
			double LAT_CPR_ODD;
			double LON_CPR_ODD;

			if (F == true)
			{
				LAT_CPR_EVEN = (double)LAT_CPR / 131072.0;
				LON_CPR_EVEN = (double)LON_CPR / 131072.0;
			}
			else
			{
				LAT_CPR_ODD = (double)LAT_CPR / 131072.0;
				LON_CPR_ODD = (double)LON_CPR / 131072.0;
			}

			//1100001 1 1000
			int mul = ((ALT & 0x00010) > 0) ? 25 : 100;
			int N = ((ALT & 0x0FE0) >> 1) | (ALT & 0x000F);
			cout << "  Altitude: " << (N * mul - 1000) << " (ft)" << endl;

			//int j = floor(59 * LAT_CPR_EVEN − 60 * LAT_CPR_ODD + 12);
		}
		if (TC >= 19)
		{
			/*
			MSG Bits  N bits	Abbr	Content
			33-37	   5        TC      Type code
			38-40	   3        ST      Subtype
			41	       1        IC      Intent change flag
			42	       1        RESV_A	Reserved-A
			43-45	   3        NAC     Velocity uncertainty (NAC)
			46	       1        S-WE	East-West velocity sign
			47-56	   10       V-WE	East-West velocity
			57	       1        S-NS	North-South velocity sign
			58-67      10       V-NS	North-South velocity
			68	       1        VrSrc	Vertical rate source
			69	       1        S-Vr	Vertical rate sign
			70-78      9        Vr      Vertical rate
			79-80      2        RESV_B	Reserved-B
			81         1        S-Dif	Diff from baro alt, sign
			82-88      7        Dif     Diff from baro alt
			*/

			unsigned short ST = getBits(bitMsg, 38,40);

			if (ST == 1 || ST == 2)
			{
				bool  bEW     =  getBits(bitMsg, 46,46);
				float vEW     = (getBits(bitMsg, 47,56) - 1) * (bEW ? -1.0 : 1.0);
				bool  bNS     =  getBits(bitMsg, 57,57);
				float vNS     = (getBits(bitMsg, 58,67) - 1) * (bNS ? -1.0 : 1.0);
				bool  bDN     =  getBits(bitMsg, 69,69);
				bool  vDN     =  getBits(bitMsg, 70,78);
				float heading = atan(vEW / vNS) * (360 / 2 * M_PI);

				if (heading < 0)
					heading += 360;
				cout << "    EW velocity: " << vEW << " (kn)  -  "<< (bEW ? "Flying East to West" : "Flying West to East") << endl;
				cout << "    NS velocity: " << vNS << " (kn)  -  "<< (bNS ? "Flying North to South" : "Flying South to North") << endl;
				cout << "       velocity: " << sqrt(vEW * vEW + vNS * vNS) << " (kn) -  heading:" << heading << " deg." << endl;
				cout << "       Vertical: " << vDN << " (ft/min)  " << (bDN ? "Down" : "UP") << endl;
			}
		}
	}
	return Err::eOK;
}
#endif


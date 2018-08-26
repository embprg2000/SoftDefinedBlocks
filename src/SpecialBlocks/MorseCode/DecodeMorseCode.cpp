/*
 * DecodeMorseCode.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: evert
 */

// http://www.embedded.com/design/configurable-systems/4007653/DSP-Tricks-DC-Removal

#include <algorithm>    // std::sort
#include <string>

#include "DecodeMorseCode.hpp"

ostream* sTmp;

template<class T>
inline short sign(T a) { return (a >= 0) ? 1 : -1; }

DecodeMorseCode::DecodeMorseCode(IBlock *blkIn)
	: blkSink(blkIn)
	, m_sum(0.0)
	, m_idx(0)
	, m_taps(1024)
	, m_sampleCnt(0)
	, m_seq(0)
	, m_twoUnits(0)
	, m_x1(0.0)
	, m_y1(0.0)
	, m_alpha(0.98)
	, m_max(0.0)
	, m_levelHigh(false)
{
    // DITs are 0b01 and DAHs are 0b10
    // List is sorted for binary search
    // HexValue       Character     BinValue                 MORSE/CW
    mCodes[0x0000] =          "<NULL>";  // 0b0
    mCodes[0x0001] =          "E";       // 0b01                     .
    mCodes[0x0002] =          "T";       // 0b10                     -
    mCodes[0x0005] =          "I";       // 0b0101                   ..
    mCodes[0x0006] =          "A";       // 0b0110                   .-
    mCodes[0x0009] =          "N";       // 0b1001                   -.
    mCodes[0x000A] =          "M";       // 0b1010                   --
    mCodes[0x0015] =          "S";       // 0b010101                 ...
    mCodes[0x0016] =          "U";       // 0b010110                 ..-
    mCodes[0x0019] =          "R";       // 0b011001                 .-.
    mCodes[0x001A] =          "W";       // 0b011010                 .--
    mCodes[0x0025] =          "D";       // 0b100101                 -..
    mCodes[0x0026] =          "K";       // 0b100110                 -.-
    mCodes[0x0029] =          "G";       // 0b101001                 --.
    mCodes[0x002A] =          "O";       // 0b101010                 ---
    mCodes[0x0055] =          "H";       // 0b01010101               ....
    mCodes[0x0056] =          "V";       // 0b01010110               ...-
    mCodes[0x0059] =          "F";       // 0b01011001               ..-.
    mCodes[0x005A] =          "Ü";       // 0b01011010               ..-- wrong? *?
    mCodes[0x0065] =          "L";       // 0b01100101               .-..
    mCodes[0x0066] =          "Ä";       // 0b01100110               .-.-
    mCodes[0x0069] =          "P";       // 0b01101001               .--.
    mCodes[0x006A] =          "J";       // 0b01101010               .---
    mCodes[0x0095] =          "B";       // 0b10010101               -...
    mCodes[0x0096] =          "X";       // 0b10010110               -..-
    mCodes[0x0099] =          "C";       // 0b10011001               -.-.
    mCodes[0x009A] =          "Y";       // 0b10011010               -.--
    mCodes[0x00A5] =          "Z";       // 0b10100101               --..
    mCodes[0x00A6] =          "Q";       // 0b10100110               --.-
    mCodes[0x00A9] =          "Ö";       // 0b10101001               ---.
    mCodes[0x00AA] =          "CH";      // 0b10101010               ----
    mCodes[0x0155] =          "5";       // 0b0101010101             .....
    mCodes[0x0156] =          "4";       // 0b0101010110             ....-
    mCodes[0x0159] =          "<SN>";    // 0b0101011001             ...-.
    mCodes[0x015A] =          "3";       // 0b0101011010             ...--
    mCodes[0x0166] =          "/";       // 0b0101100110             ..-.-
    mCodes[0x016A] =          "2";       // 0b0101101010             ..---
    mCodes[0x0195] =          "&";       // 0b0110010101             .-...
    mCodes[0x0199] =          "+";       // 0b0110011001             .-.-.
    mCodes[0x01AA] =          "1";       // 0b0110101010             .----
    mCodes[0x0255] =          "6";       // 0b1001010101             -....
    mCodes[0x0256] =          "=";       // 0b1001010110             -...-
    mCodes[0x0266] =          "<CT>";    // 0b1001100110             -.-.-
    mCodes[0x0259] =          "/";       // 0b1001011001             -..-. wrong?
    mCodes[0x0269] =          "(";       // 0b1001101001             -.--.
    mCodes[0x0295] =          "7";       // 0b1010010101             --...
    mCodes[0x02A5] =          "8";       // 0b1010100101             ---..
    mCodes[0x02A9] =          "9";       // 0b1010101001             ----.
    mCodes[0x02AA] =          "0";       // 0b1010101010             -----
    mCodes[0x0555] =          "<ERR_6>"; // 0b010101010101           ......
    mCodes[0x0566] =          "<SK>";    // 0b010101100110           ...-.-
    mCodes[0x05A5] =          "?";       // 0b010110100101           ..--..
    mCodes[0x05A6] =          "_";       // 0b010110100110           ..--.-
    mCodes[0x0659] =          "\"";      // 0b011001011001           .-..-.
    mCodes[0x0666] =          ".";       // 0b011001100110           .-.-.-
    mCodes[0x0699] =          "@";       // 0b011010011001           .--.-.
    mCodes[0x06A9] =          "'";       // 0b011010101001           .----.
    mCodes[0x0956] =          "-";       // 0b100101010110           -....-
    mCodes[0x096A] =          "<DO>";    // 0b100101101010           -..---
    mCodes[0x0999] =          ";";       // 0b100110011001           -.-.-.
    mCodes[0x099A] =          "!";       // 0b100110011010           -.-.--
    mCodes[0x09A6] =          ")";       // 0b100110100110           -.--.-
    mCodes[0x0A5A] =          ",";       // 0b101001011010           --..--
    mCodes[0x0A95] =          ":";       // 0b101010010101           ---...
    mCodes[0x1555] =          "<ERR_7>"; // 0b01010101010101         .......
    mCodes[0x1596] =          "$";       // 0b01010110010110         ...-..-
    mCodes[0x2566] =          "<BK>";    // 0b10010101100110         -...-.-
    mCodes[0x5555] =          "<ERR_8>"; // 0b0101010101010101       ........
    mCodes[0x9965] =          "<CL>";    // 0b1001100101100101       -.-..-..
    mCodes[0x15555] =         "<ERR_9>"; // 0b010101010101010101     .........
    mCodes[0x15A95] =         "<SOS>";   // 0b010101101010010101     ...---...
    mCodes[0x55555] =         "<ERR_10>";// 0b01010101010101010101   ..........
    mCodes[0x155555] =        "<ERR_11>";// 0b0101010101010101010101 ...........
    mCodes[0x555555] =        "<ERR_12>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x1555555] =       "<ERR_13>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x5555555] =       "<ERR_14>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x15555555] =      "<ERR_15>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x55555555] =      "<ERR_16>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x155555555] =     "<ERR_17>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x555555555] =     "<ERR_18>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x1555555555] =    "<ERR_19>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x5555555555] =    "<ERR_20>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x15555555555] =   "<ERR_21>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x55555555555] =   "<ERR_22>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x155555555555] =     "<ERR_23>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x555555555555] =     "<ERR_24>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x1555555555555] =    "<ERR_25>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x5555555555555] =    "<ERR_26>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x15555555555555] =   "<ERR_27>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x55555555555555] =   "<ERR_28>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x155555555555555] =  "<ERR_29>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x555555555555555] =  "<ERR_30>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x1555555555555555] = "<ERR_31>";// 0b0101010101010101(...)  ......(...)
    mCodes[0x5555555555555555] = "<ERR_32>";// 0b0101010101010101(...)  ......(...)
    // Sequences longer than 32 DITs will overflow and show up as <ERR_32> as well.

    sTmp = &cout;
}


int aa = 0;
FILE* fp;
Err DecodeMorseCode::init(vector<tVar>& args)
{
	m_sum   = 0.0;
	m_idx   = 0;
	m_taps  = 1024;
	m_seq   = 0;
	m_x1    = 0.0;
	m_y1    = 0.0;
	m_alpha = 0.98;
	m_max   = 0.0;
	m_twoUnits  = 0;
	m_sampleCnt = 0;
	m_levelHigh = false;

	m_vecTime.clear();
	m_vec.resize(m_taps);
	m_vec.clear();
	m_message = "";
	m_morse = "";
fp = fopen("test.bin","wb"); aa = 0;
	return blkSink::init();
}


Err DecodeMorseCode::process()
{
    for (auto it = m_in->begin() + 1; it != m_in->end(); ++it)
    {
    	// Remove DC and take absolute value.
    	double x = *it;
    	double y = x - m_x1 + m_alpha * m_y1;
    	double mag = abs( y );

    	m_x1 = x;
    	m_y1 = y;

    	m_sum += mag;
    	m_vec[m_idx++] = mag;
    	if (m_idx >= m_taps)
    		m_idx = 0;
    	m_sum -= m_vec[m_idx];

    	double yy = m_sum; //y * m_taps;
    	if (yy > m_max)
    		m_max = yy;
    	else
    		m_max *= 0.99999;

    	double tmp = m_max; fwrite((void *) &tmp, sizeof(double), 1, fp);

    	aa++;
    	m_sampleCnt++;

    	if (m_sum > (m_max * 0.33))
    	{
    		if (m_levelHigh == false)
    		{
    			m_vecTime.push_back(-m_sampleCnt);
    			m_levelHigh = true;
    			m_sampleCnt = 0;
    		}
    	}
    	else
    	{
    		if (m_levelHigh == true)
    		{
    			m_vecTime.push_back(m_sampleCnt);
    			m_levelHigh = false;
    			m_sampleCnt = 0;
    		}
    	}
    }

    if (m_vecTime.size() > 60)
    {
    	Err err = calcTwoUnits();
    	if (err == false)
    		decode();
    }
    else if (bTerminate.load() == true)
    {
    	if (m_vecTime.size() > 10)
    		calcTwoUnits();

    	if (m_twoUnits != 0)
    		decode();
    }

    if (bTerminate.load() == true)
    {
    	cout << "Message:" << endl;
    	cout << m_message << endl;
    	cout << m_morse << endl << endl;
    }
	return Err::eOK;
}

Err DecodeMorseCode::calcTwoUnits()
{
//	vector<int> timeSort = m_vecTime;
//
//	for_each(timeSort.begin(), timeSort.end(), [](int &n){ n = abs(n); });
//
//	sort(timeSort.begin(), timeSort.end());
//
//	int min  = 800; //timeSort[2];
//	int hMin = min / 2;
//	//int max = timeSort[timeSort.size()-1];
//
//	int sum = 0;
//	for (auto it=timeSort.begin()+1; it != timeSort.end(); ++it)
//	{
//		int val  = *it;
//		int div  = val/min;
//		int diff = val - (val + hMin) / min * min;
//
//		sum += diff;
//		cout << "############################  " << val << " ** " << div << " *** " << diff << endl;
//	}

	vector<int> highTime;

	for (auto it = m_vecTime.begin()+2; it != m_vecTime.end()-2; ++it)
	{
		int val = *it;

		if (val > 0)
			highTime.push_back( val );
	}

	sort(highTime.begin(), highTime.end());

	int sumH = 0;
	int cntH = 0;
	int sumL = 0;
	int cntL = 0;
	int min  = *(highTime.begin());
	int max  = *(highTime.end() - 1);
	for (auto it = highTime.begin(); it < highTime.end(); ++it)
	{
		int val = *it;

		if (abs(val - max) > abs(val - min))
		{
			sumL += val;
			cntL++;
		}
		else
		{
			sumH += val;
			cntH++;
		}
	}
	int meanL = round((double)sumL / (double) cntL);
	int meanH = round((double)sumH / (double) cntH);

	// meanL times three should be about meanH
	double diff = ((double)meanL * 3.0 - (double)meanH) / ((double) meanH);

	if (abs(diff) > 0.30)
		return Err::eErrDataProcessError;

	//m_twoUnits = meanL + round((double)(meanH - meanL) * 0.55);
	m_twoUnits = round((double)(meanH + meanL) * 0.5);

	return Err::eOK;
}

Err DecodeMorseCode::decode()
{
	string msg;
	string morse;
	uint64_t code = 0;

	vector<int>::iterator last = m_vecTime.end();
	for (auto it=m_vecTime.begin(); it != m_vecTime.end(); ++it)
	{
		int val = *it;

		if (val  > 0 )
		{
			code <<= 2;
			if ( val > m_twoUnits)
			{
				morse += "-";  // dah
				code |= 0x00000002;
			}
			else
			{
				morse += ".";  // dit
				code |= 0x00000001;
			}
		}
		else
		{
			if (val < -m_twoUnits)
			{
				msg += mCodes.find(code)->second;
				code = 0;
			}
			if (val < -((double)m_twoUnits * 1.5))
			{
				m_message += msg + " ";
				msg = "";
				m_morse   += morse + " ";
				morse = "";
				last = it + 1;
			}
		}
	}

	vector<int> tmp(last, m_vecTime.end() );
	m_vecTime = tmp;

	cout << "******* " + to_string(m_twoUnits) + "  " << m_message << endl;
	cout << m_morse << endl;

	return Err::eOK;
}

Err DecodeMorseCode::stop()
{
	fclose(fp);
	return Err::eOK;
}


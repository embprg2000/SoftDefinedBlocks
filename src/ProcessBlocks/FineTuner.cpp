/*
 * FineTuner.cpp
 *
 *  Created on: Sep 17, 2016
 *      Author: evert
 */

#include "../ProcessBlocks/FineTuner.hpp"

// Construct finetuner.  table_size default = 64
FineTuner::FineTuner(IBlock* blkIn, unsigned int table_size, int freq_shift)
    : blkCommon(blkIn)
	, m_index(0)
    , m_table(table_size)
{
    double phase_step = 2.0 * M_PI / double(table_size);

    for (unsigned int i = 0; i < table_size; i++)
    {
        double phi = (((int64_t)freq_shift * i) % table_size) * phase_step;
        double pcos = cos(phi);
        double psin = sin(phi);
        m_table[i] = IQSample(pcos, psin);
    }
}

// Process samples.
Err FineTuner::process()
{
    unsigned int tblidx = m_index;
    unsigned int tblsiz = m_table.size();
    unsigned int n = m_in->size();

    m_out->resize(n);

    for (unsigned int i = 0; i < n; i++)
    {
        (*m_out)[i] = (*m_in)[i] * m_table[tblidx];
        tblidx++;
        if (tblidx == tblsiz)
            tblidx = 0;
    }
    m_index = tblidx;

    blkCommon::process();
    return Err::eOK;
}



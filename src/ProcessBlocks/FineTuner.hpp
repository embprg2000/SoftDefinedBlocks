/*
 * FineTuner.hpp
 *
 *  Created on: Sep 17, 2016
 *      Author: evert
 */

#ifndef FINETUNER_HPP_
#define FINETUNER_HPP_

#include "../Main/IBlock.hpp"

/** Fine tuner which shifts the frequency of an IQ signal by a fixed offset. */
class FineTuner : public blkCommon<IQSampleVector,IQSampleVector>
{
public:

    /**
     * Construct fine tuner.
     *
     * table_size :: Size of internal sin/cos tables, determines the resolution
     *               of the frequency shift.
     *
     * freq_shift :: Frequency shift. Signal frequency will be shifted by
     *               (sample_rate * freq_shift / table_size).
     */
    FineTuner(IBlock* blkIn, unsigned int table_size, int freq_shift);

	virtual Err process();

	virtual std::string getBlkName() { return "FineTuner"; }

private:

    unsigned int    m_index;
    IQSampleVector  m_table;
};

#endif /* FINETUNER_HPP_ */

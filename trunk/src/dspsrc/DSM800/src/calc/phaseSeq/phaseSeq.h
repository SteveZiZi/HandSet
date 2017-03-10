/*
 * phaseSeq.h
 *
 *  Created on: 2012-12-31
 *      Author: chao
 */

#ifndef PHASESEQ_H_
#define PHASESEQ_H_
#include "../inc/inc.h"

extern void phaseSeqCalc(const float pHarmContents[3], const float pHarmAngles[3],
                         float pPhaseSeqContents[3], float pPhaseSeqAngles[3], 
                         float *pUnbalanceRate);

#endif /* PHASESEQ_H_ */

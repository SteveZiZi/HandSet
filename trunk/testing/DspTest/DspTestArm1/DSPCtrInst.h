#ifndef DSPCTR_INSTANCE_H_

#define DSPCTR_INSTANCE_H_

#include "DSPControllor.h"

class DSPCtrInst
{
public:
	static DSPControllor *Instance();
	static void DeleteInstance();
private:
	static DSPControllor *m_pInstance;
};

#endif

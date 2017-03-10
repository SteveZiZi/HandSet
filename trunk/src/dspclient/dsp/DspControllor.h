#ifndef DSP_CONTROLLOR_H_

#define DSP_CONTROLLOR_H_

#include <WinDef.h>
#include <types.h>

#include "../dsp/lock.h"

#define DSP_LOOP_PATH ("\\FlashDisk2\\DSTester\\loop.out")     // DSP�Ĵ����ļ�

class CDspController
{
public:
    static CDspController *GetInstance();	
    int FrameSend(const UINT8 nCommandID, 
        const void* pDatas[],
        const UINT32 nDataLengths[], 
        const UINT8 nDataNum);
    int FrameSend(const UINT8 nCommandID, 
        const void* pData, 
        const UINT32 nDataLength);
    int FrameSend(const UINT8 nCommandID);
    int FrameRecv(UINT8 &nCommandID, void **pData, UINT32 &nDataLength);    
    bool DspInit();
    void DspDelete();
    bool IsInitSuccess();
    virtual ~CDspController();
private:
    CDspController();
    bool m_fDspRunning;
    Lock m_rLock;           ///< ����
    Lock m_wLock;           ///< д��
    static Lock m_singleLock;       ///< ������
};

#endif
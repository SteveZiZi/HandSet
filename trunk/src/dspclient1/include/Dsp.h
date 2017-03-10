#pragma once
#include "../../../inc/DspArmProtocol.h"
#include "../dsp/lock.h"
#include <afxmt.h>
class CDspDataSet;
class CDspController;
class CDsp
{
public:
	enum Type{
		T_SMV,
		T_CONFIG,
		T_RESET_CACHE,
		T_SMV_GEN,
		T_MSG_TIME,
		T_DSP_INFO
	};
    static CDsp *GetInstance();
    int SmvCalc(DWORD dwTimeout, const SmvData &smvData, SMV_DATA_CALC_RT *pSmvCalcRT);
    int SetConfig(DWORD dwTimeout, const Config &dspConfig);
    int ResetCache(DWORD dwTimeout, int nType);
    int SmvGenCalc(DWORD dwTimeout, const SmvGenData &smvGenData, 
            SMV_GEN_DATA_CALC_RT *pSmvGenCalcRT);
    int MsgTimeCalc(DWORD dwTimeout, const MsgTimeData &msgTimeData, 
            MSG_DISP_CALC_RT *pMsgDispRT);
    int GetDspInfo(DWORD dwTimeout, DspInfo *pDspInfo);
    int WaitForObject(Type type, DWORD dwMilliseconds);
    virtual ~CDsp();
private: 
    static const int TypeNum = 6;
    static Lock m_singleLock;
    //Lock m_ioLock;                        ///< ��д������֤һ�ζ�д������ȷ
    static bool m_fRead;                    ///< ����־��ָʾDsp��״̬
    static HANDLE m_iMutexs[TypeNum];             ///< д������
    static HANDLE m_oMutexs[TypeNum];             ///< ��������

    //CDspDataSet &m_dspDataSet;            ///< �洢DSP�����������ݼ�����
    CDspController *m_pDspCtrl;    
    int m_errno[TypeNum];                         ///< ���ּ��㷵�صĴ�����
    HANDLE m_hThreadReadData;               ///< ���߳̾��

    static DWORD ReadDataThread(LPVOID pArg);

    CDsp();
    DWORD CheckSend(Type type);               ///< ��ⷢ������    
    void ReadData();
    bool StartRead();
    void StopRead();
};

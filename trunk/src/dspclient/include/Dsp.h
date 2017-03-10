#pragma once
#include "../../../inc/DspArmProtocol.h"
#include "../dsp/lock.h"
class CDspDataSet;
class CDspController;

/******************************************************************************/
/*            ���ڳ����������ڵĿ�ʼ�ͽ���ʱ��ʼ�����ͷ�Dsp��Դ               */
/******************************************************************************/
// ��ʼ��Dsp
bool DspInit();

// �ͷ�Dsp
void DspRelease();
/******************************************************************************/

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
    CDsp();
    CDsp(CDspDataSet *pDspDataSet);
    bool SendSmv(const SmvData &smvData);
    bool SetConfig(const Config &dspConfig);
    bool ResetCache(int nType = 0);
    bool SendMsgTime(const MsgTimeData &msgTimeData);
    bool GetDspInfo();

    // ר��Ϊ�������ɶ���Ľӿ�
    int SmvGenCalc(DWORD dwTimeout, const SmvGenData &smvGenData, 
        SMV_GEN_DATA_CALC_RT *pSmvGenCalcRT);
    int WaitForObject(Type type, DWORD dwMilliseconds);
    
    bool IsReading();
    bool StartRead();
    void StopRead();
    virtual ~CDsp();
private:
    

    // ���Կ����������
    static const int TypeNum = 6;    
    static HANDLE m_iEvents[TypeNum];           ///< д������
    static HANDLE m_oEvents[TypeNum];           ///< ��������
    static bool m_fRead;                        ///< ����־��ָʾDsp��״̬
	static SMV_GEN_DATA_CALC_RT *m_pSmvGenCalcData;        ///< ���ɵĲ�������ָ��
    static CDspController *m_pDspCtrl;          ///< �洢DspController
    static CDspDataSet *m_pDspDataSet;          ///< �洢DSP�����������ݼ�
    static int m_nErrno[TypeNum];               ///< ���ּ��㷵�صĴ����� 
    static HANDLE m_hThreadReadData;                   ///< ���߳̾��    

// ���ں���չ
#if 0
    DspInfo *m_pDspInfo;                            
    SMV_DATA_CALC_RT *m_pSmvCalcData;               ///< г���ȼ�����
#endif

// �����Ժ���չ
#if 0
    stdext::hash_map<UINT8, void *> m_ptrMap;     ///< ָ��ӳ��
    stdext::hash_map<UINT8, UINT32> m_lengthMap;  ///< ����ӳ��
#endif

    static DWORD ReadDataThread(LPVOID pArg);
    DWORD CheckSend(Type type, DWORD dwTimeout);    ///< ��ⷢ������ 
};
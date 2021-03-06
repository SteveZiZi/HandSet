#include "stdafx.h"
#include "DspTest.h"
extern "C"
{
#include <assert.h>
#include "../../dspsrc/DSM800/src/calc/calc.h"
};
#include "../../src/dspclient/include/Dsp.h"
#include "DataManager.h"

bool CDspTest::m_fRun = false;
uint32_t CDspTest::nSamplingFreq = 12800;
uint32_t CDspTest::nRatedFreq = 50;
DWORD CDspTest::SendSmvThread( LPVOID pArg )
{
    CDsp *pDsp = static_cast<CDsp *>(pArg);
    SmvData *pSd = new SmvData;
    pSd->nChannelCount = MAX_SMV_CHANNELS;
    pSd->nSmvPerCircle = CDspTest::nSamplingFreq/CDspTest::nRatedFreq;
    for(int i=0; i<MAX_SMV_CHANNELS; i+=2)
    {
        pSd->fScaleFactor[i] = 1.0;
        pSd->fScaleFactor[i+1] = 1.0;
        pSd->nChannelVCType[i] = SMV_VCTYPE_VOLTAGE;
        pSd->nChannelVCType[i+1] = SMV_VCTYPE_CURRENT;
    }
    pSd->nChannelVCType[0] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[1] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[2] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[3] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[4] |= SMV_VCTYPE_PHASE_C;
    pSd->nChannelVCType[5] |= SMV_VCTYPE_PHASE_C;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={0,30,90,120};
    float fWidths[4]={220,30,50,20};
    
    sinSamplesCreate(pSd->fChannelData[0],pSd->nSmvPerCircle,CDspTest::nSamplingFreq, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);

    for(int i=1; i<MAX_SMV_CHANNELS; i++)
    {
        memcpy(pSd->fChannelData[i], pSd->fChannelData[0],
            256*sizeof(float));
    }

    fInitPhaseAngles[0] = 120;
    sinSamplesCreate(pSd->fChannelData[2],pSd->nSmvPerCircle,CDspTest::nSamplingFreq, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);

    fInitPhaseAngles[0] = 240;
    sinSamplesCreate(pSd->fChannelData[4],pSd->nSmvPerCircle,CDspTest::nSamplingFreq, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);

    while(m_fRun)
    {
        bool fRet = pDsp->SendSmv(*pSd);
        //Sleep(20);
    }
    return 0;
}
DWORD CDspTest::SendSmvThread1( LPVOID pArg )
{

    CDsp *pDsp = static_cast<CDsp *>(pArg);
    SmvData *pSd = new SmvData;
    pSd->nChannelCount = 30;
    pSd->nSmvPerCircle = 80;
    for(int i=0; i<30; i+=2)
    {
        pSd->fScaleFactor[i] = 1;
        pSd->fScaleFactor[i+1] = 1;
        pSd->nChannelVCType[i] = SMV_VCTYPE_VOLTAGE;
        pSd->nChannelVCType[i+1] = SMV_VCTYPE_CURRENT;
    }
    pSd->nChannelVCType[0] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[1] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[2] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[3] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[4] |= SMV_VCTYPE_PHASE_C;
    pSd->nChannelVCType[5] |= SMV_VCTYPE_PHASE_C;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={0,30,90,120};
    float fWidths[4]={9333.809511662427322091145579784,30,50,20};

    static float test =0;
    test += 5;

    sinSamplesCreate(pSd->fChannelData[0],80,4000, 0,fOmigas, 
        fInitPhaseAngles, fWidths, 1);
    for(int i=1; i<10; i++)
    {
        for(int j=0; j<80; j++)
        {
            pSd->fChannelData[0][i*80+j]=pSd->fChannelData[0][j];
        }        
    }

    for(int i=1; i<30; i++)
    {
        memcpy(pSd->fChannelData[i], pSd->fChannelData[0],
            80*sizeof(float));
    }
#if 0
    SmvGenData *pSgd = new SmvGenData;

    float fRmsValues[4];
    for(int i=0; i<4; i++)
    {
        fRmsValues[i] = fWidths[i]/sqrt(3);
    }

    pSgd->nChannelCount = 12;
    pSgd->nSampleRate = 4000;
    pSgd->nSmvCount = 80;
    //memset(&pSgd->nChannelVCType[0], SMV_VCTYPE_VOLTAGE, 8*sizeof(uint8_t));
    //memset(&pSgd->nChannelVCType[8], SMV_VCTYPE_CURRENT, 8*sizeof(uint8_t));
    memset(pSgd->nHarmCount , 4, pSgd->nChannelCount*sizeof(uint8_t));
    for(int i=0; i<pSgd->nChannelCount; i++)
    {
        pSgd->fScaleFactor[i]=1.0;
        pSgd->fDCValue[i]=35.0;
        memcpy(pSgd->fRmsValue[i], fRmsValues, 4*sizeof(float));
        memcpy(pSgd->fPhaseAngle[i], fInitPhaseAngles, 4*sizeof(float));
        memcpy(pSgd->fFreq[i], fOmigas, 4*sizeof(float));
    }
#endif
#if 0
    srand(GetTickCount());

    Config *pCf = new Config;
    //pCf->nSmvCalcCount = 800;
    pCf->nMsgTimeCalcCount = 4000;
    pCf->nASDUCount = 1;
#endif    
    while(m_fRun)
    {
        test += 5;

        bool fRet = pDsp->SendSmv(*pSd);
        Sleep(20);
        //pDsp->SendSmvGen(*pSgd);
        /*mt.nSecond = rand();
        mt.nMicrosecond = rand();
        pDsp->SendMsgTime(mt);*/
        //assert(fRet);
        //Sleep(5);
//         pDsp->SetConfig(*pCf);
//         pDsp->GetDspInfo();
//         pDsp->GetDspInfo();
//         pDsp->GetDspInfo();
//         pDsp->GetDspInfo();
    }   
    delete pSd;
//    delete pSgd;
    return 0;
}
void CDspTest::SetConfig()
{
    Config *pCf = new Config;
    pCf->nMsgTimeCalcCount = 468;
    pCf->nASDUCount = 1;
    
    pCf->nSamplingFreq = CDspTest::nSamplingFreq;
    pCf->nRatedFreq = CDspTest::nRatedFreq;

    bool fRet = m_pDsp->SetConfig(*pCf);
    assert(fRet);
    delete pCf;
}

#if 0
///
/// 35+220*sin(2*pi*50*(n-1)/4000-60/180*pi)+30*sin(2*pi*100*(n-1)/4000+30/180*pi)
/// +50*sin(2*pi*200*(n-1)/4000+90/180*pi)+20*sin(2*pi*300*(n-1)/4000+120/180*pi)
/// 
/// −73.20508075688771,-68.98254325029008,−71.13634777211134,−77.20034096062287
void CDspTest::SendSmvGen()
{
    SmvGenData *pSgd = new SmvGenData;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={-60,30,90,120};
    float fWidths[4]={220,30,50,20};
    float fRmsValues[4];
    for(int i=0; i<4; i++)
    {
        fRmsValues[i] = fWidths[i]/sqrt(3);
    }
    
    pSgd->nChannelCount = 12;
    pSgd->fSampleRate = 4000;
    pSgd->nSmvCount = 256;
    memset(pSgd->nHarmCount , 4, pSgd->nChannelCount*sizeof(uint8_t));
    for(int i=0; i<pSgd->nChannelCount; i++)
    {
        pSgd->fScaleFactor[i]=1.0;
        pSgd->fDCValue[i]=35.0;
        memcpy(pSgd->fRmsValue[i], fRmsValues, 4*sizeof(float));
        memcpy(pSgd->fPhaseAngle[i], fInitPhaseAngles, 4*sizeof(float));
        memcpy(pSgd->fFreq[i], fOmigas, 4*sizeof(float));
    }

    bool fRet = m_pDsp->SendSmvGen(*pSgd);
    assert(fRet);  
    delete pSgd;
}
#endif

DWORD CDspTest::SendMsgTimeThread( LPVOID pArg )
{
//#define _DEBUG_INFO
    MsgTimeData mtd;
    mtd.nMsgTimeCount = 468;
    CDsp *pDsp = static_cast<CDsp *>(pArg);
    srand(GetTickCount());
    pDsp->ResetCache(1);
    mtd.msgTimes[0].nSecond = 0;
    mtd.msgTimes[0].nMicrosecond= 0;
    // 模拟存储多个报文时间数据    
    while(m_fRun)
    {
        for(int i = 1; i<mtd.nMsgTimeCount; i++)
        {
            mtd.msgTimes[i].nSecond = mtd.msgTimes[i-1].nSecond;
            mtd.msgTimes[i].nMicrosecond = mtd.msgTimes[i-1].nMicrosecond+rand()%499+1;
            //进位
            if(mtd.msgTimes[i].nMicrosecond>=1000000)
            {
                mtd.msgTimes[i].nMicrosecond-=1000000;
                mtd.msgTimes[i].nSecond++;
            }
#ifdef _DEBUG_INFO
            printf("%d\t\t%d\t\t%d\n",mtd.msgTimes[i].nMicrosecond,
                mtd.msgTimes[i].nSecond, 
                mtd.msgTimes[i].nMicrosecond-mtd.msgTimes[i-1].nMicrosecond-250);
#endif
            //TRACE(L"%d\t", mtd.msgTimes[i].nMicrosecond);
        }
        //TRACE(L"\r\n\r\n");
        bool fRet = pDsp->SendMsgTime(mtd);
        Sleep(30);
        mtd.msgTimes[0].nSecond = mtd.msgTimes[mtd.nMsgTimeCount-1].nSecond;
        mtd.msgTimes[0].nMicrosecond = mtd.msgTimes[mtd.nMsgTimeCount-1].nMicrosecond
            +rand()%499+1;
        //进位
        if(mtd.msgTimes[0].nMicrosecond>=1000000)
        {
            mtd.msgTimes[0].nMicrosecond-=1000000;
            mtd.msgTimes[0].nSecond++;
        }
#ifdef _DEBUG_INFO
        printf("%d\t\t%d\n",mtd.msgTimes[0].nMicrosecond, mtd.msgTimes[0].nSecond,
            mtd.msgTimes[0].nMicrosecond-mtd.msgTimes[mtd.nMsgTimeCount-1].nMicrosecond-250);
#endif
    }
    return 0;
}

void CDspTest::GetInfo()
{  
    bool fRet  = m_pDsp->GetDspInfo();
    assert(fRet);
}

void CDspTest::ResetCache()
{
    bool fRet = m_pDsp->ResetCache(0);
    assert(fRet);
	fRet = m_pDsp->ResetCache(1);
	assert(fRet);
}

CDspTest::CDspTest()
:m_hThreadSmv(NULL)
,m_hThreadMsgTime(NULL)
{

    CDspDataSet &dspDataSet = CDataManager::GetInstance().GetDspDataSet();
    m_pDsp = new CDsp(&dspDataSet);

    m_pDsp1 = new CDsp();    
}

CDspTest::~CDspTest( void )
{
    m_fRun = false;
#if 1
    delete m_pDsp;
    delete m_pDsp1;
#endif
}

void CDspTest::RunTest()
{
    TRACE(L"FrameHead:%d\n",sizeof(FrameHead));
    TRACE(L"Error:%d\n",sizeof(Error));
    TRACE(L"DspInfo:%d\n",sizeof(DspInfo));
    TRACE(L"SmvData:%d\n",sizeof(SmvData));
    TRACE(L"SMV_DATA_CACL_RT:%d\n",sizeof(SMV_DATA_CALC_RT));
    TRACE(L"SMV_GEN_DATA_CACL_RT:%d\n",sizeof(SMV_GEN_DATA_CALC_RT));
    
    CDspTest::nSamplingFreq = 12800;
    CDspTest::nRatedFreq = 50;

    if(m_fRun == false)
    {
        m_fRun = true;
               
        m_pDsp->StartRead();

        ResetCache();
        SetConfig();
        
        //SmvGenCalc();
#if 1
#if 1
#if 1
        m_hThreadSmv = CreateThread(NULL, 0, SendSmvThread, m_pDsp, 0, NULL);
        Sleep(1000);
        DWORD dwResult = WaitForSingleObject(m_hThreadSmv,100);
        if(WAIT_OBJECT_0 != dwResult)
        {
            TerminateThread(m_hThreadSmv, -1);
        }
#endif
        m_hThreadMsgTime = CreateThread(NULL, 0, SendMsgTimeThread, m_pDsp, 0, NULL);
#else
        m_hThreadSmv = CreateThread(NULL, 0, SendSmvThread1, m_pDsp, 0, NULL);
#endif
#else
        m_hThreadMsgTime = CreateThread(NULL, 0, SendMsgTimeThread, m_pDsp, 0, NULL);
#endif
    }
    if(m_fRun == true)
    {        
#if 1             
		//SendSmv();
		//Sleep(1000);
        //ResetCache();
        //SetConfig();
        //GetInfo();
        //SendSmvGen();
#endif  
    }
    //SendSmv();
}

void CDspTest::ExitTest()
{
#if 1
    if(m_fRun == true)
    {
        m_fRun = false;        
#if 1
        DWORD dwResult = WaitForSingleObject(m_hThreadSmv,100);
        if(WAIT_OBJECT_0 != dwResult)
        {
            TerminateThread(m_hThreadSmv, -1);
        }
        dwResult = WaitForSingleObject(m_hThreadMsgTime, 100);
        if(WAIT_OBJECT_0 != dwResult)
        {
            TerminateThread(m_hThreadMsgTime, -1);
        }        
        CloseHandle(m_hThreadSmv);
        CloseHandle(m_hThreadMsgTime);    
        m_pDsp->StopRead();
#endif
    }
#endif
}

void CDspTest::SendSmv()
{
    CDsp *pDsp = this->m_pDsp;
    SmvData *pSd = new SmvData;
    pSd->nChannelCount = 30;
    pSd->nSmvPerCircle = 256;
    for(int i=0; i<30; i+=2)
    {
        pSd->fScaleFactor[i] = 1.0;
        pSd->fScaleFactor[i+1] = 1.0;
        pSd->nChannelVCType[i] = SMV_VCTYPE_VOLTAGE;
        pSd->nChannelVCType[i+1] = SMV_VCTYPE_CURRENT;
    }
    pSd->nChannelVCType[0] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[1] |= SMV_VCTYPE_PHASE_A;
    pSd->nChannelVCType[2] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[3] |= SMV_VCTYPE_PHASE_B;
    pSd->nChannelVCType[4] |= SMV_VCTYPE_PHASE_C;
    pSd->nChannelVCType[5] |= SMV_VCTYPE_PHASE_C;

    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={-60,30,90,120};
    float fWidths[4]={220,30,50,20};
    sinSamplesCreate(pSd->fChannelData[0],256,4000, 35,fOmigas, 
        fInitPhaseAngles, fWidths, 4);
    for(int i=1; i<30; i++)
    {
        memcpy(pSd->fChannelData[i], pSd->fChannelData[0],
            256*sizeof(float));
    }
    for(int i=0; i<10; i++)
    {
        pDsp->SendSmv(*pSd);
        Sleep(20);
    }
}
///
/// 35+220*sin(2*pi*50*(n-1)/4000-60/180*pi)+30*sin(2*pi*100*(n-1)/4000+30/180*pi)
/// +50*sin(2*pi*200*(n-1)/4000+90/180*pi)+20*sin(2*pi*300*(n-1)/4000+120/180*pi)
/// 
/// −73.20508075688771,-68.98254325029008,−71.13634777211134,−77.20034096062287
void CDspTest::SmvGenCalc()
{
    SMV_GEN_DATA_CALC_RT *pSgdRT = new SMV_GEN_DATA_CALC_RT;
    SmvGenData *pSgd = new SmvGenData;    
    
    float fOmigas[4]={50,100,200,300};
    float fInitPhaseAngles[4]={-60,30,90,120};
    float fWidths[4]={220,30,50,20};
    float fRmsValues[4];
    for(int i=0; i<4; i++)
    {
        fRmsValues[i] = fWidths[i]/sqrt(3);
    }

    pSgd->nChannelCount = 12;
    pSgd->nSampleRate = 4000;
    pSgd->nSmvCount = 256;
    memset(pSgd->nHarmCount , 4, pSgd->nChannelCount*sizeof(uint8_t));
    for(int i=0; i<pSgd->nChannelCount; i++)
    {
        pSgd->fScaleFactor[i]=1.0;
        pSgd->fDCValue[i]=35.0;
        memcpy(pSgd->fRmsValue[i], fRmsValues, 4*sizeof(float));
        memcpy(pSgd->fPhaseAngle[i], fInitPhaseAngles, 4*sizeof(float));
        memcpy(pSgd->fFreq[i], fOmigas, 4*sizeof(float));
    }

    pSgd->fDCValue[0] = 0.0f;
    pSgd->fRmsValue[0][0] = 0.008;
    pSgd->fPhaseAngle[0][0] = 0.0f;
    pSgd->fFreq[0][0] = 50.0f;
    pSgd->nHarmCount[0] = 1;

    int fRet = m_pDsp1->SmvGenCalc(1000, *pSgd, pSgdRT);
    assert(fRet == 0);

    fRet = m_pDsp1->WaitForObject(CDsp::T_SMV_GEN, 1000);
    assert(fRet == 0);
    TRACE(L"SmvGenData[0]:%f\n", pSgdRT->fSmvGenData[0][0]);

    CDspDataSet &dspDataSet = CDataManager::GetInstance().GetDspDataSet();
    dspDataSet.Write(DSP2ARM_CONTROLCODE_SMV_GEN_RT, pSgdRT, sizeof(SMV_GEN_DATA_CALC_RT));

    delete pSgd;
    delete pSgdRT;
}

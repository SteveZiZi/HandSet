#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "AudioTest.h"
#include "iniFile.h"


//定义音频测试参数命令
#define    PARAM_BEGIN_RECORD           0       //开始录音
#define    PARAM_STOP_RECORD            1       //停止录音
#define    PARAM_BEGIN_PLAY             2       //开始播放声音
#define    PARAM_STOP_PLAY              3       //停止播放声音
#define    PARAM_GET_POS_RECORD         4       //获取录制的声音位置信息
#define    PARAM_GET_POS_PLAY           5       //获取的声音位置信息



BOOL CAudioTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"AudioTest_\0\0";
    strSection[10]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"音频",
        m_strTestName,31,strIniFile);    
    m_uiCmd=index+CMD_TEST_AUDIO;
    *uipCmdIndex=m_uiCmd;
    return TRUE;
}

TEST_STATUS CAudioTest::OnConnectDown()
{
    m_wavIn.Stop();
    m_wavOut.Stop();
    m_wavOut.CloseWavFile();

    return TEST_NORMAL;
}

TEST_STATUS CAudioTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                         const BYTE *pData,CConnect *pConnect)
{
    m_ti.bError=0;
    switch (param)
    {
    case PARAM_BEGIN_RECORD:
        {
            if (m_wavIn.CreateWavFile(L"\\test.wav"))
            {
                if (!m_wavIn.BeginRecord())
                {
                    m_ti.bError=1;
                    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"开始录音失败");
                    m_wavIn.Stop();
                }
            }
            else
            {
                m_ti.bError=1;
                m_ti.uiErrorLen=swprintf(m_ti.strErr,L"创建音频文件失败");
            }
        }
        break;
    case PARAM_STOP_RECORD:
        {
            m_wavIn.Stop();
        }
        break;
    case  PARAM_BEGIN_PLAY:
        {
            if (m_wavOut.OpenWavFile(L"\\test.wav") )
            {
                if (!m_wavOut.Play(TRUE) )
                {
                    m_ti.bError=1;
                    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"开始播放失败");
                    m_wavOut.Stop();
                    m_wavOut.CloseWavFile();
                }
            }
            else
            {
                m_ti.bError=1;
                m_ti.uiErrorLen=swprintf(m_ti.strErr,L"打开音频文件失败");
                m_wavOut.Stop();
                m_wavOut.CloseWavFile();
            }
        }
        break;
    case  PARAM_STOP_PLAY:
        {
            m_wavOut.Stop();
            m_wavOut.CloseWavFile();
        }
        break;
    case  PARAM_GET_POS_RECORD:
        {
            WAV_POS_INFO wpi;
            m_wavIn.GetPos(wpi);
            
            int nRet=pConnect ->SendAPacket(m_uiCmd,param,sizeof(WAV_POS_INFO),
                (BYTE *)&wpi );
             return nRet >0?TEST_NORMAL:TEST_ERROR;
        }
        break;
    case PARAM_GET_POS_PLAY:
        {
            WAV_POS_INFO wpi;
            m_wavOut.GetPos(wpi);

            int nRet=pConnect ->SendAPacket(m_uiCmd,param,sizeof(WAV_POS_INFO),
                (BYTE *)&wpi );
            return nRet >0?TEST_NORMAL:TEST_ERROR;
        }
        break;
    default:
        break;
    }

    int nRet;
    TEST_INFO_TRANSFER  tit;
    TestInfoLocalToTransfer(&tit,&m_ti);

    nRet =pConnect->SendAPacket(m_uiCmd,param,sizeof(tit),(BYTE *)&tit);

    return nRet >0?TEST_NORMAL:TEST_ERROR;
}
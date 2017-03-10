#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "AudioTest.h"
#include "iniFile.h"


//������Ƶ���Բ�������
#define    PARAM_BEGIN_RECORD           0       //��ʼ¼��
#define    PARAM_STOP_RECORD            1       //ֹͣ¼��
#define    PARAM_BEGIN_PLAY             2       //��ʼ��������
#define    PARAM_STOP_PLAY              3       //ֹͣ��������
#define    PARAM_GET_POS_RECORD         4       //��ȡ¼�Ƶ�����λ����Ϣ
#define    PARAM_GET_POS_PLAY           5       //��ȡ������λ����Ϣ



BOOL CAudioTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"AudioTest_\0\0";
    strSection[10]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"��Ƶ",
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
                    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"��ʼ¼��ʧ��");
                    m_wavIn.Stop();
                }
            }
            else
            {
                m_ti.bError=1;
                m_ti.uiErrorLen=swprintf(m_ti.strErr,L"������Ƶ�ļ�ʧ��");
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
                    m_ti.uiErrorLen=swprintf(m_ti.strErr,L"��ʼ����ʧ��");
                    m_wavOut.Stop();
                    m_wavOut.CloseWavFile();
                }
            }
            else
            {
                m_ti.bError=1;
                m_ti.uiErrorLen=swprintf(m_ti.strErr,L"����Ƶ�ļ�ʧ��");
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
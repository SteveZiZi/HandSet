#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "gpsTest.h"
#include "assertMsg.h"
#include "iniFile.h"
#include "nmea/nmea.h"

#pragma comment(lib,"nmea.lib")
#pragma comment(lib,"nmea_ppc.lib")

BOOL CGpsTest::Init(const WCHAR *strIniFile, int index,
                    UINT16 *uipCmdIndex,CTestManager *pManager/* =NULL */)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"GPSTest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPS",m_strTestName,31,strIniFile);
    m_iComIndex=GetPrivateProfileInt(strSection,L"SerialPortIndex",0,strIniFile)+1;
    m_iBraudRate = GetPrivateProfileInt(strSection,L"BraudRate ",9600,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_GPS;
    *uipCmdIndex=m_uiCmd;

    return TRUE;
}

TEST_STATUS CGpsTest::ProcessExcuteCmd(UINT16 param, UINT16 uiDatalen, 
                                       const BYTE *pData, CConnect *pConnect)
{
    m_ti.bError=0;
    m_ti.uiErrorLen = 0;
    //���Դ򿪴���
    BOOL  bGpsGPRMC = FALSE;
    BOOL  bGpsGPGSV = FALSE;
    char readBuf1[200]={0};
    char readBuf2[200]={0};


    if (m_xComCE.Open(m_iComIndex,m_iBraudRate))
    {
        m_xComCE.ClearRecv();
        char sendbuf[] = {'a','t','+','g','p','s','s','t','a','r','t','\r','\n'};
        m_xComCE.WriteNByte(sendbuf,sizeof(sendbuf));

        int REV_BUF_LEN = 2048;
        char        revBuffer[2048]={0};
        size_t      dwRecvLen =0;
        m_xComCE.ClearRecv();
        m_xComCE.ClearCommError();
        int i,j;
        m_xComCE.ReadNByteTimeOut(revBuffer,2048,5000,&dwRecvLen);
        if (dwRecvLen >0)
        {
            REV_BUF_LEN = dwRecvLen;
            for (i = 0; i < REV_BUF_LEN; i++) 
            {
                if (revBuffer[i] == '$') 
                {
                    //���ش���λ�ú����ǵȵ���Ϣ
                    if ((revBuffer[i+5] == 'C') || (revBuffer[i+5] == 'V')) 
                    {
                        for (j = i + 5; j < REV_BUF_LEN; j++) 
                        {
                            if (revBuffer[j] == '$') 
                            {
                                break;
                            }
                        }
                        if (j >= REV_BUF_LEN) 
                        { 
                            m_ti.bError=1;
                            m_ti.uiErrorLen=swprintf(m_ti.strErr,
                                L"��GPS����%d���յ��������Ҳ���λ����Ϣ",m_iComIndex);
                            goto END;
                        }
                        if ((j - i) > 200) 
                        {
                            m_ti.bError=1;
                            m_ti.uiErrorLen=swprintf(m_ti.strErr,
                                L"��GPS����%d���յ������ݲ���ȷ",m_iComIndex);
                            goto END;
                        }
                        if((revBuffer[i+5] == 'C') && (bGpsGPRMC == FALSE)) 
                        {
                            memcpy(readBuf1, revBuffer + i, (j - i));
                            readBuf1[j - i] = '\0';
                            bGpsGPRMC = TRUE;
                        }
                        if((revBuffer[i+5] == 'V') && (bGpsGPGSV == FALSE)) 
                        {
                            memcpy(readBuf2, revBuffer + i, (j - i));
                            readBuf2[j - i] = '\0';
                            bGpsGPGSV = TRUE;
                        }
                        if (bGpsGPRMC && bGpsGPGSV) 
                        {
                            break;
                        }
                    }
                }
            }

            if(bGpsGPRMC && bGpsGPGSV) 
            {
                nmeaPARSER parser;
                nmea_parser_init(&parser);
                nmeaINFO info;
                nmea_zero_INFO(&info);
                if(nmea_parse(&parser, readBuf1, (int)strlen(readBuf1), &info) == FALSE) 
                {
                    //return;
                }
                nmea_parse(&parser, readBuf2, (int)strlen(readBuf2), &info);
                info.lat = nmea_ndeg2degree(info.lat);
                info.lon = nmea_ndeg2degree(info.lon);

                nmea_parser_destroy(&parser);

                if (info.sig != NMEA_SIG_BAD ) 
                {
                    if(info.satinfo.inview >3)
                    {
                        
                        CString        str,str1;
                        str = L"��ǰ����:";
                        if (info.lon >= 0) 
                        {
                            str1.Format(_T("%3.8f E"),info.lon);
                        }
                        else 
                        {
                            str1.Format(_T("%3.8f W"),fabs(info.lon));
                        }
                        str+=str1;
                        str+=L" ��ǰγ��:";
                        if (info.lat >= 0) 
                        {
                            str1.Format(_T("%3.8f N"),info.lat);
                        }
                        else 
                        {
                            str1.Format(_T("%3.8f S"),fabs(info.lat));
                        }
                        str+=str1;
                        //��鵱ǰ�ľ�γ���Ƿ��ڹ���
                        if (info.lon>=125 && info.lon <=127)
                        {
                            if (info.lat >=44 && info.lat <=46)
                            {
                                m_ti.uiErrorLen = str.GetLength();
                                wcscpy(m_ti.strErr,str);
                                goto END;
                            }
                        }
                        str +=L",�ƺ����ڹ���.";

                        m_ti.uiErrorLen = str.GetLength();
                        wcscpy(m_ti.strErr,str);
                        m_ti.bError =1;
                        goto END;
                    }
                    else
                    {
                        m_ti.bError=1;
                        m_ti.uiErrorLen=swprintf(m_ti.strErr,
                            L"��������̫��");
                    }
                }
                else
                {
                    m_ti.bError=1;
                    m_ti.uiErrorLen=swprintf(m_ti.strErr,
                        L"�ź�̫��������GPSģ�鲻���յ�������Ϣ");
                }
            }
        }
        else
        {
            m_ti.bError=1;
            m_ti.uiErrorLen=swprintf(m_ti.strErr,
                L"��GPS����%d��������ʧ��",m_iComIndex);
            goto END;
        }

    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,
            L"��GPS����%dʧ��",m_iComIndex);
        goto END;
    }

END:

    m_xComCE.CloseCom();
    TEST_INFO_TRANSFER tit;

    TestInfoLocalToTransfer(&tit,&m_ti);

    int nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(tit),
        (BYTE *)&tit );
    return nRet >0? TEST_NORMAL:TEST_CONNECTDOWN;  

}
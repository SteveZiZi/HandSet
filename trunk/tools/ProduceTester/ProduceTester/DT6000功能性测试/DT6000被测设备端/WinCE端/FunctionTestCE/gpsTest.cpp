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
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"GPSTest_\0\0";
    strSection[8]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPS",m_strTestName,31,strIniFile);
    m_iComIndex=GetPrivateProfileInt(strSection,L"SerialPortIndex",0,strIniFile)+1;
    m_iBraudRate = GetPrivateProfileInt(strSection,L"BraudRate ",9600,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_GPS;
    *uipCmdIndex=m_uiCmd;

    return TRUE;
}

TEST_STATUS CGpsTest::ProcessExcuteCmd(UINT16 param, UINT16 uiDatalen, 
                                       const BYTE *pData, CConnect *pConnect)
{
    m_ti.bError=0;
    m_ti.uiErrorLen = 0;
    //尝试打开串口
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
                    //搜素带有位置和卫星等的信息
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
                                L"从GPS串口%d接收到的数据找不到位置信息",m_iComIndex);
                            goto END;
                        }
                        if ((j - i) > 200) 
                        {
                            m_ti.bError=1;
                            m_ti.uiErrorLen=swprintf(m_ti.strErr,
                                L"从GPS串口%d接收到的数据不正确",m_iComIndex);
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
                        str = L"当前经度:";
                        if (info.lon >= 0) 
                        {
                            str1.Format(_T("%3.8f E"),info.lon);
                        }
                        else 
                        {
                            str1.Format(_T("%3.8f W"),fabs(info.lon));
                        }
                        str+=str1;
                        str+=L" 当前纬度:";
                        if (info.lat >= 0) 
                        {
                            str1.Format(_T("%3.8f N"),info.lat);
                        }
                        else 
                        {
                            str1.Format(_T("%3.8f S"),fabs(info.lat));
                        }
                        str+=str1;
                        //检查当前的经纬度是否在广州
                        if (info.lon>=125 && info.lon <=127)
                        {
                            if (info.lat >=44 && info.lat <=46)
                            {
                                m_ti.uiErrorLen = str.GetLength();
                                wcscpy(m_ti.strErr,str);
                                goto END;
                            }
                        }
                        str +=L",似乎不在广州.";

                        m_ti.uiErrorLen = str.GetLength();
                        wcscpy(m_ti.strErr,str);
                        m_ti.bError =1;
                        goto END;
                    }
                    else
                    {
                        m_ti.bError=1;
                        m_ti.uiErrorLen=swprintf(m_ti.strErr,
                            L"可视卫星太少");
                    }
                }
                else
                {
                    m_ti.bError=1;
                    m_ti.uiErrorLen=swprintf(m_ti.strErr,
                        L"信号太差以至于GPS模块不能收到卫星信息");
                }
            }
        }
        else
        {
            m_ti.bError=1;
            m_ti.uiErrorLen=swprintf(m_ti.strErr,
                L"从GPS串口%d接收数据失败",m_iComIndex);
            goto END;
        }

    }
    else
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=swprintf(m_ti.strErr,
            L"打开GPS串口%d失败",m_iComIndex);
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
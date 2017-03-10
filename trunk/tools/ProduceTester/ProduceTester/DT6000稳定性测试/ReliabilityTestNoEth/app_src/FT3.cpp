#include "stdafx.h"
#include "windows.h"
//#include "pkfuncs.h"

#include "winioctl.h"

#include "gpmcfg.h"
#include "fiber.h"

    
BOOL VerifyUtcTime(UTC_TIME_t preTime, UTC_TIME_t curTime, DWORD dwSampleRate)
{
    DWORD dwOffsetTime;
    if(curTime.uFractionOfSecond < preTime.uFractionOfSecond) {
        dwOffsetTime = 1000000 - preTime.uFractionOfSecond + curTime.uFractionOfSecond;
        curTime.uSecondSinceEpoch--;
    } else {
        dwOffsetTime = curTime.uFractionOfSecond - preTime.uFractionOfSecond;
    }

    dwSampleRate = 1000000 / dwSampleRate;
    if (dwOffsetTime<dwSampleRate-1 || dwOffsetTime>dwSampleRate+1) {
       // RETAILMSG(1, (TEXT("dwOffsetTime = %d\r\n"), dwOffsetTime));
        return FALSE;
    }
    if (curTime.uSecondSinceEpoch != preTime.uSecondSinceEpoch) {
        return FALSE;
    }

    return TRUE;
}

_FT3_c::~_FT3_c()
{
    DeInit();
}

bool _FT3_c::Ft3Tx()
{
    PUINT16 pFt3Data =  PUINT16(m_dataBuf + FT3_TX_LOOP_BUF0_POS);
    RETAILMSG(1, (_T("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \r\n"), 
        pFt3Data[0], pFt3Data[1], pFt3Data[2], pFt3Data[3], pFt3Data[4], pFt3Data[5], pFt3Data[6], pFt3Data[7]));

    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = DEVICE_FT3_TX;
    memcpy(tFt3InParam.uInBuf, &m_tTxCfg, sizeof(FT3_TX_CFG_t));

    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SEND_HEADER, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
//        MessageBox(L"DeviceIoControl:IOCTRL_SEND_HEADER failed");
        return FALSE;
    }

    /*
     *  IOCTRL_SEND_FRAME 命令  m_Ft3Tx.m_ptMapHeader->uCurTxBufItem的初始化，
     *  仅在报文格式（采样率、波特率、报文长度等）发生变化时才调用，
     *  报文格式不变，仅数据内容变化时，直接调用IOCTRL_CHANGE_MU_FRAME命令即可
     */
    m_ptMapHeader->ft3Header.uCurSmvTxBufItem = 0; 
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SEND_FRAME, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
//         MessageBox(L"DeviceIoControl:IOCTRL_SEND_FRAME failed");
        return FALSE;
    }
    return TRUE;
}


bool _FT3_c::Ft3Rx()
{
    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = m_nDeviceItem;


    //     GetDlgItemText(IDC_EDT_FT3_RX_TMEOUT, strTmp);
    //     nTmp = _wtoi(strTmp.GetBuffer());
    //tFt3InParam.uInBuf[0] = 5;
    //UINT32 nTimeOut = tFt3InParam.uInBuf[0];

    //if (nTimeOut >= 1 && nTimeOut <= 10) {                              /*  FT3接收超时时间最大10ms     */
    //    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SET_RX_TIMEOUT, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
    //         return false;
    //     }
    // }

    
    tFt3InParam.uInBuf[0] = 800;

    tFt3InParam.uInBuf[1] = 1;    
    
    tFt3InParam.uInBuf[2] = tFt3InParam.uInBuf[3] = 0;

    /*  使能超时中断 具体时间驱动固定*/
    //     }
    m_bRecv = TRUE;
    m_bGetSmpCntVal = TRUE;

    /*m_uTestResult[m_nDeviceItem*2] = 0;
    m_uTestResult[m_nDeviceItem*2+1] = 0;
    m_uUtcTest[m_nDeviceItem] = 0;*/
 
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SET_RECV_LOOP, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
//         MessageBox(L"DeviceIoControl:IOCTRL_SET_RECV_LOOP failed");
        RETAILMSG(1, (_T("FT3Rx Fail~ \r\n")));
        return FALSE;
    }

    return TRUE;
}

bool _FT3_c::Ft3RxStop(  )
{
    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = m_nDeviceItem;
    m_bRecv = FALSE;
    
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_STOP_RECV, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        RETAILMSG(1, (_T("Ft3RxStop fail \r\n")));
        return FALSE;
    }
    return TRUE;
}

bool _FT3_c::Ft3StopTx()
{
    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = DEVICE_FT3_TX;
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_STOP_SEND, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
//         MessageBox(L"DeviceIoControl:IOCTRL_STOP_SEND failed");
        return FALSE;
    }
    return TRUE;
}

void _FT3_c::SetFt3TxFrame( PVOID pData /*= NULL*/ )
{
    if( NULL == pData)
    {
        GetFT3TxFrame(m_dataBuf+FT3_TX_LOOP_BUF0_POS);
    }
    else
    {
        GetFT3TxFrame(pData);
    }
}

DWORD _FT3_c::GetFT3RxFrame( PVOID pData )
{
    PUINT16 pFt3TxData;
    pFt3TxData = (PUINT16)pData;

    if(NULL == pFt3TxData)
    {
        return FALSE;
    }

    DWORD dwDataLen = 0;
    UINT16 uLoopItem = m_ptMapHeader->ft3Header.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->ft3Header.uFramesSum;
    Ft3ControlWord_t tCtrlW = m_ptMapHeader->ft3Header.tCtrlWord;
    UINT32 uActiveLen = m_ptMapHeader->ft3Header.uActiveLength;

 //   dwDataLen = EXFT3_FRAME_LEN[(tCtrlW.CtrlBit.bFormat == _NAN_RUI) ? _NAN_RUI : _GUO_WANG ]/2;
    dwDataLen = EXFT3_FRAME_LEN[tCtrlW.CtrlBit.bFormat]/2;
    RETAILMSG(1, (_T("Control = 0x%x  uLoopItem = %d  FramesSum = %d,  FrameLen = %d, ActiveLen = %d\r\n"), 
        tCtrlW.data, uLoopItem, uFramesSum, dwDataLen, uActiveLen));

    UINT32 uSum = 0;
    PUINT16 pFt3Data;
    if (uLoopItem == 0) {
        pFt3Data = (PUINT16)(m_dataBuf+FT3_LOOP_BUF0_POS);
    } else {
        pFt3Data = (PUINT16)(m_dataBuf+FT3_LOOP_BUF1_POS);
    }
    
    UINT16 uSampleRate = 0;

    if (tCtrlW.CtrlBit.bSampleRate == 5) {
        uSampleRate = 12800;
    } else if (tCtrlW.CtrlBit.bSampleRate == 4) {
        uSampleRate = 10000;
    } else if (tCtrlW.CtrlBit.bSampleRate == 3) {
        uSampleRate = 8000;
    } else if (tCtrlW.CtrlBit.bSampleRate == 2) {
        uSampleRate = 5000;
    } else if (tCtrlW.CtrlBit.bSampleRate == 1) {
        uSampleRate = 4000;
    }

    for (UINT16 i = 0; i < uSampleRate / 50; i++) {
#if ZONE_FT3_TIME_STRAP
        /*
         *  添加FT3时间戳
         */ 
        {
             pFt3Data += FT3_TIME_STRAP_LEN/2;
        }
#endif
        
        for (int j = 0; j < dwDataLen;) 
        {
            if (j%9 == 0) 
            {
                j++;
                pFt3Data++;/*  跳过报文的起始字节、CRC校验值*/
                continue;
            }
        memcpy(pFt3TxData, pFt3Data, 16);  

        if (i == 0) {
            RETAILMSG(1, (_T("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \r\n"), 
                pFt3Data[0], pFt3Data[1], pFt3Data[2], pFt3Data[3], pFt3Data[4], pFt3Data[5], pFt3Data[6], pFt3Data[7]));
            //Sleep(1);
        }

        pFt3TxData += 8;
       
        uSum += 8;
        pFt3Data += 8;
        j += 8;
        Sleep(1);
        }
    }
    return TRUE;
}

void _FT3_c::SetFt3TxAddress( PVOID pData )
{
    m_pFt3TxData = (PUINT16)pData;
}

void _FT3_c::SetFT3TxInfo(Ft3SendCtlWord_t tCtrlW)
{

//    RETAILMSG(ZONE_INFO, (_T("APP:FT3 FT3SendCtlWord is 0x%x\r\n"), tCtrlW.wData));

    m_tTxCfg.uFramesStyle = (UINT16)tCtrlW.wData;
}

void _FT3_c::GetFT3TxInfo( Ft3SendCtlWord_t &tSendCtrlW )
{
    Ft3ControlWord_t tCtrlW = m_ptMapHeader->ft3Header.tCtrlWord;
    tSendCtrlW.CtrlBit.bBaudRate = tCtrlW.CtrlBit.bBaudRate;
    tSendCtrlW.CtrlBit.bSampleRate = tCtrlW.CtrlBit.bSampleRate;
    tSendCtrlW.CtrlBit.bFrameStyle = tCtrlW.CtrlBit.bFormat;
}


DWORD _FT3_c::GetFT3TxFrame( PVOID pData)
{
    CFile file;
    FT3FrameStyle_e eFt3FrameStyle;
    UINT16 uSampleRate = 0;


    if (file.Open((LPCTSTR)m_strFilePath, CFile::modeRead, NULL)) {
        UINT16 uFrameLen = 0;
        Ft3SendCtlWord_t tCtrlWord;
        PUINT16 pFt3Data = (PUINT16)pData;

        tCtrlWord.wData = m_tTxCfg.uFramesStyle;
        eFt3FrameStyle = (FT3FrameStyle_e)(tCtrlWord.CtrlBit.bFrameStyle);
        /*
         *  仅提供了南瑞、国网两种报文文件，其他扩展的4种FT3报文，都通过国网报文转换而来
         */
        if (eFt3FrameStyle == _NAN_RUI) {
            uFrameLen = EXFT3_FRAME_LEN[_NAN_RUI];
        } else {
            uFrameLen = EXFT3_FRAME_LEN[_GUO_WANG];
        }

        if (tCtrlWord.CtrlBit.bSampleRate == 5) {
            uSampleRate = 12800;
        } else if (tCtrlWord.CtrlBit.bSampleRate == 4) {
            uSampleRate = 10000;
        } else if (tCtrlWord.CtrlBit.bSampleRate == 3) {
            uSampleRate = 8000;
        } else if (tCtrlWord.CtrlBit.bSampleRate == 2) {
            uSampleRate = 5000;
        } else if (tCtrlWord.CtrlBit.bSampleRate == 1) {
            uSampleRate = 4000;
        }
        
        INT16 uSum = 0;
        UINT16 *pReadData = new UINT16[uFrameLen/2];
        for (int i = 0; i < uSampleRate/50; i++)
        {
            if (uFrameLen != file.Read(pReadData, uFrameLen)) {
                 AfxMessageBox(_T("Read File Failed"));
                file.Close();
                delete pReadData;
                return FALSE;
            }
            for (int j = 0; j < uFrameLen/2;) {
                if (j%9 == 0) {
                    j++;                                                /*  跳过报文的起始字节、CRC校验值*/
                    continue;
                }
                memcpy(pFt3Data, &pReadData[j], 16);                    /*  按数据集拷贝数据            */
                //if (i == 0) {
                //    RETAILMSG(1, (_T("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \r\n"), 
                //                  pFt3Data[0], pFt3Data[1], pFt3Data[2], pFt3Data[3], pFt3Data[4], pFt3Data[5], pFt3Data[6], pFt3Data[7]));
                //    //Sleep(1);
                //}
                pFt3Data+= 8;
                uSum += 8;
                j+=8;
            }
            //RETAILMSG(1, (_T("\r\n")));
        }

        file.Close();
        delete pReadData;
    } 
    else 
    {
        RETAILMSG(1, (_T("Open File failed\r\n")));
        return FALSE;
    }

    return TRUE;
 
}



/*
 *  校验接收到的报文是否正确
 */
BOOL _FT3_c::VerifySmvFrame(void)
{
    DWORD dwDataLen = 0;
    UINT16 uLoopItem = m_ptMapHeader->ft3Header.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->ft3Header.uFramesSum;
    Ft3ControlWord_t tCtrlW = m_ptMapHeader->ft3Header.tCtrlWord;
    UINT16 uSmpcntPos = 0;  //按16位为单位偏移
    BOOL bFoundErr = FALSE;
    UINT16 puCmpBuf[8] = {0};
    BOOL bSkipVerifyFrame = FALSE;                                      /*  不校验报文内容，只校验采样计数器*/
    UTC_TIME_t *ptUtcTime;
    UINT16 uSampleRate = 0;
    BOOL   bVertifyRst = TRUE;
    UINT16 uErrorFrame = 0;

    if (tCtrlW.CtrlBit.bFormat == _GUO_WANG) {
        dwDataLen  = EXFT3_FRAME_LEN[_GUO_WANG]/2;
        uSmpcntPos = 8;
    } 
    else if (tCtrlW.CtrlBit.bFormat == _NAN_RUI){
        dwDataLen  = EXFT3_FRAME_LEN[_NAN_RUI]/2;
        uSmpcntPos = 25;
    }
    else {
        dwDataLen  = EXFT3_FRAME_LEN[tCtrlW.CtrlBit.bFormat]/2;
        uSmpcntPos = dwDataLen-2;
        bSkipVerifyFrame = TRUE;
    }

    switch(tCtrlW.CtrlBit.bSampleRate) {
        case _4K:
            uSampleRate = 4000;
            break;
        case _5K:
            uSampleRate = 5000;
            break;
        case _8K:
            uSampleRate = 8000;
            break;
        case _10K:
            uSampleRate = 10000;
            break;
        case _12p8K:
            uSampleRate = 12800;
            break;
        default:
            break;
    }

    PUINT16 pFt3Data;
    if (uLoopItem == 0) {
        pFt3Data = (PUINT16)(m_dataBuf+FT3_LOOP_BUF0_POS);
    } else {
        pFt3Data = (PUINT16)(m_dataBuf+FT3_LOOP_BUF1_POS);
    }

    m_uTestResult[m_nDeviceItem*2] += uFramesSum;

    for (UINT16 j = 0; j < uFramesSum; j++) {
#if ZONE_FT3_TIME_STRAP
        /*
         *  添加FT3时间戳
         */
        ptUtcTime = (UTC_TIME_t *)pFt3Data;
        pFt3Data += FT3_TIME_STRAP_LEN/2;
#endif
        if (j!=0) {
            if (!bSkipVerifyFrame) {
                if (memcmp(puCmpBuf, pFt3Data, 16) != 0) {
                    bFoundErr = TRUE;         
                    //报文最前端16个字节的内容都是一样的
                }
            }
        } else {
            if (m_bGetSmpCntVal) {
                m_uSmpCntVal = pFt3Data[uSmpcntPos];
                memcpy(&m_tUtcTime, ptUtcTime, sizeof(UTC_TIME_t));
            }
            if (!bSkipVerifyFrame) 
                memcpy(puCmpBuf, pFt3Data, 16);
        }

        if (m_uSmpCntVal != pFt3Data[uSmpcntPos]) {
            if (m_uSmpCntVal == uSampleRate && pFt3Data[uSmpcntPos] == 0) {
                m_uSmpCntVal = pFt3Data[uSmpcntPos];
            } else {
                m_uSmpCntVal = pFt3Data[uSmpcntPos];
                bFoundErr = TRUE;
            }
        }

        /*
         *  UTC Time 对比
         */
        if (!m_bGetSmpCntVal) {
            if (!VerifyUtcTime(m_tUtcTime, *ptUtcTime, uSampleRate)) {
                m_uUtcTest[m_nDeviceItem]++;
                bVertifyRst = FALSE;
            }
        }
        memcpy(&m_tUtcTime, ptUtcTime, sizeof(UTC_TIME_t));

        if (bFoundErr) {
            bFoundErr = FALSE;
            m_uTestResult[m_nDeviceItem*2+1]++;
            uErrorFrame++;
            bVertifyRst = FALSE;
        }
        m_uSmpCntVal++;       //累加
        pFt3Data += dwDataLen;
        m_bGetSmpCntVal = FALSE;
    }


    //UINT32 fScale = (g_uTestResult[m_nDeviceItem*2+1]*10000 / g_uTestResult[m_nDeviceItem*2]);
    RETAILMSG(ZONE_VERIFY_INFO, (_T("Device%d: Sum:%d  Err:%d  UTC Err: %d\r\n"), m_nDeviceItem, 
        m_uTestResult[m_nDeviceItem*2], m_uTestResult[m_nDeviceItem*2+1], m_uUtcTest[m_nDeviceItem]));

    return bVertifyRst;
}
/*
 *  从串口中打印接收到的报文
 */
void _FT3_c::PrintFrame(void)
{
    DWORD dwDataLen = 0;
    UINT16 uLoopItem = m_ptMapHeader->ft3Header.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->ft3Header.uFramesSum;
    Ft3ControlWord_t tCtrlW = m_ptMapHeader->ft3Header.tCtrlWord;
    UINT32 uActiveLen = m_ptMapHeader->ft3Header.uActiveLength;

//     if (tCtrlW.CtrlBit.bFormat == GUO_WANG) {
//         dwDataLen  = GUOWANG_FRAME_LEN/2;
//     } else {
//         dwDataLen  = NANRUI_FRAME_LEN/2;
//     }
    dwDataLen = EXFT3_FRAME_LEN[tCtrlW.CtrlBit.bFormat]/2;
    RETAILMSG(1, (_T("Control = 0x%x  uLoopItem = %d  FramesSum = %d,  FrameLen = %d, ActiveLen = %d\r\n"), 
        tCtrlW.data, uLoopItem, uFramesSum, dwDataLen, uActiveLen));

    PUINT16 pFt3Data;
    if (uLoopItem == 0) {
        pFt3Data = (PUINT16)(m_dataBuf+FT3_LOOP_BUF0_POS);
    } else {
        pFt3Data = (PUINT16)(m_dataBuf+FT3_LOOP_BUF1_POS);
    }
    for (UINT16 j = 0; j < uFramesSum; j++) {
#if ZONE_FT3_TIME_STRAP
        /*
         *  添加FT3时间戳
         */ 
        {
             PUINT8 pTmp = (PUINT8)pFt3Data;
             UTC_TIME_t *putcTime = (UTC_TIME_t *)pFt3Data;
             CTime tm(putcTime->uSecondSinceEpoch);
             CString strTmp;
             RETAILMSG(1, (_T("UTC Time: %02X %02X %02X %02X %02X %02X %02X %02X \r\n"), \
                 pTmp[0], pTmp[1], pTmp[2], pTmp[3], pTmp[4], pTmp[5], pTmp[6], pTmp[7]));

             RETAILMSG(1, (_T("%04d-%02d-%02d %02d:%02d:%02d.%d\r\n"), 
                 tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond(), putcTime->uFractionOfSecond));

             pFt3Data += FT3_TIME_STRAP_LEN/2;
        }
#endif
        for (DWORD i = 0; i < dwDataLen; i++) {
            RETAILMSG(1, (_T("0x%04x "), pFt3Data[i]));
        }
        pFt3Data += dwDataLen;
        RETAILMSG(1, (_T("\r\n")));
        Sleep(1);
    }
}
/*
 *  录取接收到的报文 
 */


BOOL _FT3_c::userFt3Proc(PVOID pVoid)
{
    if (!m_bRecv) {
        RETAILMSG(1, (_T("APP:Warning _FT3_c::userFt3Proc Device_%d Not Work!!!\r\n"), m_nDeviceItem));
        Sleep(1000);
        return FALSE;
    }
    LockInter();
    /*
     *  FT3接收超时处理  停止接收并通知用户
     */
    if (m_ptMapHeader->ft3Header.uIsTimeOut == 1) {
//         IOCTRL_IN_PARAM_t tFt3InParam;
//         tFt3InParam.uDeviceItem = m_nDeviceItem;
//         DeviceIoControl(m_hDevice, IOCTRL_STOP_RECV, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL);
        m_ptMapHeader->ft3Header.uIsTimeOut = 0;
        UnLockInter();
        return TRUE;
//         RETAILMSG(1, (_T("Make Sure that the connection of FT3 Fiber is OK!!!\r\n")));
    } else {
        /*
         *  不想循环接收 可以收满了就停止
         */
      /*  if (g_bDisableLoopMode) {
            IOCTRL_IN_PARAM_t tFt3InParam;
            tFt3InParam.uDeviceItem = m_nDeviceItem;
            DeviceIoControl(m_hDevice, IOCTRL_STOP_RECV, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL);
        }*/
    }

 //   PrintFrame();

    VerifySmvFrame();

    GetFT3RxFrame(m_pFt3TxData);

    UnLockInter();

    return TRUE;
}


DWORD ft3ThreadProcRx1(PVOID pArg)
{
    _FT3_c *pFt3 = (_FT3_c *)pArg;
    DWORD dwRet = 0;
    HANDLE hEvt[] = {pFt3->m_hNotifyEvent, pFt3->m_hExitEvent};

    RETAILMSG(ZONE_INFO, (_T("APP:ft3ThreadProcRx1\r\n")));
    while (TRUE)
    {
        /*
        **  等待接收事件、线程退出事件
        */
        dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);

    
        if (dwRet == WAIT_OBJECT_0) 
        {

            SetEvent(pFt3->m_hRxEvent);
            Sleep(2);
           
        }
        else if(dwRet == WAIT_OBJECT_0 + 1)
        {
            /*  退出线程                    */
            break;
        }
        else 
        {
            RETAILMSG(ZONE_ERROR, (_T("APP: ft3ThreadProcRx1 found error\r\n")));
        }
    }
    RETAILMSG(ZONE_INFO, (_T("APP: ft3ThreadProcRx1 exit\r\n")));

    return 1;
}

DWORD ft3ThreadProcRx2(PVOID pArg)
{
    _FT3_c *pFt3 = (_FT3_c *)pArg;
    DWORD dwRet = 0;
    HANDLE hEvt[] = {pFt3->m_hNotifyEvent, pFt3->m_hExitEvent};

    RETAILMSG(ZONE_INFO, (_T("APP:ft3ThreadProcRx2\r\n")));
    while (TRUE)
    {
        /*
        **  等待接收事件、线程退出事件
        */
        dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);


        if (dwRet == WAIT_OBJECT_0) 
        {

            SetEvent(pFt3->m_hRxEvent);
            Sleep(2);

        }
        else if(dwRet == WAIT_OBJECT_0 + 1)
        {
            /*  退出线程                    */
            break;
        }
        else 
        {
            RETAILMSG(ZONE_ERROR, (_T("APP: ft3ThreadProcRx2 found error\r\n")));
        }
    }
    RETAILMSG(ZONE_INFO, (_T("APP: ft3ThreadProcRx2 exit\r\n")));

    return 1;
}
#include "stdafx.h"
#include "windows.h"
//#include "pkfuncs.h"

#include "winioctl.h"

#include "gpmcfg.h"
#include "fiber.h"

/*
 *  У��UTCʱ��
 */
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

/*
 *  ����B��
 */
bool _FT3_c::SendBcode()
{
    FT3_IRIGB_TX_STYLE_e eTxStyle = _TX_IRIGB;
    if (!DeviceIoControl(m_hDevice, IOCTRL_SET_FT3_TX_STYLE, (PBYTE)&eTxStyle, sizeof(eTxStyle), NULL, 0, NULL, NULL))
    {
        RETAILMSG(TRUE, (TEXT("DeviceIoControl(IOCTRL_SET_FT3_TX_STYLE, ...) failed.\r\n")));
        return FALSE;
    }
    return TRUE;
}

/*
 *  ֹͣB��ķ���
 */
bool _FT3_c::StopBcode()
{
    FT3_IRIGB_TX_STYLE_e eTxStyle = _TX_FT3;
    if (!DeviceIoControl(m_hDevice, IOCTRL_SET_FT3_TX_STYLE, (PBYTE)&eTxStyle, sizeof(eTxStyle), NULL, 0, NULL, NULL))
    {
        RETAILMSG(TRUE, (TEXT("DeviceIoControl(IOCTRL_SET_FT3_TX_STYLE, ...) failed.\r\n")));
        return FALSE;
    }
    return TRUE;
}

/*
 *  ����FT3����
 */
bool _FT3_c::Ft3Tx()
{
    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = DEVICE_FT3_TX;
    memcpy(tFt3InParam.uInBuf, &m_tTxCfg, sizeof(FT3_TX_CFG_t));

    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SEND_HEADER, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        RETAILMSG(TRUE, (TEXT("DeviceIoControl:IOCTRL_SEND_HEADER failed\r\n")));
        return FALSE;
    }

    /*
     *  IOCTRL_SEND_FRAME ����  m_Ft3Tx.m_ptMapHeader->uCurTxBufItem�ĳ�ʼ����
     *  ���ڱ��ĸ�ʽ�������ʡ������ʡ����ĳ��ȵȣ������仯ʱ�ŵ��ã�
     *  ���ĸ�ʽ���䣬���������ݱ仯ʱ��ֱ�ӵ���IOCTRL_CHANGE_MU_FRAME�����
     */
    m_ptMapHeader->ft3Header.uCurSmvTxBufItem = 0; 
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SEND_FRAME, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        RETAILMSG(TRUE, (TEXT("DeviceIoControl:IOCTRL_SEND_FRAME failed\r\n")));
        return FALSE;
    }
    return TRUE;
}

/*
 *  ����FT3����
 */
bool _FT3_c::Ft3Rx()
{
    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = m_nDeviceItem;
    tFt3InParam.uInBuf[0] = 800;
    tFt3InParam.uInBuf[1] = 1;      
    tFt3InParam.uInBuf[2] = tFt3InParam.uInBuf[3] = 0;

    m_bRecv = TRUE;
    m_bGetSmpCntVal = TRUE;
 
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SET_RECV_LOOP, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        return FALSE;
    }

    return TRUE;
}

/*
 *  ֹͣ����FT3����
 */
bool _FT3_c::Ft3RxStop()
{
    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = m_nDeviceItem;
    m_bRecv = FALSE;
 
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_STOP_RECV, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        return FALSE;
    }

    return TRUE;
}

/*
 *  ֹͣ����FT3����
 */
bool _FT3_c::Ft3StopTx()
{
    IOCTRL_IN_PARAM_t tFt3InParam;
    tFt3InParam.uDeviceItem = DEVICE_FT3_TX;
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_STOP_SEND, &tFt3InParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        return FALSE;
    }
    return TRUE;
}

/*
 *  ���÷��ͱ��ĵ�����
 */
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

/*
 *  �ӽ��յ��ı�����ת��Ϊ���ͱ��ĸ�ʽ
 */
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

    dwDataLen = EXFT3_FRAME_LEN[tCtrlW.CtrlBit.bFormat]/2;

    /*RETAILMSG(1, (_T("Control = 0x%x  uLoopItem = %d  FramesSum = %d,  FrameLen = %d, ActiveLen = %d\r\n"), 
        tCtrlW.data, uLoopItem, uFramesSum, dwDataLen, uActiveLen));*/

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
         *  ���FT3ʱ���
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
                pFt3Data++;/*  �������ĵ���ʼ�ֽڡ�CRCУ��ֵ*/
                continue;
            }
        memcpy(pFt3TxData, pFt3Data, 16);  

        pFt3TxData += 8;
       
        uSum += 8;
        pFt3Data += 8;
        j += 8;
        Sleep(1);
        }
    }
    return TRUE;
}

/*
 *  ����FT3�������ݵ���ʼ��ַ
 */
void _FT3_c::SetFt3TxAddress( PVOID pData )
{
    m_pFt3TxData = (PUINT16)pData;
}

/*
 *  ����FT3����
 */
void _FT3_c::SetFT3TxInfo(Ft3SendCtlWord_t tCtrlW)
{

    RETAILMSG(ZONE_INFO, (_T("APP:FT3 FT3SendCtlWord is 0x%x\r\n"), tCtrlW.wData));

    m_tTxCfg.uFramesStyle = (UINT16)tCtrlW.wData;
}

/*
 *  ��ȡFT3���Ͳ���
 */
void _FT3_c::GetFT3TxInfo( Ft3SendCtlWord_t &tSendCtrlW )
{
    Ft3ControlWord_t tCtrlW = m_ptMapHeader->ft3Header.tCtrlWord;
    tSendCtrlW.CtrlBit.bBaudRate = tCtrlW.CtrlBit.bBaudRate;
    tSendCtrlW.CtrlBit.bSampleRate = tCtrlW.CtrlBit.bSampleRate;
    tSendCtrlW.CtrlBit.bFrameStyle = tCtrlW.CtrlBit.bFormat;
}

/*
 *  ���ļ��л�ȡFT3����
 */
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
         *  ���ṩ�����𡢹������ֱ����ļ���������չ��4��FT3���ģ���ͨ����������ת������
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
                    j++;                                                /*  �������ĵ���ʼ�ֽڡ�CRCУ��ֵ*/
                    continue;
                }
                memcpy(pFt3Data, &pReadData[j], 16);                    /*  �����ݼ���������            */
                if (i == 0) {
                    RETAILMSG(1, (_T("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \r\n"), 
                                  pFt3Data[0], pFt3Data[1], pFt3Data[2], pFt3Data[3], pFt3Data[4], pFt3Data[5], pFt3Data[6], pFt3Data[7]));
                    //Sleep(1);
                }
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
        AfxMessageBox(_T("Open File failed\r\n"));
        return FALSE;
    }

    return TRUE;
 
}



/*
 *  У����յ��ı����Ƿ���ȷ
 */
BOOL _FT3_c::VerifySmvFrame(void)
{
    DWORD dwDataLen = 0;
    UINT16 uLoopItem = m_ptMapHeader->ft3Header.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->ft3Header.uFramesSum;
    Ft3ControlWord_t tCtrlW = m_ptMapHeader->ft3Header.tCtrlWord;
    UINT16 uSmpcntPos = 0;  //��16λΪ��λƫ��
    BOOL bFoundErr = FALSE;
    UINT16 puCmpBuf[8] = {0};
    BOOL bSkipVerifyFrame = FALSE;                                      /*  ��У�鱨�����ݣ�ֻУ�����������*/
    UTC_TIME_t *ptUtcTime;
    UINT16 uSampleRate = 0;
    BOOL   bVertifyRst = TRUE;

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
         *  ���FT3ʱ���
         */
        ptUtcTime = (UTC_TIME_t *)pFt3Data;
        pFt3Data += FT3_TIME_STRAP_LEN/2;
#endif
        if (j!=0) {
            if (!bSkipVerifyFrame) {
                if (memcmp(puCmpBuf, pFt3Data, 16) != 0) {
                    bFoundErr = TRUE;                                   //������ǰ��16���ֽڵ����ݶ���һ����
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
         *  UTC Time �Ա�
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
            bVertifyRst = FALSE;
        }
        m_uSmpCntVal++;       //�ۼ�
        pFt3Data += dwDataLen;
        m_bGetSmpCntVal = FALSE;
    }


    //UINT32 fScale = (g_uTestResult[m_nDeviceItem*2+1]*10000 / g_uTestResult[m_nDeviceItem*2]);
    RETAILMSG(ZONE_VERIFY_INFO, (_T("Device%d: Sum:%d  Err:%d  UTC Err: %d\r\n"), m_nDeviceItem, 
        m_uTestResult[m_nDeviceItem*2], m_uTestResult[m_nDeviceItem*2+1], m_uUtcTest[m_nDeviceItem]));



    return bVertifyRst;
}
/*
 *  �Ӵ����д�ӡ���յ��ı���
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
         *  ���FT3ʱ���
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
 *  ¼ȡ���յ��ı��� 
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
     *  FT3���ճ�ʱ����  ֹͣ���ղ�֪ͨ�û�
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
         *  ����ѭ������ ���������˾�ֹͣ
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
    bool bRecv = FALSE;

    RETAILMSG(ZONE_INFO, (_T("APP:ft3ThreadProcRx1\r\n")));
    while (TRUE)
    {
        /*
        **  �ȴ������¼����߳��˳��¼�
        */
        if(FALSE == bRecv)
        {
            dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);
        }
        else
        {
            dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, RECV_TIME_OUT);
        }
        if (dwRet == WAIT_OBJECT_0) 
        {
        
            if(TRUE == bRecv)
            {
                Sleep(5);
                continue;
            }
            ::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, 
            WM_STOP_TX, 0, 0);
            DWORD dwStopRet = WaitForSingleObject(pFt3->m_hStopTxEvent, RECV_TIME_OUT);
            if(dwStopRet == WAIT_TIMEOUT)
            {
                bRecv = FALSE;
                continue;
            }
            else if(dwStopRet == WAIT_OBJECT_0)
            {
                ResetEvent(pFt3->m_hStopTxEvent);
            }
            
            pFt3->userFt3Proc(NULL);
            ::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, 
                WM_CHANGE_TX, 0, 0);
//            Sleep(50);
            bRecv = TRUE; 

        } 
        else if (dwRet == WAIT_TIMEOUT) 
        {                                                               
            bRecv = FALSE;
            ::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, 
                WM_RECOVER_TX, 0, 0);
        }
        else if(dwRet == WAIT_OBJECT_0 + 1)
        {
            /*  �˳��߳�                    */
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
    bool bRecv = FALSE;

    RETAILMSG(ZONE_INFO, (_T("APP:ft3ThreadProcRx2\r\n")));
    while (TRUE)
    {
        /*
        **  �ȴ������¼����߳��˳��¼�
        */
        if(FALSE == bRecv)
        {
            dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);
        }
        else
        {
            dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, RECV_TIME_OUT);
        }
        if (dwRet == WAIT_OBJECT_0) 
        {

            if(FALSE == bRecv)
            {

                ::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, 
                    WM_STOP_TX, 0, 1);
                DWORD dwStopRet = WaitForSingleObject(pFt3->m_hStopTxEvent, RECV_TIME_OUT);
                if(dwStopRet == WAIT_TIMEOUT)
                {
                    bRecv = FALSE;
                    continue;
                }
                else if(dwStopRet == WAIT_OBJECT_0)
                {
                    ResetEvent(pFt3->m_hStopTxEvent);
                }
            }
            pFt3->userFt3Proc(NULL);
            if(FALSE == bRecv)
            {
                ::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, 
                    WM_CHANGE_TX, 0, 0);
//                Sleep(50);
            }
            bRecv = TRUE; 

        } 
        else if (dwRet == WAIT_TIMEOUT) 
        {                                                               
            bRecv = FALSE;
            ::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, 
                WM_RECOVER_TX, 0, 0);
        }
        else if(dwRet == WAIT_OBJECT_0 + 1)
        {
            /*  �˳��߳�                    */
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
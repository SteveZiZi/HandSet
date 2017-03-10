#include "stdafx.h"
#include "windows.h"
#include "winioctl.h"

#include "fiber.h"




static BOOL VerifyGooseFrame(IN PVOID pData, IN UINT16 uSize, OUT UINT16 *puStNumPos, OUT UINT16 *puSqNumPos);


PCAP_HEADER_t pCapHeader = {0xA1B2C3D4, 0x02, 0x04, 0x00, 0x00, 0xFFFF, 0x01};

/********************************************************************************************************/
/*  Net_Base_c 类成员函数实现区                                                                         */
/********************************************************************************************************/
Net_Base_c::Net_Base_c(HANDLE hDevice, UINT16 nDeviceItem) 
{
    m_hMapping      = INVALID_HANDLE_VALUE;
    m_hNotifyEvent  = INVALID_HANDLE_VALUE;
    m_dataBuf       = NULL;

    m_hRxThread     = INVALID_HANDLE_VALUE;
    m_hExitEvent    = INVALID_HANDLE_VALUE;

    m_hDevice       = hDevice;
    m_nDeviceItem   = nDeviceItem;

    m_hRxEvent  = INVALID_HANDLE_VALUE;

    memset(m_uTestResult, 0, sizeof(m_uTestResult));
    memset(m_uUtcTest, 0, sizeof(m_uUtcTest));
}


Net_Base_c::~Net_Base_c()
{
    DeInit();
}

BOOL Net_Base_c::Init(WCHAR *pNotifyEvtName, WCHAR *pMapFileName, UINT32 nMapFileLen, LPTHREAD_START_ROUTINE lpStartAddress)
{

    InitializeCriticalSection(&m_cs);
    //创建通道x用到的内存映射
    m_hMapping=CreateFileMapping(INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE|SEC_COMMIT,
        0,
        nMapFileLen,
        pMapFileName);
    if (m_hMapping==INVALID_HANDLE_VALUE)
    {
        RETAILMSG(1,(TEXT("APP: Goose: Create FileMapping for Channel FAILED")));
        goto Init_Failed;
    }

 
    m_ptMapHeader = (Map_File_Header_t *)MapViewOfFile(m_hMapping,FILE_MAP_WRITE|FILE_MAP_READ, 0,0,0);
    if (m_ptMapHeader==NULL)
    {
        RETAILMSG(1,(TEXT("APP: Goose: FileMapping for Channel map FAILED")));
        goto Init_Failed;
    }
    m_dataBuf = (PBYTE)m_ptMapHeader;
    m_dataBuf += sizeof(Map_File_Header_t);

    m_hRxEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
    //创建用于通知通道 接收操作完成的事件对象
    m_hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, pNotifyEvtName);
    if (m_hNotifyEvent == INVALID_HANDLE_VALUE) {
        RETAILMSG(ZONE_ERROR, (_T("APP:  Net_Base_c::Init: Open NotifyEvent failed,error code is %d\r\n"), GetLastError()));
        goto Init_Failed;
    } else {
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            RETAILMSG(1, (_T("APP: Please sure that OS had Installed the GPM Driver\r\n")));
            goto Init_Failed;
        }
        RETAILMSG(1, (_T("APP:m_hNotifyEvt = 0x%x NotifyName = %s\r\n"),m_hNotifyEvent, pNotifyEvtName));
    }

    if (lpStartAddress != NULL) {
        m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (m_hExitEvent == INVALID_HANDLE_VALUE) {
            RETAILMSG(ZONE_ERROR, (_T("APP: Net_Base_c::Init: Create Exit Event failed\r\n")));
            goto Init_Failed;
        }
        if (m_hRxThread == INVALID_HANDLE_VALUE) {
            m_hRxThread = CreateThread(NULL, 0, lpStartAddress, this, NULL, NULL);
            if (m_hRxThread == INVALID_HANDLE_VALUE) {
                RETAILMSG(ZONE_ERROR, (_T("APP: Create Thread Failed\r\n")));
                goto Init_Failed;
            } else {
                if(CeSetThreadPriority(m_hRxThread, APP_PRIO)){
                    RETAILMSG(ZONE_INFO, (_T("APP: Net_Base_c::Init  Rx Priority:%d\r\n"),CeGetThreadPriority(m_hRxThread)));
                }
            }
        }
    }


    return TRUE;
Init_Failed:
    DeInit();
    return FALSE;
}

void Net_Base_c::GetSmvVertifyRst( UINT32 &uTotalFrame, UINT32 &uErrorFrame )
{
    uTotalFrame = m_uTestResult[m_nDeviceItem * 2];
    uErrorFrame = max(m_uTestResult[m_nDeviceItem * 2 + 1], m_uTestResult[m_nDeviceItem]);
}


VOID Net_Base_c::DeInit()
{
    ResetNet();
    if (m_hExitEvent != INVALID_HANDLE_VALUE) {
        SetEvent(m_hExitEvent);                                        /*  发送线程推出事件            */
    }
    if (m_hRxThread != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hRxThread);
        m_hRxThread = INVALID_HANDLE_VALUE;
    }
    if (m_hExitEvent != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hExitEvent);
        m_hExitEvent = INVALID_HANDLE_VALUE;
    }

    if (m_hNotifyEvent != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hNotifyEvent);
        m_hNotifyEvent = INVALID_HANDLE_VALUE;
    }

    if (m_ptMapHeader != NULL) {
        UnmapViewOfFile(m_ptMapHeader);
        m_ptMapHeader = NULL;
        m_dataBuf = NULL;
    }
    if (m_hMapping != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hMapping);
        m_hMapping = INVALID_HANDLE_VALUE;
    }

    DeleteCriticalSection(&m_cs);
}

void Net_Base_c::ResetNet()
{

}

void Net_Base_c::SetFilePath( CString strFilePath )
{
    m_strFilePath = strFilePath;
}


void _SMV_92_c::SetTxParam( IOCTRL_FIBER_IN_PARAM_t & tMuSvTxParam ,  bool bFromFile)
{
    m_tMuSvTxParam = tMuSvTxParam;
    m_bFromFile = bFromFile;
}

void _SMV_92_c::SetFrameFlag( UINT8 uFrameFlag )
{
    m_uFrameFlag = uFrameFlag;
}


bool _SMV_92_c::MuTx()
{
    bool bIsSmv = (m_tMuSvTxParam.uInBuf[0] == 0);
   

    if(bIsSmv)
    {
        m_ptMapHeader->OpticalNetHeader.uCurSmvTxBufItem = 0; 
        if(m_bFromFile)
        {
            if (FALSE == GetMuTxFrame(m_dataBuf+SMV_TX_BUF0_POS)) {
                return FALSE;
            }
        }
        
    }
    else
    {
        m_ptMapHeader->OpticalNetHeader.uCurGooseTxBufItem = 0; 
        if (FALSE == GetGooseTxFrame(m_dataBuf + GOOSE_TX_BUF0_POS)) {
            return FALSE;
        }
    }

    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SEND_HEADER, &m_tMuSvTxParam, sizeof(m_tMuSvTxParam), NULL,NULL,NULL,NULL) ) {
 //       MessageBox(L"DeviceIoControl:IOCTRL_SEND_HEADER failed");
        return  FALSE;
    }

    IOCTRL_IN_PARAM_t tInParam;
    tInParam.uDeviceItem = m_nDeviceItem;
    tInParam.uInBuf[0] = m_tMuSvTxParam.uInBuf[0];

    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SEND_FRAME, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
//        MessageBox(L"DeviceIoControl:IOCTRL_SEND_FRAME failed");
        return FALSE;
    }

    //    pFiber->VerifySmvFrame();

    return TRUE;
}

static PCAP_HEADER_t s_pCapHeader = {0xA1B2C3D4, 0x02, 0x04, 0x00, 0x00, 0xFFFF, 0x01};

bool _SMV_92_c::VertifyBcodeTime()
{
    if(FALSE == DeviceIoControl(m_hDevice, IOCTRL_GET_UTC_TIME, NULL,NULL, &m_tUtcTime, sizeof(UTC_TIME_t),NULL,NULL) )
    {
        return FALSE;
    }

    UINT16 uLoopItem = m_ptMapHeader->OpticalNetHeader.uLoopItem;
    PSMVFrame_t pSmvFrame = NULL;

    if (uLoopItem == 0) {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF0_POS);
    } else {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF1_POS);
    }
    UTC_TIME_t tUtcTime;
    memcpy(&tUtcTime, (UTC_TIME_t*)pSmvFrame->uData, sizeof(UTC_TIME_t));
    return VerifyUtcTime(tUtcTime, m_tUtcTime, 1000);
}


BOOL _SMV_92_c::GetMuTxFrame( PVOID pData )
{
    CFile file;
    UINT32 uByteSum = 0;


    if (file.Open((LPCTSTR)m_strFilePath, CFile::modeRead, NULL)) {
        PCAP_HEADER_t pCapHeader;
        file.Read(&pCapHeader, sizeof(PCAP_HEADER_t));
        if (memcmp(&pCapHeader,&s_pCapHeader, sizeof(PCAP_HEADER_t)) != 0) {
//             MessageBox(_T("the file is not pcap file"));
            return FALSE;
        }

        UINT16 uSmvNumPerGroup = (UINT16)m_tMuSvTxParam.uInBuf[1];
        UINT16 uSmvAsduNum = (UINT16)(m_tMuSvTxParam.uInBuf[1]>>16);
        if (uSmvNumPerGroup > MAX_TX_FRAMES_CNT ||
            uSmvAsduNum == 0 || uSmvAsduNum > MAX_ASDU_NUM) {
            return FALSE;
        }

        UINT16 uCircleSum = (UINT16)m_tMuSvTxParam.uInBuf[2];
        UINT8 uUTCTime[8];
        UINT32 uFrameLen = 0;
        PUINT8 pMuTxData = (PUINT8)pData;
        m_pTempTxData = pMuTxData;
        
        UINT32 uTxSumFrameLen = 0;
        BOOL bMacAppidMatched = FALSE;
        
        if (uSmvNumPerGroup > 1) {
            Filter_t tMacAppid[2];
            for(int i = 0; i < 2; i++) {
                file.Read(uUTCTime, sizeof(uUTCTime));
                file.Read(&uFrameLen, sizeof(uFrameLen));
                file.Read(&uFrameLen, sizeof(uFrameLen));
                file.Read(pMuTxData, uFrameLen);
                memcpy(tMacAppid[i].MacAddr, &pMuTxData[5], sizeof(tMacAppid[i].MacAddr));
                memcpy(tMacAppid[i].MacAppid, &pMuTxData[19], sizeof(tMacAppid[i].MacAppid));
            }
            if (0 == memcmp(&tMacAppid[0], &tMacAppid[1], sizeof(Filter_t))) {
                bMacAppidMatched = TRUE;
            }
        }

        for (UINT16 i = 0; i < uCircleSum; i++) {
            for (UINT16 j = 0; j < uSmvNumPerGroup; j++) {
                if (!bMacAppidMatched || j==0 ) {
                    file.Read(uUTCTime, sizeof(uUTCTime));
                    file.Read(&uFrameLen, sizeof(uFrameLen));
                    file.Read(&uFrameLen, sizeof(uFrameLen));
                } else {
                    file.Seek(16, CFile::current);
                }

                uTxSumFrameLen += uFrameLen;
                if (uTxSumFrameLen > SMV_TX_BUF_LEN) {
//                     MessageBox(_T("发送的报文过长，请重新设置发送参数"));
                    file.Close();
                    return FALSE;
                }

                if (i == 0) {
                    m_tMuSvTxParam.uInBuf[j+4] = uFrameLen; 
                }

                
                file.Read(pMuTxData, uFrameLen);
                pMuTxData[0] = m_uFrameFlag;
                if (bMacAppidMatched) {
                    pMuTxData[5] = (UINT8)j;
                    pMuTxData[19] = (UINT8)j;
                }

                /*
                 *  确保填写的报文按照4字节对齐
                 */
                pMuTxData += ((uFrameLen&0x03) != 0) ? ((uFrameLen&0xFFFC)+4) : uFrameLen;
                uByteSum += ((uFrameLen&0x03) != 0) ? ((uFrameLen&0xFFFC)+4) : uFrameLen;
            }
        }

        file.Close();
    } else {
//         MessageBox(_T("Open File failed\r\n"));
        return FALSE;
    }
  
    return TRUE;
}

BOOL _SMV_92_c::GetGooseTxFrame( OUT PVOID pData, IN BOOL bUpdate /*= FALSE*/ )
{
    CFile file;

    static BOOL bChangeMAc = FALSE;
    static UINT32 s_uStNum[10] = {0};

    if (file.Open((LPCTSTR)m_strFilePath, CFile::modeRead, NULL)) {
        PCAP_HEADER_t pCapHeader;
        file.Read(&pCapHeader, sizeof(PCAP_HEADER_t));
        if (memcmp(&pCapHeader,&s_pCapHeader, sizeof(PCAP_HEADER_t)) != 0) {
//             MessageBox(_T("the file is not pcap file"));
            return FALSE;
        }

        m_uGooseTxCnt = (UINT16)m_tMuSvTxParam.uInBuf[3];
        if (m_uGooseTxCnt > MAX_TX_FRAMES_CNT) {
            return FALSE;
        }
        UINT8 uUTCTime[8];
        UINT32 uFrameLen = 0;
        PUINT8 pMuTxData = (PUINT8)pData;
        UINT16 uStNumPos = 0;

        for (UINT16 j = 0; j < m_uGooseTxCnt; j++) {
            file.Read(uUTCTime, sizeof(uUTCTime));
            file.Read(&uFrameLen, sizeof(uFrameLen));
            file.Read(&uFrameLen, sizeof(uFrameLen));

            m_tMuSvTxParam.uInBuf[j+4] = uFrameLen; 
 
            file.Read(pMuTxData, uFrameLen);

            if (::VerifyGooseFrame(pMuTxData, uFrameLen, &uStNumPos, NULL) == FALSE) {
                file.Close();
//                 MessageBox(_T("the File is not GOOSE Frame\r\n"));
                return FALSE;
            }
            /*
             *  使每个GOOSE的MAC APPID都不一样
             */
            pMuTxData[5] = 1+j;
            pMuTxData[19] = 1+j;
            /*
             *  应用需要维护StNum值
             */
            if (!bUpdate) {
                int i = 1;                                              /*  跳过表示StNum字节数的那个值 */
                while (i <= pMuTxData[uStNumPos]-1) {
                    pMuTxData[uStNumPos+i] = 0x00;
                    i++;
                }
                pMuTxData[uStNumPos+i] = 0x01;                          /*  StNum 初值为1               */
                s_uStNum[j] = 0x01;
            } 
            else {
                if (bChangeMAc) {
//                     pMuTxData[5] = 6+j; //test
//                     pMuTxData[19] = 6+j;//test
                }
                s_uStNum[j]++;
                int i = 1;
                UINT32 uStNum = s_uStNum[j];
                if (pMuTxData[uStNumPos] <= 4) {
                    i = pMuTxData[uStNumPos];
                    while (i > 0) {
                        pMuTxData[uStNumPos+i] = (UINT8)uStNum;
                        uStNum >>= 8;
                        i--;
                    }
                } else { 
                    i = pMuTxData[uStNumPos];
                    while (i > pMuTxData[uStNumPos]-4) {
                        pMuTxData[uStNumPos+i] = (UINT8)uStNum;
                        uStNum >>= 8;
                        i--;
                    }
                }
            }
//             RETAILMSG(1, (_T("Change GOOSE: %x "), pMuTxData[uStNumPos]));
//             for (int i = 1; i <= pMuTxData[uStNumPos]; i++) {
//                 RETAILMSG(1, (_T("%x "), pMuTxData[uStNumPos+i]));
//             }
//             RETAILMSG(1, (_T("\r\n")));

            /*
             *  确保填写的报文按照4字节对齐
             */
            uFrameLen = ((uFrameLen&0x03) != 0) ? ((uFrameLen&0xFFFC)+4) : uFrameLen;
            pMuTxData += uFrameLen;
        }
        if (bChangeMAc) {
            bChangeMAc = FALSE;
        } else {
            bChangeMAc = TRUE;
        }


        file.Close();
    } else {
//         MessageBox(_T("Open File failed\r\n"));
        return FALSE;
    }
    return TRUE;
}

bool _SMV_92_c::StartMuDetect()
{

    IOCTRL_IN_PARAM_t tInParam;
    m_RxStyle = (FIBER_RX_STYLE_e)m_tMuSvTxParam.uInBuf[0];
    tInParam.uDeviceItem = m_nDeviceItem;
    tInParam.uInBuf[0] = m_tMuSvTxParam.uInBuf[0];

    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_DETECT, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        RETAILMSG(1, (_T("DeviceIoControl:IOCTRL_DETECT failed\r\n")));
        return FALSE;
    }


    //     GetDlgItemText(IDC_EDT_MU_RX_TMEOUT, strTmp);
    //     nTmp = _wtoi(strTmp.GetBuffer());
    //     tInParam.uInBuf[0] = (UINT32)nTmp;
    //     UINT32 nTimeOut = tInParam.uInBuf[0];

    //     if (nTimeOut >= 1 && nTimeOut <= 21474) {                           /*  MU接收超时时间最大21474ms   */
    //         if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SET_RX_TIMEOUT, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
    //             MessageBox(L"DeviceIoControl:IOCTRL_SEND_FRAME failed");
    //             return;
    //         }
    //     }


    //     if (nTimeOut == 0) {       
    //         tInParam.uInBuf[1] = 0;
    //     } else {
    tInParam.uInBuf[1] = 1;                                         /*  使能超时中断 具体时间驱动固定*/
    //     }

    m_bRecv = TRUE;
    m_bGetSmpCntVal = TRUE;
    m_bGetGooseVal = TRUE;
    m_uTestResult[m_nDeviceItem*2] = 0;
    m_uTestResult[m_nDeviceItem*2+1] = 0;
    m_uUtcTest[m_nDeviceItem] = 0;
        
    if(m_tMuSvTxParam.uInBuf[0] == 0)
    {
        tInParam.uInBuf[0] = 800;
    }
    else
    {
        tInParam.uInBuf[0] = 5;
    }

    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_SET_RECV_LOOP, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
//         MessageBox(L"DeviceIoControl:IOCTRL_SEND_FRAME failed");
        return FALSE;
    }
    return TRUE;
}

bool _SMV_92_c::StopMuDetect()
{
    IOCTRL_IN_PARAM_t tInParam;
    tInParam.uDeviceItem = m_nDeviceItem;
    m_bRecv = FALSE;
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_STOP_RECV, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        RETAILMSG(1, (_T("DeviceIoControl:IOCTRL_STOPDETECT failed\r\n")));
        return FALSE;
    }
    return TRUE;
}

bool _SMV_92_c::MuStop()
{
    IOCTRL_IN_PARAM_t tInParam;
    tInParam.uDeviceItem = m_nDeviceItem;
    tInParam.uInBuf[0] = m_tMuSvTxParam.uInBuf[0];
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_STOP_SEND, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL) ) {
        RETAILMSG(1, (_T("DeviceIoControl:IOCTRL_STOP_SEND failed\r\n")));
        return FALSE;
    }
    return TRUE;
}

BOOL _SMV_92_c::GetMuRxFrame( PVOID pData )
{
    UINT16 uLoopItem = m_ptMapHeader->OpticalNetHeader.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->OpticalNetHeader.uFramesSum;
    UINT32 uActiveLen = m_ptMapHeader->OpticalNetHeader.uActiveLength;

    RETAILMSG(1, (_T("Device%d: uLoopItem = %d  FramesSum = %d  ActiveLength = %d \r\n"), m_nDeviceItem, uLoopItem, uFramesSum, uActiveLen));

    

    PSMVFrame_t pSmvFrame = NULL;
    PUINT16 pTempData = NULL;
    if (uLoopItem == 0) {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF0_POS);
        pTempData = (PUINT16)(m_dataBuf+FIBER_LOOP_BUF0_POS);
    } else {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF1_POS);
        pTempData = (PUINT16)(m_dataBuf+FIBER_LOOP_BUF1_POS);
    }

    PUINT16 pSmvData = (PUINT16)(pData);

    UINT32 uByteSum = 0;
    UINT16 uFrameLen, uTxFrameLen = 0;
    PUINT8 pTmp;
    uTxFrameLen = m_tMuSvTxParam.uInBuf[4] = pSmvFrame->uFrameLen;
    m_tMuSvTxParam.uInBuf[2] = m_ptMapHeader->OpticalNetHeader.uFramesSum;
    for (UINT16 j = 0; j < uFramesSum; j++) {
        uFrameLen = uTxFrameLen = pSmvFrame->uFrameLen;;
        
    if(pSmvFrame->uFrameLen == 0)
    {
        int a = 0;
    }

        Sleep(1);
        pTmp = (PUINT8)pSmvFrame;
        memcpy(pSmvData, pSmvFrame->uData, uFrameLen);
        uFrameLen += PCAP_FRAME_HEADER_LEN;
        if ((uFrameLen&0x01) != 0) {
            uFrameLen++;
        }
        uByteSum += ((uTxFrameLen&0x03) != 0) ? ((uTxFrameLen&0xFFFC)+4) : uTxFrameLen;
        pSmvData += ((uTxFrameLen&0x03) != 0) ? ((uTxFrameLen&0xFFFC)+4) : uTxFrameLen;
        if(j == 100 || j == 200 || j == 500 || j == 700)
        {
            Sleep(1);
            int a = uByteSum;
        }
        pSmvFrame = (PSMVFrame_t)(pTmp+uFrameLen);
    }
    
    m_bFromFile = FALSE;
    MuTx();
    return TRUE;
}


/********************************************************************************************************/
/*  _SMV_92_c类成员函数                                                                                 */
/********************************************************************************************************/

/*
 *  校验接收到的报文是否正确
 */
BOOL _SMV_92_c::VerifySmvFrame(void)
{
    UINT16 uLoopItem = m_ptMapHeader->OpticalNetHeader.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->OpticalNetHeader.uFramesSum;
    UINT32 uActiveLen = m_ptMapHeader->OpticalNetHeader.uActiveLength;

    RETAILMSG(ZONE_INFO, (_T("uLoopItem = %d  FramesSum = %d  ActiveLength = %d \r\n"), uLoopItem, uFramesSum, uActiveLen));

    PSMVFrame_t pSmvFrame = NULL;
    if (uLoopItem == 0) {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF0_POS);
    } else {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF1_POS);
    }

    BOOL bFoundErr = FALSE;
    BOOL bFoundVerifyFrame = FALSE;
    UINT16 uFrameLen = 0;
    PUINT8 pTmp;
    BOOL bVertifyRst = TRUE;

    m_uTestResult[m_nDeviceItem*2] += uFramesSum;
    PSMVFrame_t pFirstFrame;                                            /*  以第一帧报文作为校验报文    */
    UINT16 uTmp = 0;
    UINT16 uPos = 0;
    UINT16 uSmpCntPos = 0;
    UINT16 uLen = 0;

    for (UINT16 j = 0; j < uFramesSum; j++) {
        if (j != 0) {                                                   /*  移到下一帧报文                  */
            uFrameLen += PCAP_FRAME_HEADER_LEN;
            if ((uFrameLen&0x01) != 0) {
                uFrameLen++;
            }
            pTmp = (PUINT8)pSmvFrame;
            pSmvFrame = (PSMVFrame_t)(pTmp+uFrameLen);
        }
        if (!bFoundVerifyFrame) {
            pFirstFrame = pSmvFrame;
            uFrameLen = pFirstFrame->uFrameLen;
            if (uFrameLen > FIBER_MAX_LEN_A_FRAME) {
                m_uTestResult[m_nDeviceItem*2+1]++;
                continue;
            }
            uPos = 26;                                                  //跳过以太网帧头，直接到以太网PDU
            //Tag
            if (pFirstFrame->uData[uPos] == 0x60) {
                uPos++;
                if (pFirstFrame->uData[uPos] < 0x80) {                  /*  没超过128，后续值就是PDU的长度值*/
                    //Length
                    uLen = pFirstFrame->uData[uPos];
//                     uPos++;
//                     //Val
//                     uTmp = pFirstFrame->uData[uPos];
                } 
                else {
                    //Tag+Length
                    if (pFirstFrame->uData[uPos] == 0x81) {             /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                        uPos++;
                        uLen = pFirstFrame->uData[uPos];
                    } else {
                        uPos++;
                        uLen = pFirstFrame->uData[uPos]*256;
                        uPos++;
                        uLen += pFirstFrame->uData[uPos];
                    }
                }
            } else {
                m_uTestResult[m_nDeviceItem*2+1]++;
                continue;
            }
            if ((uPos+1+uLen) != pFirstFrame->uFrameLen) {
                m_uTestResult[m_nDeviceItem*2+1]++;
                continue;
            }

            while(uPos < uFrameLen) {                                   /*  查找Sequence of ASDU的位置  */
                uPos++;
                //Tag
                if (pFirstFrame->uData[uPos] == 0xA2) {
                    uPos++;
                    //Length
                    if (pFirstFrame->uData[uPos] < 0x80) {              /*  没超过128，后续值就是PDU的长度值*/
                        //Length
                        uLen = pFirstFrame->uData[uPos];
                    } 
                    else {
                        //Tag+Length
                        if (pFirstFrame->uData[uPos] == 0x81) {         /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                            uPos++;
                            uLen = pFirstFrame->uData[uPos];
                        } else {
                            uPos++;
                            uLen = pFirstFrame->uData[uPos]*256;
                            uPos++;
                            uLen += pFirstFrame->uData[uPos];
                        }
                    }
                    break;
                }
                if ((pFirstFrame->uData[uPos]&0x80) == 0x80) {
                    uPos++;
                    if (pFirstFrame->uData[uPos] < 0x80) {              /*  没超过128，后续值就是PDU的长度值*/
                        //Length
                        uLen = pFirstFrame->uData[uPos];
                        uPos+=uLen;
                    } 
                    else {
                        //Tag+Length
                        if (pFirstFrame->uData[uPos] == 0x81) {         /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                            uPos++;
                            uLen = pFirstFrame->uData[uPos];
                            uPos+=uLen;
                        } else {
                            uPos++;
                            uLen = pFirstFrame->uData[uPos]*256;
                            uPos++;
                            uLen += pFirstFrame->uData[uPos];
                            uPos +=uLen;
                        }
                    }
                } else {
                    uPos = uFrameLen;                                   /*  已经发生错误，直接跳出查找  */
                    break;
                }
            }
            if ((uPos+1+uLen) != pFirstFrame->uFrameLen) {
                m_uTestResult[m_nDeviceItem*2+1]++;
                continue;
            }

            while(uPos < uFrameLen) {                                   /*  查找ASDU的位置              */
                uPos++;
                if (pFirstFrame->uData[uPos] == 0x30) {
                    uPos++;
                    if (pFirstFrame->uData[uPos] < 0x80) {                  /*  没超过128，后续值就是PDU的长度值*/
                        //Length
                        uLen = pFirstFrame->uData[uPos];
                    } 
                    else {
                        //Tag+Length
                        if (pFirstFrame->uData[uPos] == 0x81) {             /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                            uPos++;
                            uLen = pFirstFrame->uData[uPos];
                        } else {
                            uPos++;
                            uLen = pFirstFrame->uData[uPos]*256;
                            uPos++;
                            uLen += pFirstFrame->uData[uPos];
                        }
                    }
                    break;
                }
                if ((pFirstFrame->uData[uPos]&0x80) == 0x80) {
                    uPos++;
                    if (pFirstFrame->uData[uPos] < 0x80) {                  /*  没超过128，后续值就是PDU的长度值*/
                        //Length
                        uLen = pFirstFrame->uData[uPos];
                        uPos+=uLen;
                    } 
                    else {
                        //Tag+Length
                        if (pFirstFrame->uData[uPos] == 0x81) {             /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                            uPos++;
                            uLen = pFirstFrame->uData[uPos];
                            uPos+=uLen;
                        } else {
                            uPos++;
                            uLen = pFirstFrame->uData[uPos]*256;
                            uPos++;
                            uLen += pFirstFrame->uData[uPos];
                            uPos +=uLen;
                        }
                    }
                } else {
                    uPos = uFrameLen;                                   /*  已经发生错误，直接跳出查找  */
                    break;
                }
            }
            if ((uPos+1+uLen) != pFirstFrame->uFrameLen) {
                m_uTestResult[m_nDeviceItem*2+1]++;
                continue;
            }

            while(uPos < uFrameLen) {                                   /*  查找uSmpCnt的位置           */
                uPos++;
                if (pFirstFrame->uData[uPos] == 0x82) {
                    break;
                }
                //Tag
                if ((pFirstFrame->uData[uPos]&0x80) == 0x80) {
                    uPos++;
                    if (pFirstFrame->uData[uPos] < 0x80) {                  /*  没超过128，后续值就是PDU的长度值*/
                        //Length
                        uLen = pFirstFrame->uData[uPos];
                        uPos+=uLen;
                    } 
                    else {
                        //Tag+Length
                        if (pFirstFrame->uData[uPos] == 0x81) {             /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                            uPos++;
                            uLen = pFirstFrame->uData[uPos];
                            uPos+=uLen;
                        } else {
                            uPos++;
                            uLen = pFirstFrame->uData[uPos]*256;
                            uPos++;
                            uLen += pFirstFrame->uData[uPos];
                            uPos +=uLen;
                        }
                    }
                }
            }

            uPos++;
            if ((uPos-1) >= uFrameLen || pFirstFrame->uData[uPos] != 0x02) {
                m_uTestResult[m_nDeviceItem*2+1]++;
                continue;
            } else {
                uPos++;
                uSmpCntPos = uPos;
                uTmp = pFirstFrame->uData[uSmpCntPos] * 256;
                uTmp += pFirstFrame->uData[uSmpCntPos+1];
                bFoundVerifyFrame = TRUE;
                if (1) {
                    m_uSmpCntVal = uTmp+1;
                    m_bGetSmpCntVal = FALSE;
                    memcpy(&m_tUtcTime, pFirstFrame->uUTCTime, sizeof(UTC_TIME_t));
                    continue;
                }
                //continue;
            }
        } 

        /*
         *  UTC 时标检验
         */
        if (!m_bGetSmpCntVal) {
            if (!VerifyUtcTime(m_tUtcTime, *(UTC_TIME_t*)(pSmvFrame->uUTCTime), 4000)) {
                m_uUtcTest[m_nDeviceItem]++;
                bVertifyRst = FALSE;
            }
        }
        memcpy(&m_tUtcTime, (UTC_TIME_t*)(pSmvFrame->uUTCTime), sizeof(UTC_TIME_t));


        uFrameLen = pSmvFrame->uFrameLen;
        if (uFrameLen > FIBER_MAX_LEN_A_FRAME) {
            m_uTestResult[m_nDeviceItem*2+1]++;
            m_uSmpCntVal++;
            bVertifyRst = FALSE;
            continue;
        }
        /*
         *  校验报文内容
         */
        if (memcmp(pFirstFrame->uData, pSmvFrame->uData, uSmpCntPos) != 0) {
            m_uTestResult[m_nDeviceItem*2+1]++;
            m_uSmpCntVal++;
            bVertifyRst = FALSE;
            continue;
        }
        /*
         *  校验SmpCnt
         */
        uTmp = pSmvFrame->uData[uSmpCntPos] * 256;
        uTmp += pSmvFrame->uData[uSmpCntPos+1];
        if (uTmp != m_uSmpCntVal) {
            if (m_uSmpCntVal == 4000 && uTmp == 0) {
                m_uSmpCntVal = uTmp;
            } else {
                m_uSmpCntVal = uTmp;
                m_uTestResult[m_nDeviceItem*2+1]++;
                m_uSmpCntVal++;
                bVertifyRst = FALSE;
                continue;
            }
        }
        m_uSmpCntVal++;

        /*
         *  采样计数器之后的报文就不再继续校验了
         */
    }

    RETAILMSG(ZONE_VERIFY_INFO, (_T("Device%d: SmvSum:%d  Err:%d  UTC Err:%d\r\n"), m_nDeviceItem, 
        m_uTestResult[m_nDeviceItem*2], m_uTestResult[m_nDeviceItem*2+1], m_uUtcTest[m_nDeviceItem]));

    if(m_uTestResult[m_nDeviceItem * 2 + 1] > 0 || m_uUtcTest[m_nDeviceItem] > 0)
    {
        bVertifyRst = FALSE;
    }

    return bVertifyRst;
}
/*
 *  校验GOOSE报文
 */
BOOL _SMV_92_c::VerifyGooseFrame(void)
{
    UINT16 uLoopItem = m_ptMapHeader->OpticalNetHeader.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->OpticalNetHeader.uFramesSum;
    UINT32 uActiveLen = m_ptMapHeader->OpticalNetHeader.uActiveLength;

    RETAILMSG(0/*ZONE_INFO*/, (_T("uLoopItem = %d  FramesSum = %d  ActiveLength = %d \r\n"), uLoopItem, uFramesSum, uActiveLen));

    PSMVFrame_t pSmvFrame = NULL;
    if (uLoopItem == 0) {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF0_POS);
    } else {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF1_POS);
    }

    UINT16 uFrameLen = 0;
    PUINT8 pTmp;

    m_uTestResult[m_nDeviceItem*2] += uFramesSum;

    UINT16 uStNumPos = 0;
    UINT16 uSqNumPos = 0;
    UINT32 uStNumVal = 0;
    UINT32 uSqNumVal = 0;

    for (UINT16 j = 0; j < uFramesSum; j++) {
        uStNumVal = 0;
        uSqNumVal = 0;
        pTmp = (PUINT8)pSmvFrame;
        uFrameLen = pSmvFrame->uFrameLen+PCAP_FRAME_HEADER_LEN;
        if (uFrameLen&0x01) {
            uFrameLen++;
        }
        
        if (FALSE == ::VerifyGooseFrame(pSmvFrame->uData, pSmvFrame->uFrameLen, &uStNumPos, &uSqNumPos)) {
            m_uTestResult[m_nDeviceItem*2+1]++;
            if (!m_bGetGooseVal) {
                m_uSqNumVal++;
            }
            pSmvFrame = (PSMVFrame_t)(pTmp+uFrameLen);
            continue;
        }

        /*
         *  获取报文中的StNum、SqNum 值
         */
        {
            int i = 1;                                              /*  跳过表示StNum字节数的那个值 */
            while (i <= pSmvFrame->uData[uStNumPos]) {
                uStNumVal |= pSmvFrame->uData[uStNumPos+i];
                if (i < pSmvFrame->uData[uStNumPos])
                    uStNumVal <<= 8;
                i++;
            }
            i = 1;
            while (i <= pSmvFrame->uData[uSqNumPos]) {
                uSqNumVal |= pSmvFrame->uData[uSqNumPos+i];
                if (i < pSmvFrame->uData[uSqNumPos])
                    uSqNumVal <<= 8;
                i++;
            }
        }
        //RETAILMSG(1, (TEXT("Device%d: StNum:%d  SqNum:%d\r\n"),m_nDeviceItem, uStNumVal, uSqNumVal));
        if (m_bGetGooseVal) {
            m_uStNumVal = uStNumVal;
            m_uSqNumVal = uSqNumVal;
            pSmvFrame = (PSMVFrame_t)(pTmp+uFrameLen);
            m_bGetGooseVal = FALSE;
            continue;
        } else {
            if ((m_uStNumVal == uStNumVal && m_uSqNumVal+1 == uSqNumVal) ||
                (m_uStNumVal+1 == uStNumVal && uSqNumVal == 0)) 
            {
                m_uStNumVal = uStNumVal;
                m_uSqNumVal = uSqNumVal;
                pSmvFrame = (PSMVFrame_t)(pTmp+uFrameLen);
                continue;
            }
            else {
                m_uTestResult[m_nDeviceItem*2+1]++;
                m_uStNumVal = uStNumVal;
                m_uSqNumVal = uSqNumVal;
                pSmvFrame = (PSMVFrame_t)(pTmp+uFrameLen);
                continue;
            }
        }
    }

    RETAILMSG(ZONE_VERIFY_INFO, (_T("Device%d: Sum:%d Err:%d \r\n"), m_nDeviceItem, 
        m_uTestResult[m_nDeviceItem*2], m_uTestResult[m_nDeviceItem*2+1]));

    if(m_uTestResult[m_nDeviceItem * 2 + 1] == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
   
}
/*
 *  校验1588报文
 */
BOOL _SMV_92_c::VerifyIEEE1588Frame(void)
{
    RETAILMSG(1, (TEXT("%S : Verify IEEE1588 Frame is Not Support\r\n"), __FUNCTION__));
    return TRUE;
}

/*
 *  从串口中打印接收到的报文
 */
void _SMV_92_c::PrintFrame(void)
{
    UINT16 uLoopItem = m_ptMapHeader->OpticalNetHeader.uLoopItem;
    UINT16 uFramesSum = m_ptMapHeader->OpticalNetHeader.uFramesSum;
    UINT32 uActiveLen = m_ptMapHeader->OpticalNetHeader.uActiveLength;

    RETAILMSG(1, (_T("Device%d: uLoopItem = %d  FramesSum = %d  ActiveLength = %d \r\n"), m_nDeviceItem, uLoopItem, uFramesSum, uActiveLen));

    /*int nIndex = 0;
    while(nIndex < 800 * 320)
    {
        if(m_pTxData[nIndex++] != *m_pTempTxData)
        {
            RETAILMSG(1, (_T("Wrong line: %d\n"), nIndex));
            break;
        }
        m_pTempTxData++;
    }*/

    PSMVFrame_t pSmvFrame = NULL;
    PUINT16 pTempData = NULL;
    if (uLoopItem == 0) {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF0_POS);
        pTempData = (PUINT16)(m_dataBuf+FIBER_LOOP_BUF0_POS);
    } else {
        pSmvFrame = (PSMVFrame_t)(m_dataBuf+FIBER_LOOP_BUF1_POS);
        pTempData = (PUINT16)(m_dataBuf+FIBER_LOOP_BUF1_POS);
    }

    PSMVFrame_t pSmvTxFrame = NULL;
    pSmvTxFrame = (PSMVFrame_t)(m_dataBuf + SMV_TX_BUF0_POS);

    PUINT16 pSmvData = (PUINT16)(m_dataBuf + SMV_TX_BUF0_POS);

    UINT32 uByteSum = 0;
    UINT16 uFrameLen, uTxFrameLen = 0;
    PUINT8 pTmp;
    uTxFrameLen = m_tMuSvTxParam.uInBuf[4] = pSmvFrame->uFrameLen;
    m_tMuSvTxParam.uInBuf[2] = m_ptMapHeader->OpticalNetHeader.uFramesSum;
    for (UINT16 j = 0; j < uFramesSum; j++) {
        uFrameLen = uTxFrameLen = pSmvFrame->uFrameLen;;
        pTmp = &pSmvFrame->uUTCTime[0];
        /*RETAILMSG(1, (_T("Device%d: UTC Time: %02X %02X %02X %02X %02X %02X %02X %02X \r\n"), m_nDeviceItem, \
            pTmp[0], pTmp[1], pTmp[2], pTmp[3], pTmp[4], pTmp[5], pTmp[6], pTmp[7]));
        RETAILMSG(1, (_T("Device%d: Len 0x%04x  FrameLen 0x%04x\r\n"), m_nDeviceItem, pSmvFrame->uLen, pSmvFrame->uFrameLen));
        if (uFrameLen > FIBER_MAX_LEN_A_FRAME) {
            RETAILMSG(1, (_T("APP:Error _SMV_92_c::PrintFrame  Rx FrameLen is Over(cur:%d max:%d)\r\n"), uFrameLen, FIBER_MAX_LEN_A_FRAME));
            break;
        }*/
       /* for (UINT16 i = 0; i < uFrameLen; i++) {
            RETAILMSG(1, (_T("%02X "), pSmvFrame->uData[i]));
        }
        RETAILMSG(1, (_T("\r\n")));*/
        
        Sleep(1);
        pTmp = (PUINT8)pSmvFrame;
        memcpy(pSmvData, pSmvFrame->uData, uFrameLen);
        uFrameLen += PCAP_FRAME_HEADER_LEN;
        if ((uFrameLen&0x01) != 0) {
            uFrameLen++;
        }
        uByteSum += ((uTxFrameLen&0x03) != 0) ? ((uTxFrameLen&0xFFFC)+4) : uTxFrameLen;
        pSmvData += ((uTxFrameLen&0x03) != 0) ? ((uTxFrameLen&0xFFFC)+4) : uTxFrameLen;
        if(j == 100 || j == 200 || j == 700 || j == 799)
        {
            Sleep(1);
            int a = uByteSum;
        }
        pSmvFrame = (PSMVFrame_t)(pTmp+uFrameLen);
    }
    
    int nIndex = 0;
    while(nIndex < 800 * 320)
    {
        if(m_pTxData[nIndex++] != *m_pTempTxData)
        {
            RETAILMSG(1, (_T("Wrong line: %d\n"), nIndex));
            break;
        }
        m_pTempTxData++;
    }
    m_bFromFile = TRUE;
    MuTx();
}



BOOL _SMV_92_c::userSmvProc(PVOID pArg)
{
    RETAILMSG(ZONE_INFO, (_T("APP: _SMV_92_c::userSmvProc-----------\r\n")));
    _SMV_92_c *pEth = (_SMV_92_c *)pArg;
    if (!pEth->m_bRecv) {
        RETAILMSG(1, (_T("APP:Warning _SMV_92_c::userSmvProc Device_%d Not Work!!!\r\n"), pEth->m_nDeviceItem));
        Sleep(1000);
        return FALSE;
    }
// 
//     DWORD dwTime = GetTickCount();
    pEth->LockInter();

    if (pEth->m_ptMapHeader->OpticalNetHeader.uNetMode != _SMV_DETECT &&
        pEth->m_ptMapHeader->OpticalNetHeader.uNetMode != _SMV_FILTER) {
            RETAILMSG(1, (_T("APP:Error _SMV_92_c::userSmvProc Fiber CurNetMode is not Rx SMV\r\n")));
            pEth->UnLockInter();
            return FALSE;
    }
    /*
     *  接收超时处理  停止接收并通知用户
     */
    if (pEth->m_ptMapHeader->OpticalNetHeader.uIsTimeOut == 1) {
//         IOCTRL_IN_PARAM_t tInParam;
//         tInParam.uDeviceItem = pEth->m_nDeviceItem;
//         DeviceIoControl(pEth->m_hDevice, IOCTRL_STOP_RECV, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL);
        pEth->m_ptMapHeader->OpticalNetHeader.uIsTimeOut = 0;
        pEth->UnLockInter();
        return TRUE;
    } else {
        /*
         *  不想循环接收 可以收满了就停止
         */
        /*if (g_bDisableLoopMode) {
            IOCTRL_IN_PARAM_t tInParam;
            tInParam.uDeviceItem = pEth->m_nDeviceItem;
            RETAILMSG(1, (_T("IOCTRL_STOP_RECV Fiber_%d\r\n"), tInParam.uDeviceItem));
            DeviceIoControl(pEth->m_hDevice, IOCTRL_STOP_RECV, &tInParam, sizeof(IOCTRL_IN_PARAM_t), NULL,NULL,NULL,NULL);
        }*/
    }

//    pEth->RecordFrame();

    pEth->VerifySmvFrame(); 
    
    pEth->GetMuRxFrame(pEth->m_dataBuf + SMV_TX_BUF0_POS);

    pEth->UnLockInter();
// 
//     dwTime = GetTickCount()-dwTime;
//     RETAILMSG(1, (_T("Device%d Verify Frame Time: %d ms--------\r\n"), pEth->m_nDeviceItem, dwTime));
    return TRUE;
}

BOOL _SMV_92_c::userGooseProc(PVOID pArg)
{
    RETAILMSG(ZONE_INFO, (_T("APP: _SMV_92_c::userSmvProc\r\n")));
    _SMV_92_c *pEth = (_SMV_92_c *)pArg;
    if (!pEth->m_bRecv) {
        RETAILMSG(1, (_T("APP:Warning _SMV_92_c::userGooseProc Device_%d Not Work!!!\r\n"), pEth->m_nDeviceItem));
        Sleep(1000);
        return FALSE;
    }

    pEth->LockInter();

    if (pEth->m_ptMapHeader->OpticalNetHeader.uNetMode != _GOOSE_DETECT &&
        pEth->m_ptMapHeader->OpticalNetHeader.uNetMode != _GOOSE_FILTER) {
            RETAILMSG(1, (_T("APP:Error _SMV_92_c::userGooseProc Fiber CurNetMode is not Rx Goose\r\n")));
            pEth->UnLockInter();
            return FALSE;
    }
    /*
     *  接收超时处理  停止接收并通知用户
     */
    if (pEth->m_ptMapHeader->OpticalNetHeader.uIsTimeOut == 1) {
        //GOOSE 接收超时是正常的
    } else {
        /*
         *  不想循环接收 可以收满了就停止
         */
       
    }

    UINT16 uLoopItem = pEth->m_ptMapHeader->OpticalNetHeader.uLoopItem;
    UINT16 uFramesSum = pEth->m_ptMapHeader->OpticalNetHeader.uFramesSum;

    RETAILMSG(ZONE_INFO, (_T("uLoopItem = %d  FramesSum = %d\r\n"), uLoopItem, uFramesSum));

    PSMVFrame_t pSmvFrame = NULL;
    if (uLoopItem == 0) {
        pSmvFrame = (PSMVFrame_t)(pEth->m_dataBuf+GOOSE_RX_BUF0_POS);
    } else {
        pSmvFrame = (PSMVFrame_t)(pEth->m_dataBuf+GOOSE_RX_BUF1_POS);
    }


    pEth->UnLockInter();
    return TRUE;
}

/*********************************************************************************************************
** Function name:       userIEEE1588Proc
**
** Descriptions:        接收IEEE1588报文
**
** input parameters:    pArg: 指向_SMV_92_c类的指针
**                      
** output parameters:   NONE
**
** Returned value:      TRUE: 接收处理函数正确返回
**                      FALSE: 接收过程出现异常
** Note:
** Created by:          YangYuChao
** Created Date:        2013-09-22
**-------------------------------------------------------------------------------------------------------
** Modified by:
** Modified Date: 
**------------------------------------------------------------------------------------------------------- 
*********************************************************************************************************/
BOOL _SMV_92_c::userIEEE1588Proc(PVOID pArg)
{
    RETAILMSG(ZONE_INFO, (_T("APP: _SMV_92_c::userSmvProc\r\n")));
    _SMV_92_c *pEth = (_SMV_92_c *)pArg;
    if (!pEth->m_bRecv) {
        RETAILMSG(1, (_T("APP:Warning _SMV_92_c::userGooseProc Device_%d Not Work!!!\r\n"), pEth->m_nDeviceItem));
        Sleep(1000);
        return FALSE;
    }

    pEth->LockInter();

    if (pEth->m_ptMapHeader->OpticalNetHeader.uNetMode != _IEEE1588_DETECT &&
        pEth->m_ptMapHeader->OpticalNetHeader.uNetMode != _IEEE1588_DETECT) {
            RETAILMSG(1, (_T("APP:Error _SMV_92_c::userGooseProc Fiber CurNetMode is not Rx IEEE1588\r\n")));
            pEth->UnLockInter();
            return FALSE;
    }
    /*
     *  接收超时处理  停止接收并通知用户
     */
    if (pEth->m_ptMapHeader->OpticalNetHeader.uIsTimeOut == 1) {
        //IEEE1588 接收超时是正常的
    } else {
        
         //*  不想循环接收 可以收满了就停止
        
    }

    UINT16 uLoopItem = pEth->m_ptMapHeader->OpticalNetHeader.uLoopItem;
    UINT16 uFramesSum = pEth->m_ptMapHeader->OpticalNetHeader.uFramesSum;

    RETAILMSG(ZONE_INFO, (_T("uLoopItem = %d  FramesSum = %d\r\n"), uLoopItem, uFramesSum));

    PSMVFrame_t pSmvFrame = NULL;
    if (uLoopItem == 0) {
        pSmvFrame = (PSMVFrame_t)(pEth->m_dataBuf+IEEE1588_RX_BUF0_POS);
    } else {
        pSmvFrame = (PSMVFrame_t)(pEth->m_dataBuf+IEEE1588_RX_BUF1_POS);
    }


    pEth->UnLockInter();
    return TRUE;
}


DWORD FiberThreadProcRx1(PVOID pArg)
{
    _SMV_92_c *pEth = (_SMV_92_c *)pArg;
    DWORD dwRet = 0;
    HANDLE hEvt[] = {pEth->m_hNotifyEvent, pEth->m_hExitEvent};

    RETAILMSG(ZONE_INFO, (_T("APP:FiberThreadProcRx1\r\n")));
    ResetEvent(pEth->m_hNotifyEvent);
    bool bSmvRecv = FALSE;
    while (TRUE)
    {
        /*
        **  等待接收事件、线程退出事件
        */
        
        dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);
    
        if (dwRet == WAIT_OBJECT_0) 
        {
            SetEvent(pEth->m_hRxEvent);
            Sleep(2);
        } 
        else if (dwRet == WAIT_OBJECT_0+1) 
        {                                                               /*  退出线程                    */
            break;
        }
        else 
        {
            RETAILMSG(ZONE_ERROR, (_T("APP: FiberThreadProcRx1 found error\r\n")));
        }
    }
    RETAILMSG(ZONE_INFO, (_T("APP: FiberThreadProcRx1 exit\r\n")));

    return 1;
}

DWORD FiberThreadProcRx2(PVOID pArg)
{
    _SMV_92_c *pEth = (_SMV_92_c *)pArg;
    DWORD dwRet = 0;
    HANDLE hEvt[] = {pEth->m_hNotifyEvent, pEth->m_hExitEvent};

    RETAILMSG(ZONE_INFO, (_T("APP:FiberThreadProcRx2\r\n")));
    bool bSmvRecv = FALSE;
    while (TRUE)
    {
        /*
        **  等待接收事件、线程退出事件
        */
        dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);

        if (dwRet == WAIT_OBJECT_0) 
        {
            SetEvent(pEth->m_hRxEvent);
            Sleep(2);
        } 
        else if (dwRet == WAIT_OBJECT_0+1) 
        {                                                               /*  退出线程                    */
            break;
        }
        else 
        {
            RETAILMSG(ZONE_ERROR, (_T("APP: FiberThreadProcRx2 found error\r\n")));
        }
    }
    RETAILMSG(ZONE_INFO, (_T("APP: FiberThreadProcRx2 exit\r\n")));

    return 1;
}

DWORD FiberThreadProcRx3(PVOID pArg)
{
    _SMV_92_c *pEth = (_SMV_92_c *)pArg;
    DWORD dwRet = 0;
    HANDLE hEvt[] = {pEth->m_hNotifyEvent, pEth->m_hExitEvent};

    RETAILMSG(ZONE_INFO, (_T("APP:FiberThreadProcRx3\r\n")));
    bool bSmvRecv = FALSE;
    while (TRUE)
    {
        /*
        **  等待接收事件、线程退出事件
        */
        dwRet = WaitForMultipleObjects(sizeof(hEvt)/sizeof(hEvt[0]), hEvt, FALSE, INFINITE);

        if (dwRet == WAIT_OBJECT_0) 
        {
            SetEvent(pEth->m_hRxEvent);
            Sleep(2);
        } 
        else if (dwRet == WAIT_OBJECT_0+1) 
        {                                                               /*  退出线程                    */
            break;
        }
        else 
        {
            RETAILMSG(ZONE_ERROR, (_T("APP: FiberThreadProcRx3 found error\r\n")));
        }
    }
    RETAILMSG(ZONE_INFO, (_T("APP: FiberThreadProcRx3 exit\r\n")));

    return 1;
}

BOOL VerifyGooseFrame(IN PVOID pData, IN UINT16 uSize, OUT UINT16 *puStNumPos, OUT UINT16 *puSqNumPos)
{
    if (pData==NULL) {
        return FALSE;
    }
    PUINT8 pFrameData = (PUINT8)pData;
    UINT16 uPos = 26;                                                   /*  直接跳转到以太网报文的PDU处 */
    UINT16 uTmp = 0;
    UINT16 uLen = 0;

    if (pFrameData[uPos] != 0x61) {                                     /*  GOOSE PDU 长度              */
        return FALSE;
    } else {
        uPos++;
        if (pFrameData[uPos] < 0x80) {                                  /*  没超过128，后续值就是PDU的长度值*/
            //Length
            uLen = pFrameData[uPos];
        } 
        else {
            //Tag+Length
            if (pFrameData[uPos] == 0x81) {                                   /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                uPos++;
                uLen = pFrameData[uPos];
            } else {
                uPos++;
                uLen = pFrameData[uPos]*256;
                uPos++;
                uLen += pFrameData[uPos];
            }
        }
    }
    if ((uPos+1+uLen) != uSize) {
        return FALSE;
    }

    int nItemNum = 0;
    while(uPos < uSize && nItemNum < 13) {                              /*  GOOSE的PDU最大有13个元素    */
        uPos++;
        if (pFrameData[uPos] == 0x85) {                                 /*  StNum Tag标签               */
            if (puStNumPos) {
                *puStNumPos = uPos+1;                                   /*  保存位置，包括StNum占用的位数*/
            }
        }
        else if (pFrameData[uPos] == 0x86) {
            if (puSqNumPos) {
                *puSqNumPos = uPos+1;                                   /*  保存位置，包括SqNum占用的位数*/
            }
        }
        if (pFrameData[uPos] == 0xAB) {                                 /*   GOOSE 数据集的Tag标签      */
            uPos++;
            if (pFrameData[uPos] < 0x80) {                              /*  没超过128，后续值就是DATA的长度值*/
                //Length
                uLen = pFrameData[uPos];
            } 
            else {
                //Tag+Length
                if (pFrameData[uPos] == 0x81) {                               /*  超过128，按照TLV格式解析  仅支持0x81、0x82 */
                    uPos++;
                    uLen = pFrameData[uPos];
                } else {
                    uPos++;
                    uLen = pFrameData[uPos]*256;
                    uPos++;
                    uLen += pFrameData[uPos];
                }
            }
            if ((uPos+1+uLen) != uSize) {
                return FALSE;
            } else {
                uPos = uSize;
            }
        }
        else if ((pFrameData[uPos]&0x80) == 0x80) {
            uPos++;
            uLen = pFrameData[uPos];                                    /*  指明该元素占用字节个数      */
            uPos += uLen;
        } 
        nItemNum++;
    }

    if (uPos != uSize) {
        return FALSE;
    }
    return TRUE;
}
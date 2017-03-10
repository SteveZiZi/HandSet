// GPRS.h: interface for the CGPRS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPRS_H__8EB70C01_884B_4974_88B3_048F9C8F9735__INCLUDED_)
#define AFX_GPRS_H__8EB70C01_884B_4974_88B3_048F9C8F9735__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xComCE.h"

class CGPRS  
{
public:
    CGPRS();
    virtual ~CGPRS();

    BOOL GPRS_Init(int Port, int BaudRate, DWORD UserParam);

    // GPRS 回调函数
    typedef void (CALLBACK *ONGPRSRECV)(DWORD UserParam, DWORD Status, CString strData);
    ONGPRSRECV OnGPRSRecv;

    BOOL GPRS_DialUp(CString strTelNum);    
    BOOL GPRS_DialDown(void);

    BOOL GPRS_TakeTheCall(void);
    BOOL GPRS_HangUpTheCall(void);
    //获取信号强度,-2表示GPRS没有返回，-1表示GPRS模块返回了错误的数据
    int  GPRS_GetSignal();

    BOOL GPRS_DeleteShortMsg(DWORD num);

    BOOL GPRS_ReadShortMessage(DWORD num, CString *strTelCode, CString *Msg);

    BOOL GPRS_SendShortMessage(CString strTelNum, CString Msg);
    BOOL GPRS_SetShortMSGCenterTel(CString strTelNum);
    
    BOOL GPRS_CheckMsgNum(CString *strNum, CString *strTotal);

private:

    CxComCE *pCeSerial;
    DWORD m_UserParam;                                                  /* 使用本模块用户               */

    HANDLE m_ATCmdRespEvent;                                            /* AT 命令回复事件              */
    
    CString m_strRespBuf;                                               /* 回复缓冲区字符串             */

    DWORD m_RespCnt;

    BOOL bSendATCmd;                                                    /* 是否发送了命令               */

    void ResetGlobalVarial(void);

    BOOL GPRS_SendATCmd(CString strCmd);                                /* 发送 AT 命令                 */
    BOOL GPRS_WriteMsg(CString Msg);                                    /* 发送短信内容                 */
    BOOL GPRS_WaitResponseOK(void);                                     /* 等待 AT 命令的响应           */

    CString GPRS_GetTheCallNum(CString str);                            /* 获取打进来的电话的号码       */

    //int FindMsgPos(void);                        /* 查找短信的位置 */
    int FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen);
    static void CALLBACK OnCommRecv(void * UserParam, const void *buf, unsigned int buflen);
};

// GPRS 状态码
#define GPRS_STATUS_RECEIVE_CALL            0x01                        /* 接到来电                     */
#define GPRS_STATUS_OTHER_SIDE_HANDUP        0x02                       /* 对方挂机                     */



#endif // !defined(AFX_GPRS_H__8EB70C01_884B_4974_88B3_048F9C8F9735__INCLUDED_)
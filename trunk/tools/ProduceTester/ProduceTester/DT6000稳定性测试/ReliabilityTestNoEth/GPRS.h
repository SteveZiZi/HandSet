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

    // GPRS �ص�����
    typedef void (CALLBACK *ONGPRSRECV)(DWORD UserParam, DWORD Status, CString strData);
    ONGPRSRECV OnGPRSRecv;

    BOOL GPRS_DialUp(CString strTelNum);    
    BOOL GPRS_DialDown(void);

    BOOL GPRS_TakeTheCall(void);
    BOOL GPRS_HangUpTheCall(void);
    //��ȡ�ź�ǿ��,-2��ʾGPRSû�з��أ�-1��ʾGPRSģ�鷵���˴��������
    int  GPRS_GetSignal();

    BOOL GPRS_DeleteShortMsg(DWORD num);

    BOOL GPRS_ReadShortMessage(DWORD num, CString *strTelCode, CString *Msg);

    BOOL GPRS_SendShortMessage(CString strTelNum, CString Msg);
    BOOL GPRS_SetShortMSGCenterTel(CString strTelNum);
    
    BOOL GPRS_CheckMsgNum(CString *strNum, CString *strTotal);

private:

    CxComCE *pCeSerial;
    DWORD m_UserParam;                                                  /* ʹ�ñ�ģ���û�               */

    HANDLE m_ATCmdRespEvent;                                            /* AT ����ظ��¼�              */
    
    CString m_strRespBuf;                                               /* �ظ��������ַ���             */

    DWORD m_RespCnt;

    BOOL bSendATCmd;                                                    /* �Ƿ���������               */

    void ResetGlobalVarial(void);

    BOOL GPRS_SendATCmd(CString strCmd);                                /* ���� AT ����                 */
    BOOL GPRS_WriteMsg(CString Msg);                                    /* ���Ͷ�������                 */
    BOOL GPRS_WaitResponseOK(void);                                     /* �ȴ� AT �������Ӧ           */

    CString GPRS_GetTheCallNum(CString str);                            /* ��ȡ������ĵ绰�ĺ���       */

    //int FindMsgPos(void);                        /* ���Ҷ��ŵ�λ�� */
    int FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen);
    static void CALLBACK OnCommRecv(void * UserParam, const void *buf, unsigned int buflen);
};

// GPRS ״̬��
#define GPRS_STATUS_RECEIVE_CALL            0x01                        /* �ӵ�����                     */
#define GPRS_STATUS_OTHER_SIDE_HANDUP        0x02                       /* �Է��һ�                     */



#endif // !defined(AFX_GPRS_H__8EB70C01_884B_4974_88B3_048F9C8F9735__INCLUDED_)
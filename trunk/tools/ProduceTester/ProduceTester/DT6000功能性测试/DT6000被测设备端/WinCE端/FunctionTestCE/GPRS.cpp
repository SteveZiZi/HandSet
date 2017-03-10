// GPRS.cpp: implementation of the CGPRS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GPRS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPRS::CGPRS()
{
    pCeSerial = NULL;
    ResetGlobalVarial();
}

CGPRS::~CGPRS()
{
    if (pCeSerial != NULL)
    {       
        delete pCeSerial;
    }
}

/********************************************************************************************************
��������: ResetGlobalVarial
��    ��: ��λһЩȫ�ֱ���ΪĬ��ֵ
�������: ��
�������: ��
��    ��: ��
*********************************************************************************************************/
void CGPRS::ResetGlobalVarial(void)
{
    bSendATCmd = FALSE;                                                 /* ��ǰ�����ڷ�������״̬       */
    m_RespCnt = 0;                            
    m_strRespBuf = _T("");
    ResetEvent(m_ATCmdRespEvent);                                       /* ��λ GPRS ��Ӧ�¼�           */
}

/********************************************************************************************************
��������: GPRS_Init
��    ��: GPRS ģ���ʼ������
�������: LPCTSTR Port   : ���ں�, �� "COM1:"
          int BaudRate   : ���ڲ�����, �� 115200
          DWORD UserParam: �û����ݵ������еĲ���
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS:: GPRS_Init(int Port, int BaudRate, DWORD UserParam)
{    
    BOOL ret;
    DWORD Trycount = 2;

    do{                                                                 /* ���Գ�ʼ�� GPRS, ��ೢ����  */
                                                                        /* ��                           */
        Trycount--;

        pCeSerial = new CxComCE;
        ret = pCeSerial->Open(Port, 115200, 8, ONESTOPBIT, NOPARITY);        
        if (ret == FALSE)                                               /* �򿪴���, ����λΪ8,ֹͣλΪ */
                                                                        /* 1,��У��λ                   */
        {
            AfxMessageBox(_T("���ڴ�ʧ�ܣ�"), MB_OK);
            delete pCeSerial;
            return FALSE;
        }

        pCeSerial->EscapeCommFunction(SETRTS);
        pCeSerial->EscapeCommFunction(SETDTR); 

       

                                   /* ���ڽ��ճɹ��ص�����         */
        m_UserParam = UserParam;                                        /* �����û��������Ĳ���         */
        m_ATCmdRespEvent = CreateEvent(NULL, FALSE, FALSE, NULL);        /* ���� GPRS ��Ӧ�¼�           */

        pCeSerial->BeginRecvThread(OnCommRecv,this);
        ret = GPRS_SendATCmd(_T("AT"));                                 /* ����ģ���Ƿ��ڼ���״̬     */
        ret = GPRS_WaitResponseOK();
        ret = GPRS_SendATCmd(_T("ATE0"));                               /* ����ģ���Ƿ��ڼ���״̬     */
        ret = GPRS_WaitResponseOK();
        ret = GPRS_SendATCmd(_T("AT"));                                 /* ����ģ���Ƿ��ڼ���״̬     */
        ret = GPRS_WaitResponseOK();
        if (ret == FALSE)
        {
            delete pCeSerial;
            if (Trycount == 0) return FALSE;
        }
        else
            Trycount = 0;

    }while(Trycount > 0);

    GPRS_SendATCmd(_T("AT+CREG?"));                
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CLIP=1"));                                    /* ����������ʾ                */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CMGF=1"));                                    /* ����Ϊ�ı�ģʽ               */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CSCS=\"GSM\""));                              /* �����ַ���                   */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    GPRS_SendATCmd(_T("AT+CSMP=17,168,0,0"));                           /* �����ı�ģʽ����             */
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
��������: OnCommRecv
��    ��: ���ڽ��ճɹ��ص�����, ��������ȷ���յ�����ʱ, �ú�����������
�������: CString strCmd: ������
�������: DWORD UserParam : �û����ݵĲ���, ͨ��ΪCGPRS���һ�������ָ��
          BYTE *buf          : ���ڽ������ݻ���
          DWORD buflen      : ���ڽ��յ������ݳ���
��    ��: ��
*********************************************************************************************************/
void CALLBACK CGPRS::OnCommRecv(void * UserParam, const void  *buf, unsigned int buflen)
{
    UINT Length;
    CGPRS *pGPRS = (CGPRS *)UserParam;    
    
    CString strTmp = _T("");
    LPTSTR pStr = strTmp.GetBuffer(100);    
    Length=MultiByteToWideChar(CP_ACP, 0, (char *)buf, buflen, pStr, 100);
    char *p=(char *)buf;
    pStr[Length]=0;
                                                                        /* �����յ�������ת��Ϊ Unicode */
                                                                        /* ����                         */
    strTmp.ReleaseBuffer();

    if (pGPRS->bSendATCmd == TRUE)
    {                                                                   /* ֮ǰ�û������� AT ָ��       */
        pGPRS->m_strRespBuf.Insert(pGPRS->m_RespCnt, strTmp);           /* ��������                     */
        SetEvent(pGPRS->m_ATCmdRespEvent);
        return;
    }
    
    if ((strTmp.Find(_T("RING")) >= 0) || 
        (strTmp.Find(_T("ring")) >= 0))
    {                                                                   /* �е绰�����                 */
        CString strCallNum = pGPRS->GPRS_GetTheCallNum(strTmp);
        pGPRS->OnGPRSRecv(pGPRS->m_UserParam, GPRS_STATUS_RECEIVE_CALL,strCallNum);
    }
    
    if ((strTmp.Find(_T("NO CARRIER")) >= 0) || 
        (strTmp.Find(_T("no carrier")) >= 0))                
    {                                                                   /* �����, �Է��һ�             */
        pGPRS->OnGPRSRecv(pGPRS->m_UserParam, GPRS_STATUS_OTHER_SIDE_HANDUP, L"");
    }

    pGPRS->bSendATCmd = FALSE;                                          /* ��λ���ȫ�ֱ���             */
    pGPRS->m_RespCnt = 0;                            
    pGPRS->m_strRespBuf = _T("");
    ResetEvent(pGPRS->m_ATCmdRespEvent);
}


/********************************************************************************************************
��������: GPRS_GetTheCallNum
��    ��: ȡ������ĵ绰����
�������: CString str:  ����ʱ, GPRS ��Ӧ�ַ�
�������: ��
��    ��: ����绰����
*********************************************************************************************************/
CString CGPRS::GPRS_GetTheCallNum(CString str)
{
    int pos1 = str.Find(_T("+CLIP: \""), 0);
    if (pos1 < 0)
        return L"";
    else
        pos1 = pos1 + wcslen(_T("+CLIP: \""));

    int pos2 = str.Find(_T("\""), pos1);
    if (pos2 < 0)
        return L"";

    CString strNum;
    strNum = str.Mid(pos1, pos2 - pos1);                                /* ȡ�õ绰����                 */

    return strNum;
}


/********************************************************************************************************
��������: GPRS_SendATCmd
��    ��: ���� AT ����
�������: CString strCmd: ������
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_SendATCmd(CString strCmd)
{
    int len = strCmd.GetLength();
    BYTE *psendbuf = new BYTE[len + 2];

    m_RespCnt = 0;
    bSendATCmd = TRUE;                                                  /* ���뷢������״̬             */

    for(int i = 0; i < len;i++)
        psendbuf[i] = (char)strCmd.GetAt(i);                            /* ת��Ϊ���ֽ��ַ�             */

    psendbuf[len] = L'\r';                                              /* ���������                   */
    psendbuf[len + 1] = L'\n';
    //strCmd += _T("\r\n");
    USES_CONVERSION;
//    pCeSerial->Senddata((BYTE*)W2A((LPCTSTR)strCmd), strCmd.GetLength());
        pCeSerial->Write(psendbuf, len + 2);                         /* �Ӵ��ڷ�������               */

    delete[] psendbuf;
    return TRUE;
}


/********************************************************************************************************
��������: GPRS_WaitResponseOK
��    ��: �ȴ� AT ������Ӧ OK
�������: ��
�������: ��
��    ��: ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_WaitResponseOK(void)
{
    DWORD ret;
    BOOL bOK = TRUE;

    while(1)
    {    /*
          * �ȴ� 0.5 ��, ����ò����ظ�, ��Ϊ����
          */
        ret = WaitForSingleObject(m_ATCmdRespEvent, 500);
        if (ret == WAIT_OBJECT_0)
        {
            if ((m_strRespBuf.Find(_T("ERROR\r\n"), 0) >= 0) ||
                (m_strRespBuf.Find(_T("error\r\n"), 0) >= 0))
            {                                                           /* ��Ӧ����"ERROR"              */
                bOK = FALSE;
                break;
            }
            if ((m_strRespBuf.Find(_T("OK\r\n"), 0) >= 0) ||
                (m_strRespBuf.Find(_T("ok\r\n"), 0) >= 0))
            {                                                           /* ��Ӧ����"OK"                 */
                bOK = TRUE;
                break;
            }

            ResetEvent(m_ATCmdRespEvent);
        }
        else
        {
            bOK = FALSE;                                                /* ��Ӧ��ʱ                     */
            break;
        }
    }

    ResetGlobalVarial();                                                /* ��λ�õ���ȫ�ֱ���           */
    if (FALSE == bOK)
        return FALSE;

    return TRUE;
}

int CGPRS::GPRS_GetSignal()
{
    ResetGlobalVarial();
    GPRS_SendATCmd(L"AT+CSQ");

    DWORD ret = WaitForSingleObject(m_ATCmdRespEvent, 5000);
    if (ret !=WAIT_OBJECT_0)
    {
        return -2;
    }

    int pos=m_strRespBuf.Find(L"+CSQ");
    if (pos <0)
    {
        return -1;
    }

    LPCWSTR p=m_strRespBuf;
    p+=pos;
    p+=5;
    int signal;
    swscanf(p,L"%d",&signal);
    
    return signal;
}



/********************************************************************************************************
��������: GPRS_WriteMsg
��    ��: ����������д�� GPRS ģ��
�������: CString Msg:  ��������
�������: ��
��    ��: ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_WriteMsg(CString Msg)
{
    int len = Msg.GetLength();
    BYTE *psendbuf = new BYTE[len + 3];

    for(int i = 0; i < len;i++)
        psendbuf[i] = (char)Msg.GetAt(i);                               /* ת��Ϊ���ֽ��ַ�             */

    psendbuf[len] = '\r';                                               /* ���������                   */
    psendbuf[len + 1] = '\n';
    psendbuf[len + 2] = 0x1A;
    pCeSerial->Write(psendbuf, len + 3);                             /* �Ӵ��ڷ�������               */

    delete[] psendbuf;
    return TRUE;
}


/********************************************************************************************************
��������: GPRS_DialUp
��    ��: ����绰(�ɱ������)
�������: CString strTelNum: �绰����
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_DialUp(CString strTelNum)
{
    CString strDialup = _T("ATD");
    strDialup += strTelNum + L";";                                      /* �Էֺ���Ϊ����               */

    GPRS_SendATCmd(strDialup);                                          /* ���Ͳ�������                 */

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
��������: GPRS_DialDown
��    ��: �һ�(�ɱ�������Ĺһ�)
�������: ��
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_DialDown(void)
{
    GPRS_SendATCmd(L"ATH");

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
��������: GPRS_TakeTheCall
��    ��: �����绰(�е绰�����, �����õ绰)
�������: ��
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_TakeTheCall(void)
{
    GPRS_SendATCmd(L"ATA");

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
��������: GPRS_TakeTheCall
��    ��: �Ҷϵ绰(�е绰�����ʱ, �Ҷϵ绰)
�������: ��
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_HangUpTheCall(void)
{
    GPRS_SendATCmd(L"AT+CHUP");
    
    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
��������: GPRS_SetShortMSGCenterTel
��    ��: ���ö������ĺ���
�������: CString strTelNum:  �������ĺ���
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_SetShortMSGCenterTel(CString strTelNum)
{
    CString strTmp;

    strTmp = strTelNum + L",145";

    GPRS_SendATCmd(strTelNum);

    return (GPRS_WaitResponseOK());
}


/********************************************************************************************************
��������: FindMsgPos
��    ��: ���ö������ĺ���
�������: CString strTelNum:  �������ĺ���
�������: ��
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
int CGPRS::FindMsgPos(int *posnum, int *numlen, int *posmsg, int *msglen)
{
    /*
     * ���ſ�ʼλ��
     */
    int length = m_strRespBuf.GetLength();
    int posStart = m_strRespBuf.Find(_T("+CMGR:"), 0);                    
    if (posStart < 0)
        return -1;

    /*
     * �绰���뿪ʼλ��
     */
    *posnum = m_strRespBuf.Find(_T("\"REC READ\",\""), posStart);    
    if (*posnum < 0)
        return -1;

    *posnum = *posnum + wcslen(_T("\"REC READ\",\""));            

    /*
     * �绰�������λ��
     */
    int posnumend = m_strRespBuf.Find(_T("\""), *posnum + 1);
    if (posnumend < 0)
        return -1;

    *numlen = posnumend - *posnum;                                      /* �绰���볤��                 */
    
    *posmsg = m_strRespBuf.Find(_T("\r\n"), posStart);                  /* ���ſ�ʼλ��                 */
    if (*posmsg < 0)
        return -1;

    *posmsg = *posmsg + wcslen(_T("\r\n"));    
    
    int posmsgend = m_strRespBuf.Find(_T("\r\n\r\nOK"), *posmsg);       /* ���Ž���λ��                 */
    *msglen = posmsgend - *posmsg;
    
    return 1;
}


/********************************************************************************************************
��������: GPRS_ReadShortMessage
��    ��: ��ȡ��������
�������: DWORD num        : �������
�������: CString *Msg    : ��������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_ReadShortMessage(DWORD num, CString *strTelCode, CString *Msg)
{
    CString strReadNum;
    BOOL ret, retvalue;

    strReadNum.Format(_T("AT+CMGR=%d"), num);
    bSendATCmd = TRUE;
    GPRS_SendATCmd(strReadNum);                                         /* ����: ���� num ������        */

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
        if (ret == WAIT_OBJECT_0)
        {
            if (m_strRespBuf.Find(_T("ERROR\r\n"), 0) > 0)              /* ��ӦΪ ERROR                 */
            {
                //AfxMessageBox(_T("��ȡ���ŷ��ִ���"));
                retvalue = FALSE;
                break;
            }
            
            int posnum, numlen, posmsg, msglen;    
            int pos = FindMsgPos(&posnum, &numlen, &posmsg, &msglen);   /* Ѱ�Ҷ���λ��                 */
            if (pos > 0)
            {
                //char buffer[1024];

                //for (int i = 0; i < m_strRespBuf.GetLength(); i++)
                //    buffer[i] = (char)m_strRespBuf.GetAt(i);

                *strTelCode = m_strRespBuf.Mid(posnum, numlen);
                *Msg = m_strRespBuf.Mid(posmsg, msglen);                /* ȡ����������                 */
                retvalue = TRUE;
                break;
            }
            
            ResetEvent(m_ATCmdRespEvent);
            
        }//end of if (ret == WAIT_OBJECT_0)
        else
        {
            //AfxMessageBox(_T("��ȡ���ų�ʱ"));
            retvalue = FALSE;
            break;
        }
    }

    ResetGlobalVarial();
    if (retvalue == FALSE)
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
��������: GPRS_SendShortMessage
��    ��: ���Ͷ���
�������: CString strTelNum : �Է��绰����
�������: CString *Msg        : ��������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_SendShortMessage(CString strTelNum, CString Msg)
{
    BOOL ret, retvalue;
    int pos;

    /* 
    *    1. ���͵绰����
    */
    CString strNum;
    strNum.Format(_T("AT+CMGS=\"%s\""), strTelNum);
    bSendATCmd = TRUE;
    GPRS_SendATCmd(strNum);                                             /* ���͵绰����                 */

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent, 2000);
        if (ret == WAIT_OBJECT_0)    
        {    
            pos = m_strRespBuf.Find(_T('>'), 0);
            if (pos > 0)
            {                                                           /* �յ���ʾ��, ���ڿ�����ģ��д */
                                                                        /* �����������                 */
                retvalue = TRUE;
                break;
            }
            pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
            if (pos > 0)
            {
                retvalue = FALSE;                                       /* GPRS ��Ӧ�ַ��а�����"ERROR" */
                break;
            }
        }
        else
        {
            retvalue = FALSE;
            break;
        }
    }

    ResetGlobalVarial();
    if (retvalue == FALSE) return FALSE;

    /* 
    *    2. �ȴ� '>' ��ʾ��, ���Ͷ�������
    */
    bSendATCmd = TRUE;
    GPRS_WriteMsg(Msg);    

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent,500);
        if (ret == WAIT_OBJECT_0)    
        {
            int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
            if (pos > 0)
            {
                retvalue = FALSE;
                break;
            }
            pos = m_strRespBuf.Find(_T("CMGS"), 0);
            if (pos > 0)
            {                                                           /* ��Ӧ��ȷ, ���ͳɹ�           */
                retvalue = TRUE;
                break;
            }
        }
    }
    
    ResetGlobalVarial();
    if (retvalue == FALSE)
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
��������: CheckMsgNum
��    ��: ����������: SIN������Ч���������������ݵ�����������(private����)
�������: CString str       : GPRS ��Ӧ�ַ�
�������: CString *strNum  : ��Ч��������
          CString *strTotal: �����ݵ�����������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CheckMsgNum(CString str, CString *strNum, CString *strTotal)
{
    int pos1 = str.Find(_T("\"SM\","), 0);                              /* ��ʶ�ַ�                     */
    if (pos1 < 0)
        return FALSE;

    int pos2 = str.Find(_T(','), pos1 + 5);                             /* �ָ�����                     */
    if (pos2 < 0)
        return FALSE;

    *strNum = str.Mid(pos1 + 5, pos2 - (pos1 + 5));                     /* ���ж�������                 */

    int pos3 = str.Find(_T(","), pos2 + 1);
    if (pos3 < 0)
        return FALSE;

    *strTotal = str.Mid(pos2 + 1, pos3 - pos2 - 1);                     /* �����ݵ�����������         */

    return TRUE;
}


/********************************************************************************************************
��������: GPRS_CheckMsgNum
��    ��: ����������: SIN������Ч���������������ݵ�����������(public����)
�������: ��
�������: CString *strNum  : ��Ч��������
          CString *strTotal: �����ݵ�����������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_CheckMsgNum(CString *strNum, CString *strTotal)
{    
    BOOL ret, retvalue;
    CString strTmp;

    bSendATCmd = TRUE;
    GPRS_SendATCmd(_T("AT+CPMS?"));                                     /* ��ѯ����                     */

    while(1)
    {
        ret = WaitForSingleObject(m_ATCmdRespEvent, 500);
        if (ret == WAIT_OBJECT_0)    
        {
            int pos = m_strRespBuf.Find(_T("ERROR\r\n"), 0);
            if (pos > 0)
            {
                retvalue = FALSE;
                break;
            }

            if (CheckMsgNum(m_strRespBuf, strNum, strTotal) == TRUE)
            {                                                           /* ��ѯ�ɹ�                     */
                retvalue = TRUE;
                break;
            }
            ResetEvent(m_ATCmdRespEvent);
        }
        else
        {
            retvalue = FALSE;
            break;            
        }
    }

    ResetGlobalVarial();
    if (retvalue == FALSE) 
        return FALSE;

    return TRUE;
}


/********************************************************************************************************
��������: GPRS_DeleteShortMsg
��    ��: ɾ������
�������: ��
�������: DWORD num  : �������
��    ��: TRUE: �ɹ�   FALSE: ʧ��
*********************************************************************************************************/
BOOL CGPRS::GPRS_DeleteShortMsg(DWORD num)
{
    CString strCmd;
    BOOL ret;

    strCmd.Format(_T("AT+CMGD=%d"), num);

    GPRS_SendATCmd(strCmd);
    ret = GPRS_WaitResponseOK();
    if (ret == FALSE)
        return FALSE;

    return TRUE;
}

void CGPRS::GPRS_Deinit()
{
    if (pCeSerial != NULL)
    {       
        delete pCeSerial;
    }
    pCeSerial= NULL;
    ResetGlobalVarial();
}
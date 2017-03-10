#pragma once

#define MAP_FILE_LEN_9192_DETECT        MAX_9192_DETECT_FRAME*2*MAX_RECV_9192_DETECT_INTRS
#define MAX_9192_DETECT_FRAME           16384                            /*  ����ĳ���                */
#define MAX_RECV_9192_DETECT_FRAMES     1280                            /*  �����յı���֡�� 1280     */
#define MAX_RECV_9192_DETECT_INTRS      22                              /*  ������43��RAM�ж�         */
#define _9192_DETECT_LEN_A_RAM          60                              /*  һ��RAM����GOOSE���ĵĳ���  */
#define _9192_DETECT_FRAME_HEADER_LEN   8                               /*  GOOSE����֡ͷ��Ϣ���� 16-bit*/
#define _9192_LOOP_BUF0_POS             0
#define _9192_LOOP_BUF1_POS             MAP_FILE_LEN_9192_DETECT / 2

/*
**  ARM���յ�Goose����
*/

#pragma pack(push)
#pragma pack (1)                                                        /*  ����Ϊ1Byte���룬Ĭ����8Byte*/

typedef struct {
    UINT8  uUTCTime[8];                                                 //UTC ʱ�� 48bit
    UINT32 uLen;                                                        //���� 32bit Ĭ��Ϊ0
    UINT16 uFrameLen;                                                   //֡���ĳ���
    UINT16 uReserve;
    UINT8  uData[(MAX_9192_DETECT_FRAME-_9192_DETECT_FRAME_HEADER_LEN)*2];           //֡����
}GooseFrame_t, *PGooseFrame_t;                                          /*  ʵ�ʵ�GOOSE���ĸ�ʽ         */


/*
**  ARM����Gooseɸѡ����
*/
typedef struct {
    UINT8 MacAddr1[6];
    UINT8 MacAppid1[2];
    UINT8 MacAddr2[6];
    UINT8 MacAppid2[2];
    UINT16 uFilterFlag;                                                 /*  ���෽ʽ��ַɸѡ��־λ      */
}GooseFilter_t;

typedef struct {
    struct _filter_t{
        UINT8 MacAddr[6];
        UINT8 MacAppid[2];
    }filter[2][5];
    UINT16 uFilterFlag;                                                 /*  ���෽ʽ��ַɸѡ��־λ      */
}Filter_t;                                                              /*  ������GOOSEɸѡ             */

#pragma pack(pop)                                                       /*  �ָ�Ĭ�϶��뷽ʽ            */


typedef void (*PFN_GOOSE_CALLBACK)(void *pCallBackParam);
class _GooseSmv_c
{
public:
    typedef enum {
        _RECV_NONE = -1,
        _9_1_9_2_DETECT = 0,
        _9_1_9_2_FILTER,
        _SMV,
        _GOOSE_DETECT,
        _GOOSE_FILTER,

        _MAX_NET_NUM
    }NET_MODE;

    _GooseSmv_c(HANDLE hDevice);
    ~_GooseSmv_c();

    BOOL InitGoose(WCHAR *pNotifyEvtName, WCHAR *pMapFileName,
        PFN_GOOSE_CALLBACK pfnGooseCallBack,void *pCallBackParam);
    void DeInitGoose(void);

    inline void SetNetMode(NET_MODE netMode) {
        m_NetMode = netMode;
    }

    PFN_GOOSE_CALLBACK  m_pfnGooseCallBack;
    void *              m_pCallBackParam;
    static DWORD gooseThreadProc(void * pArg);

private:
    
    HANDLE          m_hDevice;                                          /*  GPM�������                 */
    NET_MODE        m_NetMode;

#if EN_9192_DETECT
    HANDLE          m_hExitEvent;
    HANDLE          m_hNotifyEvent;                                     //����֪ͨͨ��xָ���Ľ��ղ������
    HANDLE          m_hMapping;                                         //ͨ��x�����������ݵ��ڴ�ӳ��
    PBYTE           m_DataBuf;                                          //ָ��ͨ��x�Ļ�����
    HANDLE          m_hThread;
    BOOL            m_bExitThread;
    UINT32          m_uBufPos;                                          //ȡ�������е����ݵ�λ��
#endif

#if EN_SMV
    //smv
    HANDLE          m_hSmvExitEvent;
    HANDLE          m_hSmvNotifyEvent;                                  //����֪ͨͨ��xָ���Ľ��ղ������
    HANDLE          m_hSmvMapping;                                      //ͨ��x�����������ݵ��ڴ�ӳ��
    PBYTE           m_SmvDataBuf;                                       //ָ��ͨ��x�Ļ�����
    HANDLE          m_hSmvThread;
    BOOL            m_bSmvExitThread;
    UINT32          m_uSmvBufPos;                                       //ȡ�������е����ݵ�λ��
#endif

#if EN_GOOSE_DETECT
    HANDLE          m_hGFExitEvent;
    HANDLE          m_hGFNotifyEvent;                                     //����֪ͨͨ��xָ���Ľ��ղ������
    HANDLE          m_hGFMapping;                                         //ͨ��x�����������ݵ��ڴ�ӳ��
    PBYTE           m_GFDataBuf;                                          //ָ��ͨ��x�Ļ�����
    HANDLE          m_hGFThread;
    BOOL            m_bGFExitThread;
    UINT32          m_uGFBufPos;                                          //ȡ�������е����ݵ�λ��
#endif

};
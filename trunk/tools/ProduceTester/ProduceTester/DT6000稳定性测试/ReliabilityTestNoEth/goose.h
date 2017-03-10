#pragma once

#define MAP_FILE_LEN_9192_DETECT        MAX_9192_DETECT_FRAME*2*MAX_RECV_9192_DETECT_INTRS
#define MAX_9192_DETECT_FRAME           16384                            /*  最大报文长度                */
#define MAX_RECV_9192_DETECT_FRAMES     1280                            /*  最多接收的报文帧数 1280     */
#define MAX_RECV_9192_DETECT_INTRS      22                              /*  最多接收43个RAM中断         */
#define _9192_DETECT_LEN_A_RAM          60                              /*  一个RAM包含GOOSE报文的长度  */
#define _9192_DETECT_FRAME_HEADER_LEN   8                               /*  GOOSE报文帧头信息长度 16-bit*/
#define _9192_LOOP_BUF0_POS             0
#define _9192_LOOP_BUF1_POS             MAP_FILE_LEN_9192_DETECT / 2

/*
**  ARM接收的Goose报文
*/

#pragma pack(push)
#pragma pack (1)                                                        /*  配置为1Byte对齐，默认是8Byte*/

typedef struct {
    UINT8  uUTCTime[8];                                                 //UTC 时间 48bit
    UINT32 uLen;                                                        //长度 32bit 默认为0
    UINT16 uFrameLen;                                                   //帧报文长度
    UINT16 uReserve;
    UINT8  uData[(MAX_9192_DETECT_FRAME-_9192_DETECT_FRAME_HEADER_LEN)*2];           //帧报文
}GooseFrame_t, *PGooseFrame_t;                                          /*  实际的GOOSE报文格式         */


/*
**  ARM发送Goose筛选命令
*/
typedef struct {
    UINT8 MacAddr1[6];
    UINT8 MacAppid1[2];
    UINT8 MacAddr2[6];
    UINT8 MacAppid2[2];
    UINT16 uFilterFlag;                                                 /*  核相方式地址筛选标志位      */
}GooseFilter_t;

typedef struct {
    struct _filter_t{
        UINT8 MacAddr[6];
        UINT8 MacAppid[2];
    }filter[2][5];
    UINT16 uFilterFlag;                                                 /*  核相方式地址筛选标志位      */
}Filter_t;                                                              /*  仅用于GOOSE筛选             */

#pragma pack(pop)                                                       /*  恢复默认对齐方式            */


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
    
    HANDLE          m_hDevice;                                          /*  GPM驱动句柄                 */
    NET_MODE        m_NetMode;

#if EN_9192_DETECT
    HANDLE          m_hExitEvent;
    HANDLE          m_hNotifyEvent;                                     //用于通知通道x指定的接收操作完成
    HANDLE          m_hMapping;                                         //通道x用来缓冲数据的内存映射
    PBYTE           m_DataBuf;                                          //指向通道x的缓冲区
    HANDLE          m_hThread;
    BOOL            m_bExitThread;
    UINT32          m_uBufPos;                                          //取缓冲区中的数据的位置
#endif

#if EN_SMV
    //smv
    HANDLE          m_hSmvExitEvent;
    HANDLE          m_hSmvNotifyEvent;                                  //用于通知通道x指定的接收操作完成
    HANDLE          m_hSmvMapping;                                      //通道x用来缓冲数据的内存映射
    PBYTE           m_SmvDataBuf;                                       //指向通道x的缓冲区
    HANDLE          m_hSmvThread;
    BOOL            m_bSmvExitThread;
    UINT32          m_uSmvBufPos;                                       //取缓冲区中的数据的位置
#endif

#if EN_GOOSE_DETECT
    HANDLE          m_hGFExitEvent;
    HANDLE          m_hGFNotifyEvent;                                     //用于通知通道x指定的接收操作完成
    HANDLE          m_hGFMapping;                                         //通道x用来缓冲数据的内存映射
    PBYTE           m_GFDataBuf;                                          //指向通道x的缓冲区
    HANDLE          m_hGFThread;
    BOOL            m_bGFExitThread;
    UINT32          m_uGFBufPos;                                          //取缓冲区中的数据的位置
#endif

};
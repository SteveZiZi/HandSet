#pragma once

#define MAP_FILE_LEN            MAX_FT3_FRAME*24
#define MAX_RECV_FT3_FRAME      1280                                    /*  应用程序能接收的最大报文数  */

#define MAX_FT3_FRAME           4096                                    /*  一个RAM缓冲区大小4096 words */

#define FT3_CONTROL_REG         0x0000
#define FT3_CONTROL_LEN         1
#define FT3_DATA_REG            FT3_CONTROL_REG+FT3_CONTROL_LEN

#define GUO_WANG  1
#define NAN_RUI   0
/*
**  国网每帧报文长度为37words  缓冲区存有110帧国网报文
*/
#define GUOWANG_FRAME_WORD      37
#define GUOWANG_FRAME_STEP      110
#define GUOWANG_FRAME_LEN       4070
/*
**  南瑞每帧报文长度为28words  缓冲区存有146帧南瑞报文
*/
#define NANRUI_FRAME_WORD       28
#define NANRUI_FRAME_STEP       146
#define NANRUI_FRAME_LEN        4088


//定义FT3的控制字，详见FT3内部规约
typedef union  _Ft3ControlWord
{
    struct 
    {
        UINT16 bNoErr:1;                                                //通讯正常
        UINT16 bReserve:4;                                              //保留
        UINT16 bFormat:2;                                               //规约格式，国网或南瑞
        UINT16 bSampleRate:4;                                           //采样率控制位
        UINT16 bBaudRate:4;                                             //波特率控制位
        UINT16 bCrc:1;                                                  //CRC校验控制位
    }CtrlBit;
    UINT16 data;
}Ft3ControlWord_t;

//在这里定义与FT3_采集相关的结构信息
typedef struct _Ft3SampleInfo
{
    UINT16 uResever;
    Ft3ControlWord_t uCtrl;                                             /*  FT3 控制字                  */
    UINT16 uData[MAX_FT3_FRAME-2];                                      /*  FT3 报文                    */
}Ft3SampleInfo_t, *PFt3SampleInfo_t;

typedef void (*PFN_FT3_CALLBACK)(void *pCallBackParam);

class _ft3_c {

public:
    _ft3_c( HANDLE hDevice);

    BOOL InitFt3(WCHAR *pNotifyEvtName, WCHAR *pMapFileName,
        WCHAR *pNotifyFT3ErrName,PFN_FT3_CALLBACK pfnCallBack,void *pCallBackParam);
    void DeInitFt3(void);

    HANDLE              m_hDevice;                                      /*  GPM驱动句柄                 */

    PFN_FT3_CALLBACK    m_pfnCallBack;              //当接收到数据时的回调函数
    void                *m_pCallBackParam;          //调用接收到数据时的回调函数的参数
    static              DWORD ft3ThreadProc(PVOID pArg);

    HANDLE              m_hExitEvent;
    HANDLE              m_hNotifyEvent;                                 /*  通知通道x指定的接收操作完成 */
    HANDLE              m_hMapping;                                     /*  通道x用来缓冲数据的内存映射 */
    HANDLE              m_hNotifyFT3Err;                                /*  FT3 接收异常的通知          */
    PFt3SampleInfo_t    m_DataBuf;                                      /*  指向通道x的缓冲区           */
    HANDLE              m_hThread;                                      /*  FT3 处理线程                */
    BOOL                m_bExitThread;                                  /*  是否退出线程                */
    Ft3SampleInfo_t     m_Ft3Frame;                                     /*  FT3 报文存储缓冲区          */
};

//中断服务线程
DWORD ft3ThreadProc1(PVOID pArg);
DWORD ft3ThreadProc2(PVOID pArg);

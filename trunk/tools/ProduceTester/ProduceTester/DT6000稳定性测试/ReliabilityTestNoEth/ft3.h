#pragma once

#define MAP_FILE_LEN            MAX_FT3_FRAME*24
#define MAX_RECV_FT3_FRAME      1280                                    /*  Ӧ�ó����ܽ��յ��������  */

#define MAX_FT3_FRAME           4096                                    /*  һ��RAM��������С4096 words */

#define FT3_CONTROL_REG         0x0000
#define FT3_CONTROL_LEN         1
#define FT3_DATA_REG            FT3_CONTROL_REG+FT3_CONTROL_LEN

#define GUO_WANG  1
#define NAN_RUI   0
/*
**  ����ÿ֡���ĳ���Ϊ37words  ����������110֡��������
*/
#define GUOWANG_FRAME_WORD      37
#define GUOWANG_FRAME_STEP      110
#define GUOWANG_FRAME_LEN       4070
/*
**  ����ÿ֡���ĳ���Ϊ28words  ����������146֡������
*/
#define NANRUI_FRAME_WORD       28
#define NANRUI_FRAME_STEP       146
#define NANRUI_FRAME_LEN        4088


//����FT3�Ŀ����֣����FT3�ڲ���Լ
typedef union  _Ft3ControlWord
{
    struct 
    {
        UINT16 bNoErr:1;                                                //ͨѶ����
        UINT16 bReserve:4;                                              //����
        UINT16 bFormat:2;                                               //��Լ��ʽ������������
        UINT16 bSampleRate:4;                                           //�����ʿ���λ
        UINT16 bBaudRate:4;                                             //�����ʿ���λ
        UINT16 bCrc:1;                                                  //CRCУ�����λ
    }CtrlBit;
    UINT16 data;
}Ft3ControlWord_t;

//�����ﶨ����FT3_�ɼ���صĽṹ��Ϣ
typedef struct _Ft3SampleInfo
{
    UINT16 uResever;
    Ft3ControlWord_t uCtrl;                                             /*  FT3 ������                  */
    UINT16 uData[MAX_FT3_FRAME-2];                                      /*  FT3 ����                    */
}Ft3SampleInfo_t, *PFt3SampleInfo_t;

typedef void (*PFN_FT3_CALLBACK)(void *pCallBackParam);

class _ft3_c {

public:
    _ft3_c( HANDLE hDevice);

    BOOL InitFt3(WCHAR *pNotifyEvtName, WCHAR *pMapFileName,
        WCHAR *pNotifyFT3ErrName,PFN_FT3_CALLBACK pfnCallBack,void *pCallBackParam);
    void DeInitFt3(void);

    HANDLE              m_hDevice;                                      /*  GPM�������                 */

    PFN_FT3_CALLBACK    m_pfnCallBack;              //�����յ�����ʱ�Ļص�����
    void                *m_pCallBackParam;          //���ý��յ�����ʱ�Ļص������Ĳ���
    static              DWORD ft3ThreadProc(PVOID pArg);

    HANDLE              m_hExitEvent;
    HANDLE              m_hNotifyEvent;                                 /*  ֪ͨͨ��xָ���Ľ��ղ������ */
    HANDLE              m_hMapping;                                     /*  ͨ��x�����������ݵ��ڴ�ӳ�� */
    HANDLE              m_hNotifyFT3Err;                                /*  FT3 �����쳣��֪ͨ          */
    PFt3SampleInfo_t    m_DataBuf;                                      /*  ָ��ͨ��x�Ļ�����           */
    HANDLE              m_hThread;                                      /*  FT3 �����߳�                */
    BOOL                m_bExitThread;                                  /*  �Ƿ��˳��߳�                */
    Ft3SampleInfo_t     m_Ft3Frame;                                     /*  FT3 ���Ĵ洢������          */
};

//�жϷ����߳�
DWORD ft3ThreadProc1(PVOID pArg);
DWORD ft3ThreadProc2(PVOID pArg);

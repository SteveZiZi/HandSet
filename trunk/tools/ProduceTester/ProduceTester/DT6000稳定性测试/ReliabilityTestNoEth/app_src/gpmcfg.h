
#pragma once

//------------���Ժ궨����-------------------------------------------

#ifdef _DEBUG
#define ZONE_ERROR      1
#define ZONE_INFO       0
#else
#define ZONE_ERROR      1
#define ZONE_INFO       0
#endif


#define ZONE_FT3_TX     1
#define ZONE_FT3_RX1    1
#define ZONE_FT3_RX2    1
#define ZONE_FIBER_A    1
#define ZONE_FIBER_B    1
#define ZONE_FIBER_C    1

#define ZONE_MU_TX      1

#define ZONE_RECORD     1                                   /*  1:������ʵʱ¼������֤��һ��ʱ���ڲ���֡*/

#define ZONE_VERIFY_INFO    1

#define ZONE_FT3_TIME_STRAP 1

#define DEMO_VERISON    (0x092)


#define APP_PRIO        108


//---------------Ӧ�ò������ָ��-------------------------------------

#define IOCTRL_GET_SYNCTIME_MODE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SET_SYNCTIME_MODE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_START_FPGA  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0306, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_LOWER_POWER  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0309, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_DRV_VERSION  /*  ��ȡ�����İ汾��  ��ʽ�̶�Ϊ"Vx.xx"*/\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0307, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_UTC_TIME     /*  ��ȡ��ǰFPGA��UTCʱ��*/\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0308, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SET_UTC_TIME     /*  ���õ�ǰFPGA��UTCʱ��*/\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030A, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_OPTICAL_POWER /*  ��ȡ����̫���ڵĹ⹦��ֵ    */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030B, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_OPTICAL_STATE /*  ��ȡ��ڵ�����״̬          */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030C, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_IRIGB_CONTROL /*  ��ȡB��ʱ�ӵĿ�����Ԫֵ    */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030D, METHOD_BUFFERED, FILE_ANY_ACCESS)


//���Ĵ�������
#define IOCTRL_DETECT   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0381, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_FILTER   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0382, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SET_RECV_LOOP   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0383, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_STOP_RECV   \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0384, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SET_RX_TIMEOUT \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0385, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_SEND_FRAME            /*  Ӧ�ó��������ͱ���        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0389, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SEND_HEADER          /*  Ӧ�ó������÷��Ͳ���        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_CHANGE_MU_FRAME          /*  ����MU�Ļ�����          */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_STOP_SEND            /*  Ӧ�ó��������ͱ���        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038C, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_IEEE1588_MAIN_CLOCK  /*  ����IEEE1588����ʱ��        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0390, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_OPERATOR_MDIO        /*  ����MDIO����                */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0399, METHOD_BUFFERED, FILE_ANY_ACCESS)

//-------------------------------------------------------------------------------------------------------
/*
 *  ��ڷ��ͻ���������ջ��������ã�
 *  1���ڽ���SMV����ʱ�����㲻�����κα��ĸ�FPGA
 *  2�����ͱ���ʱ���������GOOSE���ģ�GOOSE���ķ�����FIBER_LOOP_BUF0_POS��FIBER_LOOP_BUF1_POS
 *  3��SMV���ͱ��ķ�����GOOSE�������ĺ���
 *  4��GOOSE���ͱ��ķ�����SMV���͵ĺ���
 *  
 *  ���ͱ���ʱ�ڴ�ӳ���ļ����ݽṹ���£�
 *  ��һ����Map_File_Header_t ������ͷ��Ϣ
 *  �ڶ�����150KB��GOOSE����Ping Buffer
 *  ��������1035KB��SMV����PIng Buffer
 *  ��������15KB��GOOSE����Ping Buffer
 *  ��������150KB��GOOSE����Pong Buffer
 *  ��������1035KB��SMV����Pong Buffer
 *  ��������15KB��GOOSE����Pong Buffer
 */

/*  
**  FIBER_MAP_FILE_LEN : ����̫������ʱ���ڴ�ӳ���ļ���С
**  �����ձ��ĳ���,ӳ���ļ���ǰ����ͷ��Ϣ������������Ӧ�ò����Ϣ����
*/
#define FIBER_MAP_FILE_LEN         (FIBER_MAX_RECV_FRAMES * FIBER_MAX_LEN_A_FRAME+sizeof(Map_File_Header_t))
#define FIBER_MAX_RECV_FRAMES       10000/*1600*/                        /*  �����յı���֡�� 5000   */
#define FIBER_MAX_LEN_A_FRAME       1536                                /*  һ֡���ĵ���󳤶�         */
#define FIBER_LOOP_BUF0_POS           0
#define FIBER_LOOP_BUF1_POS         ((FIBER_MAP_FILE_LEN-sizeof(Map_File_Header_t))/2)

/*
 *  GOOSE ���յĻ�����λ��
 */
#define GOOSE_RX_MAX_GROUP          50                                  /*  �����Խ���50��GOOSE       */
#define GOOSE_RX_BUF_LEN            76800                               /*  ����Ϊ1536*50               */
#define GOOSE_RX_BUF0_POS           FIBER_LOOP_BUF0_POS
#define GOOSE_RX_BUF1_POS           FIBER_LOOP_BUF1_POS

/*
 *  IEEE1588���յĻ�����λ��
 */
#define IEEE1588_RX_MAX_GROUP       50                                  /*  �����Խ���50��IEEE1588   */
#define IEEE1588_RX_BUF_LEN         76800                               /*  ����Ϊ1536*50               */
#define IEEE1588_RX_BUF0_POS       (GOOSE_RX_BUF0_POS+GOOSE_RX_BUF_LEN)
#define IEEE1588_RX_BUF1_POS       (GOOSE_RX_BUF1_POS+GOOSE_RX_BUF_LEN)

/*
 *  SMV ���͵Ļ�����λ��
 */
#define SMV_TX_BUF_LEN              7511040                             /*  7335KB                      */
#define SMV_TX_BUF0_POS            (IEEE1588_RX_BUF0_POS+IEEE1588_RX_BUF_LEN)
#define SMV_TX_BUF1_POS            (IEEE1588_RX_BUF1_POS+IEEE1588_RX_BUF_LEN)

/*
 *  GOOSE���͵Ļ�����λ��
 */
#define GOOSE_TX_MAX_GROUP          10                                  /*  ��෢��10��GOOSE           */
#define GOOSE_TX_BUF_LEN            15360                               /*  ����Ϊ10 * 1536             */
#define GOOSE_TX_BUF0_POS          (SMV_TX_BUF0_POS+SMV_TX_BUF_LEN)
#define GOOSE_TX_BUF1_POS          (SMV_TX_BUF1_POS+SMV_TX_BUF_LEN)


//--------------------------------------------------------------------------------------------------------
#define FT3_MAP_FILE_LEN            (FT3_MAX_LEN_A_FRAME*24 + sizeof(Map_File_Header_t))
#define FT3_MAX_RECV_FRAMES         2640                                /*  Ӧ�ó����ܽ��յ��������  */
#define FT3_MAX_LEN_A_FRAME         8192/*4096*/                        /*  һ��RAM��������С4096 words */
#define FT3_LOOP_BUF0_POS           0
#define FT3_LOOP_BUF1_POS         ((FT3_MAP_FILE_LEN-sizeof(Map_File_Header_t))/2)

//--------------------------------------------------------------------------------------------------------
/*
 *  FT3_TX_MAP_FILE_LEN FT3����ʱ���ڴ�ӳ���ļ���С  256*84*2 = 42KB
 *  
 *  �����ձ��ĳ���,ӳ���ļ���ǰ����ͷ��Ϣ������������Ӧ�ò����Ϣ����
 */
#define FT3_TX_MAP_FILE_LEN         43008

#define FT3_TX_LOOP_BUF0_POS        0
#define FT3_TX_LOOP_BUF1_POS      ((FT3_TX_MAP_FILE_LEN-sizeof(Map_File_Header_t))/2)

#define FRAME_RECV_ERROR            1
#define FRAME_RECV_TIME_OUT         2
#define FRAME_INIT_ERROR            3

#define STR_FRAME_RECV_ERROR        L"���յı�������"
#define STR_FRAME_RECV_TIME_OUT     L"���ձ��ĳ�ʱ"
#define STR_FRAME_INIT_ERROR        L"��ʼ�����Ĵ���"

#define  FRAME_TEST_CMD_INI   1
#define  FRAME_TEST_CMD_EXT   2
#define  FRAME_TEST_CMD_END   3

#define  VERTIFY_TIMES      5


    #define NET_FIBER_NUM   6                                               /*  2������̫����, 1������̫���ڣ�3��FT3��*/
    #define DEVICE_FIBER_A 0
    #define DEVICE_FIBER_B 1
    #define DEVICE_FIBER_C 2
    #define DEVICE_FT3_RX1 3
    #define DEVICE_FT3_RX2 4
    #define DEVICE_FT3_TX  5

typedef enum {
    RECV_ONLY = 0,                                                      /*  ���ֻ�н��չ���            */
    SEND_ONLY,                                                          /*  ���ֻ�з��͹���            */
    RECV_SEND                                                           /*  ��ڼ��н������з��͹���    */
}FIBER_FUNC_e;


//-------------���ݽṹ����------------------------------------------

//-----------------------------------------
#define PRIO_SEND_FRAME     10

#define RECV_TIME_OUT            5000

#define WM_CHANGE_TX          WM_USER + 1
#define WM_RECOVER_TX          WM_USER + 2
#define WM_STOP_TX          WM_USER + 3
//-------------------------  FT3  ----------------------------------
/*
 *  FT3 ����IOCTRL���ò���
 */
typedef struct {
    UINT32 uDeviceItem;     //�豸�� 
    UINT32 uInBuf[4];       //����DeviceIoControl�����������������󳤶�4
}IOCTRL_IN_PARAM_t;

typedef struct {
    UINT16  uFramesLength;                                              //һ�����Ͷ���������
    UINT16  uFramesCnt;                                                 //һ����FPGA�д��Ͷ���֡����
    UINT16  uFramesStyle;                                               //��ǰFT3���ĵ�֡��ʽ
    UINT16  uReserve2;
}FT3_TX_CFG_t;

/*
 *  FT3���տ�����
 */
typedef union  _Ft3ControlWord
{
    struct {
        UINT16 bNoErr:1;                            //ͨѶ���� 0�쳣 1����
        UINT16 bReserve:2;                          //����
        UINT16 bFormat:4;                           //��Լ��ʽ
        UINT16 bSampleRate:4;                       //�����ʿ���λ
        UINT16 bBaudRate:4;                         //�����ʿ���λ
        UINT16 bCrc:1;                              //CRCУ�����λ
    }CtrlBit;
    UINT16 data;
}Ft3ControlWord_t;
/*
 *  FT3 ���Ϳ�����
 */
typedef union _Ft3SendCtlWord_t{
    struct {
        UINT16 bCntClr : 1;                                             /*  Bit[0] ����������           */
        UINT16 bCRCReverse : 1;                                         /*  CRC��0��ȡ�� 1����ȡ��      */
        UINT16 bCRCValue : 1;                                           /*  CRC��ֵ��0:0x0000,1:0xFFFF  */
        UINT16 bSendMode : 1;                                           /*  FT3����ģʽ��0:ͬ��         */
        UINT16 bFrameStyle : 4;                                         /*  ���ĸ�ʽ��1:����2:����    */
        UINT16 bSampleRate : 4;                                         /*  ������                      */
        UINT16 bBaudRate : 4;                                           /*  ������                      */
    }CtrlBit;
    UINT16 wData;
}Ft3SendCtlWord_t;

/*
 *  FT3���ĸ�ʽ����
 */
typedef enum _FT3_FRAME_STYLE {
    _NO_FT3 = 0,
    _NAN_RUI,               //�����ʽ��FT3����
    _GUO_WANG,              //������ʽ��FT3����
    _SINGLE_PHASE,          //���໥������FT3����
    _THREE_PHASE_CUR,       //���������������FT3����
    _THREE_PHASE_VOL,       //�����ѹ��������FT3����
    _THREE_PHASE_CURVOL,    //���������ѹ��������FT3����
    _RESERVE_FT3_,          //����

    _MAX_FT3_STYLE
}FT3FrameStyle_e;
/*
 *  FT3����������
 */
typedef enum _FT3_SMP_RATE {
    _4K = 1,
    _5K = 2,
    _8K = 3,
    _10K,
    _12p8K
}FT3SmpRate_e;
/*
 *  FT3����������
 */
typedef enum _FT3_BAUD_RATE {
    _2p5M  = 1,
    _5M,
    _10M
}FT3BaudRate_e;

#define FT3_TIME_STRAP_LEN      8
const UINT16 EXFT3_FRAME_LEN[_MAX_FT3_STYLE] = {0, 56, 74, 26, 34, 28, 48, 0};      //FT3���ĵ��ܳ��� ������ʼ�ַ���CRCУ���ַ�
const UINT16 EXFT3_FRAME_ACTIVE_LEN[_MAX_FT3_STYLE] = {0, 48, 64, 20, 28, 22, 40};  //FT3���ݼ��ĳ��� ȥ����ʼ�ַ���CRCУ���ַ�


//-------------------------------------  ���  ---------------------------------------------

typedef struct _FILTER_t {
    UINT8 MacAddr[6];
    UINT8 MacAppid[2];
}Filter_t;                                                               //���˲���;

typedef struct {
    UINT32 uDeviceItem;     //�豸�� 
    UINT32 uInBuf[20];
}IOCTRL_FIBER_IN_PARAM_t;
/*
 *  IOCTRL_SEND_HEADER ����������������һ��λ������ָ����ǰ�豸�ǽ����ֶ����黹��״̬���в���
 */
#define TX_TEST_STYLE_ITEM       19
typedef enum {
    _MANUAL_TEST,
    _STATUS_SEQUENCE_TEST
}TX_TEST_STYLE_e;


/*  
 *  ��������������÷���8��SMV��ʵ��ֻҪ����6��
 */
#define MAX_TX_FRAMES_CNT       8                
/*
 *  һ��SMV���ģ����֧��8��ASDU
 */
#define MAX_ASDU_NUM            8
/*
 *  FPGA����ֻ��8��MU���˵�ַ���ã�ʵ��ʹ�õ�6��
 */
#define MAX_FILTER_MU_CNT       8 

typedef enum {
    _NO_SYNC = 0,                                                       /*  ��ʹ��Уʱ                  */
    _IRIG_B_P = 1,                                                      /*  ����B��Уʱ                 */
    _IRIG_B_N,                                                          /*  ����B��Уʱ                 */
    _IEEE1588_A,                                                        /*  IEEE1588Уʱ                */
    _IEEE1588_B,                                                        /*  IEEE1588Уʱ                */
    _IEEE1588_C,                                                        /*  IEEE1588Уʱ                */
}SYNC_MODE_e;

typedef enum {
    _RECV_NONE = -1,
    _SMV_DETECT = 0,
    _SMV_FILTER,
    _GOOSE_DETECT,
    _GOOSE_FILTER,
    _IEEE1588_DETECT,
    _IEEE1588_FILTER,

    _MAX_NET_NUM
}NET_MODE;

typedef enum {
    _RX_SMV = 0,
    _RX_GOOSE,
    _RX_IEEE1588
}FIBER_RX_STYLE_e;


// typedef struct {
//     UINT16 uFramesCnt;
//     UINT16 uLen;
//     UINT16 uData[FPGA_RAM_LEN_W];
// }FpgaRamInfo_t, *PFpgaRamInfo_t;                                        /*  ����GPMC��ȡGOOSE������     */
#pragma pack(push)
#pragma pack (1)                                                        /*  ����Ϊ1Byte���룬Ĭ����8Byte*/
typedef struct {
    UINT8  uUTCTime[8];                                                 //UTC ʱ�� 64bit
    UINT16 uLen;                                                        //���� 32bit �� ֡���ĳ������  ��16λ�̶�Ϊ0
    UINT16 uReserve1;
    UINT16 uFrameLen;                                                   //֡���ĳ��� 
    UINT16 uResevre2;                               
    UINT8  uData[FIBER_MAX_LEN_A_FRAME-16];                             //֡����
}SMVFrame_t, *PSMVFrame_t;                                              /*  ʵ�ʵ�GOOSE���ĸ�ʽ         */
#define PCAP_FRAME_HEADER_LEN   16                                      /*  16�ֽڱ���ͷ                */
#pragma pack(pop)                                                       /*  �ָ�Ĭ�϶��뷽ʽ            */
/*
 *  ���ͻ������ṹ��
 */
typedef struct {
    UINT8  uFrames[FIBER_MAX_LEN_A_FRAME];
}SMV_TX_t;

typedef struct {
    UINT16 uFrameLen;
    UINT16 uStNumFlag;
    UINT8  uFrames[FIBER_MAX_LEN_A_FRAME];
}GOOSE_TX_t;

/*
 *  UTC ʱ��ṹ��
 */
#pragma pack(push)
#pragma pack (2)                                                        /*  ����Ϊ2Byte���룬Ĭ����8Byte*/
typedef struct {
    UINT32 uSecondSinceEpoch;   //��Ԫ��
    UINT32 uFractionOfSecond;   //���С������
}UTC_TIME_t;
#pragma pack(pop)                                                       /*  �ָ�Ĭ�϶��뷽ʽ            */

typedef struct {
    UINT32 uClockId[2];
    UINT32 uSourcePortId;
}IEEE1588_CLOCKID_t;

typedef struct {
    BOOL bReadOperator;
    UINT32 uAddr;
    UINT32 uPageSel;
    UINT32 uData;
}MDIO_OPERATOR_t;



//���½ṹ��̶�32*sizeof(UINT16)����
typedef union {
    struct {
        UINT16 uLoopItem;               //ѭ������ʱ ʹ����һ�������� ֻ���� 0 1
        UINT16 bAppDispDone;            //���Ӧ�û�û�д�������һ�α���
        UINT16 uFramesSum;              //�ڴ�ӳ���ļ��������н��յ����ĵ���֡��
        UINT16 uIsTimeOut;              //�Ƿ������ճ�ʱ
        Ft3ControlWord_t tCtrlWord;     //FT3���ĸ�ʽ������

        UINT16 uCurSmvTxBufItem;        //��ǰSMV���ͻ����������
        UINT32 uActiveLength;           //����������Ч�Ŀռ䳤��

    }ft3Header;

    struct {
        UINT16 uLoopItem;               //ѭ������ʱ ʹ����һ�������� ֻ���� 0 1
        UINT16 bAppDispDone;            //���Ӧ�û�û�д�������һ�α���
        UINT16 uFramesSum;              //�ڴ�ӳ���ļ��������н��յ����ĵ���֡��
        UINT16 uIsTimeOut;              //�Ƿ������ճ�ʱ

        UINT16 uNetMode;                //��ǰ��ڵĹ���ģʽ

        UINT16 uCurSmvTxBufItem;        //��ǰSMV���ͻ����������
        UINT32 uActiveLength;           //����������Ч�Ŀռ䳤��,��uFramesSum֡����ռ�õĻ������Ŀռ䳤��
        UINT16 uCurGooseTxBufItem;      //��ǰGOOSE���ͻ����������
    }OpticalNetHeader;

    UINT16 uData[32];   
}Map_File_Header_t;


#include "dp83640.h"


/*
 *  Demo Variable
 */
#define FLASHDISK_TEST_ITEM 10
#define SDCARD_TEST_ITEM 12
typedef UINT32 TEST_RESULT_a[14];



#define DM8000_TEST_FLASHDISK_PATH      _T("\\FlashDisk2\\DM8000TestFile")
#define DM8000_TEST_SDCard_PATH         _T("\\Storage Card\\DM8000TestFile")

#define DM8000_TEST_FLASHDISK_RENAME_PATH   _T("\\FlashDisk2\\DM8000TestFile_Pre")
#define DM8000_TEST_SDCard_RENAME_PATH      _T("\\Storage Card\\DM8000TestFile_Pre")

#define DM8000_TEST_FILENAME            _T("\\DM8000TestInfo.txt")
#define DM8000_RECORD_FILENAME0         _T("\\DM8000TestTimeRecord0.txt")
#define DM8000_RECORD_FILENAME1         _T("\\DM8000TestTimeRecord1.txt")


#pragma once

//------------调试宏定义区-------------------------------------------

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

#define ZONE_RECORD     1                                   /*  1:不进行实时录波，保证在一段时间内不丢帧*/

#define ZONE_VERIFY_INFO    1

#define ZONE_FT3_TIME_STRAP 1

#define DEMO_VERISON    (0x092)


#define APP_PRIO        108


//---------------应用层操作宏指令-------------------------------------

#define IOCTRL_GET_SYNCTIME_MODE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SET_SYNCTIME_MODE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_START_FPGA  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0306, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_LOWER_POWER  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0309, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_DRV_VERSION  /*  获取驱动的版本号  格式固定为"Vx.xx"*/\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0307, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_UTC_TIME     /*  获取当前FPGA的UTC时间*/\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0308, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SET_UTC_TIME     /*  设置当前FPGA的UTC时间*/\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030A, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_OPTICAL_POWER /*  获取光以太网口的光功率值    */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030B, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_OPTICAL_STATE /*  获取光口的连接状态          */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030C, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_GET_IRIGB_CONTROL /*  获取B码时钟的控制码元值    */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x030D, METHOD_BUFFERED, FILE_ANY_ACCESS)


//报文处理命令
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

#define IOCTRL_SEND_FRAME            /*  应用程序请求发送报文        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0389, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_SEND_HEADER          /*  应用程序配置发送参数        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_CHANGE_MU_FRAME          /*  更改MU的缓冲区          */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTRL_STOP_SEND            /*  应用程序请求发送报文        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x038C, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_IEEE1588_MAIN_CLOCK  /*  设置IEEE1588的主时钟        */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0390, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTRL_OPERATOR_MDIO        /*  操作MDIO总线                */\
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0399, METHOD_BUFFERED, FILE_ANY_ACCESS)

//-------------------------------------------------------------------------------------------------------
/*
 *  光口发送缓冲区与接收缓冲区共用，
 *  1、在接收SMV报文时驱动层不发送任何报文给FPGA
 *  2、发送报文时，允许接收GOOSE报文，GOOSE报文放置在FIBER_LOOP_BUF0_POS、FIBER_LOOP_BUF1_POS
 *  3、SMV发送报文放置在GOOSE接收区的后面
 *  4、GOOSE发送报文放置在SMV发送的后面
 *  
 *  发送报文时内存映射文件内容结构如下：
 *  第一区：Map_File_Header_t 共享区头信息
 *  第二区：150KB的GOOSE接收Ping Buffer
 *  第三区：1035KB的SMV发送PIng Buffer
 *  第四区：15KB的GOOSE发送Ping Buffer
 *  第五区：150KB的GOOSE接收Pong Buffer
 *  第六区：1035KB的SMV发送Pong Buffer
 *  第七区：15KB的GOOSE发送Pong Buffer
 */

/*  
**  FIBER_MAP_FILE_LEN : 光以太网接收时的内存映射文件大小
**  最大接收报文长度,映射文件最前端是头信息，便于驱动与应用层的信息传递
*/
#define FIBER_MAP_FILE_LEN         (FIBER_MAX_RECV_FRAMES * FIBER_MAX_LEN_A_FRAME+sizeof(Map_File_Header_t))
#define FIBER_MAX_RECV_FRAMES       10000/*1600*/                        /*  最多接收的报文帧数 5000   */
#define FIBER_MAX_LEN_A_FRAME       1536                                /*  一帧报文的最大长度         */
#define FIBER_LOOP_BUF0_POS           0
#define FIBER_LOOP_BUF1_POS         ((FIBER_MAP_FILE_LEN-sizeof(Map_File_Header_t))/2)

/*
 *  GOOSE 接收的缓冲区位置
 */
#define GOOSE_RX_MAX_GROUP          50                                  /*  最多可以接收50组GOOSE       */
#define GOOSE_RX_BUF_LEN            76800                               /*  长度为1536*50               */
#define GOOSE_RX_BUF0_POS           FIBER_LOOP_BUF0_POS
#define GOOSE_RX_BUF1_POS           FIBER_LOOP_BUF1_POS

/*
 *  IEEE1588接收的缓冲区位置
 */
#define IEEE1588_RX_MAX_GROUP       50                                  /*  最多可以接收50组IEEE1588   */
#define IEEE1588_RX_BUF_LEN         76800                               /*  长度为1536*50               */
#define IEEE1588_RX_BUF0_POS       (GOOSE_RX_BUF0_POS+GOOSE_RX_BUF_LEN)
#define IEEE1588_RX_BUF1_POS       (GOOSE_RX_BUF1_POS+GOOSE_RX_BUF_LEN)

/*
 *  SMV 发送的缓冲区位置
 */
#define SMV_TX_BUF_LEN              7511040                             /*  7335KB                      */
#define SMV_TX_BUF0_POS            (IEEE1588_RX_BUF0_POS+IEEE1588_RX_BUF_LEN)
#define SMV_TX_BUF1_POS            (IEEE1588_RX_BUF1_POS+IEEE1588_RX_BUF_LEN)

/*
 *  GOOSE发送的缓冲区位置
 */
#define GOOSE_TX_MAX_GROUP          10                                  /*  最多发出10组GOOSE           */
#define GOOSE_TX_BUF_LEN            15360                               /*  长队为10 * 1536             */
#define GOOSE_TX_BUF0_POS          (SMV_TX_BUF0_POS+SMV_TX_BUF_LEN)
#define GOOSE_TX_BUF1_POS          (SMV_TX_BUF1_POS+SMV_TX_BUF_LEN)


//--------------------------------------------------------------------------------------------------------
#define FT3_MAP_FILE_LEN            (FT3_MAX_LEN_A_FRAME*24 + sizeof(Map_File_Header_t))
#define FT3_MAX_RECV_FRAMES         2640                                /*  应用程序能接收的最大报文数  */
#define FT3_MAX_LEN_A_FRAME         8192/*4096*/                        /*  一个RAM缓冲区大小4096 words */
#define FT3_LOOP_BUF0_POS           0
#define FT3_LOOP_BUF1_POS         ((FT3_MAP_FILE_LEN-sizeof(Map_File_Header_t))/2)

//--------------------------------------------------------------------------------------------------------
/*
 *  FT3_TX_MAP_FILE_LEN FT3发送时的内存映射文件大小  256*84*2 = 42KB
 *  
 *  最大接收报文长度,映射文件最前端是头信息，便于驱动与应用层的信息传递
 */
#define FT3_TX_MAP_FILE_LEN         43008

#define FT3_TX_LOOP_BUF0_POS        0
#define FT3_TX_LOOP_BUF1_POS      ((FT3_TX_MAP_FILE_LEN-sizeof(Map_File_Header_t))/2)

#define FRAME_RECV_ERROR            1
#define FRAME_RECV_TIME_OUT         2
#define FRAME_INIT_ERROR            3

#define STR_FRAME_RECV_ERROR        L"接收的报文有误"
#define STR_FRAME_RECV_TIME_OUT     L"接收报文超时"
#define STR_FRAME_INIT_ERROR        L"初始化报文错误"

#define  FRAME_TEST_CMD_INI   1
#define  FRAME_TEST_CMD_EXT   2
#define  FRAME_TEST_CMD_END   3

#define  VERTIFY_TIMES      5


    #define NET_FIBER_NUM   6                                               /*  2个光以太网口, 1个电以太网口，3个FT3口*/
    #define DEVICE_FIBER_A 0
    #define DEVICE_FIBER_B 1
    #define DEVICE_FIBER_C 2
    #define DEVICE_FT3_RX1 3
    #define DEVICE_FT3_RX2 4
    #define DEVICE_FT3_TX  5

typedef enum {
    RECV_ONLY = 0,                                                      /*  光口只有接收功能            */
    SEND_ONLY,                                                          /*  光口只有发送功能            */
    RECV_SEND                                                           /*  光口既有接收又有发送功能    */
}FIBER_FUNC_e;


//-------------数据结构定义------------------------------------------

//-----------------------------------------
#define PRIO_SEND_FRAME     10

#define RECV_TIME_OUT            5000

#define WM_CHANGE_TX          WM_USER + 1
#define WM_RECOVER_TX          WM_USER + 2
#define WM_STOP_TX          WM_USER + 3
//-------------------------  FT3  ----------------------------------
/*
 *  FT3 发送IOCTRL配置参数
 */
typedef struct {
    UINT32 uDeviceItem;     //设备号 
    UINT32 uInBuf[4];       //传入DeviceIoControl函数的输入参数，最大长度4
}IOCTRL_IN_PARAM_t;

typedef struct {
    UINT16  uFramesLength;                                              //一共发送多少数据量
    UINT16  uFramesCnt;                                                 //一次向FPGA中传送多少帧报文
    UINT16  uFramesStyle;                                               //当前FT3报文的帧格式
    UINT16  uReserve2;
}FT3_TX_CFG_t;

/*
 *  FT3接收控制字
 */
typedef union  _Ft3ControlWord
{
    struct {
        UINT16 bNoErr:1;                            //通讯正常 0异常 1正常
        UINT16 bReserve:2;                          //保留
        UINT16 bFormat:4;                           //规约格式
        UINT16 bSampleRate:4;                       //采样率控制位
        UINT16 bBaudRate:4;                         //波特率控制位
        UINT16 bCrc:1;                              //CRC校验控制位
    }CtrlBit;
    UINT16 data;
}Ft3ControlWord_t;
/*
 *  FT3 发送控制字
 */
typedef union _Ft3SendCtlWord_t{
    struct {
        UINT16 bCntClr : 1;                                             /*  Bit[0] 计数器清零           */
        UINT16 bCRCReverse : 1;                                         /*  CRC，0：取反 1：不取反      */
        UINT16 bCRCValue : 1;                                           /*  CRC初值，0:0x0000,1:0xFFFF  */
        UINT16 bSendMode : 1;                                           /*  FT3发送模式，0:同步         */
        UINT16 bFrameStyle : 4;                                         /*  报文格式，1:南瑞，2:国网    */
        UINT16 bSampleRate : 4;                                         /*  采样率                      */
        UINT16 bBaudRate : 4;                                           /*  波特率                      */
    }CtrlBit;
    UINT16 wData;
}Ft3SendCtlWord_t;

/*
 *  FT3报文格式分类
 */
typedef enum _FT3_FRAME_STYLE {
    _NO_FT3 = 0,
    _NAN_RUI,               //南瑞格式的FT3报文
    _GUO_WANG,              //国网格式的FT3报文
    _SINGLE_PHASE,          //单相互感器的FT3报文
    _THREE_PHASE_CUR,       //三相电流互感器的FT3报文
    _THREE_PHASE_VOL,       //三相电压互感器的FT3报文
    _THREE_PHASE_CURVOL,    //三相电流电压互感器的FT3报文
    _RESERVE_FT3_,          //保留

    _MAX_FT3_STYLE
}FT3FrameStyle_e;
/*
 *  FT3采样率设置
 */
typedef enum _FT3_SMP_RATE {
    _4K = 1,
    _5K = 2,
    _8K = 3,
    _10K,
    _12p8K
}FT3SmpRate_e;
/*
 *  FT3波特率设置
 */
typedef enum _FT3_BAUD_RATE {
    _2p5M  = 1,
    _5M,
    _10M
}FT3BaudRate_e;

#define FT3_TIME_STRAP_LEN      8
const UINT16 EXFT3_FRAME_LEN[_MAX_FT3_STYLE] = {0, 56, 74, 26, 34, 28, 48, 0};      //FT3报文的总长度 包括起始字符、CRC校验字符
const UINT16 EXFT3_FRAME_ACTIVE_LEN[_MAX_FT3_STYLE] = {0, 48, 64, 20, 28, 22, 40};  //FT3数据集的长度 去除起始字符、CRC校验字符


//-------------------------------------  光口  ---------------------------------------------

typedef struct _FILTER_t {
    UINT8 MacAddr[6];
    UINT8 MacAppid[2];
}Filter_t;                                                               //过滤参数;

typedef struct {
    UINT32 uDeviceItem;     //设备号 
    UINT32 uInBuf[20];
}IOCTRL_FIBER_IN_PARAM_t;
/*
 *  IOCTRL_SEND_HEADER 命令输入参数的最后一个位置用于指定当前设备是进行手动试验还是状态序列操作
 */
#define TX_TEST_STYLE_ITEM       19
typedef enum {
    _MANUAL_TEST,
    _STATUS_SEQUENCE_TEST
}TX_TEST_STYLE_e;


/*  
 *  驱动下最多能设置发送8组SMV，实际只要求发送6组
 */
#define MAX_TX_FRAMES_CNT       8                
/*
 *  一组SMV报文，最多支持8个ASDU
 */
#define MAX_ASDU_NUM            8
/*
 *  FPGA下面只有8个MU过滤地址设置，实际使用到6个
 */
#define MAX_FILTER_MU_CNT       8 

typedef enum {
    _NO_SYNC = 0,                                                       /*  不使用校时                  */
    _IRIG_B_P = 1,                                                      /*  正向B码校时                 */
    _IRIG_B_N,                                                          /*  反向B码校时                 */
    _IEEE1588_A,                                                        /*  IEEE1588校时                */
    _IEEE1588_B,                                                        /*  IEEE1588校时                */
    _IEEE1588_C,                                                        /*  IEEE1588校时                */
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
// }FpgaRamInfo_t, *PFpgaRamInfo_t;                                        /*  用于GPMC读取GOOSE的数据     */
#pragma pack(push)
#pragma pack (1)                                                        /*  配置为1Byte对齐，默认是8Byte*/
typedef struct {
    UINT8  uUTCTime[8];                                                 //UTC 时间 64bit
    UINT16 uLen;                                                        //长度 32bit 与 帧报文长度相等  高16位固定为0
    UINT16 uReserve1;
    UINT16 uFrameLen;                                                   //帧报文长度 
    UINT16 uResevre2;                               
    UINT8  uData[FIBER_MAX_LEN_A_FRAME-16];                             //帧报文
}SMVFrame_t, *PSMVFrame_t;                                              /*  实际的GOOSE报文格式         */
#define PCAP_FRAME_HEADER_LEN   16                                      /*  16字节报文头                */
#pragma pack(pop)                                                       /*  恢复默认对齐方式            */
/*
 *  发送缓冲区结构体
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
 *  UTC 时间结构体
 */
#pragma pack(push)
#pragma pack (2)                                                        /*  配置为2Byte对齐，默认是8Byte*/
typedef struct {
    UINT32 uSecondSinceEpoch;   //纪元秒
    UINT32 uFractionOfSecond;   //秒的小数部分
}UTC_TIME_t;
#pragma pack(pop)                                                       /*  恢复默认对齐方式            */

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



//以下结构体固定32*sizeof(UINT16)长度
typedef union {
    struct {
        UINT16 uLoopItem;               //循环接收时 使用哪一个缓冲区 只能是 0 1
        UINT16 bAppDispDone;            //标记应用还没有处理完上一次报文
        UINT16 uFramesSum;              //内存映射文件缓冲区中接收到报文的总帧数
        UINT16 uIsTimeOut;              //是否发生接收超时
        Ft3ControlWord_t tCtrlWord;     //FT3报文格式控制字

        UINT16 uCurSmvTxBufItem;        //当前SMV发送缓冲区的序号
        UINT32 uActiveLength;           //共享缓冲区有效的空间长度

    }ft3Header;

    struct {
        UINT16 uLoopItem;               //循环接收时 使用哪一个缓冲区 只能是 0 1
        UINT16 bAppDispDone;            //标记应用还没有处理完上一次报文
        UINT16 uFramesSum;              //内存映射文件缓冲区中接收到报文的总帧数
        UINT16 uIsTimeOut;              //是否发生接收超时

        UINT16 uNetMode;                //当前光口的工作模式

        UINT16 uCurSmvTxBufItem;        //当前SMV发送缓冲区的序号
        UINT32 uActiveLength;           //共享缓冲区有效的空间长度,是uFramesSum帧报文占用的缓冲区的空间长度
        UINT16 uCurGooseTxBufItem;      //当前GOOSE发送缓冲区的序号
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

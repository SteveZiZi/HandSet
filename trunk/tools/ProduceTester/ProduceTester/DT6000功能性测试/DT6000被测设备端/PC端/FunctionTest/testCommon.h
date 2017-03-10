#ifndef        __TEST_COMMON_H__
#define        __TEST_COMMON_H__

#ifdef        __cplusplus
extern "C" {
#endif

/***************************************************************************************************
**FileName:            testCommon.h
**Description:        这个文件定义了有以太网功能性测试程序PC端和CE端的所共有的一些结构体和常量
**Author:             chao
***************************************************************************************************/
//定义在通信连接(如网络)上传输的数据包，本功能性测试程序发出的所有命令数据包都被封装成
//这种形式
#pragma  pack(push)
#pragma  pack(1)
typedef struct _PACKET
{
    UINT16 uiCmd;
    UINT16 param;
    UINT16 uiDatalen;
    BYTE pData[1024];
}PACKET;
#pragma  pack(pop)






//定义一些可能会被很多测试项所用到的结构体，包括测试信息，设备信息
//由于这些结构体中含有字符串，为了防止工控板和PC端所使用的编码不一致，规定传输的时候
//字符串都使用UTF8编码,所以这些结构体都有两个版本PC和CE工控板端的版本、用于传输的版本

//定义PC和CE端所使用的这些结构体的版本
//PC和CE端所使用的这些结构体字符串均为宽字符类型(whar_t),但是程序实际运行时whar_t的大小
//可能是不一样的

//定义测试信息结构，测试信息结构可以用于描述初始化操作的果，很多测试项的某次测试的结果
typedef     struct __TEST_INFO
{
    UINT16      bError;                     //测试操作是否发生错误，0为无错误，其它值不同的测试项可能有不同的含义
    UINT16      uiErrorLen;                 //错误信息有效字符wchar_t的个数，不包括结尾的0字符
    BYTE        bAppend[16];                //附加字段，其含义各测试项可以自定义
    wchar_t     strErr[200];                //由于对测试的错误信息进行描述
}TEST_INFO;
//定义设备信息结构
typedef  struct __DEVICE_INFO
{
    wchar_t     strDeviceType[32];          //设备类型
    wchar_t     strDeviceID[64];            //设备标识
    wchar_t     strDeviceVer[32];           //设备版本号
}DEVICE_INFO;

//定义传输时版本的测试信息，设备信息结构
#pragma  pack(push)
#pragma pack(1)
typedef     struct __TEST_INFO_TRANSFER
{
    UINT16      bError;                     //测试操作是否发生错误，0为无错误，其它值不同的测试项可能有不同的含义
    UINT16      uiErrorLen;                 //错误信息有效字符的字节数，不包括结尾的0字符
    BYTE        bAppend[16];                //附加字段，其含义各测试项可以自定义
    char        strErr[800];                //由于对测试的错误信息进行描述
}TEST_INFO_TRANSFER;

typedef  struct __DEVICE_INFO_TRANSFER
{
    char        strDeviceType[128];          //设备类型
    char        strDeviceID[256];            //设备标识
    char        strDeviceVer[32];            //设备版本号
}DEVICE_INFO_TRANSFER;
#pragma  pack(pop)

BOOL DeviceInfoLocalToTransfer(DEVICE_INFO_TRANSFER *pDevInfoTrans,const DEVICE_INFO *pDevInfoLocal);
BOOL DeviceInfoTransferToLocal(DEVICE_INFO *pDevInfoLocal,const DEVICE_INFO_TRANSFER *pDevInfoTrans);
BOOL TestInfoLocalToTransfer  (TEST_INFO_TRANSFER *pTestInfoTrans,const TEST_INFO *pTestInfoLocal);
BOOL TestInfoTransferToLocal  (TEST_INFO *pTestInfoLocal,const TEST_INFO_TRANSFER *pTestInfoTrans);


//定义PC端命令工控板进行某项测试时的发送的命令码
#define         MAX_TEST_OBJ            64              //需要实际进行测试的测试项的最大个数，
                                                        //它们所对应的命令码也不会超过这个值
#define         CMD_TEST_ETHERNET1      0               //以太网1
#define         CMD_TEST_ETHERNET2      1               //以太网2
#define         CMD_TEST_ETHERNET3      2               //以太网3
#define         CMD_TEST_SERIAL1        3               //串口测试项1
#define         CMD_TEST_SERIAL2        4               //串口测试项2
#define         CMD_TEST_SERIAL3        5               //串口测试项3
#define         CMD_TEST_SERIAL4        6               //串口测试项4
#define         CMD_TEST_SERIAL5        7               //串口测试项5
#define         CMD_TEST_SERIAL6        8               //串口测试项6
#define         CMD_TEST_SERIAL7        9               //串口测试项7
#define         CMD_TEST_SERIAL8        10              //串口测试项8
#define         CMD_TEST_SERIAL9        11              //串口测试项9
#define         CMD_TEST_SERIAL10       12              //串口测试项10
#define         CMD_TEST_SERIAL11       13              //串口测试项11

#define         CMD_TEST_CAN1           14              //CAN测试项1
#define         CMD_TEST_CAN2           15              //CAN测试项2
#define         CMD_TEST_CAN3           16              //CAN测试项3
#define         CMD_TEST_CAN4           17              //CAN测试项4
#define         CMD_TEST_CAN5           18              //CAN测试项5
#define         CMD_TEST_CAN6           19              //CAN测试项6

#define         CMD_TEST_DISK1          20              //磁盘测试项1
#define         CMD_TEST_DISK2          21              //磁盘测试项2
#define         CMD_TEST_DISK3          22              //磁盘测试项3
#define         CMD_TEST_DISK4          23              //磁盘测试项4
#define         CMD_TEST_DISK5          24              //磁盘测试项5
#define         CMD_TEST_DISK6          25              //磁盘测试项6
#define         CMD_TEST_DISK7          26              //磁盘测试项7
#define         CMD_TEST_DISK8          27              //磁盘测试项8
#define         CMD_TEST_DISK9          28              //磁盘测试项9
#define         CMD_TEST_DISK10         29              //磁盘测试项10
#define         CMD_TEST_DISK11         30              //磁盘测试项11

#define         CMD_TEST_ISA            31              //ISA测试项
#define         CMD_TEST_ACTIVE         32              //ActiveSync测试项
#define         CMD_TEST_GPIO           33              //GPIO测试项
#define         CMD_TEST_LED            34              //LED灯测试项
#define         CMD_TEST_BEEP           35              //蜂鸣器测试项
#define         CMD_TEST_SCREEN         36              //屏幕测试项
#define         CMD_TEST_RTC            37              //RTC测试项
#define         CMD_TEST_AUDIO          38              //音频测试
#define         CMD_TEST_VEDIO          39              //视频，摄像头测试项
#define         CMD_TEST_AD             40              //AD测试项
#define         CMD_TEST_GPRS           41              //GPRS测试项
#define         CMD_TEST_ZIGBEE         42              //ZigBee测试项
#define         CMD_TEST_KEYBORAD       43              //键盘测试项
#define         CMD_TEST_KBD_IO         44              //扩展键盘IO测试项
#define         CMD_TEST_7290           45              //7290测试项

#define         CMD_TEST_USE_FT3_RX1    46              //FT3_RX1接收测试
#define         CMD_TEST_USE_FT3_RX2    47              //FT3_RX2接收测试
#define         CMD_TEST_USE_FT3_TX1    48              //FT3_TX发送测试
#define         CMD_TEST_USE_MU1        49              //光以太网1收发测试
#define         CMD_TEST_USE_MU2        50              //光以太网2收发测试
#define         CMD_TEST_USE_MU3        51              //电以太网收发
#define         CMD_TEST_USE_B_CODE     52              //IRIG_B校时
#define         CMD_TEST_USE_APP        53              //APP测试

#define         CMD_TEST_WATCH_DOG      60              //看门狗测试项
#define         CMD_TEST_RFID           55              //RFID模块测试项
#define         CMD_TEST_GPS            56              //GPS模块测试项
#define         CMD_TEST_RELAY          57              //继电器测试
#define         CMD_TEST_SPI_FLASH      58              //SPI Flash测试


#define         CMD_DEVICE_INFO         0x8000          //获取设备信息的命令码
#define         CMD_TEST_CONNECT        0x8001          //用于测试连接的命令码
#define         CMD_TEST_CONNECT_ACK    0x8002          //用于测试连接的命令码


#define  FRAME_TEST_CMD_INI   1
#define  FRAME_TEST_CMD_EXT   2
#define  FRAME_TEST_CMD_END   3

//定义使用的连接方式
typedef enum __tagConnectWay
{
    CONNECT_WAY_ETNERNET=0,                             //通过以太网连接
    CONNECT_WAY_ACTIVE_SYNC,                            //通过ActiveSync连接
}CONNECT_WAY;


//定义所有测试项数据包中的参数的公共部分
#define         PARAM_GET_INIT_INFO     0x8000          //获取工控板上该测试项的初始化信息

//定义功能测试服务器端(设备端)通信连接所使用的端口
#define         SERVER_PORT             5031
#define         ETH_TEST_SERVER_PORT    5032            //以太网测试用到的tcp服务器端口
#define         ETH_TEST_PACKET_LEN     4096            //以太网测试每一个数据包的大小

#define         MAX_ETHERNET_NUM         3               //定义最大以太网数目    
#ifdef        __cplusplus
}
#endif

#endif                                                //For the definition of __TEST_COMMON_H__
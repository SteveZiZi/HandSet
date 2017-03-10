#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

//定义用于测试数据包的索引码，然而实际用于测试的命令码不会超过MAX_TEST_OBJ
#define CMD_DEVICE_INFO         0x1000      //获取设备信息
#define CMD_TEST_CONNECT        0x1001      //用于测试连接的连通性

//以太网测试的命令码，以太网部分的命令码应该是连续的
#define CMD_TEST_ETHERNET1      0           //测试以太网1
#define CMD_TEST_ETHERNET2      1           //测试以太网2

#define CMD_TEST_DISK1          2           //测试磁盘1，磁盘包含FlashDisk，SD卡，TF卡等
#define CMD_TEST_DISK2          3           //测试磁盘2
#define CMD_TEST_DISK3          4           //磁盘测试3
#define CMD_TEST_DISK4          5           //磁盘测试4

#define CMD_TEST_SERIAL1        6           //串口类测试1
#define CMD_TEST_SERIAL2        7           //串口类测试2
#define CMD_TEST_SERIAL3        8           //串口类测试3
#define CMD_TEST_SERIAL4        9           //串口类测试4
#define CMD_TEST_SERIAL5        10          //串口类测试5

#define CMD_TEST_CAN1           11          //CAN类测试1
#define CMD_TEST_CAN2           12          //CAN类测试2
#define CMD_TEST_CAN3           13          //CAN类测试3

#define CMD_TEST_GPIO           14          //GPIO测试
#define CMD_TEST_LED            15          //LED测试
#define CMD_TEST_BEEP           16          //蜂鸣器测试
#define CMD_TEST_ISA            17          //ISA测试
#define CMD_TEST_AD             18          //AD测试
#define CMD_TEST_RTC            19          //RTC测试
#define CMD_TEST_WATCHDOG       20          //看门狗测试


//定义数据包公用参数的含义，仅对测试有效，0x1000以上为所有测试公用参数
#define PARAM_GET_INIT_INFO     0x1000      //获取初始化信息



//定义以太网所使用的端口
#define PC_SERVER_PORT          2049        //PC服务端所使用的端口
//以太网测试的服务器端口应该是连续的
#define ETHERNET_PORT           2050        //以太网1测试的服务器端口


//以太网测试每包的大小
#define  ETH_TEST_PACKET_LEN    10240
#define  DISK_TEST_LEN          1024        //磁盘测试每次写入的字节数

//定义TEST_INFO结构体bError字段专用的标识码
#define TEST_ERROR_NO_ERROR     0
#define TEST_ERROR_NORMAL       1
#define TEST_ERROR_NEED_REINIT  2
#define TEST_ERROR_NO_TEST_ITEM 3            //客户端并没有这个测试项

//定义测试信息的结构体，这个结构体适用于大多数简单的测试
//其中如果bError为0，则表示没有错误发生，为1的话strError描述了测试错误信息
//为2的话，表示设备需要重新初始化，但是在执行获取初始化信息时不会出现这个值
//另外
typedef struct _TEST_INFO 
{
    UINT32  bError;                          //是否发生错误
    UINT32  iErrLen;                         //错误信息的长度，包括结尾的0
    WCHAR   strError[121];                   //可以容纳大概120个字符的错误信息
}TEST_INFO;

//定义一个设备最大支持的测试项目
#define  MAX_TEST_OBJ  64


//定义已连接上的测试设备的信息
struct DEVICE_INFO
{
    WCHAR strDeviceID[32];                  //设备的标识，在一次高低温测试中，它应该是唯一的
    WCHAR strDeviceType[32];                //设备的类型，例如EPC-8000

    int   iEthNum;                          //测试设备的网卡数目
    UINT32 Ip[10];

#ifdef __cplusplus
    DEVICE_INFO():
    iEthNum(0)
    {
        for(int i=0;i<10;i++)
            Ip[i]=0;
    }
#endif
   
};


//定义通用的connect接收超时，ms为单位
#define GLOBAL_RECV_TIME_OUT 60000


#endif
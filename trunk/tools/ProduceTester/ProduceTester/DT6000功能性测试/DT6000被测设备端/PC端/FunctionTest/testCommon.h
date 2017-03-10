#ifndef        __TEST_COMMON_H__
#define        __TEST_COMMON_H__

#ifdef        __cplusplus
extern "C" {
#endif

/***************************************************************************************************
**FileName:            testCommon.h
**Description:        ����ļ�����������̫�������Բ��Գ���PC�˺�CE�˵������е�һЩ�ṹ��ͳ���
**Author:             chao
***************************************************************************************************/
//������ͨ������(������)�ϴ�������ݰ����������Բ��Գ��򷢳��������������ݰ�������װ��
//������ʽ
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






//����һЩ���ܻᱻ�ܶ���������õ��Ľṹ�壬����������Ϣ���豸��Ϣ
//������Щ�ṹ���к����ַ�����Ϊ�˷�ֹ���ذ��PC����ʹ�õı��벻һ�£��涨�����ʱ��
//�ַ�����ʹ��UTF8����,������Щ�ṹ�嶼�������汾PC��CE���ذ�˵İ汾�����ڴ���İ汾

//����PC��CE����ʹ�õ���Щ�ṹ��İ汾
//PC��CE����ʹ�õ���Щ�ṹ���ַ�����Ϊ���ַ�����(whar_t),���ǳ���ʵ������ʱwhar_t�Ĵ�С
//�����ǲ�һ����

//���������Ϣ�ṹ��������Ϣ�ṹ��������������ʼ�������Ĺ����ܶ�������ĳ�β��ԵĽ��
typedef     struct __TEST_INFO
{
    UINT16      bError;                     //���Բ����Ƿ�������0Ϊ�޴�������ֵ��ͬ�Ĳ���������в�ͬ�ĺ���
    UINT16      uiErrorLen;                 //������Ϣ��Ч�ַ�wchar_t�ĸ�������������β��0�ַ�
    BYTE        bAppend[16];                //�����ֶΣ��京�������������Զ���
    wchar_t     strErr[200];                //���ڶԲ��ԵĴ�����Ϣ��������
}TEST_INFO;
//�����豸��Ϣ�ṹ
typedef  struct __DEVICE_INFO
{
    wchar_t     strDeviceType[32];          //�豸����
    wchar_t     strDeviceID[64];            //�豸��ʶ
    wchar_t     strDeviceVer[32];           //�豸�汾��
}DEVICE_INFO;

//���崫��ʱ�汾�Ĳ�����Ϣ���豸��Ϣ�ṹ
#pragma  pack(push)
#pragma pack(1)
typedef     struct __TEST_INFO_TRANSFER
{
    UINT16      bError;                     //���Բ����Ƿ�������0Ϊ�޴�������ֵ��ͬ�Ĳ���������в�ͬ�ĺ���
    UINT16      uiErrorLen;                 //������Ϣ��Ч�ַ����ֽ�������������β��0�ַ�
    BYTE        bAppend[16];                //�����ֶΣ��京�������������Զ���
    char        strErr[800];                //���ڶԲ��ԵĴ�����Ϣ��������
}TEST_INFO_TRANSFER;

typedef  struct __DEVICE_INFO_TRANSFER
{
    char        strDeviceType[128];          //�豸����
    char        strDeviceID[256];            //�豸��ʶ
    char        strDeviceVer[32];            //�豸�汾��
}DEVICE_INFO_TRANSFER;
#pragma  pack(pop)

BOOL DeviceInfoLocalToTransfer(DEVICE_INFO_TRANSFER *pDevInfoTrans,const DEVICE_INFO *pDevInfoLocal);
BOOL DeviceInfoTransferToLocal(DEVICE_INFO *pDevInfoLocal,const DEVICE_INFO_TRANSFER *pDevInfoTrans);
BOOL TestInfoLocalToTransfer  (TEST_INFO_TRANSFER *pTestInfoTrans,const TEST_INFO *pTestInfoLocal);
BOOL TestInfoTransferToLocal  (TEST_INFO *pTestInfoLocal,const TEST_INFO_TRANSFER *pTestInfoTrans);


//����PC������ذ����ĳ�����ʱ�ķ��͵�������
#define         MAX_TEST_OBJ            64              //��Ҫʵ�ʽ��в��ԵĲ��������������
                                                        //��������Ӧ��������Ҳ���ᳬ�����ֵ
#define         CMD_TEST_ETHERNET1      0               //��̫��1
#define         CMD_TEST_ETHERNET2      1               //��̫��2
#define         CMD_TEST_ETHERNET3      2               //��̫��3
#define         CMD_TEST_SERIAL1        3               //���ڲ�����1
#define         CMD_TEST_SERIAL2        4               //���ڲ�����2
#define         CMD_TEST_SERIAL3        5               //���ڲ�����3
#define         CMD_TEST_SERIAL4        6               //���ڲ�����4
#define         CMD_TEST_SERIAL5        7               //���ڲ�����5
#define         CMD_TEST_SERIAL6        8               //���ڲ�����6
#define         CMD_TEST_SERIAL7        9               //���ڲ�����7
#define         CMD_TEST_SERIAL8        10              //���ڲ�����8
#define         CMD_TEST_SERIAL9        11              //���ڲ�����9
#define         CMD_TEST_SERIAL10       12              //���ڲ�����10
#define         CMD_TEST_SERIAL11       13              //���ڲ�����11

#define         CMD_TEST_CAN1           14              //CAN������1
#define         CMD_TEST_CAN2           15              //CAN������2
#define         CMD_TEST_CAN3           16              //CAN������3
#define         CMD_TEST_CAN4           17              //CAN������4
#define         CMD_TEST_CAN5           18              //CAN������5
#define         CMD_TEST_CAN6           19              //CAN������6

#define         CMD_TEST_DISK1          20              //���̲�����1
#define         CMD_TEST_DISK2          21              //���̲�����2
#define         CMD_TEST_DISK3          22              //���̲�����3
#define         CMD_TEST_DISK4          23              //���̲�����4
#define         CMD_TEST_DISK5          24              //���̲�����5
#define         CMD_TEST_DISK6          25              //���̲�����6
#define         CMD_TEST_DISK7          26              //���̲�����7
#define         CMD_TEST_DISK8          27              //���̲�����8
#define         CMD_TEST_DISK9          28              //���̲�����9
#define         CMD_TEST_DISK10         29              //���̲�����10
#define         CMD_TEST_DISK11         30              //���̲�����11

#define         CMD_TEST_ISA            31              //ISA������
#define         CMD_TEST_ACTIVE         32              //ActiveSync������
#define         CMD_TEST_GPIO           33              //GPIO������
#define         CMD_TEST_LED            34              //LED�Ʋ�����
#define         CMD_TEST_BEEP           35              //������������
#define         CMD_TEST_SCREEN         36              //��Ļ������
#define         CMD_TEST_RTC            37              //RTC������
#define         CMD_TEST_AUDIO          38              //��Ƶ����
#define         CMD_TEST_VEDIO          39              //��Ƶ������ͷ������
#define         CMD_TEST_AD             40              //AD������
#define         CMD_TEST_GPRS           41              //GPRS������
#define         CMD_TEST_ZIGBEE         42              //ZigBee������
#define         CMD_TEST_KEYBORAD       43              //���̲�����
#define         CMD_TEST_KBD_IO         44              //��չ����IO������
#define         CMD_TEST_7290           45              //7290������

#define         CMD_TEST_USE_FT3_RX1    46              //FT3_RX1���ղ���
#define         CMD_TEST_USE_FT3_RX2    47              //FT3_RX2���ղ���
#define         CMD_TEST_USE_FT3_TX1    48              //FT3_TX���Ͳ���
#define         CMD_TEST_USE_MU1        49              //����̫��1�շ�����
#define         CMD_TEST_USE_MU2        50              //����̫��2�շ�����
#define         CMD_TEST_USE_MU3        51              //����̫���շ�
#define         CMD_TEST_USE_B_CODE     52              //IRIG_BУʱ
#define         CMD_TEST_USE_APP        53              //APP����

#define         CMD_TEST_WATCH_DOG      60              //���Ź�������
#define         CMD_TEST_RFID           55              //RFIDģ�������
#define         CMD_TEST_GPS            56              //GPSģ�������
#define         CMD_TEST_RELAY          57              //�̵�������
#define         CMD_TEST_SPI_FLASH      58              //SPI Flash����


#define         CMD_DEVICE_INFO         0x8000          //��ȡ�豸��Ϣ��������
#define         CMD_TEST_CONNECT        0x8001          //���ڲ������ӵ�������
#define         CMD_TEST_CONNECT_ACK    0x8002          //���ڲ������ӵ�������


#define  FRAME_TEST_CMD_INI   1
#define  FRAME_TEST_CMD_EXT   2
#define  FRAME_TEST_CMD_END   3

//����ʹ�õ����ӷ�ʽ
typedef enum __tagConnectWay
{
    CONNECT_WAY_ETNERNET=0,                             //ͨ����̫������
    CONNECT_WAY_ACTIVE_SYNC,                            //ͨ��ActiveSync����
}CONNECT_WAY;


//�������в��������ݰ��еĲ����Ĺ�������
#define         PARAM_GET_INIT_INFO     0x8000          //��ȡ���ذ��ϸò�����ĳ�ʼ����Ϣ

//���幦�ܲ��Է�������(�豸��)ͨ��������ʹ�õĶ˿�
#define         SERVER_PORT             5031
#define         ETH_TEST_SERVER_PORT    5032            //��̫�������õ���tcp�������˿�
#define         ETH_TEST_PACKET_LEN     4096            //��̫������ÿһ�����ݰ��Ĵ�С

#define         MAX_ETHERNET_NUM         3               //���������̫����Ŀ    
#ifdef        __cplusplus
}
#endif

#endif                                                //For the definition of __TEST_COMMON_H__
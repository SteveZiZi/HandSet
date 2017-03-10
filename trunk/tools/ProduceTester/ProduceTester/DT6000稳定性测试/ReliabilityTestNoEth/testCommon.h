#ifndef __TEST_COMMON_H__
#define __TEST_COMMON_H__

//�������ڲ������ݰ��������룬Ȼ��ʵ�����ڲ��Ե������벻�ᳬ��MAX_TEST_OBJ
#define CMD_DEVICE_INFO         0x1000      //��ȡ�豸��Ϣ
#define CMD_TEST_CONNECT        0x1001      //���ڲ������ӵ���ͨ��

//��̫�����Ե������룬��̫�����ֵ�������Ӧ����������
#define CMD_TEST_ETHERNET1      0           //������̫��1
#define CMD_TEST_ETHERNET2      1           //������̫��2

#define CMD_TEST_DISK1          2           //���Դ���1�����̰���FlashDisk��SD����TF����
#define CMD_TEST_DISK2          3           //���Դ���2
#define CMD_TEST_DISK3          4           //���̲���3
#define CMD_TEST_DISK4          5           //���̲���4

#define CMD_TEST_SERIAL1        6           //���������1
#define CMD_TEST_SERIAL2        7           //���������2
#define CMD_TEST_SERIAL3        8           //���������3
#define CMD_TEST_SERIAL4        9           //���������4
#define CMD_TEST_SERIAL5        10          //���������5

#define CMD_TEST_CAN1           11          //CAN�����1
#define CMD_TEST_CAN2           12          //CAN�����2
#define CMD_TEST_CAN3           13          //CAN�����3

#define CMD_TEST_GPIO           14          //GPIO����
#define CMD_TEST_LED            15          //LED����
#define CMD_TEST_BEEP           16          //����������
#define CMD_TEST_ISA            17          //ISA����
#define CMD_TEST_AD             18          //AD����
#define CMD_TEST_RTC            19          //RTC����
#define CMD_TEST_WATCHDOG       20          //���Ź�����


//�������ݰ����ò����ĺ��壬���Բ�����Ч��0x1000����Ϊ���в��Թ��ò���
#define PARAM_GET_INIT_INFO     0x1000      //��ȡ��ʼ����Ϣ



//������̫����ʹ�õĶ˿�
#define PC_SERVER_PORT          2049        //PC�������ʹ�õĶ˿�
//��̫�����Եķ������˿�Ӧ����������
#define ETHERNET_PORT           2050        //��̫��1���Եķ������˿�


//��̫������ÿ���Ĵ�С
#define  ETH_TEST_PACKET_LEN    10240
#define  DISK_TEST_LEN          1024        //���̲���ÿ��д����ֽ���

//����TEST_INFO�ṹ��bError�ֶ�ר�õı�ʶ��
#define TEST_ERROR_NO_ERROR     0
#define TEST_ERROR_NORMAL       1
#define TEST_ERROR_NEED_REINIT  2
#define TEST_ERROR_NO_TEST_ITEM 3            //�ͻ��˲�û�����������

//���������Ϣ�Ľṹ�壬����ṹ�������ڴ�����򵥵Ĳ���
//�������bErrorΪ0�����ʾû�д�������Ϊ1�Ļ�strError�����˲��Դ�����Ϣ
//Ϊ2�Ļ�����ʾ�豸��Ҫ���³�ʼ����������ִ�л�ȡ��ʼ����Ϣʱ����������ֵ
//����
typedef struct _TEST_INFO 
{
    UINT32  bError;                          //�Ƿ�������
    UINT32  iErrLen;                         //������Ϣ�ĳ��ȣ�������β��0
    WCHAR   strError[121];                   //�������ɴ��120���ַ��Ĵ�����Ϣ
}TEST_INFO;

//����һ���豸���֧�ֵĲ�����Ŀ
#define  MAX_TEST_OBJ  64


//�����������ϵĲ����豸����Ϣ
struct DEVICE_INFO
{
    WCHAR strDeviceID[32];                  //�豸�ı�ʶ����һ�θߵ��²����У���Ӧ����Ψһ��
    WCHAR strDeviceType[32];                //�豸�����ͣ�����EPC-8000

    int   iEthNum;                          //�����豸��������Ŀ
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


//����ͨ�õ�connect���ճ�ʱ��msΪ��λ
#define GLOBAL_RECV_TIME_OUT 60000


#endif
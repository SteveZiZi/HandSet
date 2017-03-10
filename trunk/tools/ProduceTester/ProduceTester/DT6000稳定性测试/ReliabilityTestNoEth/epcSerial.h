/****************************************Copyright (c)****************************************************
**                            GuangZhou XuanTong Electric Technology Co.,LTD.
**                                      
**                                 http://www.xtoee.com.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               epcSerial.h
** Latest modified Date:    2008-02-28
** Latest Version:          V1.0
** Descriptions:            epcSerial�����ͷ�ļ�(Ӧ�ó���ר��).
**
**--------------------------------------------------------------------------------------------------------
** Created by:              chao
** Created date:            2008-02-28
** Version:                 V1.0
** Descriptions:            ԭʼ�汾
**
**--------------------------------------------------------------------------------------------------------
** Modified by:                  
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef __EPCSERIAL_H
#define __EPCSERIAL_H


/*********************************************************************************************************
  ���ڲ������󷵻�ֵ����
*********************************************************************************************************/
#define COM_OK              0
#define COM_ERR_PARA        41
#define COM_ERR_TIME        42
#define COM_ERR_USING       43
#define COM_ERR_OPEN        44



/*********************************************************************************************************
  ���ںŲ����궨��
*********************************************************************************************************/
#define COM1                0
#define COM2                1
#define COM3                2
#define COM4                3
#define COM5                4
#define COM6                5
#define COM7                6
#define COM8                7
#define COM9                8


/*********************************************************************************************************
  �����ʲ����궨��
*********************************************************************************************************/
#define BAUD110             0 		                                    /*  110 bits/s                  */
#define BAUD300             1	                                        /*  300 bits/s                  */
#define BAUD600             2	                                        /*  600 bits/s                  */
#define BAUD1200            3	                                        /*  1200 bits/s                 */
#define	BAUD2400            4	                                        /*  2400 bits/s                 */
#define	BAUD4800            5	                                        /*  4800 bits/s                 */
#define	BAUD9600            6	                                        /*  9600 bits/s                 */
#define	BAUD14400           7	                                        /*  14400 bits/s                */
#define	BAUD19200           8	                                        /*  19200 bits/sec              */
#define	BAUD38400           9	                                        /*  38400 bits/sec              */
#define	BAUD56000           10	                                        /*  56000 bits/sec              */
#define	BAUD57600           11	                                        /*  57600 bits/sec              */
#define	BAUD115200          12	                                        /*  115200 bits/sec	            */



/*********************************************************************************************************
  ����λ�����궨��
*********************************************************************************************************/
#define	DATA_BIT5           0		                                    /*  5 bits per byte             */
#define	DATA_BIT6           1		                                    /*  6 bits per byte             */
#define	DATA_BIT7           2		                                    /*  7 bits per byte             */
#define	DATA_BIT8           3		                                    /*  8 bits per byte (default)   */



/*********************************************************************************************************
  ֹͣλ�����궨��
*********************************************************************************************************/
#define	STOP_BIT1           0		                                    /*  1 stopbit (default)         */
#define	STOP_BIT1_5         1		                                    /*  1.5 stopbit                 */
#define	STOP_BIT2           2		                                    /*  2 stopbits                  */



/*********************************************************************************************************
  ��ż����λ�����궨��
*********************************************************************************************************/
#define	PAR_NONE            0    	                                    /*  ��У�� (default)	        */
#define	PAR_ODD             1   		                                /*  ��У��                      */
#define	PAR_EVEN            2   	                                    /*  żУ��                      */
#define	PAR_MARK            3   	                                    /*  ���У��                    */
#define	PAR_SPACE           4		                                    /*  �ո�У��                    */


/*********************************************************************************************************
DTRӲ����������ֵ����
*********************************************************************************************************/
#define DTR_DISABLE         0    	                                    /*  DTR��ΪOFF (default)	    */
#define	DTR_ENABLE          1 	                                        /*  DTR��ΪON                   */
#define	DTR_HANDSHAKE       2    	                                    /*  ����DTR�����֡�             */


/*********************************************************************************************************
RTSӲ����������ֵ����
*********************************************************************************************************/
#define	RTS_DISABLE         0    	                                    /*  RTS��ΪOFF (default)        */
#define	RTS_ENABLE	        1 		                                    /*  RTS��ΪON                   */
#define	RTS_HANDSHAKE       2  	                            /*  �����ջ���������С�ڰ���ʱRTSΪON,      */
                                                            /*  �����ջ��������ݳ����ķ�֮����ʱRTSΪOFF*/
#define	RTS_TOGGLE          3		                                    /*  �����ݷ���ʱRTS��ΪON,      */
                                                                        /*  ���ݷ������RTS�ָ�ΪOFF    */




/*********************************************************************************************************
   �������ڽ��ջص�����ָ�� ��������
*********************************************************************************************************/
typedef void  (CALLBACK *PFUN_COMRCV)(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen);




/*********************************************************************************************************
** Function name:           epcSerialOpen
** Descriptions:            �򿪴��ڡ����ڹ���ģʽΪ8λ����λ��1λֹͣλ������żУ��λ��                        
** input parameters:        ucPort      ���ں�,��COM1,COM2
**                          ucBaudRate  ������,��BAUD9600,BAUD115200
** output parameters:       NONE
** 
** Returned value:          ��ȷ����COM_OK, �������ò������󷵻�COM_ERR_PARA, ��ʱ���ô���
**                          ����COM_ERR_TIME, �����Ѵ򿪷���COM_ERR_USING, ���ڴ򿪴���COM_ERR_OPEN.
*********************************************************************************************************/
extern "C"  int epcSerialOpen(UCHAR  ucPort, UCHAR  ucBaudRate);



/*********************************************************************************************************
** Function name:           epcSerialOpen1
** Descriptions:            �򿪴��ڡ�Ӳ�������ƽ�ֹ��                         
** input parameters:        ucPort      ���ں�,��COM1,COM2
**                          ucBaudRate  ������,��BAUD9600,BAUD115200
**                          ucDataBit   ����λ,��DATA_BIT8
**                          ucStopBit   ֹͣλ,��STOP_BIT1
**                          ucParity    ��żУ��λ,��PAR_NONE, PAR_ODD
** output parameters:       NONE
** 
** Returned value:          ��ȷ����COM_OK, �������ò������󷵻�COM_ERR_PARA, ��ʱ���ô���
**                          ����COM_ERR_TIME, �����Ѵ򿪷���COM_ERR_USING, ���ڴ򿪴���COM_ERR_OPEN.
*********************************************************************************************************/
extern "C"    int epcSerialOpen1(UCHAR  ucPort,    UCHAR  ucBaudRate,
                                                     UCHAR  ucDataBit, UCHAR  ucStopBit, UCHAR  ucParity);



/*********************************************************************************************************
** Function name:           epcSerialOpen2
** Descriptions:            �򿪴��ڡ�                       
** input parameters:        ucPort      ���ں�,��COM1,COM2
**                          ucBaudRate  ������,��BAUD9600,BAUD115200
**                          ucDataBit   ����λ,��DATA_BIT8
**                          ucStopBit   ֹͣλ,��STOP_BIT1
**                          ucParity    ��żУ��λ,��PAR_NONE, PAR_ODD
**                          ucDTRShake  DTR���ƣ���DTR_DISABLE
**                          ucRTSShake  RTS���ƣ���RTS_DISABLE
** output parameters:       NONE
** 
** Returned value:          ��ȷ����COM_OK, �������ò������󷵻�COM_ERR_PARA, ��ʱ���ô���
**                          ����COM_ERR_TIME, �����Ѵ򿪷���COM_ERR_USING, ���ڴ򿪴���COM_ERR_OPEN.
*********************************************************************************************************/
extern "C"    int epcSerialOpen2(UCHAR  ucPort,    UCHAR  ucBaudRate,
                                                     UCHAR  ucDataBit, UCHAR  ucStopBit, UCHAR  ucParity,
                                                     UCHAR  ucDTRShake, UCHAR  ucRTSShake);



/*********************************************************************************************************
** Function name:           epcSerialClose
** Descriptions:            �رմ��ڡ�                        
** input parameters:        ucPort      ���ں�,��COM1,COM2
** output parameters:       NONE
** 
** Returned value:          �����ɹ�����TRUE, ����ʧ�ܷ���FALSE.
*********************************************************************************************************/
extern "C"    BOOL epcSerialClose(UCHAR  ucPort);



/*********************************************************************************************************
** Function name:           epcSerialSendData
** Descriptions:            ���ʹ�������                        
** input parameters:        ucPort      ���ں�,��COM1,COM2
**                          pucSendBuf  �������ݻ�����ָ��
**                          dwLength    �������ݸ���
** output parameters:       NONE
** 
** Returned value:          �����ɹ�����TRUE, ����ʧ�ܷ���FALSE.
**                          (����ʧ��ԭ��: ��������򴮿�û�д�)
*********************************************************************************************************/
extern "C"    BOOL epcSerialSendData(UCHAR  ucPort, BYTE  *pucSendBuf, DWORD  dwLength);



/*********************************************************************************************************
** Function name:           epcSerialRcvData
** Descriptions:            ���մ������ݡ�������ʽ��                        
** input parameters:        ucPort      ���ں�,��COM1,COM2
**                          dwLength    Ҫ�������ݸ��� (���Ϊ1024)
**                          dwOutTime   ���ճ�ʱʱ�� (Ҫ����10)
**                          bClrComBuf  ΪTRUEʱ,�ڽ���ǰ��������ڽ��ջ�����, ΪFLASEʱ�����
** output parameters:       pucRcvBuf   �������ݻ�����ָ��
** 
** Returned value:          ���������ݸ��� (�������󡢴���û�д򿪻�û�н��յ����ݣ�����0).
*********************************************************************************************************/
extern "C"    DWORD epcSerialRcvData(UCHAR  ucPort, BYTE  *pucRcvBuf, DWORD  dwLength, 
                                                         DWORD  dwOutTime, BOOL  bClrComBuf);



/*********************************************************************************************************
** Function name:           epcSerialRcvDataTread
** Descriptions:            ���մ������ݡ��̷߳�ʽ��                         
** input parameters:        ucPort          ���ں�,��COM1,COM2
**                          dwLength        �������ݸ��������Ϊ1024
**                          dwOutTime       ���ճ�ʱʱ����� (��λΪmS����СΪ10)
**                          pfunOnComRcv    �������ݳɹ��ص�����ָ��
**                          pvUserParam     �ص�������Ҫ���û���������
** output parameters:       NONE
** 
** Returned value:          �����ɹ�����TRUE, ����ʧ�ܷ���FALSE.
**                          (����ʧ��ԭ��: �������󡢴���û�д򿪻򴮿ڽ����߳��ѽ�����)
*********************************************************************************************************/
extern "C"    BOOL epcSerialRcvDataTread(UCHAR  ucPort, DWORD  dwLength, DWORD  dwOutTime,
				                                             PFUN_COMRCV  pfunOnComRcv, LPVOID  pvUserParam);




/*********************************************************************************************************
** Function name:           epcSerialRxClear
**
** Descriptions:            ������ڵĽ��ջ�������
**                          
** input parameters:        ucPort          ���ں�,��COM1,COM2
** output parameters:       NONE
** 
** Returned value:          �����ɹ�����TRUE, ����ʧ�ܷ���FALSE.
**                          (����ʧ��ԭ��: ����û�д�)
*********************************************************************************************************/
extern "C"    BOOL epcSerialRxClear(UCHAR  ucPort);





/*********************************************************************************************************
** Function name:           epcSerialGetVersion
** Descriptions:            ��ȡepcSerial������汾��                         
** input parameters:        NONE
** output parameters:       NONE
** 
** Returned value:          ���ص�ǰ������汾��.
**                          (����0x100����ʾV1.00������0x110����ʾV1.10......)
*********************************************************************************************************/
extern "C"    DWORD epcSerialGetVersion(void);

#ifdef _DEBUG
#pragma comment(lib,"epcSerial_d.lib")
#else
#pragma comment(lib,"epcSerial.lib")
#endif


#endif                                                                  /*  __EPCSERIAL_H               */

/*********************************************************************************************************
END FILE
*********************************************************************************************************/
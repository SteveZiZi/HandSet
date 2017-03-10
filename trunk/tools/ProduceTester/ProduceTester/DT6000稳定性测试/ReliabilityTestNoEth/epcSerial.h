/****************************************Copyright (c)****************************************************
**                            GuangZhou XuanTong Electric Technology Co.,LTD.
**                                      
**                                 http://www.xtoee.com.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               epcSerial.h
** Latest modified Date:    2008-02-28
** Latest Version:          V1.0
** Descriptions:            epcSerial软件包头文件(应用程序专用).
**
**--------------------------------------------------------------------------------------------------------
** Created by:              chao
** Created date:            2008-02-28
** Version:                 V1.0
** Descriptions:            原始版本
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
  串口操作错误返回值定义
*********************************************************************************************************/
#define COM_OK              0
#define COM_ERR_PARA        41
#define COM_ERR_TIME        42
#define COM_ERR_USING       43
#define COM_ERR_OPEN        44



/*********************************************************************************************************
  串口号参数宏定义
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
  波特率参数宏定义
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
  数据位参数宏定义
*********************************************************************************************************/
#define	DATA_BIT5           0		                                    /*  5 bits per byte             */
#define	DATA_BIT6           1		                                    /*  6 bits per byte             */
#define	DATA_BIT7           2		                                    /*  7 bits per byte             */
#define	DATA_BIT8           3		                                    /*  8 bits per byte (default)   */



/*********************************************************************************************************
  停止位参数宏定义
*********************************************************************************************************/
#define	STOP_BIT1           0		                                    /*  1 stopbit (default)         */
#define	STOP_BIT1_5         1		                                    /*  1.5 stopbit                 */
#define	STOP_BIT2           2		                                    /*  2 stopbits                  */



/*********************************************************************************************************
  奇偶较验位参数宏定义
*********************************************************************************************************/
#define	PAR_NONE            0    	                                    /*  无校验 (default)	        */
#define	PAR_ODD             1   		                                /*  奇校验                      */
#define	PAR_EVEN            2   	                                    /*  偶校验                      */
#define	PAR_MARK            3   	                                    /*  标号校验                    */
#define	PAR_SPACE           4		                                    /*  空格校验                    */


/*********************************************************************************************************
DTR硬件流控设置值定义
*********************************************************************************************************/
#define DTR_DISABLE         0    	                                    /*  DTR置为OFF (default)	    */
#define	DTR_ENABLE          1 	                                        /*  DTR置为ON                   */
#define	DTR_HANDSHAKE       2    	                                    /*  允许DTR“握手”             */


/*********************************************************************************************************
RTS硬件流控设置值定义
*********************************************************************************************************/
#define	RTS_DISABLE         0    	                                    /*  RTS置为OFF (default)        */
#define	RTS_ENABLE	        1 		                                    /*  RTS置为ON                   */
#define	RTS_HANDSHAKE       2  	                            /*  当接收缓冲区数据小于半满时RTS为ON,      */
                                                            /*  当接收缓冲区数据超过四分之三满时RTS为OFF*/
#define	RTS_TOGGLE          3		                                    /*  有数据发送时RTS置为ON,      */
                                                                        /*  数据发送完后RTS恢复为OFF    */




/*********************************************************************************************************
   声明串口接收回调函数指针 数据类型
*********************************************************************************************************/
typedef void  (CALLBACK *PFUN_COMRCV)(LPVOID pvUserParam, BYTE *pucBuf, DWORD dwRcvLen);




/*********************************************************************************************************
** Function name:           epcSerialOpen
** Descriptions:            打开串口。串口工作模式为8位数据位，1位停止位，无奇偶校验位。                        
** input parameters:        ucPort      串口号,如COM1,COM2
**                          ucBaudRate  波特率,如BAUD9600,BAUD115200
** output parameters:       NONE
** 
** Returned value:          正确返回COM_OK, 串口设置参数错误返回COM_ERR_PARA, 超时设置错误
**                          返回COM_ERR_TIME, 串口已打开返回COM_ERR_USING, 串口打开错误COM_ERR_OPEN.
*********************************************************************************************************/
extern "C"  int epcSerialOpen(UCHAR  ucPort, UCHAR  ucBaudRate);



/*********************************************************************************************************
** Function name:           epcSerialOpen1
** Descriptions:            打开串口。硬件流控制禁止。                         
** input parameters:        ucPort      串口号,如COM1,COM2
**                          ucBaudRate  波特率,如BAUD9600,BAUD115200
**                          ucDataBit   数据位,如DATA_BIT8
**                          ucStopBit   停止位,如STOP_BIT1
**                          ucParity    奇偶校验位,如PAR_NONE, PAR_ODD
** output parameters:       NONE
** 
** Returned value:          正确返回COM_OK, 串口设置参数错误返回COM_ERR_PARA, 超时设置错误
**                          返回COM_ERR_TIME, 串口已打开返回COM_ERR_USING, 串口打开错误COM_ERR_OPEN.
*********************************************************************************************************/
extern "C"    int epcSerialOpen1(UCHAR  ucPort,    UCHAR  ucBaudRate,
                                                     UCHAR  ucDataBit, UCHAR  ucStopBit, UCHAR  ucParity);



/*********************************************************************************************************
** Function name:           epcSerialOpen2
** Descriptions:            打开串口。                       
** input parameters:        ucPort      串口号,如COM1,COM2
**                          ucBaudRate  波特率,如BAUD9600,BAUD115200
**                          ucDataBit   数据位,如DATA_BIT8
**                          ucStopBit   停止位,如STOP_BIT1
**                          ucParity    奇偶校验位,如PAR_NONE, PAR_ODD
**                          ucDTRShake  DTR控制，如DTR_DISABLE
**                          ucRTSShake  RTS控制，如RTS_DISABLE
** output parameters:       NONE
** 
** Returned value:          正确返回COM_OK, 串口设置参数错误返回COM_ERR_PARA, 超时设置错误
**                          返回COM_ERR_TIME, 串口已打开返回COM_ERR_USING, 串口打开错误COM_ERR_OPEN.
*********************************************************************************************************/
extern "C"    int epcSerialOpen2(UCHAR  ucPort,    UCHAR  ucBaudRate,
                                                     UCHAR  ucDataBit, UCHAR  ucStopBit, UCHAR  ucParity,
                                                     UCHAR  ucDTRShake, UCHAR  ucRTSShake);



/*********************************************************************************************************
** Function name:           epcSerialClose
** Descriptions:            关闭串口。                        
** input parameters:        ucPort      串口号,如COM1,COM2
** output parameters:       NONE
** 
** Returned value:          操作成功返回TRUE, 操作失败返回FALSE.
*********************************************************************************************************/
extern "C"    BOOL epcSerialClose(UCHAR  ucPort);



/*********************************************************************************************************
** Function name:           epcSerialSendData
** Descriptions:            发送串口数据                        
** input parameters:        ucPort      串口号,如COM1,COM2
**                          pucSendBuf  发送数据缓冲区指针
**                          dwLength    发送数据个数
** output parameters:       NONE
** 
** Returned value:          操作成功返回TRUE, 操作失败返回FALSE.
**                          (操作失败原因: 参数错误或串口没有打开)
*********************************************************************************************************/
extern "C"    BOOL epcSerialSendData(UCHAR  ucPort, BYTE  *pucSendBuf, DWORD  dwLength);



/*********************************************************************************************************
** Function name:           epcSerialRcvData
** Descriptions:            接收串口数据。阻塞方式。                        
** input parameters:        ucPort      串口号,如COM1,COM2
**                          dwLength    要接收数据个数 (最大为1024)
**                          dwOutTime   接收超时时间 (要大于10)
**                          bClrComBuf  为TRUE时,在接收前先清除串口接收缓冲区, 为FLASE时不清除
** output parameters:       pucRcvBuf   接收数据缓冲区指针
** 
** Returned value:          读出的数据个数 (参数错误、串口没有打开或没有接收到数据，返回0).
*********************************************************************************************************/
extern "C"    DWORD epcSerialRcvData(UCHAR  ucPort, BYTE  *pucRcvBuf, DWORD  dwLength, 
                                                         DWORD  dwOutTime, BOOL  bClrComBuf);



/*********************************************************************************************************
** Function name:           epcSerialRcvDataTread
** Descriptions:            接收串口数据。线程方式。                         
** input parameters:        ucPort          串口号,如COM1,COM2
**                          dwLength        接收数据个数，最大为1024
**                          dwOutTime       接收超时时间控制 (单位为mS，最小为10)
**                          pfunOnComRcv    接收数据成功回调函数指针
**                          pvUserParam     回调函数需要的用户参数变量
** output parameters:       NONE
** 
** Returned value:          操作成功返回TRUE, 操作失败返回FALSE.
**                          (操作失败原因: 参数错误、串口没有打开或串口接收线程已建立过)
*********************************************************************************************************/
extern "C"    BOOL epcSerialRcvDataTread(UCHAR  ucPort, DWORD  dwLength, DWORD  dwOutTime,
				                                             PFUN_COMRCV  pfunOnComRcv, LPVOID  pvUserParam);




/*********************************************************************************************************
** Function name:           epcSerialRxClear
**
** Descriptions:            清除串口的接收缓冲区。
**                          
** input parameters:        ucPort          串口号,如COM1,COM2
** output parameters:       NONE
** 
** Returned value:          操作成功返回TRUE, 操作失败返回FALSE.
**                          (操作失败原因: 串口没有打开)
*********************************************************************************************************/
extern "C"    BOOL epcSerialRxClear(UCHAR  ucPort);





/*********************************************************************************************************
** Function name:           epcSerialGetVersion
** Descriptions:            获取epcSerial软件包版本号                         
** input parameters:        NONE
** output parameters:       NONE
** 
** Returned value:          返回当前软件包版本号.
**                          (返回0x100，表示V1.00；返回0x110，表示V1.10......)
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
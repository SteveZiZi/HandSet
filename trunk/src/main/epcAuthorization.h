/****************************************Copyright (c)**************************************************
**						GuangZhou XuanTong Electric Technology Co.,LTD.
**                                     
**							http://www.xtoee.com.cn
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:				epcAuthorization.h
** Last modified Date: 		2007.12.16
** Last Version:			V1.0
** Description: 			the head file of Authorization library(授权认证库头文件)
**                          	 
**------------------------------------------------------------------------------------------------------
** Created By: 				chao
** Created date: 			2008.03.20
** Version: 				V1.0
** Descriptions:			The original version 初始版本
**
**
********************************************************************************************************/
#ifndef __AUTORIZATION_H
#define __AUTORIZATION_H
	
#define __BLOCK_DATA_SIZE			64									/*	保留数据块的大小			*/

/********************************************************************************************************
** Function name:           epcAuthorizeInit()
** Descriptions:            初始化授权加密
** Input parameters:        无
** Output parameters:		无
** Returned value:			TRUE: 成功	FALSE: 失败
*********************************************************************************************************/
BOOL epcAuthorizeInit(void);

/********************************************************************************************************
** Function name:           epcAuthorizeDeinit()
** Descriptions:            释放授权加密申请的资源
** Input parameters:        无
** Output parameters:		无
** Returned value:			无
*********************************************************************************************************/
void epcAuthorizeDeinit(void);

/********************************************************************************************************
** Function name:           epcReadBlockData()
** Descriptions:            读取保留数据块中的数据
** Input parameters:        dwToReadNum:	要读取的字节数
** Output parameters:		pBuf:			读数据缓冲区指针
** Returned value:			实际读取的字节数
*********************************************************************************************************/
extern DWORD epcReadBlockData(void *pBuf, DWORD dwToReadNum);

/********************************************************************************************************
** Function name:           epcWriteBlockData()
** Descriptions:            向保留数据块中的写入数据
** Input parameters:        pBuf:			写数据缓冲区指针
							dwWriteNum:		要写入的字节数
** Output parameters:		无
** Returned value:			实际写入的字节数
*********************************************************************************************************/
extern DWORD epcWriteBlockData(void *pBuf, DWORD dwWriteNum);

/********************************************************************************************************
** Function name:           epcGetSystemIdentityCode()
** Descriptions:            读取平台身份标识码
** Input parameters:        pAuthorData:	存放身份标识码的指针
** Output parameters:		无
** Returned value:			TRUE: 成功		FALSE: 失败
*********************************************************************************************************/
extern BOOL epcGetSystemIdentityCode(PULONGLONG pAuthorData);

/********************************************************************************************************
** Function name:           epcAuthorizeEncode()
** Descriptions:            授权加密函数
** Input parameters:        lpszPassword:	要加密的密码
							pOutDataLen:	表示缓冲区大小的变量的指针
** Output parameters:		pEncodeOutData:	存放加密结果缓冲区的指针
							*pOutDataLen:	加密结果的长度
** Returned value:			TRUE: 成功		FALSE: 失败
*********************************************************************************************************/
extern BOOL epcAuthorizeEncode(LPCSTR lpszPassword, void *pEncodeOutData, PDWORD pOutDataLen);

#endif
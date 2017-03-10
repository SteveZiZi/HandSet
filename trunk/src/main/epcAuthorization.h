/****************************************Copyright (c)**************************************************
**						GuangZhou XuanTong Electric Technology Co.,LTD.
**                                     
**							http://www.xtoee.com.cn
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:				epcAuthorization.h
** Last modified Date: 		2007.12.16
** Last Version:			V1.0
** Description: 			the head file of Authorization library(��Ȩ��֤��ͷ�ļ�)
**                          	 
**------------------------------------------------------------------------------------------------------
** Created By: 				chao
** Created date: 			2008.03.20
** Version: 				V1.0
** Descriptions:			The original version ��ʼ�汾
**
**
********************************************************************************************************/
#ifndef __AUTORIZATION_H
#define __AUTORIZATION_H
	
#define __BLOCK_DATA_SIZE			64									/*	�������ݿ�Ĵ�С			*/

/********************************************************************************************************
** Function name:           epcAuthorizeInit()
** Descriptions:            ��ʼ����Ȩ����
** Input parameters:        ��
** Output parameters:		��
** Returned value:			TRUE: �ɹ�	FALSE: ʧ��
*********************************************************************************************************/
BOOL epcAuthorizeInit(void);

/********************************************************************************************************
** Function name:           epcAuthorizeDeinit()
** Descriptions:            �ͷ���Ȩ�����������Դ
** Input parameters:        ��
** Output parameters:		��
** Returned value:			��
*********************************************************************************************************/
void epcAuthorizeDeinit(void);

/********************************************************************************************************
** Function name:           epcReadBlockData()
** Descriptions:            ��ȡ�������ݿ��е�����
** Input parameters:        dwToReadNum:	Ҫ��ȡ���ֽ���
** Output parameters:		pBuf:			�����ݻ�����ָ��
** Returned value:			ʵ�ʶ�ȡ���ֽ���
*********************************************************************************************************/
extern DWORD epcReadBlockData(void *pBuf, DWORD dwToReadNum);

/********************************************************************************************************
** Function name:           epcWriteBlockData()
** Descriptions:            �������ݿ��е�д������
** Input parameters:        pBuf:			д���ݻ�����ָ��
							dwWriteNum:		Ҫд����ֽ���
** Output parameters:		��
** Returned value:			ʵ��д����ֽ���
*********************************************************************************************************/
extern DWORD epcWriteBlockData(void *pBuf, DWORD dwWriteNum);

/********************************************************************************************************
** Function name:           epcGetSystemIdentityCode()
** Descriptions:            ��ȡƽ̨��ݱ�ʶ��
** Input parameters:        pAuthorData:	�����ݱ�ʶ���ָ��
** Output parameters:		��
** Returned value:			TRUE: �ɹ�		FALSE: ʧ��
*********************************************************************************************************/
extern BOOL epcGetSystemIdentityCode(PULONGLONG pAuthorData);

/********************************************************************************************************
** Function name:           epcAuthorizeEncode()
** Descriptions:            ��Ȩ���ܺ���
** Input parameters:        lpszPassword:	Ҫ���ܵ�����
							pOutDataLen:	��ʾ��������С�ı�����ָ��
** Output parameters:		pEncodeOutData:	��ż��ܽ����������ָ��
							*pOutDataLen:	���ܽ���ĳ���
** Returned value:			TRUE: �ɹ�		FALSE: ʧ��
*********************************************************************************************************/
extern BOOL epcAuthorizeEncode(LPCSTR lpszPassword, void *pEncodeOutData, PDWORD pOutDataLen);

#endif
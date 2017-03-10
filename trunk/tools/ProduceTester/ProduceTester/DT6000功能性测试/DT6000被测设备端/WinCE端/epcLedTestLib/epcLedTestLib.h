/****************************************Copyright (c)****************************************************
**                            GuangZhou XuanTong Electric Technology Co.,LTD.
**                                      
**                                 http://www.xtoee.com.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               epcLedTestLib.h
** Latest modified Date:    2012-04-01
** Latest Version:          1.0
** Descriptions:            Ϊ������Զ���װ��LED��̬��
**
**--------------------------------------------------------------------------------------------------------
** Created by:              chao
** Created date:            2012-04-01
** Version:                 1.0
** Descriptions:            The original version
**
*********************************************************************************************************/


#ifndef         __EPC_LED_TEST_LIB_H__
#define         __EPC_LED_TEST_LIB_H__

#ifdef  __cplusplus
extern "C" {
#endif
;
/*********************************************************************************************************
** Function name:           epcSetLedMask
** Descriptions:            ����dwMaskָ�����������رջ��ߵ���LED��
** input parameters:        dwMask        LED״̬����
**                          ���dwMask���ĳһλΪ1�����Ӧ��LED������
                            dwMask��һλ��������LED�Ƶı�Ų�û���Ȼ��ϵ����ʵ���������ʱ�ɸ��ݾ������
                            ����ָ����Ȼ���Ƽ���LED�ƴ�0��ʼ������ţ�����dwMask�ĵڼ�λ�ʹ���ڼ���LED��
** output parameters:       ��
** Returned value:          ��
** Note:
*********************************************************************************************************/
void epcSetLedMask(DWORD    dwMask);

/*********************************************************************************************************
** Function name:           epcGetLedMask
** Descriptions:            ��ȡLED�Ƶ�״̬
** output parameters:       ��
** Returned value:          ��
** Note:
*********************************************************************************************************/
void epcGetLedMask(DWORD    *dwMask);



#ifdef  __cplusplus
};
#endif


#endif
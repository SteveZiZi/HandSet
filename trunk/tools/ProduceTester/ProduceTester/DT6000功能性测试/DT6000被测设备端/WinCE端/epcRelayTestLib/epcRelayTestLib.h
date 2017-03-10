/****************************************Copyright (c)****************************************************
**                            GuangZhou XuanTong Electric Technology Co.,LTD.
**                                      
**                                 http://www.xtoee.com.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               epcRelayTestLib.h
** Latest modified Date:    2012-07-05
** Latest Version:          1.0
** Descriptions:            Ϊ������Զ���װ�ļ̵���������
**
**--------------------------------------------------------------------------------------------------------
** Created by:              chao
** Created date:            2012-07-05
** Version:                 1.0
** Descriptions:            The original version
**
*********************************************************************************************************/

#ifndef         __EPC_RELAY_TEST_LIB_H__
#define         __EPC_RELAY_TEST_LIB_H__

#ifdef  __cplusplus
extern "C" {
#endif
    ;
    /*********************************************************************************************************
    ** Function name:           epcRelayInit
    ** Descriptions:            ��ʼ���̵�������
    ** input parameters:
    **
    ** output parameters:       ��
    ** Returned value:          ������ʼ���Ƿ�ɹ�
    ** Note:
    *********************************************************************************************************/
    BOOL epcRelayInit();

    /*********************************************************************************************************
        ** Function name:           epcRelayOn
        ** Descriptions:            ָ���ļ̵�����
        ** input parameters:        dwIndex ָ���ļ̵�������
        **
        ** output parameters:       ��
        ** Returned value:          ���������Ƿ�ɹ�
        ** Note:
    *********************************************************************************************************/
    BOOL epcRelayOn(DWORD  dwIndex);

    /*********************************************************************************************************
    ** Function name:           epcRelayOff
    ** Descriptions:            ָ���ļ̵����ر�
    ** input parameters:        dwIndex ָ���ļ̵�������
    **
    ** output parameters:       ��
    ** Returned value:          ���������Ƿ�ɹ�
    ** Note:
    *********************************************************************************************************/
    BOOL epcRelayOff(DWORD  dwIndex);

    /*********************************************************************************************************
    ** Function name:           epcGetOpenInputState
    ** Descriptions:            ��ȡָ����©����˵�����״̬
    ** input parameters:        dwIndex  ָ����©����˵�����
    **
    ** output parameters:       ��
    ** Returned value:          0 ��ʾ �����
    **                          1 ��ʾ �����
    ** Note:
    *********************************************************************************************************/
    DWORD epcGetOpenInputState(DWORD  dwIndex);



#ifdef  __cplusplus
};
#endif

#endif
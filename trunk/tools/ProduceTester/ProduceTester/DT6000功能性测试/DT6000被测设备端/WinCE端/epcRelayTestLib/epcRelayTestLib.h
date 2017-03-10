/****************************************Copyright (c)****************************************************
**                            GuangZhou XuanTong Electric Technology Co.,LTD.
**                                      
**                                 http://www.xtoee.com.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               epcRelayTestLib.h
** Latest modified Date:    2012-07-05
** Latest Version:          1.0
** Descriptions:            为方便测试而封装的继电器操作库
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
    ** Descriptions:            初始化继电器操作
    ** input parameters:
    **
    ** output parameters:       无
    ** Returned value:          表明初始化是否成功
    ** Note:
    *********************************************************************************************************/
    BOOL epcRelayInit();

    /*********************************************************************************************************
        ** Function name:           epcRelayOn
        ** Descriptions:            指定的继电器打开
        ** input parameters:        dwIndex 指定的继电器索引
        **
        ** output parameters:       无
        ** Returned value:          表明操作是否成功
        ** Note:
    *********************************************************************************************************/
    BOOL epcRelayOn(DWORD  dwIndex);

    /*********************************************************************************************************
    ** Function name:           epcRelayOff
    ** Descriptions:            指定的继电器关闭
    ** input parameters:        dwIndex 指定的继电器索引
    **
    ** output parameters:       无
    ** Returned value:          表明操作是否成功
    ** Note:
    *********************************************************************************************************/
    BOOL epcRelayOff(DWORD  dwIndex);

    /*********************************************************************************************************
    ** Function name:           epcGetOpenInputState
    ** Descriptions:            获取指定开漏输入端的输入状态
    ** input parameters:        dwIndex  指定开漏输入端的索引
    **
    ** output parameters:       无
    ** Returned value:          0 表示 输入低
    **                          1 表示 输入高
    ** Note:
    *********************************************************************************************************/
    DWORD epcGetOpenInputState(DWORD  dwIndex);



#ifdef  __cplusplus
};
#endif

#endif
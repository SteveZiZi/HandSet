/****************************************Copyright (c)****************************************************
**                            GuangZhou XuanTong Electric Technology Co.,LTD.
**                                      
**                                 http://www.xtoee.com.cn
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               epcLedTestLib.h
** Latest modified Date:    2012-04-01
** Latest Version:          1.0
** Descriptions:            为方便测试而封装的LED动态库
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
** Descriptions:            根据dwMask指定的掩码来关闭或者点亮LED灯
** input parameters:        dwMask        LED状态掩码
**                          如果dwMask如果某一位为1，则对应的LED灯亮，
                            dwMask哪一位与其代表的LED灯的编号并没与必然关系，在实现这个函数时可根据具体情况
                            自行指定，然而推荐将LED灯从0开始递增编号，这样dwMask的第几位就代表第几个LED灯
** output parameters:       无
** Returned value:          无
** Note:
*********************************************************************************************************/
void epcSetLedMask(DWORD    dwMask);

/*********************************************************************************************************
** Function name:           epcGetLedMask
** Descriptions:            获取LED灯的状态
** output parameters:       无
** Returned value:          无
** Note:
*********************************************************************************************************/
void epcGetLedMask(DWORD    *dwMask);



#ifdef  __cplusplus
};
#endif


#endif
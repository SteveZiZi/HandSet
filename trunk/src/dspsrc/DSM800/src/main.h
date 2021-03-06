/****************************************************************************/
/*  main.h          v0.0.1                                                 */
/*  Copyright (c) 2010-2015   Guangzhou XuanTong electronics Co.,LTD.                */
/****************************************************************************/


/*********************************************************************************************************
 *   Power Quality parameters struct
 ********************************************************************************************************/
#include <std.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>
#include <swi.h>
#include <sys.h>
#include <sio.h>
#include <tsk.h>
#include <gio.h>
#include <pool.h>


/*----------------------------------- DSP/BIOS LINK 头文件-----------------------------------------------*/
#include <dsplink.h>
#include <failure.h>

#include <math.h>
#include <mathf.h>

#include <tskLoop.h>
#include <loop_config.h>

#if 0
#define  SNID_ERROR       	(1<<0)   
#define  CONNECT_ERROR    	(1<<1) 
#define  RTime_ERROR     	(1<<2)   
#define  PT_ERROR     		(1<<3)   
#define  CT_ERROR       	(1<<4)   
#define  VOLTRMS_ERROR 		(1<<5)   
#define  CURRRMS_ERROR      (1<<6)   
#define  FREQ_ERROR     	(1<<7)
#endif



extern  SEM_Obj SEM0;							// 发送信号量
extern  SEM_Obj SEM1;							// 40ms计算信号量
extern  SEM_Obj SEM2;							// 200ms计算信号量
extern  SEM_Obj SEM3;							// 5120ms计算信号量
extern  SEM_Obj SEM4;							// 10min计算信号量
extern  SEM_Obj SEM5;							// 谐波计算完成信号量
extern  SEM_Obj SEM6;							// 报文离散度计算完成信号量
extern  SEM_Obj SEM7;							// 接收信号量

extern void tskRecevieFun();						// 接收ARM端数据线程
extern void tskSendFun();							// DSP端发送数据线程
extern void tskCalc40msFun();						// 运算线程1
extern void tskCalc200msFun();						// 运算线程2
extern void tskCalc5120msFun();						// 运算线程3
extern void tskCalc10minFun();						// 运算线程4
extern void tskCalcPowerFun();






/*********************************************************************************************************
 * CRC校验表
 ********************************************************************************************************/
unsigned char CRC_TAB[256] = {    
    0,   94,  188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
    157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,      
     35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98, 
    190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
     70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
    219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
    101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
    248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
    140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
     17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
    175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
     50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
    202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
     87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
    233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
    116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53
};

/*********************************************************************************************************
 * 返回值0为OK，非0为错误; Fcs?, n为个数
 ********************************************************************************************************/
Uint8 CRC8_Tab(Uint8 Fcs, Uint8 *p, unsigned int n)
{
    unsigned char crc;
    
    crc = Fcs;
    do
    { 
        crc = CRC_TAB[(crc ^ (*p))];
        p++;
    } while(--n != 0);
    
    return (crc);
 }




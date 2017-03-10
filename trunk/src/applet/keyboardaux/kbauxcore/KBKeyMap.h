#pragma once

#include "KBDefines.h"

//
//=============================================================================
// DSM800 键盘概述
// 采用4×5矩阵键盘、7个独立键盘的组合方式
// 按键号0～19：数字键盘、方向键盘、ESC、ENTER、Menu、Home键
// 按键号20~25：F1 ~ F6 6个功能键
// 按键号26：电源开关键
// ScanKey扫描码分按下和松开两种情况：
// 按下时 ScanKey = 按键值*4 + 2
// 松开是 ScanKey = 按键值*4 + 3
//=============================================================================
// 布局与按键值(keyid):
//
//   F1    F2    F3    F4    F5    F6
//                       esc         enter 
//   1      2    3              up
//   4      5    6       left  menu  right      
//   7      8    9             down
// shift    0   .        home        power
//
// +------------+------------+------------+------------+
// |    P1_10   |    P1_2    |    P1_1    |    P1_0    |
// +------------+------------+------------+------------+
// |     ROW3   |    ROW2    |    ROW1    |    ROW0    |
// +------------+------------+------------+------------+-------+-------+
// |   1   (15) |   4  (10)  |   7  (05)  | SHIFT (00) | LINE0 | P0_6  |
// +------------+------------+------------+------------+-------+-------+
// |   2   (16) |   5  (11)  |   8  (06)  |   0   (01) | LINE1 | P0_7  |
// +------------+------------+------------+------------+-------+-------+
// |   3   (17) |   6  (12)  |   9  (07)  |   *   (02) | LINE2 | P0_8  |
// +------------+------------+------------+------------+-------+-------+
// |  ESC  (18) |  LEFT (13) | MENU (08)  |  DEL  (03) | LINE3 | P0_9  |
// +------------+------------+------------+------------+-------+-------+
// | ENTER (19) |   UP  (14) | RIGHT (09) |  DOWN (04) | LINE4 | P0_11 |
// +------------+------------+------------+------------+-------+-------+
//
//
// +-----------+-----------+-----------+-----------+-----------+-----------+
// |   P1_11   |   P2_0    |   P1_5    |   P0_1    |   P0_2    |   P0_3    |
// +-----------+-----------+-----------+-----------+-----------+-----------+
// |   F1 (20) |   F2 (21) |   F3 (22) |   F4 (23) |   F5 (24) |   F6 (25) |
// +-----------+-----------+-----------+-----------+-----------+-----------+
//=============================================================================
// 按键与VK_XXX的对应关系:
//
// 0-9  .  ESC  Enter  F1-F6  Up/Down/Left/Right  与真实键盘的VK_一一对应
// 其中: shift(VK_F9)  power(VK_F10)  menu(VK_TAB)  home(VK_F12)  
//=============================================================================
//

#define KB_KEY_NUM          31                  // 按键数目
#define KB_SCANMAP_SIZE     (KB_KEY_NUM)        // 扫描码映射表大小

//
//=============================================================================
// 按键码keyid定义
//=============================================================================
//
#define KB_KEYID_SHIFT      0                  // 转换当前模式, 比如数字态, 字符态
#define KB_KEYID_NUM0       1
#define KB_KEYID_DOT        2
#define KB_KEYID_RESERVE1   3
#define KB_KEYID_DOWN       4

#define KB_KEYID_NUM7       5
#define KB_KEYID_NUM8       6
#define KB_KEYID_NUM9       7
#define KB_KEYID_MENU       8
#define KB_KEYID_RIGHT      9

#define KB_KEYID_NUM4       10
#define KB_KEYID_NUM5       11
#define KB_KEYID_NUM6       12
#define KB_KEYID_LEFT       13
#define KB_KEYID_UP         14

#define KB_KEYID_NUM1       15
#define KB_KEYID_NUM2       16
#define KB_KEYID_NUM3       17
#define KB_KEYID_ESC        18
#define KB_KEYID_ENTER      19

#define KB_KEYID_F1         20
#define KB_KEYID_F2         21
#define KB_KEYID_F3         22
#define KB_KEYID_F4         23
#define KB_KEYID_F5         24
#define KB_KEYID_F6         25
#define KB_KEYID_HOME       26                       // 弹出HOME菜单
#define KB_KEYID_TESTQUIT   27                       // 测试状态, 点击标准101键盘的Q键退出本程序

#define KB_KEYID_RESERVE2   28       
#define KB_KEYID_RESERVE3   29       

#define KB_KEYID_POWER      30                       // 电源开关, 锁定屏幕


// 合成扫描码, up 输入 1代表 松开状态, 0代表按下状态
#define KB_SCODE_MAKE(keyid, up)    (((keyid) << 2) + ((up)? 3: 2))

// 扫描码分解, 获得keyid, 或按键是否按下, 弹起状态
#define KB_SCODE_GETKEYID(scode)    ((scode) >> 2)
#define KB_SCODE_ISKEYDOWN(scode)   (((scode) & 0x1) == 0)
#define KB_SCODE_ISKEYUP(scode)     (((scode) & 0x1) != 0)

//
//-----------------------------------------------------------------------------
// 按键按压状态
//-----------------------------------------------------------------------------
//
enum KBPressState
{
    KB_PRESS_DOWN = 0,
    KB_PRESS_UP = 1
};


//
//-----------------------------------------------------------------------------
// 扫描码映射信息结构
//-----------------------------------------------------------------------------
//
struct KBScancodeInfo
{
    UINT8                   keyid;              // 驱动发送过来的扫描码, 由 KeyId和KeyState组成 scancode = (KeyId << 2) + KeyState
    UINT                    nChar;              // VK_XXX
    LPCTSTR                 numModeChars;       // 数字模式下的可选字符, 比如L"1"
    LPCTSTR                 letModeChars;       // 字母模式下的可选字符, 比如L"ABCabc1"
    KBPressState            curPressState;      // 按键当前的按压状态
};



const KBScancodeInfo*  KBFindKeyFromScancode (UINT8 scancode);
const KBScancodeInfo*  KBFindKeyFromMessage (UINT nChar);
void  KBSetKeyCurPressState (UINT8 scancode, KBPressState state);
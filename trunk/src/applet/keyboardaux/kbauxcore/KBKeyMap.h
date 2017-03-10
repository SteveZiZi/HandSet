#pragma once

#include "KBDefines.h"

//
//=============================================================================
// DSM800 ���̸���
// ����4��5������̡�7���������̵���Ϸ�ʽ
// ������0��19�����ּ��̡�������̡�ESC��ENTER��Menu��Home��
// ������20~25��F1 ~ F6 6�����ܼ�
// ������26����Դ���ؼ�
// ScanKeyɨ����ְ��º��ɿ����������
// ����ʱ ScanKey = ����ֵ*4 + 2
// �ɿ��� ScanKey = ����ֵ*4 + 3
//=============================================================================
// �����밴��ֵ(keyid):
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
// ������VK_XXX�Ķ�Ӧ��ϵ:
//
// 0-9  .  ESC  Enter  F1-F6  Up/Down/Left/Right  ����ʵ���̵�VK_һһ��Ӧ
// ����: shift(VK_F9)  power(VK_F10)  menu(VK_TAB)  home(VK_F12)  
//=============================================================================
//

#define KB_KEY_NUM          31                  // ������Ŀ
#define KB_SCANMAP_SIZE     (KB_KEY_NUM)        // ɨ����ӳ����С

//
//=============================================================================
// ������keyid����
//=============================================================================
//
#define KB_KEYID_SHIFT      0                  // ת����ǰģʽ, ��������̬, �ַ�̬
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
#define KB_KEYID_HOME       26                       // ����HOME�˵�
#define KB_KEYID_TESTQUIT   27                       // ����״̬, �����׼101���̵�Q���˳�������

#define KB_KEYID_RESERVE2   28       
#define KB_KEYID_RESERVE3   29       

#define KB_KEYID_POWER      30                       // ��Դ����, ������Ļ


// �ϳ�ɨ����, up ���� 1���� �ɿ�״̬, 0������״̬
#define KB_SCODE_MAKE(keyid, up)    (((keyid) << 2) + ((up)? 3: 2))

// ɨ����ֽ�, ���keyid, �򰴼��Ƿ���, ����״̬
#define KB_SCODE_GETKEYID(scode)    ((scode) >> 2)
#define KB_SCODE_ISKEYDOWN(scode)   (((scode) & 0x1) == 0)
#define KB_SCODE_ISKEYUP(scode)     (((scode) & 0x1) != 0)

//
//-----------------------------------------------------------------------------
// ������ѹ״̬
//-----------------------------------------------------------------------------
//
enum KBPressState
{
    KB_PRESS_DOWN = 0,
    KB_PRESS_UP = 1
};


//
//-----------------------------------------------------------------------------
// ɨ����ӳ����Ϣ�ṹ
//-----------------------------------------------------------------------------
//
struct KBScancodeInfo
{
    UINT8                   keyid;              // �������͹�����ɨ����, �� KeyId��KeyState��� scancode = (KeyId << 2) + KeyState
    UINT                    nChar;              // VK_XXX
    LPCTSTR                 numModeChars;       // ����ģʽ�µĿ�ѡ�ַ�, ����L"1"
    LPCTSTR                 letModeChars;       // ��ĸģʽ�µĿ�ѡ�ַ�, ����L"ABCabc1"
    KBPressState            curPressState;      // ������ǰ�İ�ѹ״̬
};



const KBScancodeInfo*  KBFindKeyFromScancode (UINT8 scancode);
const KBScancodeInfo*  KBFindKeyFromMessage (UINT nChar);
void  KBSetKeyCurPressState (UINT8 scancode, KBPressState state);
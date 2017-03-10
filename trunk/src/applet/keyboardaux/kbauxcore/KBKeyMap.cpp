#include "stdafx.h"

#include "KBKeyMap.h"


//
//-----------------------------------------------------------------------------
// 扫描码映射关系列表
//  其中扫描码是严格的按照keyid顺序存放到数组, 便于快速查找
//  如果numModeChars 或letModeChars 为NULL 或L"", 则按下只发送原始的VK按键
//-----------------------------------------------------------------------------
//
static KBScancodeInfo  g_kbScancodeInfoMap[KB_SCANMAP_SIZE + 1] = {

    {KB_KEYID_SHIFT,    VK_F9,          NULL,   NULL,           KB_PRESS_UP },  // 0
    {KB_KEYID_NUM0,     VK_NUMPAD0,     L"0",   L" 0",          KB_PRESS_UP },  // 1
    {KB_KEYID_DOT,      VK_DECIMAL,     L".",   L"-*+/\\=.",    KB_PRESS_UP },  // 2
    {KB_KEYID_RESERVE1, VK_F12,         NULL,   NULL,           KB_PRESS_UP },  // 3
    {KB_KEYID_DOWN,     VK_DOWN,        NULL,   NULL,           KB_PRESS_UP },  // 4

    {KB_KEYID_NUM7,     VK_NUMPAD7,     L"7",   L"PQRSpqrs7",   KB_PRESS_UP },  // 5
    {KB_KEYID_NUM8,     VK_NUMPAD8,     L"8",   L"TUVtuv8",     KB_PRESS_UP },  // 6
    {KB_KEYID_NUM9,     VK_NUMPAD9,     L"9",   L"WXYZwxyz9",   KB_PRESS_UP },  // 7
    {KB_KEYID_MENU,     VK_F8/*VK_TAB*/,         NULL,   NULL,           KB_PRESS_UP },  // 8
    {KB_KEYID_RIGHT,    VK_RIGHT,       NULL,   NULL,           KB_PRESS_UP },  // 9

    {KB_KEYID_NUM4,     VK_NUMPAD4,     L"4",   L"GHIghi4",     KB_PRESS_UP },  // 10
    {KB_KEYID_NUM5,     VK_NUMPAD5,     L"5",   L"JKLjkl5",     KB_PRESS_UP },  // 11
    {KB_KEYID_NUM6,     VK_NUMPAD6,     L"6",   L"MNOmno6",     KB_PRESS_UP },  // 12
    {KB_KEYID_LEFT,     VK_LEFT,        NULL,   NULL,           KB_PRESS_UP },  // 13
    {KB_KEYID_UP,       VK_UP,          NULL,   NULL,           KB_PRESS_UP },  // 14

    {KB_KEYID_NUM1,     VK_NUMPAD1,     L"1",   L"#@$%&1",      KB_PRESS_UP },  // 15
    {KB_KEYID_NUM2,     VK_NUMPAD2,     L"2",   L"ABCabc2",     KB_PRESS_UP },  // 16
    {KB_KEYID_NUM3,     VK_NUMPAD3,     L"3",   L"DEFdef3",     KB_PRESS_UP },  // 17
    {KB_KEYID_ESC,      VK_ESCAPE,      NULL,   NULL,           KB_PRESS_UP },  // 18
    {KB_KEYID_ENTER,    VK_RETURN,      NULL,   NULL,           KB_PRESS_UP },  // 19
#if 1
    {KB_KEYID_F1,       VK_F1,          NULL,   NULL,           KB_PRESS_UP },  // 20
    {KB_KEYID_F2,       VK_F2,          NULL,   NULL,           KB_PRESS_UP },  // 21
    {KB_KEYID_F3,       VK_F3,          NULL,   NULL,           KB_PRESS_UP },  // 22
    {KB_KEYID_F4,       VK_F4,          NULL,   NULL,           KB_PRESS_UP },  // 23
    {KB_KEYID_F5,       VK_F5,          NULL,   NULL,           KB_PRESS_UP },  // 24
    {KB_KEYID_F6,       VK_F6,          NULL,   NULL,           KB_PRESS_UP },  // 25
#else
    {KB_KEYID_F1,       VK_F6,          NULL,   NULL,           KB_PRESS_UP },  // 20
    {KB_KEYID_F2,       VK_F5,          NULL,   NULL,           KB_PRESS_UP },  // 21
    {KB_KEYID_F3,       VK_F4,          NULL,   NULL,           KB_PRESS_UP },  // 22
    {KB_KEYID_F4,       VK_F3,          NULL,   NULL,           KB_PRESS_UP },  // 23
    {KB_KEYID_F5,       VK_F2,          NULL,   NULL,           KB_PRESS_UP },  // 24
    {KB_KEYID_F6,       VK_F1,          NULL,   NULL,           KB_PRESS_UP },  // 25
#endif
    {KB_KEYID_HOME,     VK_F12,         NULL,   NULL,           KB_PRESS_UP },  // 26
    {KB_KEYID_TESTQUIT, L'Q',           NULL,   NULL,           KB_PRESS_UP },  // 27

    {KB_KEYID_RESERVE2, VK_TAB,         NULL,   NULL,           KB_PRESS_UP },  // 28
    {KB_KEYID_RESERVE3, VK_CAPITAL,     NULL,   NULL,           KB_PRESS_UP },  // 29
    {KB_KEYID_POWER,    VK_F10,         NULL,   NULL,           KB_PRESS_UP },  // 30

    {0, 0, 0, 0, KB_PRESS_UP}

};

//
//-----------------------------------------------------------------------------
// 通过驱动传来的扫描码, 获得按键信息
//-----------------------------------------------------------------------------
//
const KBScancodeInfo*  KBFindKeyFromScancode (UINT8 scancode)
{
    // 使用快速查找法, 应为扫描码中的按键id按顺序从0开始编号, 
    // 并从0开始顺序存放到g_kbScancodeInfoMap
    int  keyid = KB_SCODE_GETKEYID(scancode);
    ASSERT(keyid >= 0 && keyid < KB_SCANMAP_SIZE);
    ASSERT(keyid == g_kbScancodeInfoMap[keyid].keyid);

    return &(g_kbScancodeInfoMap[keyid]);
}

//
//-----------------------------------------------------------------------------
// 通过按键VK_值(windows定义的虚拟按键消息), 获得按键信息
//-----------------------------------------------------------------------------
//
const KBScancodeInfo*  KBFindKeyFromMessage (UINT nChar)
{
    for (int i = 0; i < KB_SCANMAP_SIZE; i++)
    {
        if (nChar == g_kbScancodeInfoMap[i].nChar)
        {
            return &(g_kbScancodeInfoMap[i]);
        }
    }

    return NULL;
}

//
//-----------------------------------------------------------------------------
// 设置按键当前状态
//-----------------------------------------------------------------------------
//
void  KBSetKeyCurPressState (UINT8 scancode , KBPressState state)
{
    KBScancodeInfo*  keyInfo = const_cast<KBScancodeInfo*>(KBFindKeyFromScancode(scancode));
    ASSERT(keyInfo);

    keyInfo->curPressState = state;
}
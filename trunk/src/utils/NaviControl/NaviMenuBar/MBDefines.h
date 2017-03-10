#pragma once


//
//=============================================================================
// �汾��Ϣ
//=============================================================================
//
#define _MB_VERSION_                    MAKELONG(1, 0)

#define _MB_VERSION_MAJOR               0

#define _MB_VERSION_MINOR               1

#define _MB_VERSION_REVISION            1



// ����
#ifdef _DEBUG
#define MB_DEBUGLOG  NKDbgPrintfW
#else
#define MB_DEBUGLOG(...)
#endif

#define MB_STRING2(x) #x
#define MB_STRING(x) EL_STRING2(x)


//
//=============================================================================
// �����������
//=============================================================================
//
class CNaviToolBar;
class CNaviPopupBar;
class CNaviMenu;
class CNaviCmdUI;


//
//=============================================================================
// ��������
//=============================================================================
//
#define MBSUPER             CListCtrlEL


// ��������ӳ��

#define MBVK_ESC            VK_ESCAPE
#define MBVK_OK             VK_RETURN

// UI����ģ��
#define MBT_DRAWTEXTSHADOW          1                                   // �Ƿ������Ӱ
#define MBT_DRAWTEXTSHADOW_COLOR    RGB(32, 32, 32)                     // ��Ӱ��ɫ
#define MBT_TEXTLFHEIGHT            -16                                 // �Ƽ������С, ��Ҫ���Ӧ�õ���SetFont
#define MBT_TEXTCOLOR               RGB(255, 255, 255)                  // �Ƽ�������ɫ
#define MBT_TEXTHLCOLOR             RGB(255, 255, 91)                   // ���ָ�����ɫ
#define MBT_TEXTGRYCOLOR            RGB(127, 127, 127)                  // ���ֱ����ɫ
#define MBT_TEXTBLACKCOLOR          RGB(40,  50,  68)                   // ���ֱ����ɫ



//
//=============================================================================
// ֪ͨ��Ϣ����
//=============================================================================
//





//
//=============================================================================
// ö��
//=============================================================================
//

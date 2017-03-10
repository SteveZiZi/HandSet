#ifndef __PUBLIC_WND_USER_MESSAGE_ID_H
#define __PUBLIC_WND_USER_MESSAGE_ID_H




#define PUBLIC_WND_USERID   (WM_USER + 100)     // 公共窗口消息起始ID
#define PRIVATE_WND_USERID  (WM_USER + 300)     // 私有窗口消息起始ID


//
//=============================================================================
// 进程间通信公共消息ID定义,  内存映射文件: DSM800APP_FILEMAPPING
//=============================================================================
//

//
// 获得窗口的焦点CaretPos位置,
//  因为GetCaretPos不能跨进程访问, 所以, Edit窗口应该提供一个这个消息响应, 
//  使得其他进程可以获得焦点位置, PWID_RET_CARETPOS_OK 后通过内存映射文件提供的结构体获得位置
//
#define PWID_GET_CARETPOS       (PUBLIC_WND_USERID + 1)     // 获从窗口获取CaretPos的消息
#define PWID_RET_CARETPOS_OK    0x55AA                      // 获取CaretPos成功, 从消息响应函数返回该值


//
// 通知目标窗口, 输入法状态已经修改,
//  wParam代表当前输入法状态, AFM_KBInputMode
//  同时, 在内存映射文件的结构体也同时更新该状态
//
#define PWID_UPDATE_INPUTMODE   (PUBLIC_WND_USERID + 2)     // 通知目标窗口, 输入法状态已经修改,




#endif  // __PUBLIC_WND_USER_MESSAGE_ID_H




#ifndef __PUBLIC_WND_USER_MESSAGE_ID_H
#define __PUBLIC_WND_USER_MESSAGE_ID_H




#define PUBLIC_WND_USERID   (WM_USER + 100)     // ����������Ϣ��ʼID
#define PRIVATE_WND_USERID  (WM_USER + 300)     // ˽�д�����Ϣ��ʼID


//
//=============================================================================
// ���̼�ͨ�Ź�����ϢID����,  �ڴ�ӳ���ļ�: DSM800APP_FILEMAPPING
//=============================================================================
//

//
// ��ô��ڵĽ���CaretPosλ��,
//  ��ΪGetCaretPos���ܿ���̷���, ����, Edit����Ӧ���ṩһ�������Ϣ��Ӧ, 
//  ʹ���������̿��Ի�ý���λ��, PWID_RET_CARETPOS_OK ��ͨ���ڴ�ӳ���ļ��ṩ�Ľṹ����λ��
//
#define PWID_GET_CARETPOS       (PUBLIC_WND_USERID + 1)     // ��Ӵ��ڻ�ȡCaretPos����Ϣ
#define PWID_RET_CARETPOS_OK    0x55AA                      // ��ȡCaretPos�ɹ�, ����Ϣ��Ӧ�������ظ�ֵ


//
// ֪ͨĿ�괰��, ���뷨״̬�Ѿ��޸�,
//  wParam����ǰ���뷨״̬, AFM_KBInputMode
//  ͬʱ, ���ڴ�ӳ���ļ��Ľṹ��Ҳͬʱ���¸�״̬
//
#define PWID_UPDATE_INPUTMODE   (PUBLIC_WND_USERID + 2)     // ֪ͨĿ�괰��, ���뷨״̬�Ѿ��޸�,




#endif  // __PUBLIC_WND_USER_MESSAGE_ID_H




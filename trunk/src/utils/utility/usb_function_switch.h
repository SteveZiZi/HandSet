/**
 * \file
 *      USBFunctionSwitch.h
 *
 * \brief
 *      usb����ģʽ�л�����ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/6/21
 */

#define USB_RNDIS  0 //Note that X11 don't support this client mode
#define USB_Serial 1
#define USB_MSF    2


extern HANDLE GetUfnController(void);
extern BOOL ChangeClient(HANDLE hUfn, LPCTSTR pszNewClient);
extern BOOL SetClientMode(UINT mode);
extern BOOL GetClientName(HANDLE hUfn, LPTSTR pszNewClient);


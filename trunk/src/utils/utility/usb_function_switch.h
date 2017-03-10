/**
 * \file
 *      USBFunctionSwitch.h
 *
 * \brief
 *      usb连接模式切换功能头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
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


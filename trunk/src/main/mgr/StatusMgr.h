/**
 * \file
 *      StatusMgr.h
 *
 * \brief
 *      ״̬������
 *          ��ȡDSTester����Ӳ��״̬��Ϣ
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/3/25
 */
#pragma once

#if !SHENG_ZE
#include "devload.h"
#endif
#include "src/utils/ipc/filemapping.h"
#include "src/include/p_filemapping_def.h"


#define DSM_STATU_CONNECT_USB               1           ///< ����USB
#define DSM_STATU_DISCONNECT_USB            0           ///< �γ�USB
#define DSM_STATU_CONNECT_SD                1           ///< ����SD��
#define DSM_STATU_DISCONNECT_SD             0           ///< �γ�SD��
#if !SHENG_ZE
#define DSM_STATU_CONNECT_UCARD             1           ///< ����U��
#define DSM_STATU_DISCONNECT_UCARD          0           ///< �γ�U��
#endif

namespace statusmgr
{
    /**
     *      ͬ��ģʽ
     */
    enum SyncMode
    {
        Mode_Err,               ///< ����
        Mode_UNSYNC,            ///< δͬ��
        Mode_1588,              ///< 1588
        Mode_IRIGB,             ///< ����B��
        Mode_IRIGBN             ///< ����B��
    };
}


/**
 *      ��Ӳ����Ϣ��״̬������
 */
class CStatusMgr
{
public:

    /**
     *      ��ȡ����ֵ
     *
     *  \return
     *      int             ����ֵ��0~100
     */
    static int GetBattery();

    /**
     *      ��ȡSD��״̬
     *
     *  \return
     *      int         DSM_STATU_CONNECT_SD:       ����SD��
     *                  DSM_STATU_DISCONNECT_SD:    �γ�SD��
     */
    static int GetSD();

    /**
     *      ��ȡUSB״̬
     *
     *  \return
     *      int         DSM_STATU_CONNECT_USB:      ����USB
     *                  DSM_STATU_DISCONNECT_USB:   �γ�USB
     */
    static int GetUSB();
/**
     *      ��ȡU��״̬
     *
     *  \return
     *      int         DSM_STATU_CONNECT_USB:      ����USB
     *                  DSM_STATU_DISCONNECT_USB:   �γ�USB
     */
#if !SHENG_ZE
    /**
     *      ����SD��״̬
     *
     *  \param
     *      int         DSM_STATU_CONNECT_SD:       ����SD��
     *                  DSM_STATU_DISCONNECT_SD:    �γ�SD��
     */
    static void SetSD(int);
    /**
     *      ��ȡU��״̬
     *
     *  \return
     *      int         DSM_STATU_CONNECT_USB:      ����USB
     *                  DSM_STATU_DISCONNECT_USB:   �γ�USB
     */
    static int GetUSBDisk();

    /**
     *      ����U��״̬
     *
     *  \param
     *      int         DSM_STATU_CONNECT_USB:      ����USB
     *                  DSM_STATU_DISCONNECT_USB:   �γ�USB
     */
    static void SetUSBDisk(int);
#endif
    /**
     *      ��ȡ����״̬
     *  \return
     *      bool        true��ʶ����״̬��false��ʶ������״̬
     */
    static bool GetScreenLock();

    /**
     *      ��������״̬
     */
    static void SetScreenLock(bool bLock);


    /**
     *      ��ȡ���õ�Դ����״̬
     *
     *  /return
     *      int         AC_LINE_OFFLINE Offline 
     *                  AC_LINE_ONLINE Online 
     *                  AC_LINE_BACKUP_POWER Backup Power 
     *                  AC_LINE_UNKNOWN Unknown status 
     */
    //static int GetExPower();

    /**
     *      �ж��Ƿ��ڳ��״̬
     */
    static bool GetCharge();

    /**
     *      ��ȡҵ�񴰿ڱ���
     */
    static CString GetTitle();

    /**
     *      ����ҵ�񴰿ڱ���
     */
    static void SetTitle(const CString& csTitle);

    /**
     *      ��ʾ��ʾ��Ϣ
     */
    static void SetTips(const CString& csTips, UINT uTimeOut = 3000, COLORREF color = RGB(0xFF, 0xFF, 0xFF));

    /**
     *      ��ȡ��ʾ��Ϣ�ַ���
     */
    static CString GetTips();

    /**
     *      ��ȡ��ʾ��Ϣ��ɫ
     */
    static COLORREF GetTipsColor();

    /**
     *      �Ƿ���ʾ��ʾ��Ϣ
     *
     *  \return
     *      DWORD       0x01                ��ʾ��ʾ��Ϣ
     *                  0x11                ��Ϣ��˸
     */
    static DWORD ShowTipsStyle();

    /**
     *      ��ȡSD��״̬ͼ
     */
    static HBITMAP GetSDBitmap();

    /**
     *      ��ȡUSB״̬ͼ
     */
    static HBITMAP GetUSBBitmap();
#if !SHENG_ZE
	/**
     *      ��ȡU��״̬ͼ
     */
	static HBITMAP GetUSBDiskBitmap();
#endif

    /**
     *      ��ȡ���״̬ͼ
     */
    static HBITMAP GetBatteryBitmap();

    /**
     *      ��ȡ��ǰ�������
     */
    static void Snapshot();

    /**
     *      �����һ�ν�ͼ��Ϣ
     */
    static void ClearSnapshot();

    /**
     *      �����ͼ���ݵ��ļ�
     *
     *  \param[in]
     *      csFile          ָ��������ļ�����Ϊ����ʹ��Ĭ���ļ���
     */
    static bool SaveSnapshot(CString csFile = L"");


    /**
     *      �ж��Ƿ�ӵ��Ȩ��
     *
     *  \return
     *      bool            ���Ѿ�ӵ��Ȩ�޷���true�����򷵻�false
     */
    static bool IsAuthorized();

    /**
     *      ��ȡ����Ȩ��
     *
     *  \param[in]
     *      csPassword      ����
     *  \param[in]
     *      nTime           Ȩ����Чʱ�䣬Ĭ��2����
     *  \return
     *      bool            ��ȡȨ�޳ɹ�����true�����򷵻�false
     */
    static bool GetAuthority(CString csPassword, unsigned int nTimeCycle = 120000);


    /**
     *      ����Ȩ�޹���
     */
    static void ExpireAuthority();

//////////////////////////////////////////////////////////////////////////
public:

    /**
     *      �����Ƿ�������ʾ����ģʽ
     */
    static void SetShowInputMode(bool bShow = true);

    /**
     *      ��ȡ�Ƿ�������ʾ����ģʽ
     */
    static bool GetShowInputMode();


    /**
     *      ���Ӧ�ó���ǰ��״̬
     */
    static bool GetDSM800APPInfo(DSM800AppInfo* retInfo);

    /**
     *      ��ȡ����״̬
     */
    static APP_KBInputMode GetDSM800InputMode();

    /**
     *      ��ȡ��������
     */
    static BYTE GetBrightness();

    /**
     *      ���ñ�������
     */
    static void SetBrightness(BYTE byteBrightness);

    /**
     *      ��ȡ����ʱ��
     */
    static DWORD GetBrightTime();

    /**
     *      ���ñ���ʱ��
     */
    static void SetBrightTime(DWORD dwTime);

    /**
     *      ��ȡͬ��ģʽ
     */
    static statusmgr::SyncMode GetSyncMode();

    /**
     *      �޸�ϵͳע���Ĭ��ֵ
     */
    static void FixDefault();




private:

    /**
     *      ����BITMAP��Ϣ
     */
    static PBITMAPINFO _createBitmapInfoStruct();

    /**
     *      ����BMP�ļ�
     */
    static bool _createBMPFile(CString csFile, PBITMAPINFO pbi);

    /**
     *      ����Ĭ�Ͻ��ͱ���
     */
    //static void _setDefaultBKLight(DWORD dwTime = 60);

    /**
     *      �޸ı���ʱ���ע����ֵ
     */
    static void _fixBkLight();


private:
    /**
     *      ��ȡ״̬�������ľ�̬ʵ������
     */
    static CStatusMgr& getInstance()
    {
        static CStatusMgr s_Instance;
        return s_Instance;
    }

private:
    CStatusMgr(void);
    ~CStatusMgr(void);

private:
    typedef CMap<int, int, int, int>    CResidMap;

    CString                 m_csTitle;              ///< ҵ�񴰿ڱ���
    CResidMap               m_mapBatteryResID;      ///< �����ԴIDӳ���

    CString                 m_csTips;               ///< ��ʾ��Ϣ
    bool                    m_bTips;                ///< �Ƿ���ʾ��ʾ��Ϣ
    UINT                    m_uTimeOut;             ///< ������ʾtipsʱ��
    COLORREF                m_crTips;               ///< ��ʾ��Ϣ������ɫ
    DWORD                   m_dwStyle;              ///< ��ʾ��Ϣ����

    static HBITMAP          m_hSnapPanel;           ///< ��ͼ����

    bool                    m_bLock;                ///< ����״̬

    bool                    m_bAuthority;           ///< Ȩ��״̬

    unsigned int            m_nTimerAuth;           ///< Ȩ�޶�ʱ��ID

    HANDLE                  m_hBrightFile;          ///< �����豸

    bool                    m_bShowInputMode;       ///< ��ʾ����ģʽ״̬
    static CShareMemory     m_sfileMapping;         ///< �ڴ�ӳ���ļ�������������

    static SYSTEM_POWER_STATUS_EX2  m_sStatus;      ///< �豸ϵͳ��Դ״̬
#if !SHENG_ZE
	static int              m_nSDStatu;             ///< SD����״̬
	static int              m_nUCardStatu;          ///< U�̽���״̬
#endif
};

#pragma once
#include "afxwin.h"

#include <vector>

// CQuickSaveDlg �Ի���

class CQuickSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CQuickSaveDlg)

public:
	CQuickSaveDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CQuickSaveDlg();

// �Ի�������
	enum { IDD = IDD_QUICK_SAVE_DIALOG };

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnDropdownSaveDrive();

	///
	/// @brief
	///     ����Ĭ���ļ���
	///
	/// @param[in]
	///     fileName - Ĭ���ļ���
	///
	void SetDefaultFileName(const CString &fileName);

	///
	/// @brief
	///     ����ļ�����ʷ�������5��
	///
	/// @param[in]
	///     fileName - Ҫ��ӵ��ļ���
	///
	void AddFileNameHistory(const CString &fileName);

	///
	/// @brief
	///     ��ȡ�ļ���
	///
	/// @return
	///     �ļ���
	///
	CString GetFileName();

	///
	/// @brief
	///     ��ȡ�ļ�·��
	///
	/// @return
	///     �ļ�·��
	///
	CString GetFilePath();

	///
	/// @brief
	///     ̽��ָ���ļ��Ƿ����
	///
	/// @param[in]
	///     csPath - ��̽����ļ�·��
	///
	/// @return
	///     ���ڷ���TRUE�����򷵻�FALSE
	///
	BOOL IsFileExist(const CString &csPath);

	///
	/// @brief
	///     ̽��ָ��Ŀ¼�Ƿ����
	///
	/// @param[in]
	///     csPath - ��̽���Ŀ¼·��
	///
	/// @return
	///     ���ڷ���TRUE�����򷵻�FALSE
	///
	BOOL IsDirExist(const CString &csPath);

	///
	/// @brief
	///     ����Ŀ¼
	///
	/// @param[in]
	///     csPath - ������Ŀ¼·��
	///
	/// @return
	///     ����Ŀ¼�ɹ�����TRUE��ʧ�ܷ���FALSE
	///
	BOOL CreateDir(const CString &csPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_cSaveDrive;
	CComboBox m_cSaveFileName;
	CString   m_csDrive;
	CString   m_csFileName;
	std::vector<CString> m_vFileNameOpts;
};

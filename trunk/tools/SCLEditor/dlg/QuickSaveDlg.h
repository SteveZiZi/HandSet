#pragma once
#include "afxwin.h"

#include <vector>

// CQuickSaveDlg 对话框

class CQuickSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CQuickSaveDlg)

public:
	CQuickSaveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CQuickSaveDlg();

// 对话框数据
	enum { IDD = IDD_QUICK_SAVE_DIALOG };

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnDropdownSaveDrive();

	///
	/// @brief
	///     设置默认文件名
	///
	/// @param[in]
	///     fileName - 默认文件名
	///
	void SetDefaultFileName(const CString &fileName);

	///
	/// @brief
	///     添加文件名历史，最多有5项
	///
	/// @param[in]
	///     fileName - 要添加的文件名
	///
	void AddFileNameHistory(const CString &fileName);

	///
	/// @brief
	///     获取文件名
	///
	/// @return
	///     文件名
	///
	CString GetFileName();

	///
	/// @brief
	///     获取文件路径
	///
	/// @return
	///     文件路径
	///
	CString GetFilePath();

	///
	/// @brief
	///     探测指定文件是否存在
	///
	/// @param[in]
	///     csPath - 待探测的文件路径
	///
	/// @return
	///     存在返回TRUE，否则返回FALSE
	///
	BOOL IsFileExist(const CString &csPath);

	///
	/// @brief
	///     探测指定目录是否存在
	///
	/// @param[in]
	///     csPath - 待探测的目录路径
	///
	/// @return
	///     存在返回TRUE，否则返回FALSE
	///
	BOOL IsDirExist(const CString &csPath);

	///
	/// @brief
	///     创建目录
	///
	/// @param[in]
	///     csPath - 待创建目录路径
	///
	/// @return
	///     创建目录成功返回TRUE，失败返回FALSE
	///
	BOOL CreateDir(const CString &csPath);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_cSaveDrive;
	CComboBox m_cSaveFileName;
	CString   m_csDrive;
	CString   m_csFileName;
	std::vector<CString> m_vFileNameOpts;
};

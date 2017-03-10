#pragma once


// CFullScreenDlg dialog

class CFullScreenDlg : public CDialog
{
	DECLARE_DYNAMIC(CFullScreenDlg)

public:
	CFullScreenDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFullScreenDlg();

// Dialog Data
	enum { IDD = IDD_FULL_SCREEN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
    int     m_showLevel;                                //������ֵ������ʾ������
    HDC     m_hMemDC;
    HBITMAP m_hMemBitmap;
    int     m_iImgWidth;                                //����ͼƬ�Ŀ��
    int     m_iImgHeight;                               //����ͼƬ�ĸ߶�

    BOOL LoadTestImage();

public:

    BOOL    DisplayColorLevel();                        //��ʾɫ��ͼ
    BOOL    DisplayStartInterface();                    //��ʾ��ʼ����
    BOOL    DisplayTestImage();                         //��ʾ����ͼƬ

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
};

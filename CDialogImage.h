//
// CDialogImage.h : ヘッダー ファイル
//
#pragma once

class CDialogImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogImage)

// コンストラクション・デストラクション
public:
	CDialogImage(CWnd* pParent = nullptr);	// 標準コンストラクター
	virtual ~CDialogImage();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_IMAGE };
#endif


// 実装
protected:
	HICON m_hIcon;
	virtual void PostNcDestroy();
	CWnd* m_pParent; 
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit1;
	CStatic m_Picture1;
	CRect m_Rect;
	afx_msg void OnBnClickedFile();
	afx_msg void OnBnClickedRead();
	afx_msg void OnEnKillfocusFile();
	afx_msg void OnEnChangeFile();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

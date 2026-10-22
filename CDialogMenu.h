//
// CDialogMenu.h 
//
#pragma once

class CDialogMenu : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogMenu)

public:
	CDialogMenu(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CDialogMenu();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MENU };
#endif

private:
	afx_msg LRESULT OnImageDialogClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListDialogClosed(WPARAM wParam, LPARAM lParam);

protected:
	HICON m_hIcon;
	class CDialogImage* m_pCDialogImage;
	class CDialogList* m_pCDialogList;
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonImage();
	afx_msg void OnBnClickedButtonList();
	afx_msg void OnBnClickedCancel();
};

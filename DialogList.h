//
// DialogEdit ダイアログ
//

#pragma once

class CDialogList : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogList)

public:
	CDialogList(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CDialogList();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LIST };
#endif

protected:
	HICON m_hIcon;
	class CDialogEdit* m_pCDialogEdit;
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List1;
	CImageList m_ImageList;
	CEdit m_Edit1;
	CEdit m_Edit2;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedNew();
	afx_msg void OnCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult);
	void ConvertIconToBitmap(CBitmap* bmpObj, HICON hIcon, int cx, int cy);
	HICON CreateIconFromBMPBuffer(unsigned char* pBuffer, size_t bufferSize);
	void SetImage(unsigned char* pBuffer, size_t bufferSize, CDC* pScreenDC, CBitmap& bitmapMerg);
};

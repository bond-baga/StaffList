//
// CDialogEdit ダイアログ
//
#pragma once
#include "MyIconButton.h"

//#include <guiddef.h>

class CDialogEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogEdit)

public:
	CDialogEdit(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CDialogEdit();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDIT };
#endif

protected:
	HICON m_hIcon;
	CWnd* m_pParent;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CString m_Code;
	CEdit m_Edit1;
	CEdit m_Edit2;
	CEdit m_Edit3;
	CEdit m_Edit4;
	CEdit m_Edit5;
	CStatic m_Picture1;
	CMyIconButton m_Button_Right;
	CMyIconButton m_Button_Left;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedFile();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedRight();
	afx_msg void OnBnClickedLeft();

	void OnPictureDraw(CString strFile);
	int IsDuplicated(CString code);
	BOOL CImageToByteArray(CImage& image, std::vector<BYTE>& bytes, const GUID& format);
	BOOL CImageToByteArrayOld(CImage& image, std::vector<BYTE>& bytes, const GUID& format);
	void RotateImage90Clockwise(CImage& srcImage, CImage& destImage);
	void RotateImageCounter90Clockwise(CImage& srcImage, CImage& destImage);
};

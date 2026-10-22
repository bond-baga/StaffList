
// StaffList.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once
#include <vector>

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CStaffListApp:
// このクラスの実装については、StaffList.cpp を参照してください
//
#define WM_IMAGE_DIALOG_CLOSED (WM_USER + 1)
#define WM_LIST_DIALOG_CLOSED (WM_USER + 2)

class CStaffListApp : public CWinApp
{
public:
	CStaffListApp();

	CString GetDefaultConnect();
	void Utf8ToLPTSTR(const char* utf8Str, std::vector<TCHAR>& vec);
	void LPTSTRToUtf8(const TCHAR* tStr, std::vector<CHAR>& vec);

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CStaffListApp theApp;

enum ITEMS {
	CODE,
	NAME,
	KANA,
	STATECODE,
	STATUSCODE,
	DIVISIONNAME,
	REMARKS,
	IMAGE
};

enum RETURN {
	ERR,
	NO,
	YES
};

extern CStaffListApp theApp;

inline CStaffListApp* thisApp() {
	return static_cast<CStaffListApp*>(AfxGetApp());
}


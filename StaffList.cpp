
// StaffList.cpp : アプリケーションのクラス動作を定義します。
//

#include "pch.h"
#include "framework.h"
#include "StaffList.h"
#include "DialogList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStaffListApp

BEGIN_MESSAGE_MAP(CStaffListApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CStaffListApp の構築

CStaffListApp::CStaffListApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CStaffListApp オブジェクト

CStaffListApp theApp;


// CStaffListApp の初期化

BOOL CStaffListApp::InitInstance()
{
	CWinApp::InitInstance();


	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	CShellManager *pShellManager = new CShellManager;

	// MFC コントロールでテーマを有効にするために、"Windows ネイティブ" のビジュアル マネージャーをアクティブ化
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	CDialogList dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
		TRACE(traceAppMsg, 0, "警告: ダイアログで MFC コントロールを使用している場合、#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS を指定できません。\n");
	}

	// 上で作成されたシェル マネージャーを削除します。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

CString CStaffListApp::GetDefaultConnect()
{
	return _T("ODBC;DSN=afx;Trusted_Connection=Yes;");
}


void CStaffListApp::Utf8ToLPTSTR(const char* utf8Str, std::vector<TCHAR>& vec)
{
	if (!utf8Str) return;
#ifdef UNICODE
	// utf-8 to Unicode conversion
	// Get the required buffer size
	// このパラメーターが -1 の場合、関数は入力文字列全体 (終端の null 文字を含む) を処理します。 
	// そのため、結果の Unicode 文字列は終端の null 文字を持ち、関数によって返される長さにはこの文字が含まれます。
	int len = ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
	if (len > 0) {
		vec.resize(len);
		::MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, vec.data(), len);
	}
#else
	// utf-8 to ANSI conversion　
	int len = ::MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, nullptr, 0);
	if (len > 0) {
		// utf-8 -> utf-16
		std::vector<WCHAR>vec16(len);
		::MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, vec16.data(), len);
		// utf-16 -> ANSI
		int len2 = ::WideCharToMultiByte(CP_ACP, 0, vec16.data(), -1, nullptr, 0, nullptr, nullptr);
		if (len2 > 0) {
			vec.resize(len2);
			::WideCharToMultiByte(CP_ACP, 0, vec16.data(), -1, vec.data(), len2, nullptr, nullptr);
		}
	}
#endif
	return;
}

void CStaffListApp::LPTSTRToUtf8(const TCHAR* tStr, std::vector<CHAR>& vec)
{
	if (!tStr) return;
#ifdef UNICODE
	// utf-8 to Unicode conversion
	// Get the required buffer size
	// このパラメーターが -1 の場合、関数は入力文字列全体 (終端の null 文字を含む) を処理します。 
	// そのため、結果の Unicode 文字列は終端の null 文字を持ち、関数によって返される長さにはこの文字が含まれます。
	int len = ::WideCharToMultiByte(CP_UTF8, 0, tStr, -1, nullptr, 0, nullptr, nullptr);
	if (len > 0) {
		vec.resize(len);
		// Perform conversion into the existing vector
		::WideCharToMultiByte(CP_UTF8, 0, tStr, -1, vec.data(), len, nullptr, nullptr);
	}
#else
	int len = ::MultiByteToWideChar(CP_ACP, 0, tStr, -1, nullptr, 0);
	if (len > 0) {
		std::vector<WCHAR> vec16(len);
		::MultiByteToWideChar(CP_ACP, 0, tStr, -1, vec16.data(), len);

		int len2 = ::WideCharToMultiByte(CP_UTF8, 0, vec16.data(), (int)vec16.size(), nullptr, 0, nullptr, nullptr);
		vec.resize(len2);
		::WideCharToMultiByte(CP_UTF8, 0, vec16.data(), (int)vec16.size(), vec.data(), len2, nullptr, nullptr);
	}
#endif
	return;
}
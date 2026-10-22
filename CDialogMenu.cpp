//
// CDialogMenu.cpp : 実装ファイル
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CDialogMenu.h"
#include "CDialogImage.h"
#include "CDialogList.h"


// CDialogMenu ダイアログ

IMPLEMENT_DYNAMIC(CDialogMenu, CDialogEx)

// コンストラクタ
CDialogMenu::CDialogMenu(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MENU, pParent)
	, m_pCDialogImage(nullptr)
	, m_pCDialogList(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);//

}

// デストラクション
CDialogMenu::~CDialogMenu()
{
	CDialogEx::~CDialogEx();
}

// メッセージマップ
BEGIN_MESSAGE_MAP(CDialogMenu, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE, &CDialogMenu::OnBnClickedButtonImage)
	ON_BN_CLICKED(IDC_BUTTON_LIST, &CDialogMenu::OnBnClickedButtonList)
	ON_BN_CLICKED(IDCANCEL, &CDialogMenu::OnBnClickedCancel)
	ON_MESSAGE(WM_IMAGE_DIALOG_CLOSED, &CDialogMenu::OnImageDialogClosed)
	ON_MESSAGE(WM_LIST_DIALOG_CLOSED, &CDialogMenu::OnListDialogClosed)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// 仮想関数のオーバーライド
void CDialogMenu::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CDialogMenu::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CDialogMenu::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this; // Delete the dialog object allocated on the heap
}

// コマンド
HCURSOR CDialogMenu::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDialogMenu::OnBnClickedButtonImage()
{
	if (m_pCDialogImage == nullptr)
	{
		m_pCDialogImage = new CDialogImage(this); // Pass 'this' as the parent
		if (!m_pCDialogImage->Create(IDD_DIALOG_IMAGE, GetDesktopWindow()))
		{
			// handle creation error
			delete m_pCDialogImage;
			m_pCDialogImage = nullptr;
			return;
		}
		m_pCDialogImage->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pCDialogImage->BringWindowToTop(); // Optional: bring existing dialog to front
	}
}

void CDialogMenu::OnBnClickedButtonList()
{
	if (m_pCDialogList == NULL)
	{
		m_pCDialogList = new CDialogList(this);
		if (!m_pCDialogList->Create(IDD_DIALOG_LIST, GetDesktopWindow()))
		{
			// handle creation error
			delete m_pCDialogList;
			m_pCDialogList = NULL;
			return;
		}
		m_pCDialogList->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pCDialogList->BringWindowToTop(); // Optional: bring existing dialog to front
	}
}

void CDialogMenu::OnBnClickedCancel()
{
	if (m_pCDialogImage != nullptr || m_pCDialogList != nullptr) {
		CString strError;
		strError.Format(_T("%sが終了していません"), _T("一覧または画像"));
		AfxMessageBox(strError);
		return;
	}
	DestroyWindow(); // This triggers the window destruction sequence
}

// ユーザー定義メッセージ
LRESULT CDialogMenu::OnImageDialogClosed(WPARAM wParam, LPARAM lParam)
{
	m_pCDialogImage = nullptr;
	return 0;
}

LRESULT CDialogMenu::OnListDialogClosed(WPARAM wParam, LPARAM lParam)
{
	m_pCDialogList = nullptr;
	return 0;
}

// ユーザー定義
void CDialogMenu::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

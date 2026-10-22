//
// ImageDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "MFCApplication1.h"
#include "CDialogImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageDlg ダイアログ
IMPLEMENT_DYNAMIC(CDialogImage, CDialogEx)

// コンストラクタ
CDialogImage::CDialogImage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_IMAGE, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_pParent = pParent;
}

// デストラクション
CDialogImage::~CDialogImage()
{
	CDialogEx::~CDialogEx();
}


// メッセージマップ
BEGIN_MESSAGE_MAP(CDialogImage, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CDialogImage::OnBnClickedFile)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CDialogImage::OnBnClickedRead)
	ON_EN_KILLFOCUS(IDC_EDIT_FILE, &CDialogImage::OnEnKillfocusFile)
	ON_EN_CHANGE(IDC_EDIT_FILE, &CDialogImage::OnEnChangeFile)
	ON_BN_CLICKED(IDOK, &CDialogImage::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogImage::OnBnClickedCancel)
END_MESSAGE_MAP()

// 仮想関数のオーバーライド
void CDialogImage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILE, m_Edit1);
	DDX_Control(pDX, IDC_PICTURE1, m_Picture1);
}

BOOL CDialogImage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 1. Load the icon from your project resources
	HICON hIcon_Right = (HICON)::LoadImage(
		AfxGetApp()->m_hInstance,
		MAKEINTRESOURCE(IDI_RIGHT), // Replace with your actual Icon ID
		IMAGE_ICON,
		0, 0,                        // 0,0 forces the system to use the actual icon file dimensions
		LR_DEFAULTCOLOR
	);

	HICON hMyIcon = AfxGetApp()->LoadIcon(IDI_RIGHT);
	// 2. Get a reference to your button control
	CStatic* pPic = (CStatic*)GetDlgItem(IDC_STATIC); // Replace with your Button ID

	pPic->SetIcon(hMyIcon);

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
 	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	SetDlgItemText(m_Edit1.GetDlgCtrlID(), _T("画像ファイルを指定してください。"));

	// ピクチャーコントロールのサイズの保存
	m_Picture1.GetClientRect(m_Rect);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CDialogImage::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this; // Delete the dialog object allocated on the heap
}

// コマンド
HCURSOR CDialogImage::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDialogImage::OnPaint()
{
	if (IsIconic()){
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
	}else{
		CDialogEx::OnPaint();
	}
}

void CDialogImage::OnBnClickedFile()
{
	// CFileDialog TRUE for FileOpen, FALSE for FileSaveAs
	CFileDialog fDlg(true);
	// GetOFN() ファイル構造体
	fDlg.GetOFN().lpstrFilter = _T("jpgファイル(*.jpg)|*.jpg");

	// ダイアログ表示
	if (fDlg.DoModal() != IDOK) return;

	// フルパス取得
	CString csPath = fDlg.GetPathName();

	SetDlgItemText(m_Edit1.GetDlgCtrlID(), csPath);
	UpdateData();

	// イメージの表示
	OnBnClickedRead();
}

void CDialogImage::OnBnClickedRead()
{
	UpdateData(TRUE);
	CString strText;
	m_Edit1.GetWindowText(strText);
	if (strText.IsEmpty()) {
		MessageBox(_T("画像を選択してください。"), _T("エラー"), MB_OK | MB_ICONERROR);
		return;
	}

	CFileFind find;
	if (!find.FindFile(strText)){
		MessageBox(_T("画像ファイルが存在しません。"), _T("エラー"), MB_OK | MB_ICONERROR);
		return;
	}
	// 矩形
	UINT flags = SWP_NOZORDER | SWP_NOMOVE;

	m_Picture1.SetWindowPos(NULL, m_Rect.left, m_Rect.top, m_Rect.Width(), m_Rect.Height(), flags);

	// デバイスコンテキスト
	CDC* pDC = m_Picture1.GetDC();
	CDC cDC;
	// ビットマップ描画用のデバイスコンテキスト
	cDC.CreateCompatibleDC(pDC);

	// 画像
	CImage image;
	image.Load(strText);

	double ratio_width = (double)image.GetWidth() / (double)m_Rect.Width();
	double ratio_height = (double)image.GetHeight() / (double)m_Rect.Height();

	if (ratio_width > ratio_height){
		// 画像のプロポーションが矩形のプロポーションより横長　＞　矩形の幅を縮める
		m_Picture1.SetWindowPos(NULL, m_Rect.left, m_Rect.top, m_Rect.Width(), (int)(m_Rect.Height() / ratio_width * ratio_height), flags);		
	}else if (ratio_width < ratio_height){
		// 画像のプロポーションが矩形のプロポーションより縦長　＞　矩形の高さを縮める
		m_Picture1.SetWindowPos(NULL, m_Rect.left, m_Rect.top, (int)(m_Rect.Width() / ratio_height * ratio_width), m_Rect.Height(), flags);
	}

	// ビットマップ
	CBitmap* map;
	map = CBitmap::FromHandle(image);

	// ビットマップ描画用のデバイスコンテキストでビットマップを選択する???
	cDC.SelectObject(map);

	//伸縮すると画像が汚くなるので伸縮モードを指定
	//詳細はMSDN参照 
	pDC->SetStretchBltMode(STRETCH_HALFTONE);

	//ブラシのずれを防止するためSetBrushOrgExを呼び出す
	pDC->SetBrushOrg(0, 0);

	// 変形後の矩形のrectを取得
	CRect rect;
	m_Picture1.GetClientRect(&rect);

	// イメージを矩形に描画
	pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &cDC, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
	
	// 後片付け
	image.Destroy();
	map->DeleteObject();
	cDC.DeleteDC();
	ReleaseDC(pDC);
}

void CDialogImage::OnEnKillfocusFile()
{
	OnBnClickedRead();
}

void CDialogImage::OnEnChangeFile()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialogEx::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を関数し呼び出します。
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
}

void CDialogImage::OnBnClickedOk()
{
	OnBnClickedCancel();
}

void CDialogImage::OnBnClickedCancel()
{
	m_pParent->PostMessage(WM_IMAGE_DIALOG_CLOSED, (WPARAM)IDCANCEL, (LPARAM)this);
	DestroyWindow(); // This triggers the window destruction sequence
}

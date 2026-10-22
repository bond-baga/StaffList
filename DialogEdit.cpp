//
// CDialogEdit.cpp : 実装ファイル
//

#include "pch.h"
#include "StaffList.h"
#include "DialogEdit.h"

#include <stdexcept>
#include <iostream>
#include <string.h>
#include <libpq-fe.h>
#include <gdiplus.h>

// CDialogEdit ダイアログ

IMPLEMENT_DYNAMIC(CDialogEdit, CDialogEx)

// コンストラクタ
CDialogEdit::CDialogEdit(CWnd* pParent)
	: CDialogEx(IDD_DIALOG_EDIT, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_STAFF);
	m_pParent = pParent;
}

// デストラクション
CDialogEdit::~CDialogEdit()
{
	CDialogEx::~CDialogEx();
}

// メッセージマップ
BEGIN_MESSAGE_MAP(CDialogEdit, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogEdit::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogEdit::OnBnClickedCancel)
	ON_BN_CLICKED(IDDELETE, &CDialogEdit::OnBnClickedDelete)
	ON_BN_CLICKED(IDFILE, &CDialogEdit::OnBnClickedFile)
	ON_EN_CHANGE(IDC_EDIT1, &CDialogEdit::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_RIGHT, &CDialogEdit::OnBnClickedRight)
	ON_BN_CLICKED(IDC_LEFT, &CDialogEdit::OnBnClickedLeft)
END_MESSAGE_MAP()

// 仮想関数のオーバーライド
void CDialogEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	DDX_Control(pDX, IDC_EDIT3, m_Edit3);
	DDX_Control(pDX, IDC_EDIT4, m_Edit4);
	DDX_Control(pDX, IDC_PICTURE1, m_Picture1);
	DDX_Control(pDX, IDC_EDIT5, m_Edit5);

	DDX_Control(pDX, IDC_RIGHT, m_Button_Right);
	DDX_Control(pDX, IDC_LEFT, m_Button_Left);
}

BOOL CDialogEdit::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	m_Edit1.SetWindowText(_T(""));
	m_Edit2.SetWindowText(_T(""));
	m_Edit3.SetWindowText(_T(""));
	m_Edit4.SetWindowText(_T(""));

	m_Button_Right.SetIcon(IDI_RIGHT);
	m_Button_Left.SetIcon(IDI_LEFT);

	// ピクチャーコントロールのサイズの初期化
//	m_Rect = CRect(0, 0, 200, 200);

	if (m_Code.IsEmpty()) {
		// 新規
		GetDlgItem(IDCANCEL)->SetFocus();
		m_Button_Right.EnableWindow(FALSE);
		m_Button_Left.EnableWindow(FALSE);
		m_Picture1.ShowWindow(SW_HIDE);
		return FALSE;
	}
	else {
		// 変更	
		m_Edit1.EnableWindow(FALSE);
	}

	const char* conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=root";
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		// Handle error: PQerrorMessage(conn)
		CString strError;
		strError.Format(_T("Connection error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		return FALSE;
	}
	CStringA strSql = "SELECT a.code, a.name, a.kana, a.statecode, a.statuscode, a.divisionname, a.remarks, b.image"
		" FROM members a"
		" LEFT JOIN images b"
		" ON a.code=b.code"
		" WHERE  a.code = $1";

	std::vector<CHAR> buf_code; thisApp()->LPTSTRToUtf8(m_Code, buf_code);
	std::vector<char*> paramValues = { buf_code.data() };
	PGresult* result = PQexecParams(conn,
		strSql,
		1,       // number of parameters
		nullptr,    // let the backend deduce param types
		paramValues.data(),
		nullptr,    // don't need param lengths
		nullptr,    // default to text
		0);      // result format 0 (text)
	ExecStatusType status = PQresultStatus(result);
	if (status == PGRES_TUPLES_OK) {
		int ret = 0;
		if (status != NULL && status == PGRES_TUPLES_OK) {
			std::vector<TCHAR> buf_code; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, 0, ITEMS::CODE), buf_code);
			std::vector<TCHAR> buf_name; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, 0, ITEMS::NAME), buf_name);
			std::vector<TCHAR> buf_kana; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, 0, ITEMS::KANA), buf_kana);
			std::vector<TCHAR> buf_divisionname; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, 0, ITEMS::DIVISIONNAME), buf_divisionname);

			m_Edit1.SetWindowText(buf_code.data());
			m_Edit2.SetWindowText(buf_name.data());
			m_Edit3.SetWindowText(buf_kana.data());
			m_Edit4.SetWindowText(buf_divisionname.data());

			// 画像
			m_Picture1.ModifyStyle(0, SS_BITMAP);

			if (!PQgetisnull(result, 0, ITEMS::IMAGE)) {
				CString strTmp;
				size_t bufferSize = 0;
				// rawBuffer is allocated with malloc()
				unsigned char* pBuffer = PQunescapeBytea((unsigned char*)PQgetvalue(result, 0, ITEMS::IMAGE), &bufferSize);
				if (bufferSize > 0){
					void* pData = ::malloc(bufferSize);
					if (pData) {
						::memcpy(pData, pBuffer, bufferSize);
						IStream* pStream = SHCreateMemStream(reinterpret_cast<const BYTE*>(pData), bufferSize);
						// reinterpret_cast
						//IStream* pStream = SHCreateMemStream((const BYTE*)(pData), bufferSize);
						free(pData);
						if (pStream) {
							CImage image;
							//	HRESULT hr = image.Load(pStream);
							if (SUCCEEDED(image.Load(pStream))){
								HBITMAP hOldBmp = m_Picture1.SetBitmap(image.Detach());
								if (hOldBmp)
									::DeleteObject(hOldBmp); // Clean up previous bitmap if any
							}
							pStream->Release();
						}
					}
				}
			}
		}
	}
	if (m_Picture1.GetBitmap() != NULL){
		m_Button_Right.EnableWindow(TRUE);	
		m_Button_Left.EnableWindow(TRUE);
		m_Picture1.ShowWindow(SW_SHOW);
	}else {
		m_Button_Right.EnableWindow(FALSE);
		m_Button_Left.EnableWindow(FALSE);
		m_Picture1.ShowWindow(SW_HIDE);
	}

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// コマンド
void CDialogEdit::OnBnClickedOk()
{
	CString strItem;
	// 入力チェック
	if (m_Code.IsEmpty()) {
		m_Edit1.GetWindowText(strItem);
		if (strItem.IsEmpty()) {
			MessageBox(_T("コードが未入力です。"), _T("エラー"), MB_OK | MB_ICONERROR);
			return;
		}
		int ret = IsDuplicated(strItem);
		if (ret == RETURN::YES) {
			MessageBox(_T("そのコードは入力済みです。"), _T("エラー"), MB_OK | MB_ICONERROR);
			return;
		}else if(ret == RETURN::ERR){
			return;
		}
	}

	m_Edit2.GetWindowText(strItem);
	if (strItem.IsEmpty()) {
		MessageBox(_T("氏名が未入力です。"), _T("エラー"), MB_OK | MB_ICONERROR);
		return;
	}

	CString strText;
	m_Edit1.GetWindowText(strText);
	std::vector<CHAR> buf_code; thisApp()->LPTSTRToUtf8(strText, buf_code);
	m_Edit2.GetWindowText(strText);
	 std::vector<CHAR> buf_name; thisApp()->LPTSTRToUtf8(strText, buf_name);
	m_Edit3.GetWindowText(strText);
	std::vector<CHAR> buf_kana; thisApp()->LPTSTRToUtf8(strText, buf_kana);
	m_Edit4.GetWindowText(strText);
	std::vector<CHAR> buf_divisionname; thisApp()->LPTSTRToUtf8(strText, buf_divisionname);

	const char* conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=root";
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		// Handle error: PQerrorMessage(conn)
		CString strError;
		strError.Format(_T("Connection error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		return;
	}

	CStringA strSql;
	if (m_Code.IsEmpty()) {
		strSql = "INSERT INTO members (code,name,kana,divisionname) VALUES($1,$2,$3,$4)";
	}
	else {
		strSql = "UPDATE members SET name=$2,kana=$3,divisionname=$4 WHERE code=$1";
	}

	std::vector<char*> paramValues = { buf_code.data(), buf_name.data(), buf_kana.data(), buf_divisionname.data() };
	PGresult* result = PQexecParams(conn,
		strSql,
		4,       // number of parameters
		nullptr,    // let the backend deduce param types
		paramValues.data(),
		nullptr,    // don't need param lengths
		nullptr,    // default to text
		0);      // result format 0 (text)
	ExecStatusType status = PQresultStatus(result);
	if (status == NULL || status != PGRES_COMMAND_OK){
		CString strError;
		strError.Format(_T("Update error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		PQclear(result);
		return;
	}

	// 写真
	strSql = "DELETE FROM images WHERE code=$1";
	paramValues.clear();
	paramValues = { buf_code.data() };
	result = PQexecParams(conn,
		strSql,
		1,       // number of parameters
		nullptr,    // let the backend deduce param types
		paramValues.data(),
		nullptr,    // don't need param lengths
		nullptr,    // default to text
		0);      // result format 0 (text)
	status = PQresultStatus(result);
	if (status == NULL || status != PGRES_COMMAND_OK) {
		// 画像データがない場合
	}

	// ピクチャーコントロールの画像のハンドルを取得
	HBITMAP hBitmap = m_Picture1.GetBitmap();
	if (hBitmap) {
		// ピクチャーコントロールのハンドルをCImageオブジェクトにアタッチする
		CImage image;
		image.Attach(hBitmap);
		std::vector<BYTE> imageBytes;

		// Imageをバイト配列に変換
		if (CImageToByteArray(image, imageBytes, Gdiplus::ImageFormatJPEG)) {
			int bufferSize = imageBytes.size();
			std::vector<CHAR> buf_type; thisApp()->LPTSTRToUtf8(_T("Bitmap"), buf_type);
			strSql = "INSERT INTO images (code, image, contenttype) VALUES ($1,$2,$3)";
			paramValues.clear();
			paramValues = { buf_code.data(), (char*)imageBytes.data(), buf_type.data() };
			std::vector<int> paramLengths = { 0, bufferSize, 0 };
			// Text = 0, BYTE* = 1
			std::vector<int> paramFormats = { 0,1,0 };
			result = PQexecParams(conn,
				strSql,
				3,						// number of parameters
				nullptr,				// let the backend deduce param types
				paramValues.data(),
				paramLengths.data(),    // don't need param lengths
				paramFormats.data(),    // default to text 
				0);						// result format 0 (text)
			status = PQresultStatus(result);
			if (status == NULL || status != PGRES_COMMAND_OK) {
				CString strError;
				strError.Format(_T("Insert error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
				AfxMessageBox(strError);
				PQfinish(conn);
				PQclear(result);
				return;
			}
		}
	}
	PQfinish(conn);
	CDialogEx::OnOK();
}

void CDialogEdit::OnBnClickedDelete()
{
	// 入力チェック
	if (m_Code.IsEmpty()) {
		MessageBox(_T("削除できません。"), _T("エラー"), MB_OK | MB_ICONERROR);
		return;
	}
	int ret = MessageBox(_T("削除すると元には戻せません。\r\n削除しますか保存しますか？"), _T("確認"), MB_YESNO | MB_ICONQUESTION);
	if (ret != IDYES) return;

	const char* conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=root";
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		// Handle error: PQerrorMessage(conn)
		CString strError;
		strError.Format(_T("Connection error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		return;
	}

	CStringA strSql = "DELETE FROM members WHERE code = $1";
	CStringA strSql2 = "DELETE FROM images WHERE code = $1";

	CString strText;
	m_Edit1.GetWindowText(strText);
	std::vector<CHAR> buf_code; thisApp()->LPTSTRToUtf8(strText, buf_code);

	std::vector<char*> paramValues = { buf_code.data() };
	PGresult* result = PQexecParams(conn,
		strSql,
		1,       // number of parameters
		nullptr,    // let the backend deduce param types
		paramValues.data(),
		nullptr,    // don't need param lengths
		nullptr,    // default to text
		0);      // result format 0 (text)
	ExecStatusType status = PQresultStatus(result);
	if (status == NULL || status != PGRES_COMMAND_OK) {
		// Handle error: PQerrorMessage(conn)
		CString strError;
		strError.Format(_T("Connection error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		return;
	}

	PGresult* result2 = PQexecParams(conn,
		strSql2,
		1,       // number of parameters
		nullptr,    // let the backend deduce param types
		paramValues.data(),
		nullptr,    // don't need param lengths
		nullptr,    // default to text
	 1);      // result format 1 (binary)
	status = PQresultStatus(result2);
	if (status == NULL || status != PGRES_COMMAND_OK) {
		// Handle error: PQerrorMessage(conn)
		CString strError;
		strError.Format(_T("Connection error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		return;
	}			
	PQfinish(conn);
	CDialogEx::OnOK();
}

void CDialogEdit::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void CDialogEdit::OnBnClickedFile()
{
	// CFileDialog TRUE for FileOpen, FALSE for FileSaveAs
	CFileDialog fDlg(true);
	// GetOFN() ファイル構造体
	fDlg.GetOFN().lpstrFilter = _T("jpgファイル(*.jpg)|*.jpg");

	// ダイアログ表示
	INT_PTR result = fDlg.DoModal();
	if (result != IDOK) return;

	// フルパス取得
	CString csPath = fDlg.GetPathName();

	SetDlgItemText(m_Edit5.GetDlgCtrlID(), csPath);
	UpdateData(TRUE);

	CString strFile;
	m_Edit5.GetWindowText(strFile);
	if (strFile == _T("")) {
		MessageBox(_T("画像を選択してください。"), _T("エラー"), MB_OK | MB_ICONERROR);
		return;
	}

	CFileFind find;
	if (!find.FindFile(strFile)) {
		MessageBox(_T("画像ファイルが存在しません。"), _T("エラー"), MB_OK | MB_ICONERROR);
		return;
	}

	// イメージの表示
	OnPictureDraw(strFile);
	if (m_Picture1.GetBitmap() != NULL) {
		m_Button_Right.EnableWindow(TRUE);
		m_Button_Left.EnableWindow(TRUE);
		m_Picture1.ShowWindow(SW_SHOW);
	}
	else {
		m_Button_Right.EnableWindow(FALSE);
		m_Button_Left.EnableWindow(FALSE);
		m_Picture1.ShowWindow(SW_HIDE);
	}
}

void CDialogEdit::OnEnChangeEdit1()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialogEx::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を関数し呼び出します。
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
}

void CDialogEdit::OnBnClickedRight()
{
	if (m_Picture1.GetSafeHwnd() != nullptr){
		// Retrieve the handle to the bitmap currently in the picture control
		HBITMAP hBmp = m_Picture1.GetBitmap();
		if (hBmp != nullptr){
			CImage image;
			image.Attach(hBmp);
			CImage rotatedImage;
			RotateImage90Clockwise(image, rotatedImage);
			HBITMAP hOldBitmap = m_Picture1.SetBitmap(rotatedImage.Detach());

			if (hOldBitmap) {
				::DeleteObject(hOldBitmap);
			}
			image.Destroy();
		}
	}
}

void CDialogEdit::OnBnClickedLeft()
{
	if (m_Picture1.GetSafeHwnd() != nullptr){
		// Retrieve the handle to the bitmap currently in the picture control
		HBITMAP hBmp = m_Picture1.GetBitmap();
		if (hBmp != nullptr){
			CImage image;
			image.Attach(hBmp);
			CImage rotatedImage;
			RotateImageCounter90Clockwise(image, rotatedImage);
			HBITMAP hOldBitmap = m_Picture1.SetBitmap(rotatedImage.Detach());

			if (hOldBitmap) {
				::DeleteObject(hOldBitmap);
			}
			image.Destroy();
		}
	}
}

// ユーザー定義関数
int CDialogEdit::IsDuplicated(CString code)
{
	const char* conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=root";
	PGconn* conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		// Handle error: PQerrorMessage(conn)
		CString strError;
		strError.Format(_T("Connection error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		return RETURN::ERR;
	}
	CString strText;
	CStringA strSql = "SELECT * FROM members WHERE code = $1";
#ifdef _UNICODE
	CT2A utf8Str_code(code, CP_UTF8);
#else
	CA2W utf16Str(code);
	CW2A utf8Str_code(utf16Str, CP_UTF8);
#endif
	std::vector<char*> paramValues = { utf8Str_code };
	PGresult* result = PQexecParams(conn,
		strSql,
		1,       // number of parameters
		nullptr,    // let the backend deduce param types
		paramValues.data(),
		nullptr,    // don't need param lengths
		nullptr,    // default to text
		0);      // result format 0 (text)

	// 1. Check if the result object is NULL(connection / memory error)
	ExecStatusType status = PQresultStatus(result);
	if (status == NULL) {
		CString strError;
		strError.Format(_T("Fatal error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		return RETURN::ERR;
	}

	// 2. Check if the query itself failed (e.g., syntax, constraint violation)
	if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
		CString strError;
		strError.Format(_T("SQL Error: %s"), (LPCTSTR)CString(PQresultErrorMessage(result)));
		AfxMessageBox(strError);
		PQclear(result);
		return RETURN::ERR;
	}
	if (PQntuples(result) > 0) {
		return RETURN::YES;
	}
	else {
		return RETURN::NO;
	}
}

void CDialogEdit::OnPictureDraw(CString strFile)
{
	// デバイスコンテキスト
	CDC* pDC = m_Picture1.GetDC();

	// イメージを取得
	CImage image;
	image.Load(strFile);

	double ratio_width = (double)image.GetWidth() / 200;
	double ratio_height = (double)image.GetHeight() / 200;

	CRect rect(0, 0, 200, 200);
	if (ratio_width > ratio_height) {
		// 画像のプロポーションが矩形のプロポーションより横長　＞　矩形の高さを縮める
		rect = CRect(0, 0, 200, (int)(200 / ratio_width * ratio_height));
	}else if (ratio_width < ratio_height) {
		// 画像のプロポーションが矩形のプロポーションより縦長　＞　矩形の幅を縮める
		rect = CRect(0, 0, (int)(200 / ratio_height * ratio_width), 200);
	}

	//伸縮すると画像が汚くなるので伸縮モードを指定
	pDC->SetStretchBltMode(STRETCH_HALFTONE);

	//ブラシのずれを防止するためSetBrushOrgExを呼び出す
//	pDC->SetBrushOrg(0, 0);

	// 矩形の更新。画像が途切れる場合があるから。
//	m_Picture1.UpdateWindow();

	// 1. Create the new CImage object with desired dimensions
	// Ensure the bit depth (BPP) is compatible, e.g., 24 or 32 bpp for color images
	CImage resizedImage;
	resizedImage.Create(rect.Width(), rect.Height(), image.GetBPP());

	// 2. Get the Device Context (DC) of the new image
	HDC hdcDest = resizedImage.GetDC();

	// Optional: Improve the quality of the stretch operation
	// HALFTONE mode often provides better results than the default
	int nOldMode = ::SetStretchBltMode(hdcDest, HALFTONE);
	::SetBrushOrgEx(hdcDest, 0, 0, nullptr);

	// 3. Use StretchBlt to draw the original image onto the new one, resizing in the process
	image.StretchBlt(
		hdcDest,               // Destination HDC
		0, 0,                  // Destination top-left corner
		rect.Width(), rect.Height(),   // Destination width and height
		0, 0,                  // Source top-left corner
		image.GetWidth(),  // Source width
		image.GetHeight(), // Source height
		SRCCOPY                // Raster operation code (copy source to destination)
	);

	// Cleanup
	::SetStretchBltMode(hdcDest, nOldMode);
	// 4. Release the DC of the new image
	resizedImage.ReleaseDC();
	
	HBITMAP hOldBitmap = m_Picture1.SetBitmap(resizedImage.Detach());

	if (hOldBitmap) {
		::DeleteObject(hOldBitmap);
	}
	image.Destroy();
	m_Picture1.ReleaseDC(pDC);
}

BOOL CDialogEdit::CImageToByteArray(CImage& image, std::vector<BYTE>& bytes, const GUID& format)
{
	IStream* pStream = nullptr;
	HRESULT hr = ::CreateStreamOnHGlobal(nullptr, TRUE, &pStream);
	if (SUCCEEDED(hr)) {
		// ImageFormatBMP 圧縮されていないビットマップ画像。
		hr = image.Save(pStream, format); // Save as specified format (e.g., ImageFormatPNG)
		if (SUCCEEDED(hr)) {
			HGLOBAL hGlobal = nullptr;
			hr = ::GetHGlobalFromStream(pStream, &hGlobal);
			if (hr == S_OK && hGlobal) {
				SIZE_T size = ::GlobalSize(hGlobal);
				void* pData = ::GlobalLock(hGlobal);
				if (pData) {
					bytes.resize(size);
					::memcpy(bytes.data(), pData, size);
					GlobalUnlock(hGlobal);
				}
			}
		}
		pStream->Release();
	}
	return SUCCEEDED(hr);
}

void CDialogEdit::RotateImage90Clockwise(CImage& srcImage, CImage& destImage)
{
	int width = srcImage.GetWidth();
	int height = srcImage.GetHeight();
	int bpp = srcImage.GetBPP(); // Bits per pixel (usually 24 or 32)

	// 1. Create destination image with swapped dimensions
	destImage.Create(height, width, bpp);

	// If the image uses a color table (like 8-bit images), copy it over
	if (srcImage.IsIndexed()) {
		int maxColors = srcImage.GetMaxColorTableEntries();
		if (maxColors > 0) {
			std::vector<RGBQUAD> colors(maxColors);
			srcImage.GetColorTable(0, maxColors, colors.data());
			destImage.SetColorTable(0, maxColors, colors.data());
		}
	}

	// 2. Loop through pixels and remap them
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// Calculate new coordinates for 90-degree clockwise rotation
			int newX = height - 1 - y;
			int newY = x;

			COLORREF pixelColor = srcImage.GetPixel(x, y);
			destImage.SetPixel(newX, newY, pixelColor);
		}
	}
}

void CDialogEdit::RotateImageCounter90Clockwise(CImage& srcImage, CImage& destImage)
{
	int width = srcImage.GetWidth();
	int height = srcImage.GetHeight();
	int bpp = srcImage.GetBPP(); // Bits per pixel (usually 24 or 32)

	// 1. Create destination image with swapped dimensions
	destImage.Create(height, width, bpp);

	// If the image uses a color table (like 8-bit images), copy it over
	if (srcImage.IsIndexed()) {
		RGBQUAD colors[256];
		int maxColors = srcImage.GetMaxColorTableEntries();
		srcImage.GetColorTable(0, maxColors, colors);
		destImage.SetColorTable(0, maxColors, colors);
	}

	// 2. Loop through pixels and remap them
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// Calculate new coordinates for 90-degree clockwise rotation
			int newX = y;
			int newY = width - 1 - x;

			COLORREF pixelColor = srcImage.GetPixel(x, y);
			destImage.SetPixel(newX, newY, pixelColor);
		}
	}
}


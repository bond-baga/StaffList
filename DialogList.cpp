//
// CDialogList.cpp : 実装ファイル
//

#include "pch.h"
#include "StaffList.h"
#include "DialogList.h"
#include "DialogEdit.h"

#include <libpq-fe.h>
#include <string>
#include <string_view>
#include <atlconv.h> // Required for conversion macros
#include <iostream>
// CDialogList ダイアログ

IMPLEMENT_DYNAMIC(CDialogList, CDialogEx)

// コンストラクタ
CDialogList::CDialogList(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LIST, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_STAFF);
	m_pCDialogEdit = nullptr;
}

// デストラクション
CDialogList::~CDialogList()
{
	CDialogEx::~CDialogEx();
}

// メッセージマップ
BEGIN_MESSAGE_MAP(CDialogList, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogList::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDialogList::OnBnClickedCancel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDialogList::OnNMDblclkList1)
	ON_BN_CLICKED(IDNEW, &CDialogList::OnBnClickedNew)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CDialogList::OnCustomdrawList1)
END_MESSAGE_MAP()

// 仮想関数のオーバーライド
void CDialogList::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List1);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
}

BOOL CDialogList::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	CRect rct;
	m_List1.GetClientRect(&rct);
	m_List1.InsertColumn(0, _T("コード"), LVCFMT_LEFT, (int)(rct.Width() * 0.10), -1);
	m_List1.InsertColumn(1, _T("氏名"), LVCFMT_LEFT, (int)(rct.Width() * 0.20), -1);
	m_List1.InsertColumn(2, _T("カナ"), LVCFMT_LEFT, (int)(rct.Width() * 0.20), -1);
	m_List1.InsertColumn(3, _T("所属"), LVCFMT_LEFT, (int)(rct.Width() * 0.40), -1);
	m_List1.InsertColumn(4, _T("写真"), LVCFMT_CENTER, (int)(rct.Width() * 0.10), -1);


	// Change the position of columns header from the left to the right. 
	m_List1.SetExtendedStyle(m_List1.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);

	m_Edit1.SetWindowText(_T(""));
	m_Edit2.SetWindowText(_T(""));

	/*
	追加するビットマップのピクセル（Cx,CY）を指定する。ビットマップはこのサイズで無い場合イメージリストに追加されない。
	32 × 32 ピクセル: Windowsのデスクトップ等で使われる標準的な小さいアイコン。
	16 × 16の場合はでも動作OK。画像が32 × 32 ピクセルの場合は縮小される。
	*/
	m_ImageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 0, 1);
	// 32 × 32または16 × 16ピクセルの場合でも「LVSIL_SMALL」
	m_List1.SetImageList(&m_ImageList, LVSIL_SMALL);

	LVCOLUMN lvc = { 0 };
	lvc.mask = LVCF_FMT;

	for (int i = ITEMS::CODE; i < ITEMS::IMAGE; i++) {
		m_List1.GetColumn(2, &lvc);
		lvc.fmt &= ~LVCFMT_JUSTIFYMASK; // Clear any existing layout rules safely
		//lvc.fmt |= LVCFMT_LEFT | LVCFMT_HEADER_CENTER;
		lvc.fmt |= LVCFMT_LEFT | LVCFMT_CENTER;
		m_List1.SetColumn(2, &lvc);
	}
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CDialogList::PostNcDestroy()
{
	CDialog::PostNcDestroy();
//	delete this; // Delete the dialog object allocated on the heap
}

// コマンド
void CDialogList::OnBnClickedOk()
{
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

	// place holderに「％」は記述できない
	CStringA strSql = "SELECT a.code, a.name, a.kana, a.statecode, a.statuscode, a.divisionname, a.remarks, b.image"
		" FROM members a"
		" LEFT JOIN images b"
		" ON a.code=b.code"
		" WHERE a.name like $1"
		" AND a.kana like $2"
		" ORDER BY a.code";
	CString strText;

	// SQLの書式文字列に「％」を追加できないので文字列の前に「％」を追加する。「'」は不要。
	m_Edit1.GetWindowText(strText);
	strText = _T("%") + strText + _T("%");
	std::vector<CHAR> buf_name; thisApp()->LPTSTRToUtf8(strText, buf_name);

	m_Edit2.GetWindowText(strText);
	strText = _T("%") + strText + _T("%");
	std::vector<CHAR> buf_kana; thisApp()->LPTSTRToUtf8(strText, buf_kana);

/*
#ifdef _UNICODE
	CT2A utf8Str_name(strText, CP_UTF8);
#else
	CA2W utf16Str(strText);
	CW2A utf8Str_name(utf16Str, CP_UTF8);
#endif
*/
	std::vector<char*> paramValues = { buf_name.data(), buf_kana.data() };
	PGresult* result = PQexecParams(conn,
		strSql,
		2,       // number of parameters
		nullptr,    // let the backend deduce param types
		paramValues.data(),
		nullptr,    // don't need param lengths
		nullptr,    // default to text
		0);      // result format 0 (text)
	ExecStatusType status = PQresultStatus(result);
	if (status == NULL || status != PGRES_TUPLES_OK) {
		CString strError;
		strError.Format(_T("Select  error: %s"), (LPCTSTR)CString(PQerrorMessage(conn)));
		AfxMessageBox(strError);
		PQfinish(conn);
		return;
	}

	// リストのクリアー
	m_List1.DeleteAllItems();
	int ret = 0;

	status = PQresultStatus(result);
	if (status == PGRES_TUPLES_OK) {
		for (int i = 0; i < PQntuples(result); i++) {
			std::vector<TCHAR> buf_code; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, i, ITEMS::CODE), buf_code);
			std::vector<TCHAR> buf_name; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, i, ITEMS::NAME), buf_name);
			std::vector<TCHAR> buf_kana; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, i, ITEMS::KANA), buf_kana);
			std::vector<TCHAR> buf_divisionname; thisApp()->Utf8ToLPTSTR(PQgetvalue(result, i, ITEMS::DIVISIONNAME), buf_divisionname);

			// 先頭列を指定して行を挿入。その他の項目はSetItemTextで設定する。
			LVITEM lvItem = { 0 };
			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
			lvItem.iItem = i;			// Row index
			lvItem.iSubItem = 0;		// Column index
			lvItem.pszText = buf_code.data();// code
			lvItem.iImage = I_IMAGENONE;			// No image for the first column(-1 or I_IMAGENONE)
			m_List1.InsertItem(&lvItem);
			// 以下ではMultibyte文字コード環境で変換できない
//			m_List1.SetItemText(i, 1, CA2T(PQgetvalue(result, i, ITEMS::NAME), CP_UTF8)); // name
			m_List1.SetItemText(i, 1, buf_name.data()); // name
			m_List1.SetItemText(i, 2, buf_kana.data()); // kana
			m_List1.SetItemText(i, 3, buf_divisionname.data()); // divisionname
			if (!PQgetisnull(result, i, ITEMS::IMAGE)) {
				CString strTmp;
				size_t binaryDataLength = 0;
				// rawBuffer is allocated with malloc()
				unsigned char* rawBuffer = PQunescapeBytea((unsigned char*)PQgetvalue(result, i, ITEMS::IMAGE), &binaryDataLength);

				CRect rect = CRect(0, 0, 32, 32);
				CDC* pScreenDC = m_List1.GetDC(); 

				// ビットマップをスタックに作成
				CBitmap bitmapMerg;
				bitmapMerg.CreateCompatibleBitmap(pScreenDC, rect.Width(), rect.Height());
				SetImage(rawBuffer, binaryDataLength, pScreenDC, bitmapMerg);
				// ビットマップのメモリを解放
				PQfreemem(rawBuffer);
				// マージされたビットマップをイメージリストに追加
				int nIndex = m_ImageList.Add(&bitmapMerg, RGB(255, 0, 255)); // RGB(255,0,255) is the transparent mask color

				// リストコントロールにイメージリストのインデックスを指定してビットマップを表示する
				LV_ITEM lvItem{ 0 };
				lvItem.mask = LVIF_IMAGE;
				lvItem.stateMask = LVIS_STATEIMAGEMASK;
				lvItem.iItem = i;			// row
				lvItem.iSubItem = 4;		// column
				lvItem.iImage = nIndex;		// Index of the bitmap in m_imageList
				m_List1.SetItem(&lvItem);
				m_List1.ReleaseDC(pScreenDC);//::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());

			}
		}

		char szBuff[1024];
		snprintf(szBuff, 1024, "%d件抽出しました。", PQntuples(result));
		::MessageBox(nullptr, (CA2T)szBuff, _T("成功"), MB_OK | MB_ICONINFORMATION);
	}
	else {
		char szBuff[1024];
		snprintf(szBuff, 1024, "Error: %s", PQerrorMessage(conn));
		::MessageBox(nullptr, (CA2T)szBuff, _T("エラー"), MB_OK | MB_ICONERROR);
	}
	PQfinish(conn);
}

void CDialogList::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// This works if LVS_EX_FULLROWSELECT is set and you click the first column/label area
	int nRow = pNMItemActivate->iItem;

	CString strCode = m_List1.GetItemText(nRow, 0); // Get text from the first column (subitem 0)
	CString subItem1Text = m_List1.GetItemText(nRow, 1);

	CDialogEdit dlg(this);
	dlg.m_Code = strCode;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) OnBnClickedOk();
	*pResult = 0;
}

void CDialogList::OnBnClickedNew()
{
	CDialogEdit dlg(this);
	dlg.m_Code = _T("");
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) OnBnClickedOk();
}

void CDialogList::OnBnClickedCancel()
{
	DestroyWindow(); // This triggers the window destruction sequence
}

void CDialogList::OnCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW; // Request item draw alerts
		break;

	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW; // Request subitem alerts
		break;

		// Use the explicit bitwise combination instead of the undefined macro
	case (CDDS_ITEMPREPAINT | CDDS_SUBITEM):
		// If drawing any column except the header
		// Force the text alignment format to Left for the row items
		//pLVCD->dwItemType = LVCFMT_LEFT;
		//*pResult = CDRF_DODEFAULT;

		{
			int nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec);
			int nSubItem = pLVCD->iSubItem;
			// We only need to fix the first column where the image space is reserved
			if (nSubItem == 0){
				LVITEM lvItem = { 0 };
				lvItem.mask = LVIF_IMAGE;
				lvItem.iItem = nItem;
				m_List1.GetItem(&lvItem);
				// Check if this row is configured to have NO image
				if (lvItem.iImage == -1 || lvItem.iImage == I_IMAGENONE){
					CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
					CString strText = m_List1.GetItemText(nItem, nSubItem);
					// Get the bounding rectangle for the entire subitem area
					CRect rect;
					// ↓the operating system incorrectly returns the bounding box for the entire row (all columns combined).
					//m_List1.GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
					//// 1. Use LVIR_LABEL for the first column instead of LVIR_BOUNDS
					m_List1.GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rect);

					// 2. Adjust the left boundary back to the absolute left edge of the column
					// (LVIR_LABEL usually starts where the text begins, but we want the absolute edge)
					rect.left = 0;

					// 3. Explicitly set the right boundary using the actual column width
					rect.right = rect.left + m_List1.GetColumnWidth(nSubItem);

					// Now 'rect' has the perfect height and the exact width of column 0!
					rect.left += 6;
					
					// Add a tiny margin (e.g., 6 pixels) so text doesn't glue to the left border
//					rect.left += 6;
					// Set text color matching the current state (selected vs normal)
					COLORREF clrText = pLVCD->clrText;
/*
					if (pLVCD->nmcd.uItemState & CDIS_SELECTED) {
					//if (pLVCD->nmcd.uItemState & LVIS_SELECTED){
						// Fill background for selected items if needed, or use default background
						pDC->FillSolidRect(rect, GetSysColor(COLOR_HIGHLIGHT));
						clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
					}else{
//						pDC->FillSolidRect(rect, pLVCD->clrTextBk);
					}
					pDC->SetTextColor(clrText);
					pDC->SetBkMode(TRANSPARENT);
					// Draw the text shifted completely to the left edge of the column
					pDC->DrawText(strText, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
					// Tell Windows to SKIP default drawing for this subitem so it won't draw the empty icon space
*/

				// 行が選択されているかどうかを確実に取得
					UINT nState = m_List1.GetItemState(nItem, LVIS_SELECTED);
					BOOL bIsSelected = (nState & LVIS_SELECTED);

					// フォーカスがあるか（点線の枠を表示するか判定用）
					BOOL bHasFocus = (m_List1.GetItemState(nItem, LVIS_FOCUSED) & LVIS_FOCUSED) &&
						(::GetFocus() == m_List1.GetSafeHwnd());

					if (bIsSelected)
					{
						// 選択時のハイライト背景色と文字色を適用
						pDC->FillSolidRect(rect, GetSysColor(COLOR_HIGHLIGHT));
						clrText = GetSysColor(COLOR_HIGHLIGHTTEXT);
					}
					else
					{
						// 通常時の背景色
						//pDC->FillSolidRect(rect, pLVCD->clrTextBk);
						// 通常時の背景色（コメントアウトのままでもOKですが、明示的に白/ウィンドウ色にする場合）
						pDC->FillSolidRect(rect, GetSysColor(COLOR_WINDOW));
					}

					pDC->SetTextColor(clrText);
					pDC->SetBkMode(TRANSPARENT);

					// テキストの描画
					pDC->DrawText(strText, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					// もしフォーカス枠（点線）も綺麗に描画したい場合は、以下を追加します
					if (bHasFocus)
					{
						pDC->DrawFocusRect(rect);
					}


					*pResult = CDRF_SKIPDEFAULT;
					//return;
				}
			}
			break;
		}
	}
}
// メッセージ

// ユーザー定義
void CDialogList::SetImage(unsigned char* pBuffer, size_t bufferSize, CDC* pScreenDC, CBitmap& bitmapMerg)
{
	void* pData = ::malloc(bufferSize);
	if (pData){
		::memcpy(pData, pBuffer, bufferSize);
		IStream* pStream = SHCreateMemStream(reinterpret_cast<const BYTE*>(pData), bufferSize);
		free(pData);
		if (pStream){
			// ビットマップ情報
			BITMAP bitmapInfo;
			bitmapMerg.GetBitmap(&bitmapInfo);
			// サイズ
			CRect rect = CRect(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight);
			// 伸縮した画像のサイズを計算するための矩形
			CRect rectFore;
			// イメージ
			CImage image;
			if (SUCCEEDED(image.Load(pStream))){
				double ratio_width = (double)image.GetWidth() / (double)bitmapInfo.bmWidth;
				double ratio_height = (double)image.GetHeight() / (double)bitmapInfo.bmHeight;
				if (ratio_width > ratio_height){
					// 画像のプロポーションが矩形のプロポーションより横長　＞　矩形の高さを縮める
					rectFore = CRect(0, 0, (bitmapInfo.bmWidth - 2), (bitmapInfo.bmHeight - 2) * ratio_height / ratio_width);
				}else if (ratio_width < ratio_height){
					// 画像のプロポーションが矩形のプロポーションより縦長　＞　矩形の幅を縮める
					rectFore = CRect(0, 0, (bitmapInfo.bmWidth - 2) * ratio_width / ratio_height, (bitmapInfo.bmHeight - 2));
				}
				// メモリCDC
				CDC memDCBack, memDCFore, memDCMerg;
				memDCBack.CreateCompatibleDC(pScreenDC);
				memDCFore.CreateCompatibleDC(pScreenDC);
				memDCMerg.CreateCompatibleDC(pScreenDC);
				// ビットマップ
				CBitmap bitmapBack, bitmapFore;
				bitmapBack.CreateCompatibleBitmap(pScreenDC, bitmapInfo.bmWidth, bitmapInfo.bmHeight);
				bitmapFore.CreateCompatibleBitmap(pScreenDC, rectFore.Width(), rectFore.Height());
				// メモリCDCでビットマップを選択する
				// Crucial: Make bitmap compatible with screen DC, NOT memDC!
				CBitmap* pOldBitmapBack = memDCBack.SelectObject(&bitmapBack);
				CBitmap* pOldBitmapFore = memDCFore.SelectObject(&bitmapFore);
				CBitmap* pOldBitmapMerg = memDCMerg.SelectObject(&bitmapMerg);
				::SetStretchBltMode(memDCFore.GetSafeHdc(), HALFTONE);
				// 背景のメモリCDCで背景を描画
				// ビットマップのサイズより大きいのを指定すると描画しない。小さいのはOK
				COLORREF color = RGB(255, 255, 255); // background
				CBrush brush(color);
				memDCBack.FillRect(&rect, &brush);
				brush.DeleteObject();
				// 画像のサイズを変更。変更後の画像は画像のメモリCDCが選択しているビットマップに格納される。
				image.StretchBlt(memDCFore.GetSafeHdc(), 0, 0, rectFore.Width(), rectFore.Height(), SRCCOPY);
				// マージ用メモリCDCに背景用CDCで描画
				memDCMerg.BitBlt(0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, &memDCBack, 0, 0, SRCCOPY);
				// マージ用のメモリCDCに画像用CDCで描画
				memDCMerg.BitBlt((bitmapInfo.bmWidth - rectFore.Width()) / 2, (bitmapInfo.bmHeight - rectFore.Height()) / 2, rectFore.Width(), rectFore.Height(), &memDCFore, 0, 0, SRCCOPY);
				// ビットマップをイメージリストに挿入する前にメモリCDCから解放する
				memDCBack.SelectObject(pOldBitmapBack);
				memDCFore.SelectObject(pOldBitmapFore);
				memDCMerg.SelectObject(pOldBitmapMerg);
				image.Destroy();
			}
			pStream->Release();
		}
	}
}

void CDialogList::ConvertIconToBitmap(CBitmap* bmpObj, HICON hIcon, int cx, int cy)
{
	// Ensure icon handle is valid
	ASSERT(hIcon != NULL);

	// Get the screen device context
	CClientDC screenDC(nullptr);

	// Create a memory device context (DC) compatible with the screen
	CDC dc;
	dc.CreateCompatibleDC(&screenDC);

	// Create a temporary CBitmap object
	CBitmap bmpTmp;
	// Create a compatible bitmap with the desired dimensions (cx, cy)
	VERIFY(bmpTmp.CreateCompatibleBitmap(&screenDC, cx, cy));

	// Select the temporary bitmap into the memory DC
	CBitmap* pOldBmp = dc.SelectObject(&bmpTmp);

	// Optional: Fill the background if needed. Otherwise, it might be black or have unexpected results.
	// dc.FillSolidRect(0, 0, cx, cy, RGB(255, 255, 255)); // Example: Fill with white

	// Draw the icon onto the memory DC (which is associated with the bitmap)
	VERIFY(::DrawIconEx(dc.GetSafeHdc(), 0, 0, hIcon, cx, cy, 0, nullptr, DI_NORMAL));

	// Restore the old bitmap to the DC
	dc.SelectObject(pOldBmp);

	// Attach the HBITMAP handle from the temporary CBitmap to the final CBitmap object
	// Detaching ensures the HBITMAP is not deleted when bmpTmp goes out of scope
	bmpObj->Attach(bmpTmp.Detach());
}

HICON CDialogList::CreateIconFromBMPBuffer(unsigned char* pBuffer, size_t bufferSize)
{
	if (bufferSize < sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
		return nullptr;

	// 1. Extract the file header and verify it is a valid BMP
	BITMAPFILEHEADER* bmfh = (BITMAPFILEHEADER*)pBuffer;
	if (bmfh->bfType != 0x4D42) return nullptr; // 0x4D42 is 'BM'

	// 2. Extract the info header and find the location of the pixel bits
	BITMAPINFOHEADER* bmih = (BITMAPINFOHEADER*)(pBuffer + sizeof(BITMAPFILEHEADER));
	void* pBits = (void*)(pBuffer + bmfh->bfOffBits);

	// 3. Create a compatible memory Device Context (DC)
	CDC* pDC = CDC::FromHandle(::GetDC(nullptr));
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	// 4. Create the Color Bitmap Handle
	HBITMAP hColorBmp = ::CreateDIBitmap(
		pDC->GetSafeHdc(),
		bmih,
		CBM_INIT,
		pBits,
		(BITMAPINFO*)bmih,
		DIB_RGB_COLORS
	);

	// 5. Create a monochrome Mask Bitmap Handle (required for an icon)
	// For 32-bit bitmaps with an alpha channel, a blank mask works perfectly.
	HBITMAP hMaskBmp = ::CreateCompatibleBitmap(memDC.GetSafeHdc(), bmih->biWidth, bmih->biHeight);

	// 6. Populate the ICONINFO structural properties
	ICONINFO iconInfo = { 0 };
	iconInfo.fIcon = TRUE; // Specifying an Icon (FALSE would be a Cursor)
	iconInfo.xHotspot = 0;
	iconInfo.yHotspot = 0;
	iconInfo.hbmMask = hMaskBmp;
	iconInfo.hbmColor = hColorBmp;

	// 7. Create the actual Windows icon handle
	HICON hIcon = ::CreateIconIndirect(&iconInfo);

	// Clean up temporary Win32 handles and GDI assets
	::DeleteObject(hColorBmp);
	::DeleteObject(hMaskBmp);
	::ReleaseDC(nullptr, pDC->GetSafeHdc());

	return hIcon;
}


#include "pch.h" 
#include "MyIconButton.h"

#include <vssym32.h>

CMyIconButton::CMyIconButton()
    : m_hIcon(nullptr)
{}

CMyIconButton::~CMyIconButton()
{
    // ロードしたアイコンの破棄
    if (m_hIcon != nullptr)
    {
        ::DestroyIcon(m_hIcon);
    }
}

// 外部（ダイアログなど）から任意のアイコンIDを指定してロードする関数
void CMyIconButton::SetIcon(UINT nIconID)
{
    if (m_hIcon != nullptr)
    {
        ::DestroyIcon(m_hIcon);
        m_hIcon = nullptr;
    }

    // インスタンスから指定されたリソースIDのアイコンをロード（透過対応）
    HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(nIconID), RT_GROUP_ICON);
    m_hIcon = (HICON)::LoadImage(hInst, MAKEINTRESOURCE(nIconID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);

    // 再描画を促す
    if (GetSafeHwnd())
    {
        Invalidate();
    }
}

// オーナー描画（ボタン描画の本体）
void CMyIconButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (lpDrawItemStruct == nullptr) return;

    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;

    // 1. ダイアログ（親ウィンドウ）の背景でボタンをクリアする（透過対策）
    // これにより、ボタン本来の「白い四角」を消し、ダイアログの背景と馴染ませます
    if (GetParent())
    {
        // 独自の背景（テーマ）が有効な環境に対応
        ::DrawThemeParentBackground(GetSafeHwnd(), pDC->GetSafeHdc(), &rect);
    }
    else
    {
        // フォールバック（テーマが無効な場合などの標準塗りつぶし）
        pDC->FillSolidRect(&rect, ::GetSysColor(COLOR_3DFACE));
    }

    // アイコンがない場合はここで終了
    if (m_hIcon == nullptr) return;

    // 元のアイコン（m_hIcon）を指定サイズでコピー作成
    HICON hResizedIcon = (HICON)::CopyImage(
        m_hIcon,
        IMAGE_ICON,
        rect.Width(),
        rect.Height(),
        LR_COPYFROMRESOURCE
    );

    int x = rect.left;
    int y = rect.top;

    // 4. ボタンの状態（押されているか、フォーカスがあるか等）に応じた演出
    if (state & ODS_SELECTED)
    {
        // ボタンが押されている時は、アイコンを右下に1ピクセルずらす（Windows標準の挙動を再現）
        x += 1;
        y += 1;
    }

    if (state & ODS_DISABLED)
    {
        // ボタンのサイズに合わせてアイコンを描画するために、DrawStateを使用して無効化されたアイコンを描画します。
        pDC->DrawState(
            CPoint(x, y),
            CSize(rect.Width(), rect.Height()),
            (HICON)hResizedIcon,
            DSS_DISABLED | DST_ICON,
            (HBRUSH)nullptr
        );
    }
    else
    {
        pDC->DrawState(
            CPoint(x, y),
            CSize(rect.Width(), rect.Height()),
            (HICON)hResizedIcon,
            DSS_NORMAL | DST_ICON,
            (HBRUSH)nullptr
        );
    }
    // 使用後は必ず破棄する
    ::DestroyIcon(hResizedIcon);
    // 6. フォーカス枠の描画（必要に応じて）
    if (state & ODS_FOCUS)
    {
        // 
        CRect focusRect = rect;
        // ここで DeflateRect(1, 1) を行うと、ボタンの本当の端から1ピクセル内側に描画されます
        focusRect.DeflateRect(1, 1);
        pDC->DrawFocusRect(&focusRect);
    }
}

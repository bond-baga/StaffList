#pragma once
#include <afxwin.h>
class CMyIconButton :
    public CButton
{
public:
    CMyIconButton();
    virtual ~CMyIconButton();

//    void DoDataExchange(CDataExchange* pDX);

    void LoadImage(const int resourceID);

    // アイコンを設定するための関数
    void SetIcon(UINT nIconID);

protected:
    HICON m_hIcon; // アイコンのハンドルを保持
//	HBITMAP m_hBitmap; // ビットマップのハンドルを保持

    // オーナー描画の核心となる関数をオーバーライド
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};


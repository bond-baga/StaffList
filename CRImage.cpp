//
// CRImage : 実装ファイル　
//
#include "pch.h"
#include "CRImage.h"

IMPLEMENT_DYNAMIC(CRImage, CRecordset)

// コンストラクタ
CRImage::CRImage(CDatabase* pDatabase /*=NULL*/)
    : CRecordset(pDatabase)
{
    m_code = _T("");
//    m_image.SetSize(0);
    m_contenttype = _T("");
}

// デフォルトコネクト
CString CRImage::GetDefaultConnect()
{
    // Return the DSN name you registered (e.g., "ODBC;DSN=StudentRegistration")
    return _T("ODBC;DSN=postgres");
}

// デフォルトSQL
CString CRImage::GetDefaultSQL()
{
    // Return the default SQL SELECT statement or table name
    return _T("image");
}

// RFX 
void CRImage::DoFieldExchange(CFieldExchange* pFX)
{
    CRecordset::DoFieldExchange(pFX);

    pFX->SetFieldType(CFieldExchange::outputColumn);
    RFX_Text(pFX, _T("[code]"), m_code);
//    nMaxLength 1GB
//    RFX_Binary(pFX, _T("[image]"), m_image,1000000);
    RFX_Text(pFX, _T("[contenttype]"), m_contenttype);
}

// Implement AssertValid() and Dump() for debug builds if needed
#ifdef _DEBUG
void CRImage::AssertValid() const
{
    CRecordset::AssertValid();
}

void CRImage::Dump(CDumpContext& dc) const
{
    CRecordset::Dump(dc);
}
#endif
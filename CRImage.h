//
// CRImage.h
//
#pragma once
#include "afxdb.h"

class CRImage :
    public CRecordset
{
public:
    CRImage(CDatabase* pDatabase = NULL);
    DECLARE_DYNAMIC(CRImage)

    CString m_code;
    CByteArray m_image;
    CString m_contenttype;

    // Overrides
protected:
    virtual CString GetDefaultConnect();                // Returns the default connection string
    virtual CString GetDefaultSQL();                    // Returns the default SQL statement
    virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
    //virtual void DoBulkFieldExchange(CFieldExchange* pFX);  // RFX support

    // Implementation
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

};


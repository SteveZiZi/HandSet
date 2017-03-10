// SCLEditorDoc.h : interface of the CSCLEditorDoc class
//


#pragma once


class CSCLEditorDoc : public CDocument
{
protected: // create from serialization only
	CSCLEditorDoc();
	DECLARE_DYNCREATE(CSCLEditorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CSCLEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

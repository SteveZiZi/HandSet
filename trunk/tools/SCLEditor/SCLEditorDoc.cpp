// SCLEditorDoc.cpp : implementation of the CSCLEditorDoc class
//

#include "stdafx.h"
#include "SCLEditor.h"

#include "SCLEditorDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSCLEditorDoc

IMPLEMENT_DYNCREATE(CSCLEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSCLEditorDoc, CDocument)
END_MESSAGE_MAP()


// CSCLEditorDoc construction/destruction

CSCLEditorDoc::CSCLEditorDoc()
{
	// TODO: add one-time construction code here

}

CSCLEditorDoc::~CSCLEditorDoc()
{
}

BOOL CSCLEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CSCLEditorDoc serialization

void CSCLEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CSCLEditorDoc diagnostics

#ifdef _DEBUG
void CSCLEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSCLEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSCLEditorDoc commands

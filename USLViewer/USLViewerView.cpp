
// USLViewerView.cpp : implementation of the CUSLViewerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "USLViewer.h"
#endif

#include "USLViewerDoc.h"
#include "USLViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUSLViewerView

IMPLEMENT_DYNCREATE(CUSLViewerView, CView)

BEGIN_MESSAGE_MAP(CUSLViewerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CUSLViewerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CUSLViewerView construction/destruction

CUSLViewerView::CUSLViewerView() noexcept
{
	// TODO: add construction code here

}

CUSLViewerView::~CUSLViewerView()
{
}

BOOL CUSLViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CUSLViewerView drawing

void CUSLViewerView::OnDraw(CDC* /*pDC*/)
{
	CUSLViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CUSLViewerView printing


void CUSLViewerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CUSLViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CUSLViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CUSLViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CUSLViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CUSLViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CUSLViewerView diagnostics

#ifdef _DEBUG
void CUSLViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CUSLViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUSLViewerDoc* CUSLViewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUSLViewerDoc)));
	return (CUSLViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CUSLViewerView message handlers

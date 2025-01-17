#include "pch.h"
#include "USLViewer.h"
#include "CAScanPane.h"
#include "..\USLData\Include\USLData.h"

BEGIN_MESSAGE_MAP(CAScanPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_MESSAGE(UI_INVALIDATE_ASCAN, &CAScanPane::OnUIInvalidateAScan)
END_MESSAGE_MAP()



int CAScanPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_staticView.Create(NULL, WS_CHILD | WS_VISIBLE | 7, rectDummy, this, ID_STATIC_VIEW))
	{
		TRACE0("Failed to create Static View in CScanPane \n");
		return -1;      // fail to create
	}

	AdjustLayout();

	return 0;
}

void CAScanPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CAScanPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL) return;

	CRect rrClient;
	GetClientRect(rrClient);

	int cyTlb = 0;

	m_staticView.SetWindowPos(NULL, rrClient.left, rrClient.top + cyTlb, rrClient.Width(), rrClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	Invalidate();
}


void CAScanPane::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDockablePane::OnPaint() for painting messages
	RenderTrace();
	
}

#define RF_MAX_VALUE		0x07ffffff		//134,217,727
#define RF_MIN_VALUE		0xf8000000		//-134,217,728

void CAScanPane::RenderTrace()
{
	CRect rr;
	CDC  dcMem;
	CBitmap bmp;
	CString Buff;

	m_staticView.GetWindowRect(rr);
	rr -= rr.TopLeft();

	CDC* pDC = m_staticView.GetDC();

	bmp.CreateCompatibleBitmap(pDC, rr.Width(), rr.Height());
	dcMem.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dcMem.SelectObject(&bmp);
	dcMem.FillRect(&rr, &CBrush(RGB(0, 0, 255)));

	CPen penWhite(PS_SOLID, 1, RGB(255, 255, 255));
	dcMem.SelectObject(&penWhite);

	dcMem.SetTextColor(RGB(200, 200, 200));
	dcMem.SetBkMode(TRANSPARENT);
	Buff.Format(L"Sample = %d Line = %d", m_nDataXSample, m_nDataYLine);
	dcMem.TextOutW(3, 3, Buff);

	if (m_pFile != NULL) {
		int nSize;
		USLgetAScanLength(m_pFile, &nSize);
		if (nSize > 0) {
			int* pnTrace = new int[nSize];

			USLgetAScan(m_pFile, m_nDataXSample, m_nDataYLine, pnTrace);

			int nHalfHeight = rr.Height() / 2;
			int nX = rr.left;
			int nY = rr.CenterPoint().y - ((pnTrace[0] * nHalfHeight) / 128);
			dcMem.MoveTo(nX, nY);
			for (int ii = 1; ii < nSize; ii++) {
				nX = (ii * rr.Width()) / nSize;
				nY = rr.CenterPoint().y - MulDiv64(pnTrace[ii], nHalfHeight, -RF_MAX_VALUE);
				dcMem.LineTo(nX, nY);
			}
			delete pnTrace;
		}
	}

	pDC->BitBlt(0,0, rr.Width(), rr.Height(), &dcMem,0,0,SRCCOPY);

	ReleaseDC(pDC);
}

LRESULT CAScanPane::OnUIInvalidateAScan(WPARAM wp, LPARAM lp)
{
	m_nDataXSample = (int)wp;
	m_nDataYLine = (int)lp;

	Invalidate();

	return 0;
}

int CAScanPane::MulDiv64(int nValue, int nNumerator, int nDenominator)
{
	if (nDenominator == 0) nDenominator = 1;
	return (int)(((__int64)nValue * (__int64)nNumerator) / (__int64)nDenominator);
}



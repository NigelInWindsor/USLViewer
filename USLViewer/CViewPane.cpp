#include "pch.h"
#include "USLViewer.h"
#include "CViewPane.h"
#include "..\USLData\Include\USLData.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CViewPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_1, &CViewPane::OnView1)
	ON_COMMAND(ID_VIEW_2, &CViewPane::OnView2)
	ON_COMMAND(ID_VIEW_3, &CViewPane::OnView3)
	ON_COMMAND(ID_VIEW_4, &CViewPane::OnView4)
	ON_COMMAND(ID_VIEW_5, &CViewPane::OnView5)
	ON_COMMAND(ID_VIEW_6, &CViewPane::OnView6)
	ON_COMMAND(ID_VIEW_7, &CViewPane::OnView7)
	ON_COMMAND(ID_VIEW_8, &CViewPane::OnView8)
	ON_COMMAND(ID_VIEW_9, &CViewPane::OnView9)
	ON_COMMAND(ID_VIEW_10, &CViewPane::OnView10)
END_MESSAGE_MAP()

void CViewPane::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);
	int nErrorCode;
	if (bShow == TRUE) {

	}
	else {
		USLDataClose(&m_pFile, &nErrorCode);
	}
}

int CViewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
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


bool CViewPane::Open(CString FilePathName)
{
	m_strFilePathName = FilePathName;

	switch(USLDataOpen(m_strFilePathName, &m_pFile)) {
	case USLSuccess: return true;
		break;
	case USLNoDongle: MessageBox(L"Unable to find dongle", L"Licence", MB_ICONERROR);
		break;
	case USLIncorrectDongle: MessageBox(L"Dongle doesnt correspond to licence", L"Licence", MB_ICONERROR);
		break;
	case USLNoLicence: MessageBox(L"No licence file in working directory", L"Licence", MB_ICONERROR);
		break;
	}

	return false;
}


void CViewPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CViewPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL) return;

	CRect rrClient;
	GetClientRect(rrClient);

	int cyTlb = 0;

	m_staticView.SetWindowPos(NULL, rrClient.left, rrClient.top + cyTlb, rrClient.Width(), rrClient.Height() - cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	Invalidate();
}


void CViewPane::OnPaint()
{
	CRect rr;
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDockablePane::OnPaint() for painting messages

	CDC* pDC = m_staticView.GetDC();
	m_staticView.GetWindowRect(rr);
	rr -= rr.TopLeft();
	pDC->FillRect(&rr, &CBrush(RGB(80, 80, 80)));
	ReleaseDC(pDC);

	InvalidaCScanTypes();

	switch (m_CScanTypes[m_nCScanIndex].nImageType) {
	case _AMPLITUDE:
		RenderAmplitudeCScanFromGreyScale();
		break;
	case _TOF:
		RenderTOFCScan();
		break;
	}
}


void CViewPane::RenderAmplitudeCScanFromGreyScale()
{
	USL_RETURN_CODE nRet = USLFailed;
	int nSamples;
	USLgetCScanSamplesPerLine(m_pFile, &nSamples);

	int nLines;
	USLgetCScanNumberOfLines(m_pFile, &nLines);
	unsigned int nSize = (unsigned int)(nSamples * nLines);

	if (nSize > 0) {
		COLORREF* pRGBArray = new COLORREF[nSize];

		nRet = USLgetAmplitudeCScan(m_pFile, m_CScanTypes[m_nCScanIndex].nTS, m_CScanTypes[m_nCScanIndex].nIndex, (VOID*)pRGBArray, nSize * (sizeof COLORREF), USL_AMP_MODE::AMP_GREY_SCALE);
		if (nRet == USLSuccess) {

			StrecthBlt(&m_staticView, nSamples, nLines, pRGBArray);

		}
		delete pRGBArray;
	}
}

void CViewPane::RenderAmplitudeCScanFromSigned()
{
	USL_RETURN_CODE nRet = USLFailed;
	int nSamples;
	USLgetCScanSamplesPerLine(m_pFile, &nSamples);
	int nLines;
	USLgetCScanNumberOfLines(m_pFile, &nLines);
	unsigned int nSize = (unsigned int)(nSamples * nLines);

	if (nSize > 0) {
		int* pnArray = new int[nSize];

		nRet = USLgetAmplitudeCScan(m_pFile, m_CScanTypes[m_nCScanIndex].nTS, m_CScanTypes[m_nCScanIndex].nIndex, (VOID*)pnArray, nSize * (sizeof (int)), USL_AMP_MODE::AMP_SIGNED, 32767);
		if (nRet == USLSuccess) {

			COLORREF* pScreen = new COLORREF[nSize];
			for (unsigned int nIndex = 0; nIndex < nSize; nIndex++) {
				int nColor = (pnArray[nIndex] * 255) / 32768;
				nColor = (nColor << 16) | (nColor << 8) | nColor;
				pScreen[nIndex] = (COLORREF)nColor;
			}

			StrecthBlt(&m_staticView, nSamples, nLines, pScreen);

			delete pScreen;
		}
		delete pnArray;
	}

}

void CViewPane::RenderTOFCScan()
{
	USL_RETURN_CODE nRet = USLFailed;
	int nSamples;
	USLgetCScanSamplesPerLine(m_pFile, &nSamples);
	int nLines;
	USLgetCScanNumberOfLines(m_pFile, &nLines);
	unsigned int nSize = (unsigned int)(nSamples * nLines);

	if (nSize > 0) {
		int* pnArray = new int[nSize];

		nRet = USLgetTOFCScan(m_pFile, m_CScanTypes[m_nCScanIndex].nTS, m_CScanTypes[m_nCScanIndex].nIndex, pnArray, nSize);
		if (nRet == USLSuccess) {
			int nMaxTime = 1;
			for (unsigned int nIndex = 0; nIndex < nSize; nIndex++) {
				if (pnArray[nIndex] > nMaxTime) nMaxTime = pnArray[nIndex];
			}

			COLORREF* pScreen = new COLORREF[nSize];
			for (unsigned int nIndex = 0; nIndex < nSize; nIndex++) {
				int nColor = (pnArray[nIndex] * 255) / nMaxTime;
				nColor = (nColor << 16) | (nColor << 8) | nColor;
				pScreen[nIndex] = (COLORREF)nColor;
			}

			StrecthBlt(&m_staticView, nSamples, nLines, pScreen);

			delete pScreen;
		}
		delete pnArray;
	}

}


/*************************************************************
* Pass an array of COLORREF and place it on the window
**************************************************************/
void CViewPane::StrecthBlt(CWnd* pWnd, int nHorizSrc, int nVertSrc, COLORREF* pSrcArray)
{
	CRect rr;
	CBitmap bmp;
	CDC  dcMem;
	CDC* pDC = pWnd->GetDC();
	pWnd->GetWindowRect(rr);
	rr -= rr.TopLeft();

	bmp.CreateCompatibleBitmap(pDC, nHorizSrc, nVertSrc);
	dcMem.CreateCompatibleDC(pDC);

	bmp.SetBitmapBits((DWORD)((nHorizSrc * nVertSrc) * (sizeof COLORREF)), pSrcArray);
	dcMem.SelectObject(&bmp);

	pDC->SetStretchBltMode(HALFTONE);
	pDC->StretchBlt(rr.left, rr.top, rr.Width(), rr.Height(), &dcMem, 0, 0, nHorizSrc, nVertSrc, SRCCOPY);

	ReleaseDC(pDC);
}

void CViewPane::OnMouseMove(UINT nFlags, CPoint point)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CRect rr;

	int nSamples;
	USLgetCScanSamplesPerLine(m_pFile, &nSamples);
	int nLines;
	USLgetCScanNumberOfLines(m_pFile, &nLines);

	m_staticView.GetWindowRect(rr);
	ScreenToClient(rr);

	if (m_bLBtnDn == true && rr.PtInRect(point)) {
		int nSample = (point.x - rr.left) * nSamples / rr.Width();
		int nLine = (point.y - rr.top) * nLines / rr.Height();
		pFrame->SendMessage(UI_INVALIDATE_ASCAN, (WPARAM)nSample, (LPARAM)nLine);
		return;
	}

	CDockablePane::OnMouseMove(nFlags, point);
}


void CViewPane::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rr;

	m_staticView.GetWindowRect(rr);
	ScreenToClient(rr);
	if (rr.PtInRect(point)) {
		m_bLBtnDn = true;
		return;
	}

	CDockablePane::OnLButtonDown(nFlags, point);
}


void CViewPane::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLBtnDn = false;

	CDockablePane::OnLButtonUp(nFlags, point);
}


void CViewPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rr;
	CString Buff;
	int nIndex;

	m_staticView.GetWindowRect(&rr);
	if (rr.PtInRect(point)) {

		CMenu* menu = (CMenu*) new CMenu;
		menu->LoadMenu(IDR_POPUP_VIEW_MENU);
		CMenu* pPopup = menu->GetSubMenu(0);

		InvalidaCScanTypes();

		for (nIndex = 0; nIndex < m_nCScanCount; nIndex++) {

			Buff.Empty();
			switch(m_CScanTypes[nIndex].nImageType) {
			case _AMPLITUDE: Buff.Format(L"Timeslot %d Amplitude %d", m_CScanTypes[nIndex].nTS + 1, m_CScanTypes[nIndex].nIndex + 1);
				break;
			case _TOF: Buff.Format(L"Timeslot %d TOF %d", m_CScanTypes[nIndex].nTS + 1, m_CScanTypes[nIndex].nIndex + 1);
				break;
			}
			pPopup->ModifyMenu(ID_VIEW_1 + nIndex, MF_BYCOMMAND, ID_VIEW_1 + nIndex, Buff);
		}
		for (nIndex; nIndex < 10; nIndex++) {
			pPopup->DeleteMenu(ID_VIEW_1 + nIndex, MF_BYCOMMAND);
		}
		menu->GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		delete menu;
	}

}

/**************************************************************
* Build an array of available amplitude and TOF C-Scans
***************************************************************/
void CViewPane::InvalidaCScanTypes()
{
	ZeroMemory(m_CScanTypes, sizeof m_CScanTypes);
	int nIndex = 0;
	int nCountTS, nCountAmp, nCountToF;
	USLgetTimeSlotCount(m_pFile, &nCountTS);
	for (int nTS = 0; nTS < nCountTS; nTS++) {
		USLgetAmplitudeCScanCount(m_pFile, nTS, &nCountAmp);
		for (int nAmp = 0; nAmp < nCountAmp && nIndex < 10; nAmp++) {
			m_CScanTypes[nIndex].nTS = nTS;
			m_CScanTypes[nIndex].nImageType = _AMPLITUDE;
			m_CScanTypes[nIndex].nIndex = nAmp;
			nIndex++;
		}
		USLgetTOFCScanCount(m_pFile, nTS, &nCountToF);
		for (int nTOF = 0; nTOF < nCountToF && nIndex < 10; nTOF++) {
			m_CScanTypes[nIndex].nTS = nTS;
			m_CScanTypes[nIndex].nImageType = _TOF;
			m_CScanTypes[nIndex].nIndex = nTOF;
			nIndex++;
		}
	}
	m_nCScanCount = nIndex;
	if (m_nCScanIndex < 0) m_nCScanIndex = 0;
	if (m_nCScanIndex >= nIndex) m_nCScanIndex = nIndex - 1;
}

void CViewPane::OnView1()
{
	m_nCScanIndex = 0;
	Invalidate(FALSE);
}
void CViewPane::OnView2()
{
	m_nCScanIndex = 1;
	Invalidate(FALSE);
}
void CViewPane::OnView3()
{
	m_nCScanIndex = 2;
	Invalidate(FALSE);
}
void CViewPane::OnView4()
{
	m_nCScanIndex = 3;
	Invalidate(FALSE);
}
void CViewPane::OnView5()
{
	m_nCScanIndex = 4;
	Invalidate(FALSE);
}
void CViewPane::OnView6()
{
	m_nCScanIndex = 5;
	Invalidate(FALSE);
}
void CViewPane::OnView7()
{
	m_nCScanIndex = 6;
	Invalidate(FALSE);
}
void CViewPane::OnView8()
{
	m_nCScanIndex = 7;
	Invalidate(FALSE);
}
void CViewPane::OnView9()
{
	m_nCScanIndex = 8;
	Invalidate(FALSE);
}
void CViewPane::OnView10()
{
	m_nCScanIndex = 9;
	Invalidate(FALSE);
}

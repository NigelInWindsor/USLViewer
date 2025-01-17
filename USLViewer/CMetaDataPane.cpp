#include "pch.h"
#include "USLViewer.h"
#include "CMetaDataPane.h"
#include "..\USLData\Include\USLData.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CMetaDataPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


int CMetaDataPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rectDummy;
	rectDummy.SetRectEmpty();


	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_PROPERTY_LIST))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	AdjustLayout();

	return 0;
}

void CMetaDataPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CMetaDataPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

typedef enum _PROPERTIES_ID
{
	ID_BASE = 0,
	ID_NAME,
	ID_STEPSIZE,
	ID_MOTOR_TYPE,
	ID_AXIS_TYPE,
	ID_AMPLIFIER_TYPE,
	ID_UNITS_TYPE,
	ID_AVAILABLE,
	ID_PRECISION,
	ID_PHYSICAL_AXIS,
	ID_ETHERCAT_INDEX,
	ID_SPEED,
	ID_MOVE_SPEED,
	ID_JOG_SPEED,
	ID_JOG_ACCELERATION,
	ID_HOME_SPEED,
	ID_ACCELERATION,
	ID_STEPS_PER_REV,
	ID_MODULO_ABSOLUTE_MODE,
	ID_HW_NEGATIVE,
	ID_HW_POSITIVE,
	ID_LIMIT_NEGATIVE,
	ID_LIMIT_POSITIVE,
	ID_HOME_MODE,
	ID_HOME_PLC,
	ID_HMZ_POS,
	ID_HOME_POS,
} _PROPERTIES_ID;


int CMetaDataPane::InitPropList()
{
	CMFCPropertyGridProperty* pGroup;
	CString String;
	bool bFlag;
	int nVar;
	float fVar;
	std::wstring strDataVersion;

	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	pGroup = new CMFCPropertyGridProperty(L"Globals", ID_BASE, FALSE);
	USLgetDataVersion(NULL, strDataVersion);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(L"File version", (_variant_t)strDataVersion.c_str(), L"Number of bytes per sample along a line", 0));
	m_wndPropList.AddProperty(pGroup);

	pGroup = new CMFCPropertyGridProperty(L"C-Scan", ID_BASE, FALSE);
	USLgetCScanSamplesPerLine(m_pFile, &nVar);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Samples per line", (_variant_t)nVar, L"Number of samples along each C-Scan line", 0));
	USLgetCScanNumberOfLines(m_pFile, &nVar);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Number of lines", (_variant_t)nVar, L"Number of lines making up a C-Scan", 0));
	USLgetCScanBytesPerSample(m_pFile, &nVar);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Amplitude sample size", (_variant_t)nVar, L"Number of bytes per sample along a line", 0));
	USLgetCScanHorizontalSampleSize(m_pFile, &fVar);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Horizontal sample size", (_variant_t)fVar, L"Distance between adjacent samples", 0));
	USLgetCScanVerticalSampleSize(m_pFile, &fVar);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Vertical sample size", (_variant_t)fVar, L"Distance between adjacent lines", 0));
	m_wndPropList.AddProperty(pGroup);

	pGroup = new CMFCPropertyGridProperty(L"A-Scan", ID_BASE, FALSE);
	USLIsAScanAvailable(m_pFile, &bFlag);
	if (bFlag == true) {
		USLgetAScanLength(m_pFile, &nVar);
		pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Uncompressed array length", (_variant_t)nVar, L"Number of samples in an uncompressed A-Scan", 0));
		USLgetAScanLengthInFile(m_pFile, &nVar);
		pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Array length in file", (_variant_t)nVar, L"Number of samples in an compressed A-Scan", 0));
		USLgetAScanBytesPerSample(m_pFile, &nVar);
		pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Number bytes per sample", (_variant_t)nVar, L"Number of bytes per sample", 0));
		USLgetAScanSamplePeriod(m_pFile, &fVar);
		String.Format(L"%0.2e s", fVar);
		pGroup->AddSubItem( new CMFCPropertyGridProperty(L"Sample period", (_variant_t)(LPCWSTR)String, L"Time between samples", 0));
	}
	else {
		pGroup->AddSubItem(new CMFCPropertyGridProperty(_T("Available"), (_variant_t)bFlag, _T("No A-Scans storred in file"), 0));
	}
	m_wndPropList.AddProperty(pGroup);


	pGroup = new CMFCPropertyGridProperty(L"Ultrasonics", ID_BASE, FALSE);
	int nCount;
	USLgetTimeSlotCount(m_pFile, &nCount);
	pGroup->AddSubItem(new CMFCPropertyGridProperty(L"Number of Timelosts", (_variant_t)nCount, L"Number of timeslots", 0));
	for (int nTS = 0; nTS < nCount; nTS++) {
		String.Format(L"Number of amplitude C-scans TS:%d", nTS + 1);
		USLgetAmplitudeCScanCount(m_pFile, nTS, &nVar);
		pGroup->AddSubItem(new CMFCPropertyGridProperty(String, (_variant_t)nVar, L"Number of Amplitude c-scans", 0));
		String.Format(L"Number of TOF C-scans TS:%d", nTS + 1);
		USLgetTOFCScanCount(m_pFile, nTS, &nVar);
		pGroup->AddSubItem(new CMFCPropertyGridProperty(String, (_variant_t)nVar, L"Number of Time c-scans", 0));
	}
	m_wndPropList.AddProperty(pGroup);

	return 0;
}


void CMetaDataPane::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CMenu* menu = (CMenu*) new CMenu;
	CPoint pt;
	CString Buff;
	GetCursorPos(&pt);

	CMFCPropertyGridProperty* pProperty;
	pProperty = m_wndPropList.GetCurSel();
	if (pProperty == NULL) goto safe_return;
	m_nContextID = (int)pProperty->GetData();


safe_return:
	;
}

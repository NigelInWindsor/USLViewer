
// USLViewer.h : main header file for the USLViewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "CLastSettings.h"


enum UI_MSG
{
	UI_INVALIDATE_ASCAN = 0x8100,
};

class CUSLViewerApp : public CWinAppEx
{
public:
	CUSLViewerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CString m_StorePath;
	CLastSettings m_LS;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CUSLViewerApp theApp;

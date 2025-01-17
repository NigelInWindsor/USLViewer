#pragma once
#include <afxdockablepane.h>
#include <string>

#define _AMPLITUDE  0
#define _TOF        1

struct CScanType {
    int nTS;
    int nImageType;
    int nIndex;
};

class CViewPane :
    public CDockablePane
{
protected:
    int m_nCScanCount = 0;
    int m_nCScanIndex = 0;
    CScanType m_CScanTypes[10] = { 0 };
    bool 	m_bLBtnDn = false;
    CStatic	m_staticView;
    std::wstring m_strFilePathName;

public:
    bool Open(CString FilePathName);
    FILE* m_pFile = NULL;

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void AdjustLayout();
    afx_msg void OnPaint();
    void RenderAmplitudeCScanFromGreyScale();
    void RenderAmplitudeCScanFromSigned();
    void RenderTOFCScan();
    void StrecthBlt(CWnd* pWnd, int nHorizSrc, int nVertSrc, COLORREF* pSrcArray);
    void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    void InvalidaCScanTypes();
    afx_msg void OnView1();
    afx_msg void OnView2();
    afx_msg void OnView3();
    afx_msg void OnView4();
    afx_msg void OnView5();
    afx_msg void OnView6();
    afx_msg void OnView7();
    afx_msg void OnView8();
    afx_msg void OnView9();
    afx_msg void OnView10();
};


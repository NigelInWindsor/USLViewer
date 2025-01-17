#pragma once
#include <afxdockablepane.h>
class CMetaDataPane :
    public CDockablePane
{
protected:
    int                 m_nContextID = 0;
    CMFCPropertyGridCtrl m_wndPropList;

public:
    FILE* m_pFile = NULL;
    int InitPropList();

    DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void AdjustLayout();
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};


#pragma once
#include <afxdockablepane.h>
class CAScanPane :
    public CDockablePane
{
protected:
    CStatic	m_staticView;
    int m_nDataXSample = 0;
    int m_nDataYLine = 0;


public:
    FILE* m_pFile = NULL;

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void AdjustLayout();
    afx_msg void OnPaint();
    void RenderTrace();
    afx_msg LRESULT OnUIInvalidateAScan(WPARAM wp, LPARAM lp);
    int MulDiv64(int nValue, int nNumerator, int nDenominator);
};


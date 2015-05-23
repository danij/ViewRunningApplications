#pragma once

#include "ProcessRetriever.h"
#include "afxwin.h"

// CViewRunningApplicationsDlg dialog
class CViewRunningApplicationsDlg : public CDialog
{
// Construction
public:
    CViewRunningApplicationsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    enum { IDD = IDD_VIEWRUNNINGAPPLICATIONS_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    std::vector<ProcessInstance> m_instances;
    void DisplayProcessInstances();
    void AddProcessInfo(CString& result, const ProcessInstance& info);

    afx_msg void OnBnClickedCopytoclipboard();
    CEdit m_resultsEdit;
    CButton m_hideLibraries;
    afx_msg void OnBnClickedHidemodules();
    CComboBox m_orderCombo;
    afx_msg void OnCbnSelchangeOrdercombo();
    CButton m_hideUnknowns;
public:
    afx_msg void OnBnClickedHideunknown();
};

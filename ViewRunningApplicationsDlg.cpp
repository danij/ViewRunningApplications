#include "stdafx.h"
#include "ViewRunningApplications.h"
#include "ViewRunningApplicationsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

CViewRunningApplicationsDlg::CViewRunningApplicationsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CViewRunningApplicationsDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CViewRunningApplicationsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RESULTS, m_resultsEdit);
    DDX_Control(pDX, IDC_HIDEMODULES, m_hideLibraries);
    DDX_Control(pDX, IDC_ORDERCOMBO, m_orderCombo);
    DDX_Control(pDX, IDC_HIDEUNKNOWN, m_hideUnknowns);
}

BEGIN_MESSAGE_MAP(CViewRunningApplicationsDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CopyToClipboard, &CViewRunningApplicationsDlg::OnBnClickedCopytoclipboard)
    ON_BN_CLICKED(IDC_HIDEMODULES, &CViewRunningApplicationsDlg::OnBnClickedHidemodules)
    ON_CBN_SELCHANGE(IDC_ORDERCOMBO, &CViewRunningApplicationsDlg::OnCbnSelchangeOrdercombo)
    ON_BN_CLICKED(IDC_HIDEUNKNOWN, &CViewRunningApplicationsDlg::OnBnClickedHideunknown)
END_MESSAGE_MAP()


// CViewRunningApplicationsDlg message handlers

BOOL CViewRunningApplicationsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    m_orderCombo.SetCurSel(0);

    ProcessRetriever retriever;
    m_instances = retriever.GetInstances();
    DisplayProcessInstances();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CViewRunningApplicationsDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CViewRunningApplicationsDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CopyToClipboard(CString& value)
{
    if (value.GetLength() < 1)
    {
        return;
    }
    auto nrBytes = (value.GetLength() + 1) * sizeof(TCHAR);
    auto handle = GlobalAlloc(GHND, nrBytes);
    auto destination = (LPTSTR)GlobalLock(handle);
    CopyMemory(destination, value.GetBuffer(), nrBytes);
    GlobalUnlock(handle);

    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, handle);
    CloseClipboard();
}

void CViewRunningApplicationsDlg::OnBnClickedCopytoclipboard()
{
    CString temp;
    m_resultsEdit.GetWindowTextW(temp);
    CopyToClipboard(temp);
}

CString FormatNumericValue(DWORD64 value)
{
    CString result;
    result.AppendFormat(_T("%d"), value);
    TCHAR temp[1024] = { 0 };
    NUMBERFMT format;
    format.NumDigits = 0;
    format.LeadingZero = 0;
    format.Grouping = 3;
    format.lpDecimalSep = _T(".");
    format.lpThousandSep = _T(" ");
    format.NegativeOrder = 0;
    if (GetNumberFormat(LOCALE_USER_DEFAULT, 0, result.GetBuffer(), &format, temp, sizeof(temp) / sizeof(TCHAR)))
    {
        result = temp;
    }

    return result;
}

void CViewRunningApplicationsDlg::AddProcessInfo(CString& result, const ProcessInstance& info)
{
    if ( ! info.Process)
    {
        if (BST_CHECKED == m_hideUnknowns.GetCheck())
        {
            return;
        }
        result.AppendFormat(_T("[%d] %s\r\n"), info.Pid, _T("<unknown>"));
        return;
    }
    result.AppendFormat(_T("[%d] %s "), info.Pid, info.Process->Name);
    if (info.Process->Version.GetLength() > 0)
    {
        result.AppendFormat(_T("v%s "), info.Process->Version);
    }
    if (info.Process->Company.GetLength() > 0)
    {
        result.AppendFormat(_T("- %s"), info.Process->Company);
    }
    result += "\r\n";
    if (info.WorkingSet)
    {
        result.AppendFormat(_T("   Working Set: %s bytes\r\n"), FormatNumericValue(info.WorkingSet));
    }
    if (info.PrivateBytes)
    {
        result.AppendFormat(_T("   Private Bytes: %s bytes\r\n"), FormatNumericValue(info.PrivateBytes));
    }
    result.AppendFormat(_T("   => %s\r\n"), info.Process->ExecutablePath);
    if (BST_CHECKED == m_hideLibraries.GetCheck())
    {
        return;
    }
    for (auto& item : info.Process->Modules)
    {
        result.AppendFormat(_T("   => %s\r\n"), item);
    }
}

void CViewRunningApplicationsDlg::DisplayProcessInstances()
{   
    CString result;

    for (auto& info : m_instances)
    {
        AddProcessInfo(result, info);
    }

    m_resultsEdit.SetWindowTextW(result);
}

void CViewRunningApplicationsDlg::OnBnClickedHidemodules()
{
    DisplayProcessInstances();
}


void CViewRunningApplicationsDlg::OnCbnSelchangeOrdercombo()
{
    switch (m_orderCombo.GetCurSel())
    {
    case 1:
        sort(begin(m_instances), end(m_instances), ProcessInstance::CompareByName);
        break;
    case 2:
        sort(begin(m_instances), end(m_instances), ProcessInstance::CompareByWorkingSetDesc);
        break;
    case 3:
        sort(begin(m_instances), end(m_instances), ProcessInstance::CompareByPrivateBytesDesc);
        break;
    default:
        sort(begin(m_instances), end(m_instances), ProcessInstance::CompareByPID);
        break;
    }

    DisplayProcessInstances();
}


void CViewRunningApplicationsDlg::OnBnClickedHideunknown()
{
    DisplayProcessInstances();
}


// ViewRunningApplications.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ViewRunningApplications.h"
#include "ViewRunningApplicationsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewRunningApplicationsApp

BEGIN_MESSAGE_MAP(CViewRunningApplicationsApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CViewRunningApplicationsApp construction

CViewRunningApplicationsApp::CViewRunningApplicationsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CViewRunningApplicationsApp object

CViewRunningApplicationsApp theApp;


// CViewRunningApplicationsApp initialization

BOOL CViewRunningApplicationsApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	CViewRunningApplicationsDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
	}


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


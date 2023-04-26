/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's main application object.
*/


#include "stdafx.h"

#include "NoteWhen.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNWApp

BEGIN_MESSAGE_MAP(CNWApp, CWinApp)
	//{{AFX_MSG_MAP(CNWApp)
	ON_COMMAND(IDM_HELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWApp construction

CNWApp::CNWApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNWApp object

CNWApp theApp;

// Determine if NoteWhen is already running by using a Mutex object -- 
// returns TRUE if already running, FALSE otherwise.  Note that a mutex 
// is created with the name of the application
BOOL CNWApp::AlreadyRunning()
{
	BOOL bFound = FALSE;

	// Try to create a mutex with the app's name
	HANDLE hMutexOneInstance = ::CreateMutex(NULL,TRUE,_T("NoteWhen"));

	// Already there...means that we are already running an instance
	if(::GetLastError() == ERROR_ALREADY_EXISTS)
		bFound = TRUE;

	// Release the mutex
	if(hMutexOneInstance)
		::ReleaseMutex(hMutexOneInstance);

	return(bFound);
}

/////////////////////////////////////////////////////////////////////////////
// CNWApp initialization

BOOL CNWApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Check to make sure this app is running already
	if(AlreadyRunning())
	{
		// Oops, already running so get out
		AfxMessageBox("NoteWhen已在运行。要重新运行，请先关闭它。",MB_OK|MB_ICONWARNING);
		return(FALSE);
	}

	// Check the command line options
	CString strConfig;
	char szTemp[MAX_PATH+1];
			
	GetModuleFileName(NULL,szTemp,sizeof(szTemp));
	strConfig = SplitFileName(szTemp,DRIVE|PATH)+CString("notes.nwh");

	// Get the options filename if it's specified on the command line
	if(__argc > 1)
	{
		strConfig = __argv[1];
	}

	// Use an .ini file
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(_T(SplitFileName(szTemp,DRIVE|PATH|FNAME)+".ini"));

	// Load the option file
	if(!m_Doc.Load(strConfig))
	{
		AfxMessageBox("打开配置文件失败: "+strConfig,
								MB_ICONWARNING|MB_OK);
		return(FALSE);
	}

	// Display the main dialog
	CNWDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	if(nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if(nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	m_Doc.Save();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

// Parses a filename into the specified parts and returns a CString that
// consists of the specified parts.
CString CNWApp::SplitFileName(LPCTSTR lpszPath, int nSplit)
{
	CString strResult;

	char szPath[800],szDrive[800],szFileName[800],szExt[800];
	_splitpath(lpszPath,szDrive,szPath,szFileName,szExt);

	if(nSplit & DRIVE)
		strResult += szDrive;
	if(nSplit & PATH)
		strResult += szPath;
	if(nSplit & FNAME)
		strResult += szFileName;
	if(nSplit & EXT)
		strResult += szExt;

	return(strResult);
}

void CNWApp::OnHelp() 
{
	WinHelp(0,HELP_FINDER);
}



void CNWApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	CWinApp::WinHelp(dwData, nCmd);
}

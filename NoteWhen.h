/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines NoteWhen's main application class.
*/


#if !defined(AFX_NOTEWHEN_H__7726A24F_9475_4FD1_A23A_4DC8978E4EE0__INCLUDED_)
#define AFX_NOTEWHEN_H__7726A24F_9475_4FD1_A23A_4DC8978E4EE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "doc.h"

/////////////////////////////////////////////////////////////////////////////
// CNWApp:
// See NoteWhen.cpp for the implementation of this class
//

class CNWApp : public CWinApp
{
protected:
	CNWDocument m_Doc;

	typedef enum
	{
		DRIVE	= 0x0001,
		PATH	= 0x0002,
		FNAME	= 0x0004,
		EXT		= 0x0008,
	} SPLIT;

protected:
	BOOL AlreadyRunning();
	CString SplitFileName(LPCTSTR lpszPath, int nSplit);

public:
	CNWApp();

public:
	CNWDocument *GetDocument() { return &m_Doc; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWApp)
	public:
	virtual BOOL InitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNWApp)
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define GETAPP() ((CNWApp *) AfxGetApp())

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTEWHEN_H__7726A24F_9475_4FD1_A23A_4DC8978E4EE0__INCLUDED_)

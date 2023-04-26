/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines NoteWhen's schedule dialog class.
*/

#if !defined(AFX_SCHDDLG_H__B58DE228_82CF_4E50_9386_BC887D31A31F__INCLUDED_)
#define AFX_SCHDDLG_H__B58DE228_82CF_4E50_9386_BC887D31A31F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SchdDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CNWSchdDlg dialog

#include "schdpage.h"
#include "evntpage.h"

class CNWItem;

class CNWSchdDlg : public CDialog
{
protected:
	CPropertySheet m_Sheet;
	CNWItem *m_pItem;
	CNWSchedPage m_SchedulePage;
	CNWEventPage m_EventPage;

// Construction
public:
	CNWSchdDlg(CWnd* pParent = NULL);   // standard constructor

public:
	void SetItem(CNWItem *pItem) { m_pItem = pItem; }

// Dialog Data
	//{{AFX_DATA(CNWSchdDlg)
	enum { IDD = IDD_SCHEDULE };
	BOOL	m_DisableNotifications;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWSchdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNWSchdDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnEnableNotifications();
	afx_msg void OnHelpbtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHDDLG_H__B58DE228_82CF_4E50_9386_BC887D31A31F__INCLUDED_)

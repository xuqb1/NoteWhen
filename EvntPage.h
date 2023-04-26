/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines event property page.
*/

#if !defined(AFX_EVNTPAGE_H__4DC11657_7182_41CC_94B1_23659F88891D__INCLUDED_)
#define AFX_EVNTPAGE_H__4DC11657_7182_41CC_94B1_23659F88891D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EvntPage.h : header file
//

#define UM_ENABLE		(WM_USER+1000)

/////////////////////////////////////////////////////////////////////////////
// CNWEventPage dialog

class CNWItem;

class CNWEventPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNWEventPage)

protected:
	CNWItem *m_pItem;

	typedef enum
	{
		DRIVE = 0x0001,
		PATH	= 0x0002,
		FNAME = 0x0004,
		EXT	= 0x0008,
	} SPLIT;

protected:
	void SetDlgFields();
	BOOL GetDlgFields();
	CString SplitFileName(LPCTSTR lpszPath, int nSplit);

// Construction
public:
	CNWEventPage();
	CNWEventPage(UINT nIDTemplate, UINT nIDCaption = 0);

	~CNWEventPage();

public:
	void SetItem(CNWItem *pItem) { m_pItem = pItem; }
	BOOL SavePage();

// Dialog Data
	//{{AFX_DATA(CNWEventPage)
	enum { IDD = IDD_WHAT_PAGE };
	CSpinButtonCtrl	m_MinutesSpin;
	CSpinButtonCtrl	m_HoursSpin;
	CSpinButtonCtrl	m_DaysSpin;
	BOOL	m_AlarmEnabled;
	BOOL	m_AlwaysDisplay;
	int		m_Days;
	int		m_Hours;
	int		m_Minutes;
	CString	m_SoundFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNWEventPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNWEventPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAlarmEnabled();
	afx_msg void OnTest();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdate(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVNTPAGE_H__4DC11657_7182_41CC_94B1_23659F88891D__INCLUDED_)

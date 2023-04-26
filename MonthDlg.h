/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines NoteWhen's monthly dialog.
*/

#if !defined(AFX_MONTHDLG_H__763C6E3C_C260_48D7_B4AB_C392ABC76806__INCLUDED_)
#define AFX_MONTHDLG_H__763C6E3C_C260_48D7_B4AB_C392ABC76806__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonthDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNWMonthDlg dialog

class CNWMonthDlg : public CDialog
{
protected:
	int GetSelectedItem();

// Construction
public:
	CNWMonthDlg(CWnd* pParent = NULL);   // standard constructor
	void LoadEntries();
	static int CALLBACK SortFunc(LPARAM lp1,LPARAM lp2,LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CNWMonthDlg)
	enum { IDD = IDD_MONTH };
	CListCtrl	m_List;
	CMonthCalCtrl	m_Calendar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWMonthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNWMonthDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCalendar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectCalendar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdaystateCalendar(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChange();
	afx_msg void OnDblclkDay(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickDay(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelpbtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONTHDLG_H__763C6E3C_C260_48D7_B4AB_C392ABC76806__INCLUDED_)

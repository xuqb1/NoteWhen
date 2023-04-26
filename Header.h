/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines NoteWhen's header control handler.
*/

#if !defined(AFX_HEADER_H__76869FAF_D9D9_4843_B0D0_97BD2E3FB8A8__INCLUDED_)
#define AFX_HEADER_H__76869FAF_D9D9_4843_B0D0_97BD2E3FB8A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Header.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNWHeader window

class CNWHeader : public CHeaderCtrl
{
protected:
	BOOL CanResize(CPoint& point);

// Construction
public:
	CNWHeader();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWHeader)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNWHeader();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNWHeader)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEADER_H__76869FAF_D9D9_4843_B0D0_97BD2E3FB8A8__INCLUDED_)

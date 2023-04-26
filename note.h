/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines NoteWhen's note class.
*/

#if !defined(AFX_NOTE_H__4306B094_F328_478A_9A37_84E3FA05A8C2__INCLUDED_)
#define AFX_NOTE_H__4306B094_F328_478A_9A37_84E3FA05A8C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Note.h : header file
//

#define MARGIN			15
#define CXCORNER		20
#define CYCORNER		20

#define UM_UPDATE		WM_USER+1050
#define UPD_ALL		0
#define UPD_DELETE	1
#define UPD_COLORS	2


/////////////////////////////////////////////////////////////////////////////
// CNWNoteWnd window

#include "color.h"

class CNWItem;

class CNWNoteWnd : public CWnd
{
protected:
	BOOL m_bInMenu;
	POINT m_aPoints[10];
	int m_nPoints;
	CRgn m_rgnWnd;
	CEdit m_Edit;
	int m_nSizeBorder;
	CNWColor m_Color;
	CFont m_fontText;
	BOOL m_bIsNew;
	HBRUSH m_hbrNote;
	CNWItem *m_pItem;
	HCURSOR m_hcurHand;
	enum 
	{
		NWHT_NOWHERE		= 0x0000,
		NWHT_LEFT			= 0x0001,
		NWHT_RIGHT			= 0x0002,
		NWHT_TOP				= 0x0003,
		NWHT_TOPLEFT		= 0x0004,
		NWHT_TOPRIGHT		= 0x0005,
		NWHT_BOTTOM			= 0x0006,
		NWHT_BOTTOMLEFT	= 0x0007,
		NWHT_BOTTOMRIGHT	= 0x0008,
	} eNWHITTEST;

// Construction
public:
	CNWNoteWnd(CNWItem *pItem, BOOL bIsNew=FALSE);

// Attributes
public:

// Operations
public:
	void ShowContextMenu();
	void SetShape();
	int HitTest(CPoint point);
	void OnApply();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWNoteWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNWNoteWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNWNoteWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCloseBtn();
	afx_msg void OnDelete();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetColors();
	afx_msg void OnAlwaysontop();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSetSchedule();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnUndo();
	afx_msg void OnSelall();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnHandleExit();
	afx_msg void OnNew();
	afx_msg void OnNotelist();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	afx_msg void OnChangeText();
	afx_msg LRESULT OnUpdate(WPARAM,LPARAM);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTE_H__4306B094_F328_478A_9A37_84E3FA05A8C2__INCLUDED_)

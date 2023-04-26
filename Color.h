/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines NoteWhen's color handler.
*/

#if !defined(AFX_COLOR_H__65CC44EC_893A_4130_ABA9_6EC02E8365C1__INCLUDED_)
#define AFX_COLOR_H__65CC44EC_893A_4130_ABA9_6EC02E8365C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNWColor  
{
protected:
	COLORREF m_rgbText;
	COLORREF m_rgbBackground;
	COLORREF m_rgbTempBackground;
	CFont m_Font;
	void (*m_lpApply)(CWnd *);
	CWnd *m_pThis;
	CWnd *m_pParent;

protected:
	void HandleBkColor(HWND hDlg);
	void DrawComboMain(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void DrawComboButton(LPDRAWITEMSTRUCT lpDrawItemStruct);
	static UINT CALLBACK NWColorHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, 
															LPARAM lParam);
public:
	CNWColor();
	virtual ~CNWColor();

public:
	int Select(CWnd *pParent);

	COLORREF GetBkColor() { return m_rgbBackground; }
	void SetBkColor(COLORREF rgbBackground) { m_rgbBackground = rgbBackground; }

	COLORREF GetTextColor() { return m_rgbText; }
	void SetTextColor(COLORREF rgbText) { m_rgbText = rgbText; }

	CFont& GetFont() { return m_Font; }
	void SetFont(CFont& font);

	void SetApplyHandler(void (*pApply)(CWnd *)) { m_lpApply = pApply; };
	void SetThis(CWnd *thisref) { m_pThis = thisref; }
};

#endif // !defined(AFX_COLOR_H__65CC44EC_893A_4130_ABA9_6EC02E8365C1__INCLUDED_)

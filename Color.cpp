/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's color handler.
*/

#include "stdafx.h"
#include "notewhen.h"
#include "Color.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static CNWColor *g_pThis;

CNWColor::CNWColor()
{
	m_lpApply = NULL;
}

CNWColor::~CNWColor()
{

}

// Set the new font to use
void CNWColor::SetFont(CFont& font) 
{ 
	LOGFONT lf; 

	font.GetLogFont(&lf); 

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&lf);
}

// Handles callback for color combobox
UINT CALLBACK CNWColor::NWColorHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, 
															LPARAM lParam)
{
	// Set the class point when the dialog is initialized
	if(uiMsg == WM_INITDIALOG)
	{
		SetWindowLong(hdlg,GWL_USERDATA,(long) g_pThis);
	}
	else if(uiMsg == WM_DRAWITEM)
	{
		// Draw the appropriate control
		if(wParam == IDC_BACKGROUND)
		{
			CNWColor *pThis = (CNWColor *) GetWindowLong(hdlg,GWL_USERDATA);

			// Draw the main combo
			pThis->DrawComboMain((LPDRAWITEMSTRUCT) lParam);
			return(1);
		}
		else if(wParam == IDC_COLOR_SELECT)
		{
			CNWColor *pThis = (CNWColor *) GetWindowLong(hdlg,GWL_USERDATA);

			// Or draw a color button
			pThis->DrawComboButton((LPDRAWITEMSTRUCT) lParam);
			return(1);
		}
	}
	else if(uiMsg == WM_COMMAND && LOWORD(wParam) == IDC_COLOR_SELECT)
	{
		CNWColor *pThis = (CNWColor *) GetWindowLong(hdlg,GWL_USERDATA);

		pThis->HandleBkColor(hdlg);
	}
	else if(uiMsg == WM_COMMAND && LOWORD(wParam) == 1026)
	{
		// Handles the Apply button (not currently implemented fully)

		CFontDialog dlgFont;
		CNWColor *pThis = (CNWColor *) GetWindowLong(hdlg,GWL_USERDATA);

		dlgFont.FromHandle(hdlg);

		pThis->m_rgbBackground = pThis->m_rgbTempBackground;
		pThis->m_rgbText = dlgFont.GetColor();

		pThis->m_Font.DeleteObject();
		pThis->m_Font.CreateFontIndirect(dlgFont.m_cf.lpLogFont);

		if(g_pThis->m_lpApply != NULL)
		{	  
			(*pThis->m_lpApply)(pThis->m_pThis);
		}
	}

	return(0);
}

// Presents the user with a font/color selection dialog
int CNWColor::Select(CWnd *pParent)
{
	CFontDialog dlgFont(NULL,CF_EFFECTS|CF_SCREENFONTS,NULL,pParent);

	m_pParent = pParent;
	m_rgbTempBackground = m_rgbBackground;

	dlgFont.m_cf.Flags |= CF_ENABLETEMPLATE|CF_ENABLEHOOK|CF_INITTOLOGFONTSTRUCT;
	dlgFont.m_cf.Flags |= CF_EFFECTS;
	dlgFont.m_cf.rgbColors = m_rgbText;
//	dlgFont.m_cf.Flags |= CF_APPLY;
	dlgFont.m_cf.lpfnHook = NWColorHookProc;
	dlgFont.m_cf.lpTemplateName = MAKEINTRESOURCE(IDD_FONTCOLOR);
	dlgFont.m_cf.hInstance = AfxGetInstanceHandle();

	LOGFONT lf;

	m_Font.GetLogFont(&lf);
	dlgFont.m_cf.lpLogFont = &lf;

	g_pThis = this;

	// Display the selection dialog
	int nRet = dlgFont.DoModal();

	// Get the results if the user presses OK
	if(nRet == IDOK)
	{
		m_rgbBackground = m_rgbTempBackground;
		m_rgbText = dlgFont.GetColor();

		LOGFONT lf;

		dlgFont.GetCurrentFont(&lf);

		m_Font.DeleteObject();
		m_Font.CreateFontIndirect(&lf);
	}

	return(nRet);
}

// Handles the background color combobox
void CNWColor::HandleBkColor(HWND hDlg)
{
	CColorDialog dlgColor(0,0,CWnd::FromHandle(hDlg));

	dlgColor.m_cc.Flags |= CC_RGBINIT;
	dlgColor.m_cc.rgbResult = m_rgbTempBackground;
	dlgColor.m_cc.hwndOwner = hDlg;

	// Displays an advanced color selection box
	if(dlgColor.DoModal() == IDOK)
	{
		m_rgbTempBackground = dlgColor.GetColor();

		::InvalidateRect(::GetDlgItem(hDlg,IDC_BACKGROUND),NULL,FALSE);
	}
}

// Draw the background color combobox
void CNWColor::DrawComboMain(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	COLORREF rgbValue;

	rgbValue = m_rgbTempBackground;

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);


	// Draw the selected color now
	dc.FillSolidRect(&lpDrawItemStruct->rcItem,rgbValue);

	dc.Detach();
}

// Draw the background color button
void CNWColor::DrawComboButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	COLORREF rgbValue;

	rgbValue = m_rgbTempBackground;

	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	CBitmap bm;
	CBitmap bmp, *poldbmp;
	CDC memdc;

	bm.LoadOEMBitmap(OBM_COMBO);
	dc.SelectObject(&bm);

	// Create a compatible memory DC
   memdc.CreateCompatibleDC( &dc );

	// Select the bitmap into the DC
   poldbmp = memdc.SelectObject(&bm);

   // Copy (BitBlt) bitmap from memory DC to screen DC
	dc.Draw3dRect(&lpDrawItemStruct->rcItem,GetSysColor(COLOR_BTNHILIGHT),
									GetSysColor(COLOR_3DDKSHADOW));

	CRect rc = lpDrawItemStruct->rcItem;

	rc.DeflateRect(1,1);
   dc.BitBlt(rc.left,rc.top,rc.right,rc.bottom,&memdc,0,0,SRCCOPY);

	memdc.SelectObject( poldbmp );

	dc.Detach();
}

/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's text entry dialog.
*/

#include "stdafx.h"
#include "NoteWhen.h"
#include "TextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNWTextDlg dialog


CNWTextDlg::CNWTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNWTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNWTextDlg)
	m_Text = _T("");
	//}}AFX_DATA_INIT
}


void CNWTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNWTextDlg)
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNWTextDlg, CDialog)
	//{{AFX_MSG_MAP(CNWTextDlg)
	ON_BN_CLICKED(IDC_HELPBTN, OnHelpbtn)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWTextDlg message handlers

BOOL CNWTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNWTextDlg::OnHelpbtn() 
{
	WinHelp(103);
}


BOOL CNWTextDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN){
		BOOL bCtrl=::GetKeyState(VK_CONTROL)&0x8000;
		BOOL bShift=::GetKeyState(VK_SHIFT)&0x8000;
		// only gets here if CTRL key is pressed
		BOOL bAlt=::GetKeyState(VK_MENU)&0x8000;
		switch(pMsg->wParam){
		//case VK_ESCAPE:
		//	OnCloseBtn(); 
		//	break;
		case 'A': // Ctrl+A ȫѡ
			if(bCtrl){
				::SendMessage(GetDlgItem(IDC_TEXT)->GetSafeHwnd(),EM_SETSEL,ID_SELALL,0);
				break;
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
void CNWTextDlg::OnSize(UINT nType,int cx,int cy){
	CDialog::OnSize(nType,cx,cy);
	CRect rect;
	GetWindowRect(rect);
	CWnd *pWnd=GetDlgItem(IDC_TEXT);//,this->m_hWnd);
	//CString strText;  
	//GetDlgItemText(IDC_TEXT, strText);
	//MessageBox(strText);
	if(pWnd){
		pWnd->SetWindowPos(NULL,5,5,rect.Width()-25,rect.Height()-86,SWP_SHOWWINDOW);
	}
	pWnd=GetDlgItem(IDOK);
	if(pWnd){
		pWnd->SetWindowPos(NULL,rect.Width()-236,rect.Height()-74,70,20,SWP_SHOWWINDOW);
	}
	pWnd=GetDlgItem(IDCANCEL);
	if(pWnd){
		pWnd->SetWindowPos(NULL,rect.Width()-163,rect.Height()-74,70,20,SWP_SHOWWINDOW);
	}
	pWnd=GetDlgItem(IDC_HELPBTN);
	if(pWnd){
		pWnd->SetWindowPos(NULL,rect.Width()-90,rect.Height()-74,70,20,SWP_SHOWWINDOW);
	}
	Invalidate();
}
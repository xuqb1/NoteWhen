/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's schedule dialog.
*/

#include "stdafx.h"
#include "NoteWhen.h"
#include "SchdDlg.h"
#include "schdpage.h"
#include "evntpage.h"
#include "note.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNWSchdDlg dialog


CNWSchdDlg::CNWSchdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNWSchdDlg::IDD, pParent),
	m_SchedulePage(IDD_SCHEDULE_PAGE,IDS_SCHEDULE),
	m_EventPage(IDD_WHAT_PAGE,IDS_WHAT),
	m_Sheet()

{
	//{{AFX_DATA_INIT(CNWSchdDlg)
	m_DisableNotifications = FALSE;
	//}}AFX_DATA_INIT
}


void CNWSchdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNWSchdDlg)
	DDX_Check(pDX, IDC_ENABLE_NOTIFICATIONS, m_DisableNotifications);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNWSchdDlg, CDialog)
	//{{AFX_MSG_MAP(CNWSchdDlg)
	ON_BN_CLICKED(IDC_ENABLE_NOTIFICATIONS, OnEnableNotifications)
	ON_BN_CLICKED(IDC_HELPBTN, OnHelpbtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWSchdDlg message handlers

BOOL CNWSchdDlg::OnInitDialog() 
{
	m_DisableNotifications = (m_pItem->m_sFirstTime?FALSE:m_pItem->m_sNotifyDisabled);
	m_pItem->m_bDeferSchedule = TRUE;

	CDialog::OnInitDialog();
		
	// Initialize the propery pages and property sheet
	m_SchedulePage.SetItem(m_pItem);
	m_EventPage.SetItem(m_pItem);

	m_Sheet.m_psh.dwFlags |= (PSH_NOAPPLYNOW|PSH_MODELESS);
	m_Sheet.AddPage(&m_SchedulePage);
	m_Sheet.AddPage(&m_EventPage);
	m_Sheet.Create(this,WS_CHILD|WS_CLIPCHILDREN,WS_EX_CONTROLPARENT);
	m_Sheet.SetWindowPos(NULL,0,0,-1,-1,SWP_NOSIZE);
	m_Sheet.ShowWindow(SW_SHOWNORMAL);
	m_Sheet.ModifyStyleEx(0,WS_EX_CONTROLPARENT);
	m_Sheet.SetTitle("Scheduling properties");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNWSchdDlg::OnOK() 
{
	if(!m_SchedulePage.SavePage())
		return;

	if(!m_EventPage.SavePage())
		return;

	if(!UpdateData())
		return;

	m_pItem->m_sNotifyDisabled = m_DisableNotifications;
	m_pItem->m_dateLastRun = CTime(1980,1,1,0,0,0);
	m_pItem->m_sFirstTime = FALSE;
	GETAPP()->GetDocument()->SetModified();

	m_pItem->m_bDeferSchedule = FALSE;

	CDialog::OnOK();

	AfxGetMainWnd()->PostMessage(UM_UPDATE,UPD_ALL,(LPARAM) m_pItem->m_pos);
}

void CNWSchdDlg::OnCancel() 
{
	m_pItem->m_bDeferSchedule = FALSE;

	CDialog::OnCancel();
}

void CNWSchdDlg::OnEnableNotifications() 
{
	UpdateData();

	m_pItem->m_sNotifyDisabled = m_DisableNotifications;

	if(m_SchedulePage.GetSafeHwnd())
		m_SchedulePage.SendMessage(UM_ENABLE,m_DisableNotifications);

	if(m_EventPage.GetSafeHwnd())
		m_EventPage.SendMessage(UM_ENABLE,m_DisableNotifications);
}

void CNWSchdDlg::OnHelpbtn() 
{
	WinHelp(102);
}

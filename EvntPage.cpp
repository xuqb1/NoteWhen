/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's event property page.
*/

#include "stdafx.h"

#include <io.h>
#include <mmsystem.h>

#include "notewhen.h"
#include "EvntPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNWEventPage property page

CString g_strLastSound;


IMPLEMENT_DYNCREATE(CNWEventPage, CPropertyPage)

CNWEventPage::CNWEventPage() : CPropertyPage(CNWEventPage::IDD)
{
}

CNWEventPage::CNWEventPage(UINT nIDTemplate, UINT nIDCaption) : 
								CPropertyPage(nIDTemplate,nIDCaption)
{
	//{{AFX_DATA_INIT(CNWEventPage)
	m_AlarmEnabled = FALSE;
	m_AlwaysDisplay = FALSE;
	m_Days = 0;
	m_Hours = 0;
	m_Minutes = 0;
	m_SoundFile = _T("");
	//}}AFX_DATA_INIT
}

CNWEventPage::~CNWEventPage()
{
}

void CNWEventPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNWEventPage)
	DDX_Control(pDX, IDC_MINUTES_SPIN, m_MinutesSpin);
	DDX_Control(pDX, IDC_HOURS_SPIN, m_HoursSpin);
	DDX_Control(pDX, IDC_DAYS_SPIN, m_DaysSpin);
	DDX_Check(pDX, IDC_ALARM_ENABLED, m_AlarmEnabled);
	DDX_Check(pDX, IDC_ALWAYS_DISPLAY, m_AlwaysDisplay);
	DDX_Text(pDX, IDC_DAYS, m_Days);
	DDV_MinMaxInt(pDX, m_Days, 0, 99);
	DDX_Text(pDX, IDC_HOURS, m_Hours);
	DDV_MinMaxInt(pDX, m_Hours, 0, 23);
	DDX_Text(pDX, IDC_MINUTES, m_Minutes);
	DDV_MinMaxInt(pDX, m_Minutes, 0, 59);
	DDX_Text(pDX, IDC_SOUND_FILE, m_SoundFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNWEventPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNWEventPage)
	ON_BN_CLICKED(IDC_ALARM_ENABLED, OnAlarmEnabled)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_ENABLE, OnUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWEventPage message handlers

void CNWEventPage::SetDlgFields()
{
	m_Days = m_pItem->m_sNotifyDays;
	m_Hours = m_pItem->m_sNotifyHours;
	m_Minutes = m_pItem->m_sNotifyMinutes;
	m_AlwaysDisplay = m_pItem->m_sAlwaysDisplay;
	m_AlarmEnabled = m_pItem->m_sAlarmEnabled;
	m_SoundFile = m_pItem->m_strSoundFile;

	if(m_SoundFile.IsEmpty())
		m_SoundFile = g_strLastSound;
}

BOOL CNWEventPage::GetDlgFields()
{
	CTime dateStart, dateEnd, timeStart;

	if(!UpdateData(TRUE))
		return(FALSE);

	m_pItem->m_sNotifyDays = m_Days;
	m_pItem->m_sNotifyHours = m_Hours;
	m_pItem->m_sNotifyMinutes = m_Minutes;
	m_pItem->m_sAlwaysDisplay = m_AlwaysDisplay;
	m_pItem->m_sAlarmEnabled = m_AlarmEnabled;
	m_pItem->m_strSoundFile = m_SoundFile;

	g_strLastSound = 	m_SoundFile;

	return(TRUE);
}

BOOL CNWEventPage::SavePage()
{
	if(!GetSafeHwnd())
		return(TRUE);

	if(!GetDlgFields())
		return(FALSE);

	return(TRUE);
}

BOOL CNWEventPage::OnInitDialog() 
{
	SetDlgFields();

	CPropertyPage::OnInitDialog();

	m_DaysSpin.SetRange(0,99);
	m_HoursSpin.SetRange(0,23);
	m_MinutesSpin.SetRange(0,59);
	
	OnAlarmEnabled();

	// Set initial state
	OnUpdate((m_pItem->m_sFirstTime?FALSE:m_pItem->m_sNotifyDisabled),0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Handles the Alarm enabled checkbox
void CNWEventPage::OnAlarmEnabled() 
{
	BOOL bEnable = IsDlgButtonChecked(IDC_ALARM_ENABLED);

	GetDlgItem(IDC_SOUND_FILE)->EnableWindow(bEnable);
	GetDlgItem(IDC_BROWSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_TEST)->EnableWindow(bEnable);
}

// Handles the Test button
void CNWEventPage::OnTest() 
{
	UpdateData();

	if(!m_SoundFile.IsEmpty() && access(m_SoundFile,0) == 0) 
		sndPlaySound(m_SoundFile,SND_NODEFAULT|SND_NOSTOP|SND_SYNC);
	else AfxMessageBox("Please enter a valid wave file name.",
					MB_ICONWARNING|MB_OK);
}

// Parses a filename into the specified parts and returns a CString that
// consists of the specified parts.
CString CNWEventPage::SplitFileName(LPCTSTR lpszPath, int nSplit)
{
	CString strResult;

	char szPath[800],szDrive[800],szFileName[800],szExt[800];
	_splitpath(lpszPath,szDrive,szPath,szFileName,szExt);

	if(nSplit & DRIVE)
		strResult += szDrive;
	if(nSplit & PATH)
		strResult += szPath;
	if(nSplit & FNAME)
		strResult += szFileName;
	if(nSplit & EXT)
		strResult += szExt;

	return(strResult);
}

void CNWEventPage::OnBrowse() 
{
	CFileDialog dlgFile(TRUE,"",NULL, 
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		"WAVE (*.wav)|*.wav|All (*.*)|*.*||",this);
	CString strFile, strPath;

	UpdateData(TRUE);
	strPath = SplitFileName(m_SoundFile,DRIVE|PATH);
	strFile = SplitFileName(m_SoundFile,FNAME|EXT);

	dlgFile.m_ofn.lpstrTitle = "Select wave file";
	dlgFile.m_ofn.lpstrFile = strFile.GetBufferSetLength(MAX_PATH);
	dlgFile.m_ofn.nMaxFile = MAX_PATH;
	dlgFile.m_ofn.lpstrInitialDir = strPath.GetBufferSetLength(MAX_PATH);

	// Display the dialog
	if(dlgFile.DoModal() == IDOK)
	{
		// Get the results
		strFile = dlgFile.GetPathName();
		SetDlgItemText(IDC_SOUND_FILE,strFile);
	}
}

// Enable/disable controls based on Enable schedule value
LRESULT CNWEventPage::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_ALARM_ENABLED)->EnableWindow(!wParam);
	GetDlgItem(IDC_ALWAYS_DISPLAY)->EnableWindow(!wParam);
	GetDlgItem(IDC_DAYS)->EnableWindow(!wParam);
	GetDlgItem(IDC_DAYS_SPIN)->EnableWindow(!wParam);
	GetDlgItem(IDC_HOURS)->EnableWindow(!wParam);
	GetDlgItem(IDC_HOURS_SPIN)->EnableWindow(!wParam);
	GetDlgItem(IDC_MINUTES)->EnableWindow(!wParam);
	GetDlgItem(IDC_MINUTES_SPIN)->EnableWindow(!wParam);
	GetDlgItem(IDC_ALWAYS_DISPLAY)->EnableWindow(!wParam);

	// For some controls the enable state is based on the
	// their own controlling checkbox, so let the normal 
	// routine handle this case
	if(!wParam)
	{
		OnAlarmEnabled();
	}
	else
	{	  
		// Disable the sound fields
		GetDlgItem(IDC_BROWSE)->EnableWindow(!wParam);
		GetDlgItem(IDC_TEST)->EnableWindow(!wParam);
		GetDlgItem(IDC_SOUND_FILE)->EnableWindow(!wParam);
	}

	return(0);
}

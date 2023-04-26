/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's calendar dialog.
*/

#include "stdafx.h"

#include "NoteWhen.h"
#include "maindlg.h"
#include "MonthDlg.h"
#include "schddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static MONTHDAYSTATE mdState[10];
#define BOLDDAY(ds,iDay)	(ds |= (0x00000001<<(iDay-1)))
#define ISBOLDDAY(ds,iDay) (ds & (0x00000001<<(iDay-1)))

/////////////////////////////////////////////////////////////////////////////
// CNWMonthDlg dialog


void CNWMonthDlg::LoadEntries()
{
	static int nSel = 0;
	SYSTEMTIME stCurrent;
	DWORD dwState = 0;

	m_List.DeleteAllItems();

	GetDlgItem(IDC_CHANGE)->EnableWindow(FALSE);
	m_Calendar.GetCurSel(&stCurrent);

	if(!ISBOLDDAY(mdState[1],stCurrent.wDay))
		return;

	POSITION pos;
	int nItem = 0;
	CNWDocument *pDoc = GETAPP()->GetDocument();
	CNWSchedule& schedule = ((CNWDlg *)AfxGetMainWnd())->GetSchedule();


	for(int j = 0; j < pDoc->GetItemCount(); j++)
	{
		CNWItem *pItem = pDoc->GetItem(j,pos);
		char szTemp[100];

		CTime dateCurrent(stCurrent.wYear,
								stCurrent.wMonth,
								stCurrent.wDay,
								0,0,0);
		CTime timeCurrent = dateCurrent;


		// Make the entry bold, if necessary
		if(schedule.ShouldRun(timeCurrent,dateCurrent,pItem,TRUE) &&
									!pItem->m_sNotifyDisabled)
		{
			LVITEM lvi;

			lvi.mask = LVIF_PARAM;
			lvi.iItem = nItem;
			lvi.iSubItem = 0;
			lvi.lParam = (LPARAM) pItem->m_pos;

			m_List.InsertItem(&lvi);

			int nHour = pItem->m_timeStart.GetHour();
			char chAMPM = 'a';

			if(nHour == 0)
				nHour = 12;
			else if(nHour == 12)
				chAMPM = 'p';
			else if(nHour > 12)
			{
				nHour -= 12;
				chAMPM = 'p';
			}

			sprintf(szTemp,"%02d:%02d%c",nHour,
					pItem->m_timeStart.GetMinute(),
					chAMPM);

			m_List.SetItemText(nItem,0,szTemp);

			// Add the message text
			CString strTemp = pItem->m_strNoteText;

			strTemp.Replace('\r',' ');
			strTemp.Replace('\n',' ');
			m_List.SetItemText(nItem,1,strTemp);

			nItem++;
		}
	}

	GetDlgItem(IDC_CHANGE)->EnableWindow(nItem > 0);

	m_List.SortItems(SortFunc,0);
	m_List.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
}


CNWMonthDlg::CNWMonthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNWMonthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNWMonthDlg)
	//}}AFX_DATA_INIT
}


void CNWMonthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNWMonthDlg)
	DDX_Control(pDX, IDC_DAY, m_List);
	DDX_Control(pDX, IDC_CALENDAR, m_Calendar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNWMonthDlg, CDialog)
	//{{AFX_MSG_MAP(CNWMonthDlg)
	ON_NOTIFY(MCN_SELCHANGE, IDC_CALENDAR, OnSelchangeCalendar)
	ON_NOTIFY(MCN_SELECT, IDC_CALENDAR, OnSelectCalendar)
	ON_NOTIFY(MCN_GETDAYSTATE, IDC_CALENDAR, OnGetdaystateCalendar)
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	ON_NOTIFY(NM_DBLCLK, IDC_DAY, OnDblclkDay)
	ON_NOTIFY(NM_CLICK, IDC_DAY, OnClickDay)
	ON_BN_CLICKED(IDC_HELPBTN, OnHelpbtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWMonthDlg message handlers

BOOL CNWMonthDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Calendar.SetColor(MCSC_BACKGROUND,GetSysColor(COLOR_BTNFACE));

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_List.SetFont(GetFont());

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;

	lvc.iSubItem = 0;
	lvc.pszText = "Ê±¼ä";
	lvc.cx = 45;
	m_List.InsertColumn(0,&lvc);

	lvc.iSubItem = 1;
	lvc.pszText = "ÃèÊö";
	lvc.cx = 300;
	m_List.InsertColumn(1,&lvc);

	LoadEntries();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNWMonthDlg::OnSelchangeCalendar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LoadEntries();	

	*pResult = 0;
}

void CNWMonthDlg::OnSelectCalendar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CNWMonthDlg::OnGetdaystateCalendar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CNWDocument *pDoc = GETAPP()->GetDocument();
	int i, nMax, nMonth, nYear;
	NMDAYSTATE* pDayState= (NMDAYSTATE *) pNMHDR;
	CNWSchedule& schedule = ((CNWDlg *)AfxGetMainWnd())->GetSchedule();

	nMax = pDayState->cDayState;
	nMonth = pDayState->stStart.wMonth;
	nYear = pDayState->stStart.wYear;

	for(i = 0; i < nMax; i++)
	{
		POSITION pos;

		mdState[i] = (MONTHDAYSTATE) 0;

		for(int j = 0; j < pDoc->GetItemCount(); j++)
		{
			CNWItem *pItem = pDoc->GetItem(j,pos);

			for(int nDay = 1; nDay <= 31; nDay++)
			{
				CTime dateCurrent(nYear,
										nMonth,
										nDay,
										0,0,0);
				CTime timeCurrent = dateCurrent;


				// Did we wrap past the end of the month?
				if(dateCurrent.GetMonth() != nMonth)
					break;

				// Make the entry bold, if necessary
				if(schedule.ShouldRun(timeCurrent,dateCurrent,pItem,TRUE))
					BOLDDAY(mdState[i],nDay);
			}
		}

		nMonth++;

		if(nMonth > 12)
		{
			nMonth = 1;
			nYear++;
		}
	}

	pDayState->prgDayState = mdState;

	*pResult = 0;
}

// Called to change the schedule of a particular entry
void CNWMonthDlg::OnChange() 
{
	int nSel = GetSelectedItem();

	if(nSel >= 0)
	{
		CNWSchdDlg dlgSchedule(this);

		POSITION pos = (POSITION) m_List.GetItemData(nSel);

		if(pos)
		{
			CNWDocument *pDoc = GETAPP()->GetDocument();
			CNWItem *pItem = pDoc->GetItem(pos);
			int nMonth, nYear;

			nMonth = pItem->m_dateStart.GetMonth();
			nYear = pItem->m_dateStart.GetYear();

			dlgSchedule.SetItem(pItem);

			if(dlgSchedule.DoModal() == IDOK)
			{
				SYSTEMTIME stStart, stEnd;

				int nMaxMonths = m_Calendar.GetMonthRange(&stStart,&stEnd,GMR_DAYSTATE);
				int nMonth = stStart.wMonth;
				CNWSchedule& schedule = ((CNWDlg *)AfxGetMainWnd())->GetSchedule();

				memset(mdState,0,sizeof(mdState));

				for(int i = 0; i < nMaxMonths; i++)
				{
					for(int j = 0; j < pDoc->GetItemCount(); j++)
					{
						pItem = pDoc->GetItem(j,pos);

						for(int nDay = 1; nDay <= 31; nDay++)
						{
							CTime dateCurrent(nYear,
													nMonth,
													nDay,
													0,0,0);
							CTime timeCurrent = dateCurrent;


							// Did we wrap past the end of the month?
							if(dateCurrent.GetMonth() != nMonth)
								break;

							// Make the entry bold, if necessary
							if(schedule.ShouldRun(timeCurrent,dateCurrent,pItem,TRUE))
								BOLDDAY(mdState[i],nDay);
						}
					}

					nMonth++;

					if(nMonth > 12)
					{
						nMonth = 1;
						nYear++;
					}
				}

				m_Calendar.SetDayState(nMaxMonths,mdState);
			}
		}
	}

	LoadEntries();	
}

void CNWMonthDlg::OnDblclkDay(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnChange();
	
	*pResult = 0;
}

int CNWMonthDlg::GetSelectedItem()
{
	int nItemCount = m_List.GetItemCount();

	// Make sure there's only one selected item
	if(m_List.GetSelectedCount() == 1) 
	{
		// Search for the selected item
		for(int i = 0; i < nItemCount; i++) 
		{
			if(m_List.GetItemState(i,LVIS_SELECTED))
				return(i);
		}
	}

	return(-1);
}

// Sort callback that's used to sort listview items
int CALLBACK CNWMonthDlg::SortFunc(LPARAM lp1, LPARAM lp2, LPARAM lParam) 
{
	POSITION pos1 = (POSITION) lp1;
	POSITION pos2 = (POSITION) lp2;
	CNWDocument *pDoc = GETAPP()->GetDocument();

	CNWItem *pItem1 = pDoc->GetItem(pos1);
	CNWItem *pItem2 = pDoc->GetItem(pos2);

	if(pItem1->m_timeStart > pItem2->m_timeStart)
		return(1);

	if(pItem1->m_timeStart < pItem2->m_timeStart)
		return(-1);

	return(0);
}

void CNWMonthDlg::OnClickDay(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;

	GetCursorPos(&pt);
	m_List.ScreenToClient(&pt);

	GetDlgItem(IDC_CHANGE)->EnableWindow(m_List.HitTest(pt) >= 0);
	
	*pResult = 0;
}

void CNWMonthDlg::OnHelpbtn() 
{
	WinHelp(101);
}

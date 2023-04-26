/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's schedule page.
*/

#include "stdafx.h"

#include "NoteWhen.h"
#include "SchdPage.h"
#include "Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int g_anItems[][50] = 
{
	{-1},
	{IDC_DAILY_EVERY,IDC_DAILY_EVERY_NUM,IDC_DAILY_EVERY_SPIN,IDC_DAILY_DAYS,IDC_DAILY_WEEKDAYS,-1},
	{IDC_WEEKLY_EVERY,IDC_WEEKLY_EVERY_NUM,IDC_WEEKLY_EVERY_SPIN,IDC_WEEKLY_WEEKS,IDC_WEEKLY_SUNDAY,IDC_WEEKLY_MONDAY,IDC_WEEKLY_TUESDAY,IDC_WEEKLY_WEDNESDAY,IDC_WEEKLY_THURSDAY,IDC_WEEKLY_FRIDAY,IDC_WEEKLY_SATURDAY,-1},
	{IDC_MONTHLY_ON,IDC_MONTHLY_COUNT,IDC_MONTHLY_DAY_NUM,IDC_MONTHLY_DAY,
		IDC_MONTHLY_ON_DAY,IDC_MONTHLY_DAY_SPIN,IDC_MONTHLY_MONTHS,
		IDC_MONTHLY_JAN,IDC_MONTHLY_FEB,IDC_MONTHLY_MAR,
		IDC_MONTHLY_APR,IDC_MONTHLY_MAY,IDC_MONTHLY_JUN,IDC_MONTHLY_JUL,
		IDC_MONTHLY_AUG,IDC_MONTHLY_SEP,IDC_MONTHLY_OCT,IDC_MONTHLY_NOV,
		IDC_MONTHLY_DEC,-1},
	{-1},
	{-1},
	{-1},
	{-1},
};

/////////////////////////////////////////////////////////////////////////////
// CNWSchedPage property page

IMPLEMENT_DYNCREATE(CNWSchedPage, CPropertyPage)

CNWSchedPage::CNWSchedPage() : CPropertyPage(CNWSchedPage::IDD)
{
}

CNWSchedPage::CNWSchedPage(UINT nIDTemplate, UINT nIDCaption) : 
								CPropertyPage(nIDTemplate,nIDCaption)
{
	//{{AFX_DATA_INIT(CNWSchedPage)
	m_DailyEveryNum = 1;
	m_WeeklyEveryNum = 1;
	m_MonthlyDayNum = 1;
	//}}AFX_DATA_INIT

	m_nCurrPage = 0;
	m_bIsNew = FALSE;
}

CNWSchedPage::~CNWSchedPage()
{
}

void CNWSchedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNWSchedPage)
	DDX_Control(pDX, IDC_MONTHLY_DAY, m_MonthlyDay);
	DDX_Control(pDX, IDC_MONTHLY_COUNT, m_MonthlyCount);
	DDX_Control(pDX, IDC_MONTHLY_DAY_SPIN, m_MonthlyDaySpin);
	DDX_Control(pDX, IDC_WEEKLY_EVERY_SPIN, m_WeeklyEverySpin);
	DDX_Control(pDX, IDC_DAILY_EVERY_SPIN, m_DailyEverySpin);
	DDX_Control(pDX, IDC_END_DATE, m_EndDate);
	DDX_Control(pDX, IDC_START_TIME, m_StartTime);
	DDX_Control(pDX, IDC_START_DATE, m_StartDate);
	DDX_Control(pDX, IDC_WHEN, m_When);
	DDX_Text(pDX, IDC_DAILY_EVERY_NUM, m_DailyEveryNum);
	DDV_MinMaxInt(pDX, m_DailyEveryNum, 1, 99);
	DDX_Text(pDX, IDC_WEEKLY_EVERY_NUM, m_WeeklyEveryNum);
	DDV_MinMaxInt(pDX, m_WeeklyEveryNum, 1, 52);
	DDX_Text(pDX, IDC_MONTHLY_DAY_NUM, m_MonthlyDayNum);
	DDV_MinMaxInt(pDX, m_MonthlyDayNum, 1, 31);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNWSchedPage, CPropertyPage)
	//{{AFX_MSG_MAP(CNWSchedPage)
	ON_CBN_SELCHANGE(IDC_WHEN, OnSelchangeWhen)
	ON_BN_CLICKED(IDC_DAILY_EVERY, OnDailyEvery)
	ON_BN_CLICKED(IDC_DAILY_WEEKDAYS, OnDailyWeekdays)
	ON_BN_CLICKED(IDC_MONTHLY_ON_DAY, OnMonthlyOnDay)
	ON_BN_CLICKED(IDC_MONTHLY_ON, OnMonthlyOn)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_ENABLE, OnUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWSchedPage message handlers
void CNWSchedPage::SetDlgFields()
{
	CTime timeCurrent = CTime::GetCurrentTime();
	CTime dateCurrent(timeCurrent.GetYear(),timeCurrent.GetMonth(),
						timeCurrent.GetDay(),0,0,0);

	// Set start and end date
	if(m_pItem->m_dateStart.GetYear() == 1980)
		m_StartDate.SetTime(&dateCurrent);
	else m_StartDate.SetTime(&m_pItem->m_dateStart);
	if(m_pItem->m_sEnd)
	{
		m_EndDate.SetTime(&m_pItem->m_dateEnd);
	}

	// Set the start time
	if(m_pItem->m_sFirstTime)
		m_StartTime.SetTime(&timeCurrent);
	else m_StartTime.SetTime(&m_pItem->m_timeStart);

	// Set the script type
	m_When.SetCurSel((int) m_pItem->m_eType);
	OnSelchangeWhen();

	// Set the specific data, based on the script type
	switch(m_pItem->m_eType)
	{
		case CNWItem::ONCE:
			break;

		case CNWItem::DAILY: 
			{
				DAILY* pDaily = (DAILY *) m_pItem->m_abyData;

				// Validate the every xx days value
				CheckDlgButton(IDC_DAILY_EVERY,!pDaily->m_sWeekDays);
				CheckDlgButton(IDC_DAILY_WEEKDAYS,pDaily->m_sWeekDays);

				if(pDaily->m_sWeekDays)
				{
					SetDlgItemInt(IDC_DAILY_EVERY_NUM,1);
					OnDailyWeekdays();
				}
				else
				{
					short sDays;

					sDays = pDaily->m_sDays;
					if(sDays < 1 || sDays > 99)
						sDays = 1;

					SetDlgItemInt(IDC_DAILY_EVERY_NUM,sDays);
					OnDailyEvery();
				}
			}
			break;

		case CNWItem::WEEKLY:
			{
				WEEKLY* pWeekly = (WEEKLY *) m_pItem->m_abyData;

				short sWeeks = pWeekly->m_sWeeks;

				if(sWeeks < 1 || sWeeks > 52)
					sWeeks = 1;

				SetDlgItemInt(IDC_WEEKLY_EVERY_NUM,sWeeks); 

				for(int i = 0; i < 7; i++)
				{
					if(pWeekly->m_sDaysOfWeek & (1<<i))
						CheckDlgButton(IDC_WEEKLY_SUNDAY+i,TRUE);
				}
			}
			break;

		case CNWItem::MONTHLY:
			{
				MONTHLY* pMonthly = (MONTHLY *) m_pItem->m_abyData;

				if(pMonthly->m_sOnDays)
				{
					CheckDlgButton(IDC_MONTHLY_ON,TRUE);
					m_MonthlyCount.SetCurSel(pMonthly->m_sOnDayNum);
					m_MonthlyDay.SetCurSel(pMonthly->m_sOnDayName);
					OnMonthlyOn();
				}
				else
				{
					CheckDlgButton(IDC_MONTHLY_ON_DAY,TRUE);

					short sMonthDay = pMonthly->m_sOnMonthDay;

					if(sMonthDay < 1 || sMonthDay > 31)
						sMonthDay = 1;

					SetDlgItemInt(IDC_MONTHLY_DAY_NUM,sMonthDay);

					OnMonthlyOnDay();
				}

				// Check the days selected
				for(int i = 0; i < 12; i++)
				{
					if(pMonthly->m_sOnMonths & (1<<i))
						CheckDlgButton(IDC_MONTHLY_JAN+i,TRUE);
				}
			}
			break;
	}
}

BOOL CNWSchedPage::GetDlgFields()
{
	CTime dateStart, dateEnd, timeStart;
	int nSel = m_When.GetCurSel();

	if(!UpdateData(TRUE))
		return(FALSE);

	// Validate the start and end date
	m_StartDate.GetTime(dateStart);
	if(m_EndDate.GetTime(dateEnd) != GDT_NONE)
	{
		CTime timeCurrent = CTime::GetCurrentTime();
		CTime dateCurrent(timeCurrent.GetYear(),
							timeCurrent.GetMonth(),
							timeCurrent.GetDay(),
							0,0,0);

		if(dateStart < dateCurrent && nSel == CNWItem::ONCE)
		{
			::MessageBox(::GetParent(m_hWnd),"Please enter a starting date equal to or later than the current date.",
						"NoteWhen",
						MB_ICONWARNING|MB_OK);
			return(FALSE);
		}
		else if(dateEnd < dateStart)
		{
			::MessageBox(::GetParent(m_hWnd),"Please enter an ending date that is equal or later than the starting date.",
						"NoteWhen",
						MB_ICONWARNING|MB_OK);
			return(FALSE);
		}

		m_pItem->m_sEnd = TRUE; 
		m_pItem->m_dateEnd = dateEnd;
	}
	else m_pItem->m_sEnd = FALSE;

	// Set the start date
	m_pItem->m_dateStart = dateStart;

	// Set the start time
	m_StartTime.GetTime(timeStart);
	m_pItem->m_timeStart = timeStart;

	m_pItem->m_eType = (CNWItem::SCHD_TYPE) nSel;

	switch(m_pItem->m_eType)
	{
		case CNWItem::ONCE:
			break;

		case CNWItem::DAILY:
			{
				DAILY daily;

				memset(&daily,0,sizeof(daily));

				// Validate the every xx days value
				if(IsDlgButtonChecked(IDC_DAILY_EVERY))
				{
					short sDays = GetDlgItemInt(IDC_DAILY_EVERY_NUM,NULL,FALSE);

					// Days must be between 1 and 99
					if(sDays >= 1 && sDays <= 99)
					{
						daily.m_sDays = sDays;
						daily.m_sWeekDays = FALSE;
					}
					else 
					{
						::MessageBox(::GetParent(m_hWnd),
							"Please enter between 1 and 99 days",
							"NoteWhen",
							MB_ICONWARNING);
						return(FALSE);
					}
				}
				else
				{
					// We're only using weekdays
					daily.m_sDays = 0;
					daily.m_sWeekDays = TRUE;
				}

				// Save the data
				memcpy(m_pItem->m_abyData,&daily,sizeof(DAILY));
			}
			break;

		case CNWItem::WEEKLY:
			{
				WEEKLY weekly;
				int sWeeks = GetDlgItemInt(IDC_WEEKLY_EVERY_NUM,NULL,FALSE); 

				memset(&weekly,0,sizeof(weekly));
				if(sWeeks >= 1 && sWeeks <= 52)
				{
					weekly.m_sWeeks = sWeeks;
					weekly.m_sDaysOfWeek = 0;

					for(int i = 0; i < 7; i++)
					{
						if(IsDlgButtonChecked(IDC_WEEKLY_SUNDAY+i))
							weekly.m_sDaysOfWeek |= (1<<i);
					}

					if(!weekly.m_sDaysOfWeek)
					{
						::MessageBox(::GetParent(m_hWnd),
							"Please select at least one day of the week to run the script",
							"NoteWhen",
							MB_ICONWARNING);
						return(FALSE);
					}

				}
				else
				{
					::MessageBox(::GetParent(m_hWnd),
						"Please enter between 1 and 52 weeks",
						"NoteWhen",
						MB_ICONWARNING);
					return(FALSE);
				}

				// Save the data
				memcpy(m_pItem->m_abyData,&weekly,sizeof(weekly));
			}
			break;

		case CNWItem::MONTHLY:
			{
				MONTHLY monthly;

				memset(&monthly,0,sizeof(monthly));
				if(IsDlgButtonChecked(IDC_MONTHLY_ON))
				{
					monthly.m_sOnDayNum = (MONTHLY::DAY_TYPE) m_MonthlyCount.GetCurSel();
					monthly.m_sOnDayName = m_MonthlyDay.GetCurSel();
					monthly.m_sOnDays = TRUE;
				}
				else
				{
					monthly.m_sOnMonthDay = GetDlgItemInt(IDC_MONTHLY_DAY_NUM,NULL,FALSE);
					monthly.m_sOnDays = FALSE;
				}

				// Save the months selected
				monthly.m_sOnMonths = 0;
				for(int i = 0; i < 12; i++)
				{
					if(IsDlgButtonChecked(IDC_MONTHLY_JAN+i))
						monthly.m_sOnMonths |= (1<<i);
				}

				if(!monthly.m_sOnMonths)
				{
					::MessageBox(::GetParent(m_hWnd),
						"Please select at least one month to run the script",
						"NoteWhen",
						MB_ICONWARNING);
					return(FALSE);
				}

				// Save the data
				memcpy(m_pItem->m_abyData,&monthly,sizeof(monthly));
			}
			break;
	}

	return(TRUE);
}


BOOL CNWSchedPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(!m_strTitle.IsEmpty())
		SetWindowText(m_strTitle);

	m_StartTime.SetFormat("hh:mm tt");

	// Set default ranges for spin controls
	m_DailyEverySpin.SetRange(1,99);
	m_WeeklyEverySpin.SetRange(1,52);
	m_MonthlyDaySpin.SetRange(1,31);

	CheckDlgButton(IDC_DAILY_EVERY,TRUE);
	OnDailyEvery();
	SetDlgItemInt(IDC_DAILY_EVERY_NUM,1);
	SetDlgItemInt(IDC_WEEKLY_EVERY_NUM,1);
	SetDlgItemInt(IDC_MONTHLY_DAY_NUM,1);

	CheckDlgButton(IDC_MONTHLY_ON,TRUE);
	m_MonthlyCount.SetCurSel(0);
	m_MonthlyDay.SetCurSel(1);
	OnMonthlyOn();

	// These values are the defaults, override these with 
	// the real script values
	m_When.SetCurSel(0);

	CTime time = CTime::GetCurrentTime();
//	time += CTimeSpan(365,0,0,0);

	m_EndDate.SetTime(&time);
	DateTime_SetSystemtime(m_EndDate.GetSafeHwnd(),GDT_NONE,NULL);

	// Set the actual script values
	if(!m_bIsNew)
		SetDlgFields();

	// Set initial state
	OnUpdate((m_pItem->m_sFirstTime?FALSE:m_pItem->m_sNotifyDisabled),0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNWSchedPage::OnSelchangeWhen() 
{
	int i = 0;
	int nSel = m_When.GetCurSel();

	if(nSel != m_nCurrPage)
	{
		if(m_nCurrPage > 0)
		{
			i = 0;
			while(g_anItems[m_nCurrPage][i] > 0)
			{
				GetDlgItem(g_anItems[m_nCurrPage][i])->ShowWindow(SW_HIDE);
				i++;
			}
		}

		i = 0;
		while(g_anItems[nSel][i] > 0)
		{
			GetDlgItem(g_anItems[nSel][i])->ShowWindow(SW_SHOW);
			i++;
		}

		// Enable ending date for every option except One Time Only
		GetDlgItem(IDC_END_DATE)->EnableWindow(nSel != CNWItem::ONCE);
	}

	m_nCurrPage = nSel;
}

BOOL CNWSchedPage::SavePage() 
{
	if(!GetSafeHwnd())
		return(TRUE);

	if(!GetDlgFields())
		return(FALSE);

	return(TRUE);
}

void CNWSchedPage::OnDailyEvery() 
{
	GetDlgItem(IDC_DAILY_EVERY_NUM)->EnableWindow(TRUE);
	GetDlgItem(IDC_DAILY_EVERY_SPIN)->EnableWindow(TRUE);
	CheckDlgButton(IDC_DAILY_WEEKDAYS,FALSE);
}

void CNWSchedPage::OnDailyWeekdays() 
{
	GetDlgItem(IDC_DAILY_EVERY_NUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_DAILY_EVERY_SPIN)->EnableWindow(FALSE);
	CheckDlgButton(IDC_DAILY_EVERY,FALSE);
}

void CNWSchedPage::OnMonthlyOn() 
{
	GetDlgItem(IDC_MONTHLY_COUNT)->EnableWindow(TRUE);
	GetDlgItem(IDC_MONTHLY_DAY)->EnableWindow(TRUE);
	GetDlgItem(IDC_MONTHLY_DAY_NUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_MONTHLY_DAY_SPIN)->EnableWindow(FALSE);
	CheckDlgButton(IDC_MONTHLY_ON_DAY,FALSE);
}

void CNWSchedPage::OnMonthlyOnDay() 
{
	GetDlgItem(IDC_MONTHLY_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_MONTHLY_DAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_MONTHLY_DAY_NUM)->EnableWindow(TRUE);
	GetDlgItem(IDC_MONTHLY_DAY_SPIN)->EnableWindow(TRUE);
	CheckDlgButton(IDC_MONTHLY_ON,FALSE);
}

// Enable/disable controls based on Enable schedule value
LRESULT CNWSchedPage::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	int nSel = m_When.GetCurSel();

	GetDlgItem(IDC_WHEN)->EnableWindow(!wParam);
	GetDlgItem(IDC_START_DATE)->EnableWindow(!wParam);
	GetDlgItem(IDC_START_TIME)->EnableWindow(!wParam);
	GetDlgItem(IDC_END_DATE)->EnableWindow(!wParam && nSel != CNWItem::ONCE);

	while(g_anItems[m_nCurrPage][i] > 0)
	{
		GetDlgItem(g_anItems[m_nCurrPage][i])->EnableWindow(!wParam);
		i++;
	}

	if(!wParam)
	{
		switch(nSel)
		{
			case CNWItem::DAILY: 
				{
					if(IsDlgButtonChecked(IDC_DAILY_WEEKDAYS))
						OnDailyWeekdays();
					else OnDailyEvery();
				}
				break;

			case CNWItem::MONTHLY:
				{
					if(IsDlgButtonChecked(IDC_MONTHLY_ON))
						OnMonthlyOn();
					else OnMonthlyOnDay();
				}
				break;
		}
	}

	return(0);
}

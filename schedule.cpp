/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's schedule object.
*/

#include "stdafx.h"

#include <io.h>
#include <mmsystem.h>
#include <ctype.h>

#include "doc.h"
#include "schedule.h"
#include "note.h"

CNWSchedule::CNWSchedule()
{
} 

void CNWSchedule::Init(CNWDocument *pDocument)
{
	m_pDocument = pDocument;
}

void CNWSchedule::ResetTasks(int nIndex)
{
}


BOOL CNWSchedule::ShouldRun(CTime timeCurrent, CTime dateCurrent, CNWItem *pItem,
									 BOOL bCheckCalendar)
{
	BOOL bShouldRun = FALSE;

	CTime dateStart(pItem->m_dateStart.GetYear(),
					pItem->m_dateStart.GetMonth(),
					pItem->m_dateStart.GetDay(),
					pItem->m_timeStart.GetHour(),
					pItem->m_timeStart.GetMinute(),
					0);
	CTime dateEnd(pItem->m_dateEnd.GetYear(),
					pItem->m_dateEnd.GetMonth(),
					pItem->m_dateEnd.GetDay(),
					0,0,0);
	CTime dateCurrentTotal(timeCurrent.GetYear(),
							timeCurrent.GetMonth(),
							timeCurrent.GetDay(),
							timeCurrent.GetHour(),
							timeCurrent.GetMinute(),
							0);

	// If there's no schedule then get out now
	if(pItem->m_sNotifyDisabled)
	{
		// All finished...get out
		return(FALSE);
	}

	if(!bCheckCalendar)
	{
		// Make sure we haven't already run today
		if(dateCurrent.GetMonth() == pItem->m_dateLastRun.GetMonth() &&
			dateCurrent.GetDay() == pItem->m_dateLastRun.GetDay() &&
			dateCurrent.GetYear() == pItem->m_dateLastRun.GetYear())
		{
			return(FALSE);
		}

		// Check if prior notification is enabled
		if(pItem->m_sNotifyDays+pItem->m_sNotifyHours+pItem->m_sNotifyMinutes)
		{
			CTimeSpan diffDate(pItem->m_sNotifyDays,
						pItem->m_sNotifyHours,
						pItem->m_sNotifyMinutes,
						0);

			dateStart -= diffDate;
		}

		// Determine if the we've reached the starting hour yet
		if(timeCurrent.GetHour() < dateStart.GetHour())
			return(FALSE);

		// Determine if we've reached the starting minute yet
		if(timeCurrent.GetHour() == dateStart.GetHour() && 
			timeCurrent.GetMinute() < dateStart.GetMinute())
		{
			return(FALSE);
		}
	}

	if(pItem->m_eType == CNWItem::ONCE)
	{
		if(timeCurrent.GetMonth() != dateStart.GetMonth() ||
			timeCurrent.GetDay() != dateStart.GetDay() ||
			timeCurrent.GetYear() != dateStart.GetYear())
		{
			// Oops...one time only scripts must start on the 
			// exact date/time specified
			return(FALSE);
		}
	}
	else 
	{
		if(bCheckCalendar)
		{
			CTime dateTempStart(pItem->m_dateStart.GetYear(),
							pItem->m_dateStart.GetMonth(),
							pItem->m_dateStart.GetDay(),
							0,
							0,
							0);
			CTime dateTempCurrentTotal(timeCurrent.GetYear(),
									timeCurrent.GetMonth(),
									timeCurrent.GetDay(),
									0,
									0,
									0);

			// Is this calendar day outside the starting date?
			if(dateTempCurrentTotal < dateTempStart)
			{
				return(FALSE);
			}
		}
		else
		{
			// Ooopss...the current date is outside of the start
			// date, so get out
			if(dateCurrentTotal < dateStart)
			{
				return(FALSE);
			}
		}
	}

	bShouldRun = TRUE;

	if(pItem->m_sEnd)
	{
 		// If we're past the script's end date then 
		// we need to make sure the script doesn't start
		if(dateCurrent > dateEnd)
		{
			return(FALSE);
		}
	}

	// Is this script ready to run?
	if(bShouldRun)
	{
		// Check the special criteria for starting
		switch(pItem->m_eType)
		{
			case CNWItem::ONCE:
				break;

			case CNWItem::DAILY:
				{
					DAILY *pDaily = (DAILY *) pItem->m_abyData;
					
					// Is this a weekday script and is it a 
					// weekday?
					if(pDaily->m_sWeekDays)
					{
						if(timeCurrent.GetDayOfWeek() < 2 ||
								timeCurrent.GetDayOfWeek() > 6)
						{
							// Not a weekday, so don't run
							return(FALSE);
						}
					}
					else 
					{
						if(bCheckCalendar)
						{
							CTime dateTempStart(pItem->m_dateStart.GetYear(),
											pItem->m_dateStart.GetMonth(),
											pItem->m_dateStart.GetDay(),
											0,
											0,
											0);

							CTimeSpan diffDates = dateCurrent-dateTempStart;

							int nDiffDays = diffDates.GetDays();
							int nSpanDays = pDaily->m_sDays;
							int nDaysLeft = (nDiffDays%nSpanDays);

							if(diffDates != 0)
							{
								if(nDaysLeft)
									return(FALSE);
							}
						}
						else
						{
							// Make sure we have valid 'last run' date
							if(pItem->m_dateLastRun.GetYear() != 1980)
							{
								// Yep...see if it's been enough days yet
								if(dateCurrent-pItem->m_dateLastRun < pDaily->m_sDays)
								{
									return(FALSE);
								}
							}
						}
					}
					
				}
				break;

			case CNWItem::WEEKLY:
				{
					WEEKLY *pWeekly = (WEEKLY *) pItem->m_abyData;

					if(bCheckCalendar)
					{
						CTime dateTempStart(pItem->m_dateStart.GetYear(),
										pItem->m_dateStart.GetMonth(),
										pItem->m_dateStart.GetDay(),
										0,
										0,
										0);

						CTimeSpan diffDates = dateCurrent-dateTempStart;
						int nDiffDays = diffDates.GetDays();
						int nSpanDays = pWeekly->m_sWeeks*7;
						int nDaysLeft = (nDiffDays%nSpanDays);

						if(diffDates != 0)
						{
							if(diffDates < 0 || (nDaysLeft > 6))
								return(FALSE);
						}

					}
					else
					{
						CTimeSpan dateWeeks(pWeekly->m_sWeeks*7,0,0,0);

						// Determine how long we've waited
						if(dateCurrent-pItem->m_dateLastRun < dateWeeks &&
							dateCurrent != pItem->m_dateLastRun)
						{
							// It hasn't long enough yet, so bail out
							return(FALSE);
						}
					}

					// Determine if we're running on a day that the 
					// user has specified as acceptable
					if(pWeekly->m_sDaysOfWeek && 
						!(pWeekly->m_sDaysOfWeek & 1<<(dateCurrent.GetDayOfWeek()-1)))
					{
						return(FALSE);
					}
				}
				break;

			case CNWItem::MONTHLY:
				{
					MONTHLY *pMonthly = (MONTHLY *) pItem->m_abyData;

					if(pMonthly->m_sOnDays)
					{
						// If this isn't the day specified, then get out
						if(pMonthly->m_sOnDayName != dateCurrent.GetDayOfWeek()-1)
						{
							return(FALSE);
						}

						switch(pMonthly->m_sOnDayNum)
						{
							case MONTHLY::FIRST:
							case MONTHLY::SECOND:
							case MONTHLY::THIRD:
							case MONTHLY::FOURTH:
								{
									CTime dateTemp = dateCurrent;
									CTimeSpan dateOneWeek(7,0,0,0);

									// Back up the number of weeks specified
									// to see if we move to the previous month.  
									// If we don't then it means we're not at the 
									// particular day (for example, Monday) of this month
									for(int nWeek = 0; nWeek < 5; nWeek++)
									{
										dateTemp -= dateOneWeek;

										if(dateTemp.GetMonth() != dateCurrent.GetMonth())
											break;
									}

									if(nWeek != pMonthly->m_sOnDayNum)
										return(FALSE);
								}
								break;

							case MONTHLY::LAST:
								{
									CTime dateTemp = dateCurrent;
									CTimeSpan dateOneWeek(7,0,0,0);

									// Move forward a week to see if we
									// move to the next month. If we don't then
									// it means we haven't reached the 'last' particular
									// day (for example, Monday) of this month
									dateTemp += dateOneWeek;

									if(dateTemp.GetMonth() == dateCurrent.GetMonth())
									{
										return(FALSE);
									}
								}
								break;
						}
					}
					else
					{
						CTimeSpan dateOneDay(1,0,0,0);
						CTime dateTemp = dateCurrent+dateOneDay;

						// Determine if this day is the last day of this
						// particular month.  This is done by first determining
					 	// that the day isn't equal to the specified month
						// day.  If it's not, make sure that it's not the 'last'
						// day of the month.  For example, specifying a run
						// day of the 31st will still run on Feb 28th if it's
						// the last day of the month
			 			if(dateCurrent.GetDay() != pMonthly->m_sOnMonthDay)
						{
							if(dateTemp.GetMonth() == dateCurrent.GetMonth() ||
								dateCurrent.GetDay() > pMonthly->m_sOnMonthDay)
							{
								return(FALSE);
							}
						}
					}

					// Determine if we're running on a month that the 
					// user has specified as acceptable
					if(pMonthly->m_sOnMonths && 
						!(pMonthly->m_sOnMonths & 1<<(dateCurrent.GetMonth()-1)))
					{
						return(FALSE);
					}
				}
				break;
		}
	}

	return(bShouldRun);
}

BOOL CNWSchedule::Check()
{
	CTime timeCurrent = CTime::GetCurrentTime();
	CTime dateCurrent(timeCurrent.GetYear(),
							timeCurrent.GetMonth(),
							timeCurrent.GetDay(),
							0,0,0);

	// Iterate through each entry
	for(int i = 0; i < GetDocument()->GetItemCount(); i++)
	{
		POSITION pos;
		CNWItem *pItem = m_pDocument->GetItem(i,pos);
		BOOL bShouldRun = FALSE;

		// Ooops, the user is editing this item, so skip it
		if(pItem->m_bDeferSchedule)
			continue;

		// Display always visible notes, if necessary
		if(pItem->m_sNoteDisplay && !pItem->m_pWnd)
		{
			CNWNoteWnd *pNote = new CNWNoteWnd(pItem);
			pItem->m_pWnd = pNote;
		}

		// Determine if we should "run" this note's schedule
		bShouldRun = ShouldRun(timeCurrent,dateCurrent,pItem);

		// Should we run it?
		if(bShouldRun)
		{
			// Display the note if necessary
			if(!pItem->m_sNoteDisplay)
			{
				pItem->m_sNoteDisplay = TRUE;
				GetDocument()->SetModified();

				AfxGetMainWnd()->PostMessage(UM_UPDATE,UPD_ALL,(LPARAM) pItem->m_pos);
			}

			if(!pItem->m_pWnd)
			{
				// Okay, now let's 'run' the item
				if(pItem->m_sNoteDisplay && !pItem->m_pWnd)
				{
					CNWNoteWnd *pNote = new CNWNoteWnd(pItem);

					// Display the reminder note
					pItem->m_pWnd = pNote;
				}
			}

			// Save the last run time
			pItem->m_dateLastRun = dateCurrent;
			GetDocument()->SetModified();

			// Handle the alarm, if necessary
			if(pItem->m_sAlarmEnabled)
			{
				if(!pItem->m_strSoundFile.IsEmpty() && access(pItem->m_strSoundFile,0) == 0) 
					sndPlaySound(pItem->m_strSoundFile,SND_NODEFAULT|SND_NOSTOP|SND_SYNC);
			}
		}
	}

	return(TRUE);
}


/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's file storage object.
*/

#include "stdafx.h"

#include <io.h>

#include "notewhen.h"
#include "doc.h"

#define VERSION 0x0100

CNWItem::CNWItem()
{
	// Initialize schedule items
	m_sNotifyDisabled = TRUE;
	m_eType = ONCE;
	m_timeStart = CTime(1980,1,1,0,0,0);
	m_dateStart = CTime(1980,1,1,0,0,0);
	m_sEnd = 0;
	m_dateEnd = CTime(1980,1,1,0,0,0);
	memset(m_abyData,0,sizeof(m_abyData));

	// Initialize notification items
	m_sFirstTime = TRUE;
	m_sNotifyDays = 0;
	m_sNotifyHours = 0;
	m_sNotifyMinutes = 0;
	m_sAlwaysDisplay = 0;
	m_sAlarmEnabled = 0;
	m_strSoundFile.Empty();

	// Initialize note attributes
	CFont fontTemp;

	fontTemp.CreatePointFont(120,"Arial");
	fontTemp.GetLogFont(&m_lfNote);
	fontTemp.DeleteObject();

	m_rgbNoteText = RGB(0,0,0);
	m_rgbNoteBackground = RGB(255,255,128);
	m_strNoteText = "(ÇëÊäÈëÎÄ±¾)";
	m_sNoteTopMost = FALSE;
	m_sNoteDisplay = TRUE;
	m_rectNote.left = rand()%500;
	m_rectNote.top = rand()%500;
	m_rectNote.right = m_rectNote.left+200;
	m_rectNote.bottom = m_rectNote.top+150;
	m_dateLastRun = CTime(1980,1,1,0,0,0);

	// Initialize in-memory variables
	m_pWnd = NULL;
	m_bDeferSchedule = FALSE;
}

CNWItem::~CNWItem()
{
	if(m_pWnd)
	{
		if(m_pWnd->GetSafeHwnd())
			m_pWnd->DestroyWindow();

		m_pWnd = NULL;
	}
}

void CNWItem::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		long lTemp;

		ar << m_sNotifyDisabled;
		lTemp = (long) m_eType;
		ar << lTemp;
		ar << m_timeStart;
		ar << m_dateStart;
		ar << m_sEnd;
		ar << m_dateEnd;
		ar.Write(m_abyData,sizeof(m_abyData));
		ar << m_sFirstTime;
		ar << m_sNotifyDays;
		ar << m_sNotifyHours;
		ar << m_sNotifyMinutes;
		ar << m_sAlwaysDisplay;
		ar << m_sAlarmEnabled;
		ar << m_strSoundFile;
		ar.Write(&m_lfNote,sizeof(LOGFONT));
		ar << m_rgbNoteText;
		ar << m_rgbNoteBackground;
		ar << m_strNoteText;
		ar << m_sNoteTopMost;
		ar << m_sNoteDisplay;
		ar << m_rectNote;
		ar << m_dateLastRun;	
	}
	else
	{
		long lTemp;

		ar >> m_sNotifyDisabled;
		ar >> lTemp;
		m_eType = (SCHD_TYPE) lTemp;
		ar >> m_timeStart;
		ar >> m_dateStart;
		ar >> m_sEnd;
		ar >> m_dateEnd;
		ar.Read(m_abyData,sizeof(m_abyData));
		ar >> m_sFirstTime;
		ar >> m_sNotifyDays;
		ar >> m_sNotifyHours;
		ar >> m_sNotifyMinutes;
		ar >> m_sAlwaysDisplay;
		ar >> m_sAlarmEnabled;
		ar >> m_strSoundFile;
		ar.Read(&m_lfNote,sizeof(LOGFONT));
		ar >> m_rgbNoteText;
		ar >> m_rgbNoteBackground;
		ar >> m_strNoteText;
		ar >> m_sNoteTopMost;
		ar >> m_sNoteDisplay;
		ar >> m_rectNote;
		ar >> m_dateLastRun;	
	}
}

CNWDocument::CNWDocument()
{
	m_bIsModified = FALSE;
	m_Items.RemoveAll();
	m_sItemCount = 0;
}

CNWDocument::~CNWDocument()
{
	for(int i = 0; i < m_Items.GetCount(); i++)
	{
		CNWItem *pItem = (CNWItem *) m_Items.GetAt(m_Items.FindIndex(i));	
		delete pItem;
	}

	m_Items.RemoveAll();
}

void CNWDocument::Serialize(CArchive& ar)
{
	CNWItem* pItem;

	if(ar.IsStoring())
	{
		long lVersion = VERSION;

		ar << lVersion;
		ar << m_sItemCount;

		// Write each Item to storage
		POSITION pos = m_Items.GetHeadPosition();

		while(pos)
		{
			pItem = (CNWItem *) m_Items.GetNext(pos);
			pItem->Serialize(ar);
		}
	}
	else
	{
		// Clean up any existing items
		for(int i = 0; i < m_Items.GetCount(); i++)
		{
			CNWItem *pItem = (CNWItem *) m_Items.GetAt(m_Items.FindIndex(i));	
			delete pItem;
		}
		m_Items.RemoveAll();

		// Retrieve the data
		long lVersion;

		ar >> lVersion;
		ar >> m_sItemCount;

		// Retrieve each item
		for(i = 0; i < m_sItemCount; i++)
		{
			CNWItem *pItem = new CNWItem();

			pItem->Serialize(ar);
			pItem->m_pos = m_Items.AddTail(pItem);
		}
	}
}

BOOL CNWDocument::Load(LPCTSTR lpszFileName)
{
	CFile file;

	m_bIsModified = FALSE;

	// Check for existence of option file
	if(access(lpszFileName,0) != 0)
	{
		if(file.Open(lpszFileName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
		{
			{
				CArchive ar(&file,CArchive::store);
				Serialize(ar);
			}

			file.Close();
		}
	}

	// Open the configuration file
	if(file.Open(lpszFileName,CFile::modeRead|CFile::typeBinary))
	{
		{
			CArchive ar(&file,CArchive::load);
			Serialize(ar);
		}

		file.Close();

		m_strFileName = lpszFileName;

		return(TRUE);
	}
	else return(FALSE);
}

BOOL CNWDocument::Save()
{
	CFile file;

	if(m_bIsModified)
	{
		m_bIsModified = FALSE;

		// Open the configuration file for update
		if(file.Open(m_strFileName,CFile::modeWrite|CFile::typeBinary))
		{
			{
				CArchive ar(&file,CArchive::store);
				Serialize(ar);
			}

			file.Close();

			return(TRUE);
		}
	}

	return(FALSE);
}

// Add a new item to the document's note list
POSITION CNWDocument::AddItem(CNWItem *pItem, int nInsertAt)
{
	POSITION pos;

	m_sItemCount++;

	if(nInsertAt >= 0 && nInsertAt < m_Items.GetCount())
		pos = m_Items.InsertBefore(m_Items.FindIndex(nInsertAt),pItem);
	else pos = m_Items.AddTail(pItem);

	pItem->m_pos = pos;

	m_bIsModified = TRUE;

	return(pos);
}

// Remove an item from the document's note list
void CNWDocument::DeleteItem(POSITION pos)
{
	m_sItemCount--;

	CNWItem *pItem = (CNWItem *) m_Items.GetAt(pos);
	delete pItem;

	m_Items.RemoveAt(pos);

	m_bIsModified = TRUE;
}

// Get a specified item from the list, based on index
CNWItem* CNWDocument::GetItem(int nIndex, POSITION &pos)
{
	pos = m_Items.FindIndex(nIndex);

	return((CNWItem *) m_Items.GetAt(pos));
}

// Get a specified item from the list, based on position
CNWItem* CNWDocument::GetItem(POSITION pos)
{
	return((CNWItem *) m_Items.GetAt(pos));
}


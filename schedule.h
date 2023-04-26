/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file defines NoteWhen's schedule object.
*/

class CNWDocument;
class CNWItem;

class CNWSchedule
{
protected:
	CNWDocument *m_pDocument;

protected:
	CNWDocument *GetDocument() { return m_pDocument; };

public:
	CNWSchedule();
	void Init(CNWDocument *pDocument);
	void ResetTasks(int nIndex);

	BOOL Check();

	BOOL ShouldRun(CTime timeCurrent, CTime dateCurrent, CNWItem *pItem, 
								BOOL bCheckCalendar=FALSE);
};

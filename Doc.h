
#ifndef __DOC_H__
#define __DOC_H__

class CNWItem : public CObject
{
public:
	typedef enum { ONCE = 0,DAILY,WEEKLY,MONTHLY } SCHD_TYPE;

public:
	// Schedule
	short m_sNotifyDisabled;
	SCHD_TYPE m_eType;
	CTime m_timeStart;
	CTime m_dateStart;
	short m_sEnd;
	CTime m_dateEnd;
	BYTE m_abyData[50];
	short m_sFirstTime;

	// Notifications
	short m_sNotifyDays;
	short m_sNotifyHours;
	short m_sNotifyMinutes;
	short m_sAlwaysDisplay;
	short m_sAlarmEnabled;
	CString m_strSoundFile;

	// Note attributes
	LOGFONT m_lfNote;
	COLORREF m_rgbNoteText;
	COLORREF m_rgbNoteBackground;
	CString m_strNoteText;
	short m_sNoteTopMost;
	short m_sNoteDisplay;
	CRect m_rectNote;

	// Last run date/time
	CTime m_dateLastRun;

	// In-memory variables
	CWnd *m_pWnd;
	POSITION m_pos;
	BOOL m_bDeferSchedule;

public:
	CNWItem();
	~CNWItem();

	void Serialize(CArchive& ar);
};

typedef CObList ITEMS;

class CNWDocument : public CObject
{
protected:
	BOOL m_bIsModified;

	// Item count and items
	short m_sItemCount;
	ITEMS m_Items;


	// Configuation filename
	CString m_strFileName;

protected:
	void Serialize(CArchive& ar);

public:
	CNWDocument();
	~CNWDocument();

// Operations
public:
	POSITION AddItem(CNWItem *pItem, int nInsertAt=-1);
	void DeleteItem(POSITION pos);

	CNWItem* GetItem(int nIndex, POSITION &pos);
	CNWItem* GetItem(POSITION pos);
	int GetItemCount() { return m_sItemCount; }

	LPCTSTR GetFileName() { return m_strFileName; }
	void SetFileName(LPCTSTR lpszFileName) { m_strFileName = lpszFileName; }

	BOOL Load(LPCTSTR lpszFileName);
	BOOL Save();
	void SetModified() { m_bIsModified = TRUE; }
};

struct DAILY
{
	// Every xx days
	short m_sDays;

	// Weekdays only
	short m_sWeekDays;
};

struct WEEKLY
{
	// Every xx weeks
	short m_sWeeks;

	// Day of week, sun, mon, etc.
	short m_sDaysOfWeek;
};

struct MONTHLY
{
	// Flag to indicate user's choice
	short m_sOnDays;

	// First, second, third...last  -- radio button #1
	typedef enum { FIRST = 0, SECOND, THIRD, FOURTH, LAST } DAY_TYPE;
	DAY_TYPE m_sOnDayNum;

	// Sunday, Monday, etc.
	short m_sOnDayName;

	// Day of month		 -- radio button #2
	short m_sOnMonthDay;

	// Months to run (jan, feb, etc.)
	short m_sOnMonths;
};

#endif
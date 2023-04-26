#if !defined(AFX_SCHDPAGE_H__039749BE_C9F6_41B9_AD32_04BB1851E78B__INCLUDED_)
#define AFX_SCHDPAGE_H__039749BE_C9F6_41B9_AD32_04BB1851E78B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SchdPage.h : header file
//

class CNWItem;

/////////////////////////////////////////////////////////////////////////////
// CNWSchedPage dialog

#define UM_ENABLE		(WM_USER+1000)

class CNWSchedPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNWSchedPage)

protected:
	CString m_strTitle;
	int m_nCurrPage;
	CNWItem *m_pItem;
	BOOL m_bIsNew;

protected:
	void SetDlgFields();
	BOOL GetDlgFields();

// Construction
public:
	CNWSchedPage();
	CNWSchedPage(UINT nIDTemplate, UINT nIDCaption = 0);
	~CNWSchedPage();

public:
	void SetTitle(LPCTSTR lpszTitle) { m_strTitle = lpszTitle; }
	void SetItem(CNWItem *pItem) { m_pItem = pItem; }
	void SetNew(BOOL bIsNew) { m_bIsNew = bIsNew; }
	BOOL SavePage();

// Dialog Data
	//{{AFX_DATA(CNWSchedPage)
	enum { IDD = IDD_SCHEDULE_PAGE };
	CComboBox	m_MonthlyDay;
	CComboBox	m_MonthlyCount;
	CSpinButtonCtrl	m_MonthlyDaySpin;
	CSpinButtonCtrl	m_WeeklyEverySpin;
	CSpinButtonCtrl	m_DailyEverySpin;
	CDateTimeCtrl	m_EndDate;
	CDateTimeCtrl	m_StartTime;
	CDateTimeCtrl	m_StartDate;
	CComboBox	m_When;
	int		m_DailyEveryNum;
	int		m_WeeklyEveryNum;
	int		m_MonthlyDayNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNWSchedPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNWSchedPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWhen();
	afx_msg void OnDailyEvery();
	afx_msg void OnDailyWeekdays();
	afx_msg void OnMonthlyOnDay();
	afx_msg void OnMonthlyOn();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdate(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHDPAGE_H__039749BE_C9F6_41B9_AD32_04BB1851E78B__INCLUDED_)

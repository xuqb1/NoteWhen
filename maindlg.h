// testcbDlg.h : header file
//

#if !defined(AFX_NOTEWHENDLG_H__EB400C30_C219_40FA_BC92_A6F22ACF2BD2__INCLUDED_)
#define AFX_NOTEWHENDLG_H__EB400C30_C219_40FA_BC92_A6F22ACF2BD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define UM_TRAY (WM_USER+1000)

/////////////////////////////////////////////////////////////////////////////
// CNWDlg dialog

#include "tray.h"
#include "schedule.h"
#include "header.h"

class CNWDlg : public CDialog
{
protected:
	BOOL m_bInMenu;
	CFont *m_pOrigFont;
	CFont m_NewFont;
	CImageList m_Images;
	CZDTray m_Tray;
	CWnd *m_pwndPopup;
	CNWSchedule m_Schedule;
	CNWHeader m_Header;
	COLORREF m_rgbDefBackground;
	COLORREF m_rgbDefText;
	LOGFONT m_lfDefFont;
	BOOL m_bEnabled;
	
	bool m_bResizeFlag;


protected:
	void InitList();
	void LoadList();
	void HandleSchedule(int nRow, int nColumn);
	void HandleColor(int nRow, int nColumn);
	void HandleText(int nRow, int nColumn);
	void ShowContextMenu();
	CNWDocument *GetDocument() { return GETAPP()->GetDocument(); }
	int GetSelectedItem();
	void AddListItem(CNWItem *pItem, POSITION pos, int nIndex=-1);

// Construction
public:
	BOOL m_bStick;
	CNWDlg(CWnd* pParent = NULL);	// standard constructor

	CNWSchedule& GetSchedule() { return m_Schedule; }
	
// Dialog Data
	//{{AFX_DATA(CNWDlg)
	enum { IDD = IDD_NOTEWHEN_DIALOG };
	CListCtrl	m_List;
	CComboBox	m_Background;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CNWDlg)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnViewCalendar();
	afx_msg void OnAdd();
	afx_msg void OnSearch();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnSizing(UINT fwSize,LPRECT pRect);
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnDelete();
	afx_msg void OnDestroy();
	afx_msg void OnExit();
	afx_msg void OnHelpbtn();
	virtual BOOL OnInitDialog();
	afx_msg void OnNotelist();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAboutbox();
	afx_msg void OnNew();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDisplayNote();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnable();
	afx_msg void OnStick();
	afx_msg void OnBringtotop();
	afx_msg void OnCloseBtn();
	afx_msg void OnShowAll();
	afx_msg void OnUpdateShowAll(CCmdUI* pCmdUI);
	afx_msg void OnHideAll();
	afx_msg void OnUpdateHideAll(CCmdUI* pCmdUI);
	//}}AFX_MSG

	afx_msg void OnListDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTrayMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnQueryEndSession();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTEWHENDLG_H__EB400C30_C219_40FA_BC92_A6F22ACF2BD2__INCLUDED_)

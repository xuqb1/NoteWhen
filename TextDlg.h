#if !defined(AFX_TEXTDLG_H__4778FE2F_9E39_4A7D_BC86_4D88D110B4A5__INCLUDED_)
#define AFX_TEXTDLG_H__4778FE2F_9E39_4A7D_BC86_4D88D110B4A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNWTextDlg dialog

class CNWTextDlg : public CDialog
{
// Construction
public:
	CNWTextDlg(CWnd* pParent = NULL);   // standard constructor


public:
	void SetText(LPCTSTR lpszText) { m_Text = lpszText; }
	LPCTSTR GetText() { return m_Text; }

// Dialog Data
	//{{AFX_DATA(CNWTextDlg)
	enum { IDD = IDD_TEXT };
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNWTextDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNWTextDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHelpbtn();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTDLG_H__4778FE2F_9E39_4A7D_BC86_4D88D110B4A5__INCLUDED_)

/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's main dialog window.
*/

#include "stdafx.h"		 

#include "NoteWhen.h"
#include "MainDlg.h"
#include "MonthDlg.h"
#include "schddlg.h"
#include "note.h"
#include "textdlg.h"
#include "color.h"
#include "doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CenterWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CNWDlg dialog

CNWDlg::CNWDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNWDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNWDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bInMenu = FALSE;
	m_Schedule.Init(GetDocument());

	HDC hDC = ::GetDC(NULL);

	int nColors = GetDeviceCaps(hDC,NUMCOLORS);

	m_rgbDefBackground = RGB(255,255,(nColors==256?0:128));
	m_rgbDefText = RGB(0,0,0);

	::ReleaseDC(NULL,hDC);

	CFont fontTemp;

	fontTemp.CreatePointFont(120,"Arial");
	fontTemp.GetLogFont(&m_lfDefFont);
	fontTemp.DeleteObject();

	m_bEnabled = TRUE;
	m_bStick = TRUE;
}
void CNWDlg::OnSize(UINT nType,int cx,int cy){
	CDialog::OnSize(nType,cx,cy);	
	if(m_bResizeFlag){
		CRect rect;
		GetWindowRect(rect);
		CWnd *pWnd=GetDlgItem(IDC_SEARCHTXT);
		if(pWnd){
			pWnd->SetWindowPos(NULL,10,10,rect.Width()-90,30,SWP_SHOWWINDOW);
		}
		pWnd=GetDlgItem(IDC_SEARCHBUT);
		if(pWnd){
			pWnd->SetWindowPos(NULL,rect.Width()-75,10,50,28,SWP_SHOWWINDOW);
		}
		pWnd=GetDlgItem(IDC_LIST);
		if(pWnd){
			pWnd->SetWindowPos(NULL,10,60,rect.Width()-35,rect.Height()-140,SWP_SHOWWINDOW);
		}
		pWnd=GetDlgItem(IDC_ADD);
		if(pWnd){
			pWnd->SetWindowPos(NULL,rect.Width()-394,rect.Height()-70,70,20,SWP_SHOWWINDOW);
		}
		pWnd=GetDlgItem(IDC_DELETE);
		if(pWnd){
			pWnd->SetWindowPos(NULL,rect.Width()-321,rect.Height()-70,70,20,SWP_SHOWWINDOW);
		}
		pWnd=GetDlgItem(IDC_VIEW_CALENDAR);
		if(pWnd){
			pWnd->SetWindowPos(NULL,rect.Width()-248,rect.Height()-70,70,20,SWP_SHOWWINDOW);
		}
		pWnd=GetDlgItem(IDC_CLOSE);
		if(pWnd){
			pWnd->SetWindowPos(NULL,rect.Width()-175,rect.Height()-70,70,20,SWP_SHOWWINDOW);
		}
		pWnd=GetDlgItem(IDC_HELPBTN);
		if(pWnd){
			pWnd->SetWindowPos(NULL,rect.Width()-95,rect.Height()-70,70,20,SWP_SHOWWINDOW);
		}
		Invalidate();
	}
}
void CNWDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNWDlg)
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNWDlg, CDialog)
	//{{AFX_MSG_MAP(CNWDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_VIEW_CALENDAR, OnViewCalendar)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_DESTROY()
	ON_COMMAND(ID_HANDLE_EXIT, OnExit)
	ON_BN_CLICKED(IDC_HELPBTN, OnHelpbtn)
	ON_COMMAND(ID_NOTELIST, OnNotelist)
	ON_WM_TIMER()
	ON_COMMAND(IDM_ABOUTBOX, OnAboutbox)
	ON_COMMAND(ID_NEW, OnNew)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_BN_CLICKED(IDC_DISPLAY_NOTE, OnDisplayNote)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_COMMAND(ID_ENABLE, OnEnable)
	ON_COMMAND(ID_CANSTICK, OnStick)
	ON_COMMAND(ID_BRINGTOTOP, OnBringtotop)
	ON_BN_CLICKED(IDC_CLOSE, OnCloseBtn)
	ON_COMMAND(ID_SHOW_ALL, OnShowAll)
	ON_UPDATE_COMMAND_UI(ID_SHOW_ALL, OnUpdateShowAll)
	ON_COMMAND(ID_HIDE_ALL, OnHideAll)
	ON_UPDATE_COMMAND_UI(ID_HIDE_ALL, OnUpdateHideAll)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST, OnListDraw)
	ON_MESSAGE(UM_TRAY,OnTrayMsg)
	ON_MESSAGE(UM_UPDATE, OnUpdate)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_SEARCHBUT, OnSearch)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWDlg message handlers


BOOL CNWDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pOrigFont = m_List.GetFont();
	m_Header.SubclassDlgItem(0,&m_List);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Setup the tray window
	m_Tray.SetNotifyWindow(GetSafeHwnd(),UM_TRAY);
	m_Tray.Add(IDR_MAINFRAME,"NoteWhen");

	// Setup the note list control
	InitList();

	// Load the initial note list control
	LoadList();

	// Hide the window initially
	SetTimer(1,500,NULL);
	m_bResizeFlag=true;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNWDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		OnAboutbox();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNWDlg::OnPaint() 
{
	if(IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CNWDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Setup the note list control and load the values from the document
void CNWDlg::InitList()
{
	// Initialize the list control style and font
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|
							LVS_EX_CHECKBOXES|LVS_EX_SUBITEMIMAGES);
	m_List.SetFont(GetFont());

	// Create the image list and associate it with the list control
	m_Images.Create(IDB_LISTIMAGES,45,2,RGB(255,0,255));
	m_List.SetImageList(&m_Images,LVSIL_NORMAL);
	m_List.SetImageList(&m_Images,LVSIL_SMALL);

	// Add the columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;

	m_List.InsertColumn(0,"置顶",LVCFMT_LEFT,36);

	lvc.iSubItem = 1;
	lvc.pszText = "显示";
	lvc.cx = 36;
	m_List.InsertColumn(1,&lvc);

	lvc.iSubItem = 2;
	lvc.pszText = "计划";
	lvc.cx = 40;
	m_List.InsertColumn(2,&lvc);

	lvc.iSubItem = 3;
	lvc.pszText = "文本";
	lvc.cx = 300;
	m_List.InsertColumn(3,&lvc);

	lvc.iSubItem = 4;
	lvc.pszText = "颜色";
	lvc.cx = 70;
	m_List.InsertColumn(4,&lvc);

/*
	CString strTemp;
	int nWidth;
	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();

	for(int i = 1; i <= pHeader->GetItemCount(); i++)
	{
		strTemp.Format("Col%dWidth",i);
		nWidth = AfxGetApp()->GetProfileInt("List",strTemp,-1);
		if(nWidth > 0)
			m_List.SetColumnWidth(i,nWidth);
	}*/
}

void CNWDlg::LoadList()
{
	CNWDocument *pDoc = GetDocument();

	m_List.DeleteAllItems();

	for(int i = 0; i < pDoc->GetItemCount(); i++)
	{
		POSITION pos;
		CNWItem *pItem = pDoc->GetItem(i,pos);

		// Add an item to the list
		AddListItem(pItem,pos);
	}
}	

// Handles the listview's custom drawing
void CNWDlg::OnListDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVCUSTOMDRAW *pDraw = (NMLVCUSTOMDRAW *) pNMHDR;
	int nItem;

	// Do we have a valid item?
	if(pDraw->nmcd.dwDrawStage & CDDS_ITEM)
	{
		nItem = (int) pDraw->nmcd.dwItemSpec;
	}
	else nItem = -1;

	// Draw the item
	if(pDraw->nmcd.dwDrawStage & CDDS_ITEMPREPAINT && nItem >= 0)
	{
		int nSubItem = pDraw->iSubItem;

		// Draw the sample font/color column
		if(nSubItem == 4)
		{
			CNWDocument *pDoc = GetDocument();
			CNWItem *pItem = pDoc->GetItem((POSITION) m_List.GetItemData(nItem));

			if(pItem)
			{
				// Set the note's background and text colors
				pDraw->clrTextBk = pItem->m_rgbNoteBackground;
				pDraw->clrText = pItem->m_rgbNoteText;

				LOGFONT lf;

				// Get the note's logical font and adjust it to the point size
				// we want to use in the list's display
				lf = pItem->m_lfNote;
				lf.lfHeight = 9*10;
				lf.lfWidth = 0;

				// Create a new font from the logfont specification
				m_NewFont.DeleteObject();
				m_NewFont.CreatePointFontIndirect(&lf);
			}

			::SelectObject(pDraw->nmcd.hdc,m_NewFont);
			*pResult = (CDRF_NEWFONT|CDRF_NOTIFYITEMDRAW);
			return;
		}
		else
		{
			// Display the normal text entries
			pDraw->clrTextBk = GetSysColor(COLOR_WINDOW);
			pDraw->clrText = GetSysColor(COLOR_WINDOWTEXT);
			::SelectObject(pDraw->nmcd.hdc,*m_pOrigFont);
			*pResult = (CDRF_NEWFONT|CDRF_NOTIFYITEMDRAW);
			return;
		}
	}

	*pResult = CDRF_NOTIFYITEMDRAW;
}

void CNWDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMITEMACTIVATE* pIA = (LPNMITEMACTIVATE)pNMHDR;

	int nRow = pIA->iItem;

	if(nRow < 0)
	{
		OnAdd();
		return;
	}

	int nColumn = pIA->iSubItem;

	switch(nColumn)
	{
		case 2:
			HandleSchedule(nRow,nColumn);
			break;

		case 3:
			HandleText(nRow,nColumn);
			break;

		case 4:
			HandleColor(nRow,nColumn);
			break;
	}
	
	*pResult = 0;
}

void CNWDlg::OnCancel() 
{
}

// Handles tray notification message
LRESULT CNWDlg::OnTrayMsg(WPARAM wParam, LPARAM lParam)
{
	// Determine the tray message
	switch(lParam)
	{
		// Left mouse button release -- send it to the frame's normal message
		// handler
		case WM_LBUTTONDBLCLK:
			if(m_bInMenu)
			{
				MessageBeep(0);
				SetTimer(3,200,NULL);
			}
			else 
			{
				OnNotelist();
			}
			break;

		// Right mouse button release -- send it to the frame's normal message
		// handler
		case WM_RBUTTONUP:
			if(m_bInMenu)
			{
				if(m_pwndPopup && m_pwndPopup->GetSafeHwnd() && 
							::IsWindow(m_pwndPopup->GetSafeHwnd()))
				{
					m_pwndPopup->ShowWindow(SW_SHOWNORMAL);
					m_pwndPopup->SetForegroundWindow();
				}
				else if(IsWindowVisible())
				{
					SetTimer(3,200,NULL);
				}
			}
			else ShowContextMenu();
			break;
	}

	return(0);
}

// Displays tray menu
void CNWDlg::ShowContextMenu()
{
	if(m_bInMenu) 
	{
		::MessageBeep(0);
		BringWindowToTop();
		SetForegroundWindow();
		return;
	}

	m_bInMenu = TRUE;

	// Load the context menu
	CMenu menuMain;

	menuMain.LoadMenu(IDR_TRAYMENU);
	CMenu *pmenuContext = menuMain.GetSubMenu(0);

	// Make sure our window is activated
	SetForegroundWindow();

	if(m_bEnabled)
		pmenuContext->CheckMenuItem(ID_ENABLE,MF_CHECKED|MF_BYCOMMAND);
	if(m_bStick)
		pmenuContext->CheckMenuItem(ID_CANSTICK,MF_CHECKED|MF_BYCOMMAND);	
	// Set the default item
	pmenuContext->SetDefaultItem(ID_NOTELIST);

	// Display the context menu near the mouse cursor
	POINT pt;

	::GetCursorPos(&pt);
	pmenuContext->TrackPopupMenu(
               TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
               pt.x,pt.y,this,NULL);

	// We no longer need the menu so destroy it
	menuMain.DestroyMenu();

	// Works around Windows menu handling bug that occurs when no
	// selection was made on the context menu -- the basic bug is that
	// it takes 2 mouse clicks to re-activate the menu after this
	// situation.  Sending a dummy message gets around this
	PostMessage(WM_NULL);

	m_bInMenu = FALSE;
}

void CNWDlg::OnDestroy() 
{
	m_Tray.Delete(IDR_MAINFRAME);

	CString strTemp;
	CHeaderCtrl *pHeader = m_List.GetHeaderCtrl();

	for(int i = 1; i <= pHeader->GetItemCount(); i++)
	{
		strTemp.Format("Col%dWidth",i);
		AfxGetApp()->WriteProfileInt("List",strTemp,m_List.GetColumnWidth(i));
	}

	CDialog::OnDestroy();
}

void CNWDlg::OnTimer(UINT nIDEvent) 
{
	KillTimer(nIDEvent);

	if(nIDEvent == 1)
	{
		ShowWindow(SW_HIDE);

		// Start the scheduler timer
		SetTimer(2,1000,NULL);
	}
	else if(nIDEvent == 2)
	{
		// Make sure the document is saved (this routine only saves the
		// document if it's modified)
		GetDocument()->Save();

		// Checks the schedule
		if(m_bEnabled)
			m_Schedule.Check();

		// Restart the schedulre timer
		SetTimer(2,5000,NULL);
	}
	else if(nIDEvent == 3)
	{
		BringWindowToTop();
		SetForegroundWindow();
	}
}

///////////////////////////
// Context menu handlers

void CNWDlg::OnNew() 
{
	OnAdd();
}

void CNWDlg::OnNotelist() 
{
	m_pwndPopup = this;
	SetDlgItemText(IDC_SEARCHTXT,_T(""));
	m_bInMenu = TRUE;
	LoadList();
	ShowWindow(SW_SHOW);
}

void CNWDlg::OnAboutbox() 
{
	m_bInMenu = TRUE;

	CAboutDlg dlgAbout;

	m_pwndPopup = &dlgAbout;
	dlgAbout.DoModal();

	m_bInMenu = FALSE;
}

void CNWDlg::OnExit() 
{
	EndDialog(0);
}

///////////////////////////
// Button handlers
void CNWDlg::OnSearch()
{
	CWnd *pWnd=GetDlgItem(IDC_SEARCHTXT);
	if(pWnd<=0){
		return;
	}
	pWnd=GetDlgItem(IDC_LIST);
	if(pWnd<=0){
		return;
	}
	CString strText;  
	GetDlgItemText(IDC_SEARCHTXT, strText);
	//MessageBox(strText);
	CNWDocument *pDoc = GetDocument();

	m_List.DeleteAllItems();
	strText.TrimLeft("");
	strText.TrimRight("");
	for(int i = 0; i < pDoc->GetItemCount(); i++)
	{
		POSITION pos;
		CNWItem *pItem = pDoc->GetItem(i,pos);
		
		// Add an item to the list
		if(strText==_T("")){
			AddListItem(pItem,pos);
		}else{
			if(pItem->m_strNoteText.Find(strText)>=0){
				AddListItem(pItem,pos);
			}
		}
	}
}
void CNWDlg::OnAdd() 
{
	CNWItem *pItem = new CNWItem();
	CNWNoteWnd *pNote;

	pItem->m_rgbNoteBackground = m_rgbDefBackground;
	pItem->m_rgbNoteText = m_rgbDefText;
	pItem->m_lfNote = m_lfDefFont;

	pNote = new CNWNoteWnd(pItem,TRUE);

	pItem->m_pWnd = pNote;
	POSITION pos = GetDocument()->AddItem(pItem);

	// Add an item to the list
	pItem->m_pos = pos;
	AddListItem(pItem,pos);		

	pItem->m_pWnd->SetActiveWindow();
}

void CNWDlg::OnDelete() 
{
	int nSelCount = m_List.GetSelectedCount();
	int nCount = m_List.GetItemCount();
	int nLastItem = 0;

	for(int i = nCount-1; i >= 0 && nSelCount; i--) 
	{
		if(m_List.GetItemState(i,LVIS_SELECTED)) 
		{
			nSelCount--;

			CNWDocument *pDoc = GetDocument();
			CNWItem *pItem = pDoc->GetItem((POSITION) m_List.GetItemData(i));

			m_List.DeleteItem(i);
			if(nLastItem == 0)
				nLastItem = i;

			// Remove the item from the list here
			GetDocument()->DeleteItem(pItem->m_pos);
		}
	}

	nCount = m_List.GetItemCount();

	if(nLastItem >= nCount)
		nLastItem = nCount-1;


	m_List.SetItemState(nLastItem,LVIS_SELECTED,LVIS_SELECTED);
}

void CNWDlg::AddListItem(CNWItem *pItem, POSITION pos, int nIndex)
{
	BOOL bAddNew = (nIndex < 0);
	int nOrigIndex = nIndex;

	if(nIndex < 0)
		nIndex =  m_List.GetItemCount();

	// Add the item to the list here
	LVITEM lvi;

	lvi.mask = LVIF_PARAM;
	lvi.iItem = nIndex;
	lvi.iSubItem = 0;
	lvi.lParam = (LPARAM) pos;

	if(bAddNew)
		m_List.InsertItem(&lvi);

	// Check the "Top" checkbox column, if necessary
	m_List.SetCheck(nIndex,pItem->m_sNoteTopMost);

	// Set the schedule image
	lvi.mask = LVIF_IMAGE;
	lvi.iImage = (pItem->m_sNoteDisplay?3:2);
	lvi.iItem = nIndex;
	lvi.iSubItem = 1;
	m_List.SetItem(&lvi);

	// Set the schedule image
	lvi.mask = LVIF_IMAGE;
	lvi.iImage = (pItem->m_sNotifyDisabled?0:1);
	lvi.iItem = nIndex;
	lvi.iSubItem = 2;
	m_List.SetItem(&lvi);

	// Add the message text
	CString strTemp = pItem->m_strNoteText;

	strTemp.Replace('\r',' ');
	strTemp.Replace('\n',' ');
	m_List.SetItemText(nIndex,3,strTemp);

	// And some sample text
	m_List.SetItemText(nIndex,4,"Sample");

	if(!bAddNew)
		m_List.SetItemState(nOrigIndex,LVIS_SELECTED,LVIS_SELECTED);
}

void CNWDlg::OnViewCalendar() 
{
	CNWMonthDlg dlgMonth;

	m_pwndPopup = &dlgMonth;
	if(dlgMonth.DoModal() == IDOK)
	{
	}
}

void CNWDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
	m_bInMenu = FALSE;
}


///////////////////////////
// List handlers


void CNWDlg::HandleSchedule(int nRow, int nColumn)
{
	CNWSchdDlg dlgSchedule(this);

	m_pwndPopup = &dlgSchedule;

	POSITION pos = (POSITION) m_List.GetItemData(nRow);

	if(pos)
	{
		CNWDocument *pDoc = GetDocument();
		CNWItem *pItem = pDoc->GetItem(pos);

		dlgSchedule.SetItem(pItem);

		if(dlgSchedule.DoModal() == IDOK)
		{
			AddListItem(pItem,pItem->m_pos,nRow);
		}
	}
}

void CNWDlg::HandleText(int nRow, int nColumn)
{
	CNWTextDlg dlgText(this);

	POSITION pos = (POSITION) m_List.GetItemData(nRow);

	if(pos)
	{
		CNWDocument *pDoc = GetDocument();
		CNWItem *pItem = pDoc->GetItem(pos);

		dlgText.SetText(pItem->m_strNoteText);

		if(dlgText.DoModal() == IDOK)
		{
			pItem->m_strNoteText = dlgText.GetText();

			AddListItem(pItem,pItem->m_pos,nRow);

			// Notify the note about the change too
			if(pItem->m_pWnd)
				pItem->m_pWnd->PostMessage(UM_UPDATE);
		}
	}
}

void CNWDlg::HandleColor(int nRow, int nColumn)
{
	CNWColor Color;

	POSITION pos = (POSITION) m_List.GetItemData(nRow);

	if(pos)
	{
		CNWDocument *pDoc = GetDocument();
		CNWItem *pItem = pDoc->GetItem(pos);

		Color.SetBkColor(pItem->m_rgbNoteBackground);
		Color.SetTextColor(pItem->m_rgbNoteText);

		m_NewFont.DeleteObject();
		m_NewFont.CreateFontIndirect(&pItem->m_lfNote);
		Color.SetFont(m_NewFont);

		if(Color.Select(this))
		{
			pItem->m_rgbNoteBackground = Color.GetBkColor();
			pItem->m_rgbNoteText = Color.GetTextColor();

			Color.GetFont().GetLogFont(&pItem->m_lfNote);

			if(pItem->m_pWnd)
				pItem->m_pWnd->PostMessage(UM_UPDATE);
		}
	}
}

int CNWDlg::GetSelectedItem()
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

void CNWDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int nSelItem = GetSelectedItem();

	if(nSelItem >= 0)
	{
		CNWItem *pItem = GetDocument()->GetItem((POSITION) m_List.GetItemData(nSelItem));

		if(pItem)
			CheckDlgButton(IDC_DISPLAY_NOTE,pItem->m_sNoteDisplay);
	}

	int nItem = pNMListView->iItem;

	if(nItem >= 0)
	{
		int nSubItem = pNMListView->iSubItem;

		if(nSubItem == 0 && pNMListView->uOldState)
		{
			CNWItem *pItem = GetDocument()->GetItem((POSITION) m_List.GetItemData(nItem));

			BOOL bCheck = m_List.GetCheck(nItem);

			if(pItem->m_sNoteTopMost != bCheck)
			{
				pItem->m_sNoteTopMost = bCheck;
				GetDocument()->SetModified();

				if(pItem->m_pWnd)
					pItem->m_pWnd->SetWindowPos((bCheck?&CWnd::wndTopMost:
								&CWnd::wndNoTopMost),
								0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			}
		}
	}

	*pResult = 0;
}

void CNWDlg::OnDisplayNote() 
{
	BOOL bChecked = IsDlgButtonChecked(IDC_DISPLAY_NOTE);

	int nItem = GetSelectedItem();

	if(nItem >= 0)
	{
		CNWDocument *pDoc = GetDocument();
		CNWItem *pItem = pDoc->GetItem((POSITION) m_List.GetItemData(nItem));

		if(pItem)
		{
			pItem->m_sNoteDisplay = bChecked;

			GetDocument()->SetModified();

			if(pItem->m_sNoteDisplay)
			{
				CNWNoteWnd *pNote = new CNWNoteWnd(pItem);

				pItem->m_pWnd = pNote;
			}
			else
			{
				if(pItem->m_pWnd)
					pItem->m_pWnd->PostMessage(WM_CLOSE);
			}

			AddListItem(pItem,pItem->m_pos,nItem);
		}
	}
}

// Resets the specified list entry, based on the note's new attributes
LRESULT CNWDlg::OnUpdate(WPARAM wParam,LPARAM lParam)
{
	LVFINDINFO lvf;

	lvf.flags = LVFI_PARAM;
	lvf.lParam = lParam;
	lvf.vkDirection = VK_DOWN;

	int nItem = m_List.FindItem(&lvf);
	CNWDocument *pDoc = GetDocument();
	CNWItem *pItem;

	if(nItem >= 0)
	{
		pItem = pDoc->GetItem((POSITION) lParam);

		if(wParam == UPD_DELETE)
		{
			m_List.DeleteItem(nItem);
			GETAPP()->GetDocument()->DeleteItem(pItem->m_pos);
		}
		else
		{
			AddListItem(pItem,(POSITION) lParam,nItem);

			if(wParam == UPD_COLORS)
			{
				m_rgbDefBackground = pItem->m_rgbNoteBackground;
				m_rgbDefText = pItem->m_rgbNoteText;
				m_lfDefFont = pItem->m_lfNote;
			}
		}
	}

	nItem = GetSelectedItem();

	if(nItem >= 0)
	{
		POSITION pos;

		pItem = pDoc->GetItem(nItem,pos);
		CheckDlgButton(IDC_DISPLAY_NOTE,pItem->m_sNoteDisplay);
	}

	return(0);
}

// Grab the Delete and Insert key and handle them ourself
BOOL CNWDlg::PreTranslateMessage(MSG* pMsg) 
{
#ifdef SKIP
	if(pMsg->message == WM_KEYDOWN)
	{
		int nItem = GetSelectedItem();

		if(pMsg->wParam == VK_DELETE  && nItem >= 0)
		{
			OnDelete();
		}
		else if(pMsg->wParam == VK_INSERT)
		{
			OnAdd();
		}
	}
#endif

	return CDialog::PreTranslateMessage(pMsg);
}

// Called when an item is clicked in the list
void CNWDlg::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nItem;
	UINT uFlags;
	CPoint ptCursor;

	GetCursorPos(&ptCursor);
	m_List.ScreenToClient(&ptCursor);

	nItem = m_List.HitTest(ptCursor,&uFlags);

	int nWidth = m_List.GetColumnWidth(1);

	// Handle the item checkbox
	if(uFlags == LVHT_ONITEMICON && ptCursor.x > nWidth)
	{
		CNWDocument *pDoc = GetDocument();
		CNWItem *pItem = pDoc->GetItem((POSITION) m_List.GetItemData(nItem));

		pItem->m_sNoteDisplay = !pItem->m_sNoteDisplay;
		pDoc->SetModified();

		AddListItem(pItem,pItem->m_pos,nItem);

		if(pItem->m_sNoteDisplay)
		{
			CNWNoteWnd *pNote = new CNWNoteWnd(pItem);

			pItem->m_pWnd = pNote;
		}
		else
		{
			if(pItem->m_pWnd)
				pItem->m_pWnd->PostMessage(WM_CLOSE);
		}
	}

	*pResult = 0;
}

void CNWDlg::OnEnable() 
{
	m_bEnabled = !m_bEnabled;
}
void CNWDlg::OnStick() 
{
	m_bStick = !m_bStick;
	CMenu menuMain;

	menuMain.LoadMenu(IDR_TRAYMENU);
	CMenu *pmenuContext = menuMain.GetSubMenu(0);

	if(m_bStick){
		pmenuContext->CheckMenuItem(ID_CANSTICK,MF_CHECKED|MF_BYCOMMAND);
	}else{
		pmenuContext->CheckMenuItem(ID_CANSTICK,MF_BYCOMMAND);
	}
}

// Iterates through list of visible note objects and 
// places the note at the top of the z-order
void CNWDlg::OnBringtotop() 
{
	CNWDocument *pDoc = GetDocument();

	for(int i = 0; i < pDoc->GetItemCount(); i++)
	{
		POSITION pos;
		CNWItem *pItem = pDoc->GetItem(i,pos);

		// Is this note visible and does it already
		// have an associated note window?
		if(pItem->m_sNoteDisplay && pItem->m_pWnd)
			pItem->m_pWnd->SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
}

// This is called when Windows is shutting down
BOOL CNWDlg::OnQueryEndSession()
{
	// Make sure the document is saved
	GetDocument()->Save();

	return(TRUE);
}

void CNWDlg::OnCloseBtn() 
{
	OnClose();
}

void CNWDlg::OnHelpbtn() 
{
	WinHelp(100);
}


void CNWDlg::OnShowAll() 
{
	CNWDocument *pDoc = GetDocument();

	for(int i = 0; i < pDoc->GetItemCount(); i++)
	{
		POSITION pos;
		CNWItem *pItem = pDoc->GetItem(i,pos);

		// Is this note visible and does it already
		// have an associated note window?
		if(pItem->m_sNoteDisplay && pItem->m_pWnd)
		{
			pItem->m_pWnd->SetWindowPos(&CWnd::wndTop,0,0,0,0,
								SWP_NOMOVE|SWP_NOSIZE);
		}
		else
		{
			if(!pItem->m_sNoteDisplay)
			{
				CNWNoteWnd *pNote;

				if(!pItem->m_pWnd)
				{
					// Display the note
					pNote = new CNWNoteWnd(pItem,FALSE);

					pItem->m_pWnd = pNote;
				}

				pItem->m_sNoteDisplay = TRUE;
				pDoc->SetModified();

				AfxGetMainWnd()->PostMessage(UM_UPDATE,UPD_ALL,(LPARAM) pItem->m_pos);
			}
		}
	}
}

void CNWDlg::OnUpdateShowAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->GetItemCount() > 0);
}

void CNWDlg::OnHideAll() 
{
	CNWDocument *pDoc = GetDocument();

	for(int i = 0; i < pDoc->GetItemCount(); i++)
	{
		POSITION pos;
		CNWItem *pItem = pDoc->GetItem(i,pos);

		if(pItem->m_sNoteDisplay)
		{
			pItem->m_sNoteDisplay = FALSE;

			// Close the note
			if(pItem->m_pWnd)
				pItem->m_pWnd->PostMessage(WM_CLOSE);

			pDoc->SetModified();
		}
	}
}

void CNWDlg::OnUpdateHideAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->GetItemCount() > 0);
}


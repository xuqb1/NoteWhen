/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's note window.
*/

#include "stdafx.h"
#include "NoteWhen.h"
#include "Note.h"
#include "color.h"
#include "doc.h"
#include "schddlg.h"
#include "maindlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNWNoteWnd


CNWNoteWnd::CNWNoteWnd(CNWItem *pItem, BOOL bIsNew)
{
	m_hbrNote = NULL;
	m_pItem = pItem;
	m_bIsNew = bIsNew;

	m_hcurHand = GETAPP()->LoadCursor(IDC_HAND);;

	// Initialize the in-menu flag
	m_bInMenu = FALSE;

	// Set the sizing border width/height
	m_nSizeBorder = GetSystemMetrics(SM_CXSIZEFRAME)+1;

	// Set default colors and font
	m_fontText.CreateFontIndirect(&pItem->m_lfNote);

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);
	CRect rectTemp = pItem->m_rectNote;

	if(rectTemp.right > cxScreen-10)
	{
		rectTemp.left -= 10;
		rectTemp.right -= 10;
	}
		
	if(rectTemp.bottom > cyScreen-10)
	{
		rectTemp.top -= 10;
		rectTemp.bottom -= 10;
	}

	// Must use CreateEx for popup windows
	CWnd::CreateEx(WS_EX_TOOLWINDOW,AfxRegisterWndClass(0),"",
					WS_POPUP,
					rectTemp.left,
					rectTemp.top,
					rectTemp.Width(),
					rectTemp.Height(),
					::GetDesktopWindow(),0);

	SetWindowPos(&wndTop,0,0,0,0,SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);

	CRect rc;

	// Create the in-place edit control
	rc.left = MARGIN;
	rc.top = MARGIN;
	rc.bottom = rectTemp.Height()-MARGIN-5;
	rc.right = rectTemp.Width()-MARGIN;
	m_Edit.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|
							ES_WANTRETURN|ES_AUTOVSCROLL,rc,this,IDC_TEXT);

	// Set the edit control's font
	m_Edit.SetFont(&m_fontText);

	// Set the window's shape and default cursor
	SetShape();
	SetCursor(LoadCursor(NULL,IDC_ARROW));

	// Set the note's text
	m_Edit.SetWindowText(pItem->m_strNoteText);

	// Make the window topmost, if specified
	if(pItem->m_sNoteTopMost)
		SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);


	if(m_bIsNew)
		SetTimer(1,50,NULL);
}

CNWNoteWnd::~CNWNoteWnd()
{
	::DeleteObject(m_hcurHand);
}

BEGIN_MESSAGE_MAP(CNWNoteWnd, CWnd)
	//{{AFX_MSG_MAP(CNWNoteWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZING()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_CLOSE, OnCloseBtn)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_SET_COLORS, OnSetColors)
	ON_COMMAND(ID_ALWAYSONTOP, OnAlwaysontop)
	ON_WM_MOVE()
	ON_COMMAND(ID_SET_SCHEDULE, OnSetSchedule)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_UNDO, OnUndo)
	ON_COMMAND(ID_SELALL, OnSelall)
	ON_COMMAND(ID_CUT, OnCut)
	ON_COMMAND(ID_COPY, OnCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_COMMAND(ID_HANDLE_EXIT, OnHandleExit)
	ON_COMMAND(ID_NEW, OnNew)
	ON_COMMAND(ID_NOTELIST, OnNotelist)
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_TEXT, OnChangeText)
	ON_MESSAGE(UM_UPDATE, OnUpdate)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNWNoteWnd message handlers

void CNWNoteWnd::OnPaint() 
{
	CPaintDC dc(this);

	if(!m_pItem) return;

	CRect rect;
	GetClientRect(&rect);

	CBrush brNote(m_pItem->m_rgbNoteBackground);
	CBrush brCorner(RGB(128,128,128));
	CPen penFrame(PS_SOLID,2,RGB(0,0,0));
	CPen *pOldPen;
	CBrush *pOldBrush;
	COLORREF crOldText;
	int nOldMode;

	// Fill the note's background
	dc.FillRect(&rect,&brNote);

	pOldPen = (CPen *) dc.SelectObject(&penFrame);

	// Draw the left side and other sides of the note
	dc.MoveTo(rect.left,rect.top);
	dc.LineTo(rect.left,rect.bottom);
	dc.Polyline(m_aPoints,m_nPoints);

	// Calculate the location of the turned up corner
	POINT aPoints[3] = 
	{
		{rect.right-CXCORNER,rect.bottom},
		{rect.right-CXCORNER,rect.bottom-CYCORNER},
		{rect.right,rect.bottom-CYCORNER}
	};

	// Draw and shade the turned up corner
	pOldBrush = (CBrush *) dc.SelectObject(&brCorner);
	dc.Polygon(aPoints,3);

	// Paint some text on the note
	crOldText = dc.SetTextColor(RGB(0,0,0));
	nOldMode = dc.SetBkMode(TRANSPARENT);
	CRect rectText(rect.left+5,rect.top+20,rect.right-5,rect.bottom-20);

	// Restore everything
	dc.SetBkMode(nOldMode);
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
}

void CNWNoteWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nHitCode = HitTest(point);

	CWnd::OnLButtonDown(nFlags,point);

	if(nHitCode != NWHT_NOWHERE)
	{
		// Handle resizing the window
		SendMessage(WM_SYSCOMMAND,SC_SIZE|nHitCode);
	}
	else 
	{
		// Let the system menu code handle this
		// NOTE: the 0x0002 value is a "magic" value from MS
		// that tells the move code that the mouse is being used
		// for the move (as opposed to the keyboard)
		SendMessage(WM_SYSCOMMAND,SC_MOVE|0x0002);
	}
}

void CNWNoteWnd::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CWnd::OnSizing(fwSide, pRect);

	CRect rc = *pRect;
	CPoint pot;
	CRect tmprc;
	UINT tmpside=fwSide;
    int haswin=0;
	//::MessageBox(fwSide);
	char str[10];
	itoa(fwSide,str,3);
	//AfxMessageBox(str,MB_ICONWARNING|MB_OK);
	//调整窗口大小时的sticky功能
	CNWDlg *pDlg = (CNWDlg *)GETAPP()->m_pMainWnd;
	if(pDlg->m_bStick){
		//调整左边，判断左侧
		if(tmpside==WMSZ_LEFT){		
			for(int i=rc.left-1;i>rc.left-11;i--){
				pot.x=i;
				for(int j=rc.top;j<rc.bottom;j++){
					pot.y=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.right>=rc.left-10 && tmprc.right<=rc.left+10){
							pRect->left=tmprc.right;
							//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.left>=rc.left-10 && tmprc.right<=rc.left+10){
							pRect->left=tmprc.left;
							//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
			if(haswin==0){
				for(int i=rc.left;i<rc.left+11;i++){
					pot.x=i;
					for(int j=rc.top-1;j<rc.bottom+1;j++){
						pot.y=j;
						HWND tempwin = ::WindowFromPoint(pot);
						if(tempwin>0 && tempwin!=GetSafeHwnd()){
							::GetWindowRect(tempwin,tmprc);
							if(tmprc.right>=rc.left-10 && tmprc.right<=rc.left+10){
								pRect->left=tmprc.right;
								//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
							if(tmprc.left>=rc.left-10 && tmprc.left<=rc.left+10){
								pRect->left=tmprc.left;
								//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
						}
					}
					if(haswin==1){
						break;
					}
				}
			}
		}
		//调整右边,判断右侧
		if(fwSide==WMSZ_RIGHT){
			for(int i=rc.right-1;i>rc.right-11;i--){
				pot.x=i;
				for(int j=rc.top-20;j<rc.bottom+1;j++){
					pot.y=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.left>=rc.right-10 && tmprc.left<=rc.right+10){
							pRect->right=tmprc.left;
							//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.right>=rc.right-10 && tmprc.right<=rc.right+10){
							pRect->right=tmprc.right;
							//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
			if(haswin==0){
				for(int i=rc.right;i<rc.right+11;i++){
					pot.x=i;
					for(int j=rc.top-20;j<rc.bottom+1;j++){
						pot.y=j;
						HWND tempwin = ::WindowFromPoint(pot);
						if(tempwin>0 && tempwin!=GetSafeHwnd()){
							::GetWindowRect(tempwin,tmprc);
							if(tmprc.right>=rc.right-10 && tmprc.right<=rc.right+10){
								pRect->right=tmprc.right;
								//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
							if(tmprc.left>=rc.right-10 && tmprc.left<=rc.right+10){
								pRect->right=tmprc.left;
								//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
						}
					}
					if(haswin==1){
						break;
					}
				}
			}
		}
		//AfxMessageBox(str,MB_ICONWARNING|MB_OK);
		//调整上边，判断上方
		if(fwSide==WMSZ_TOP){
			//AfxMessageBox("test",MB_ICONWARNING|MB_OK);
			for(int i=rc.top-1;i>rc.top-11;i--){
				pot.y=i;
				for(int j=rc.left-1;j<rc.right+1;j++){
					pot.x=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.bottom>=rc.top-10 && tmprc.bottom<=rc.top+10){
							pRect->top=tmprc.bottom;
							//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.top>=rc.top-10 && tmprc.top<=rc.top+10){
							pRect->top=tmprc.top;
							//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
			if(haswin==0){
				for(int i=rc.top;i<rc.top+11;i++){
					pot.y=i;
					for(int j=rc.left-1;j<rc.right+1;j++){
						pot.x=j;
						HWND tempwin = ::WindowFromPoint(pot);
						if(tempwin>0 && tempwin!=GetSafeHwnd()){
							::GetWindowRect(tempwin,tmprc);
							if(tmprc.bottom>=rc.top-10 && tmprc.bottom<=rc.top+10){
								pRect->top=tmprc.bottom;
								//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
							if(tmprc.top>=rc.top-10 && tmprc.top<=rc.top+10){
								pRect->top=tmprc.top;
								//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
						}
					}
					if(haswin==1){
						break;
					}
				}
			}
		}
		//调整下边,判断下方
		if(fwSide==WMSZ_BOTTOM){
			for(int i=rc.bottom-1;i>rc.bottom-11;i--){
				pot.y=i;
				for(int j=rc.left-20;j<rc.right+1;j++){
					pot.x=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.bottom>=rc.bottom-10 && tmprc.bottom<=rc.bottom+10){
							pRect->bottom=tmprc.bottom;
							//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.top>=rc.bottom-10 && tmprc.top<=rc.bottom+10){
							pRect->bottom=tmprc.top;
							//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
			if(haswin==0){
				for(int i=rc.bottom;i<rc.bottom+11;i++){
					pot.y=i;
					for(int j=rc.left-20;j<rc.right+1;j++){
						pot.x=j;
						HWND tempwin = ::WindowFromPoint(pot);
						if(tempwin>0 && tempwin!=GetSafeHwnd()){
							::GetWindowRect(tempwin,tmprc);
							if(tmprc.bottom>=rc.bottom-10 && tmprc.bottom<=rc.bottom+10){
								pRect->bottom=tmprc.bottom;
								//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
							if(tmprc.top>=rc.bottom-10 && tmprc.top<=rc.bottom+10){
								pRect->bottom=tmprc.top;
								//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
								haswin=1;
								break;
							}
						}
					}
					if(haswin==1){
						break;
					}
				}
			}
		}
	}
	//sticky功能结束
	ScreenToClient(&rc);
	rc.left = MARGIN;
	rc.top = MARGIN;
	rc.bottom = rc.Height()-MARGIN; //-10;
	rc.right = rc.Width()-MARGIN;
	m_Edit.MoveWindow(&rc);
	return;
}

void CNWNoteWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ShowContextMenu();
}

void CNWNoteWnd::ShowContextMenu()
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

	menuMain.LoadMenu(IDR_NOTEMENU);
	CMenu *pmenuContext = menuMain.GetSubMenu(0);

	if(m_pItem->m_sNoteTopMost)
		pmenuContext->CheckMenuItem(ID_ALWAYSONTOP,MF_BYCOMMAND|MF_CHECKED);

	// Make sure our window is activated
	SetForegroundWindow();

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
	if(GetSafeHwnd())
		PostMessage(WM_NULL);

	m_bInMenu = FALSE;
}

void CNWNoteWnd::OnCloseBtn() 
{
	m_pItem->m_pWnd = NULL;
	m_pItem->m_sNoteDisplay = FALSE;
	GETAPP()->GetDocument()->SetModified();

	AfxGetMainWnd()->PostMessage(UM_UPDATE,UPD_ALL,(LPARAM) m_pItem->m_pos);

	PostMessage(WM_CLOSE);
}

void CNWNoteWnd::SetShape()
{
	CRect rc;

	GetClientRect(&rc);

	// Build the sticky note polygon shape
	m_aPoints[0].x = 0;								// Top left	  
	m_aPoints[0].y = 0;
	m_aPoints[1].x = rc.Width();					// Top right
	m_aPoints[1].y = 0;
	m_aPoints[2].x = rc.Width();					// Lower right (fold)
	m_aPoints[2].y = rc.Height()-CYCORNER;
	m_aPoints[3].x = rc.Width()-CXCORNER;		// Bottom right (fold)
	m_aPoints[3].y = rc.Height();
	m_aPoints[4].x = 0;								// Bottom left
	m_aPoints[4].y = rc.Height();
	m_nPoints = 5;

	// Create a polygon region
	SetWindowRgn(NULL,FALSE);
	m_rgnWnd.DeleteObject();
	m_rgnWnd.CreatePolygonRgn(m_aPoints,m_nPoints,ALTERNATE);

	// Set the window's visible region to match it, then repaint it
	SetWindowRgn(m_rgnWnd,TRUE);
}

void CNWNoteWnd::OnDelete() 
{
	m_pItem->m_pWnd = NULL;

	AfxGetMainWnd()->SendMessage(UM_UPDATE,UPD_DELETE,(LPARAM) m_pItem->m_pos);

	DestroyWindow();
}

HBRUSH CNWNoteWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(!m_hbrNote)
		m_hbrNote = ::CreateSolidBrush(m_pItem->m_rgbNoteBackground);
	
	if(!m_pItem) return(m_hbrNote);

	pDC->SetBkColor(m_pItem->m_rgbNoteBackground);
	pDC->SetTextColor(m_pItem->m_rgbNoteText);
	
	return m_hbrNote;
}

BOOL CNWNoteWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_RBUTTONDOWN)
	{
		ShowContextMenu();
		return(TRUE);
	}
	else if(pMsg->message == WM_KEYDOWN)
	{
		BOOL bCtrl=::GetKeyState(VK_CONTROL)&0x8000;
		BOOL bShift=::GetKeyState(VK_SHIFT)&0x8000;
		// only gets here if CTRL key is pressed
		BOOL bAlt=::GetKeyState(VK_MENU)&0x8000;
		// Close the window when Escape is pressed
		switch(pMsg->wParam){
		case VK_ESCAPE:
			OnCloseBtn(); 
			break;
		case 'A': // Ctrl+A 全选
			if(bCtrl){
				OnSelall();
				break;
			}
		case 'N': // Ctrl+N 新建
			if(bCtrl){
				OnNew();
				break;
			}
		case 'S': // Ctrl+S 保存
			if(bCtrl){
				//CFile mFile;
				CString str;
				CTime time = CTime::GetCurrentTime();
				CString m_strTime = time.Format("%Y%m%d-%H%M%S");// +".txt"; 
				CString ext=".txt";
				m_Edit.GetWindowText(str);
				/*
				if(mFile.Open(m_strTime+ext,CFile::modeWrite|CFile::modeCreate)){
					  CArchive ar(&mFile,CArchive::store);
					  UpdateData(TRUE);
					  m_Edit.GetWindowText(str);
					  ar<<str;
					  ar.Close();
					  AfxMessageBox("保存成功到\n"+mFile.GetFilePath(),MB_ICONWARNING|MB_OK);
				}
				FILE * stream;
				
				stream=fopen(m_strTime+ext,"w+");
				fprintf(stream,"%c",str);
				fclose(stream);
				CFile fp;
				fp.Open( m_strTime+ext, CFile::modeWrite|CFile::modeCreate );
				fp.Write( str , str.GetLength() );
				fp.Close();*/
				str.Replace("\r","");
				CStdioFile cFile;
				if (cFile.Open(m_strTime+ext,CFile::modeCreate|CFile::modeReadWrite))
				{
					cFile.WriteString(str);
					cFile.Close();
					//AfxMessageBox("保存成功到\n"+cFile.GetFilePath(),MB_ICONWARNING|MB_OK);
					::MessageBox(NULL,"已保存成功到\n"+cFile.GetFilePath(),"提示",MB_OK);
				}
				
				//CString msg="保存成功到"+mFile.GetFileName()
				//CString fullfn=;
				//break;
				return(TRUE);
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CNWNoteWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	int nHitCode = HitTest(point);

	switch(nHitCode)
	{
		case NWHT_TOP:
		case NWHT_BOTTOM:
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
			break;

		case NWHT_LEFT:
		case NWHT_RIGHT:
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
			break;

		case NWHT_TOPLEFT:
		case NWHT_BOTTOMRIGHT:
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE));
			break;

		case NWHT_TOPRIGHT:
		case NWHT_BOTTOMLEFT:
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENESW));
			break;

		case NWHT_NOWHERE:
			SetCursor(m_hcurHand);
			break;
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CNWNoteWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if(cx < 45 || cy < 45)
		SetWindowPos(NULL,0,0,max(cx,45),max(cy,45),SWP_NOMOVE);

	CRect rc;

	GetWindowRect(&rc);
	m_pItem->m_rectNote = rc;
	GETAPP()->GetDocument()->SetModified();

	if(::IsWindow(m_Edit.GetSafeHwnd()))		  
	{
		rc.left = MARGIN;
		rc.top = MARGIN;
		rc.bottom = cy-MARGIN; //-10;
		rc.right = cx-MARGIN;
		m_Edit.MoveWindow(&rc);
	}

	SetShape();
	Invalidate();
}

void CNWNoteWnd::OnMove(int x, int y) 
{
	CWnd::OnMove(x, y);

	CRect rc;

	GetWindowRect(&rc);
	m_pItem->m_rectNote = rc;
	GETAPP()->GetDocument()->SetModified();
}

int CNWNoteWnd::HitTest(CPoint point)
{
	CRect rc;
	int nRet = NWHT_NOWHERE;

	GetClientRect(&rc);
	CRect rcCorner(rc.Width()-CXCORNER,rc.Height()-CYCORNER,
									rc.Width(),rc.Height());

	if(point.x <= m_nSizeBorder)
	{
		// Handle top left, bottom left and left sizing
		if(point.y <= m_nSizeBorder)
			nRet = NWHT_TOPLEFT;
		else if(point.y >= rc.Height()-m_nSizeBorder)
			nRet = NWHT_BOTTOMLEFT;
		else nRet = NWHT_LEFT;
	}
	else if(rcCorner.PtInRect(point))
	{
		// Handle folded up corner sizing
		nRet = NWHT_BOTTOMRIGHT;
	}
	else if(point.x >= rc.Width()-m_nSizeBorder)
	{
		// Handle top right and right sizing
		if(point.y <= m_nSizeBorder)
			nRet = NWHT_TOPRIGHT;
		else nRet = NWHT_RIGHT;
	}
	else if(point.y <= m_nSizeBorder)
	{
		// Handle top sizing
		nRet = NWHT_TOP;
	}
	else if(point.y >= rc.Height()-m_nSizeBorder)
	{
		// Handle bottom sizing
		nRet = NWHT_BOTTOM;
	}

	return(nRet);
}

BOOL CNWNoteWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

// Handles the Apply button (note currently used)
void CNWNoteWnd::OnApply()
{
	m_pItem->m_rgbNoteBackground = m_Color.GetBkColor();
	m_pItem->m_rgbNoteText = m_Color.GetTextColor();

	CFont& fontNew = m_Color.GetFont();
	LOGFONT lf;

	fontNew.GetLogFont(&lf);

	m_fontText.DeleteObject();
	m_fontText.CreateFontIndirect(&lf);

	m_Edit.SetFont(&m_fontText);

	Invalidate();

	GETAPP()->GetDocument()->SetModified();
}

void ApplyHook(CWnd *pThis)
{
	CNWNoteWnd *pNote = (CNWNoteWnd *) pThis;

	pNote->OnApply();
}

// Handles the color/font menu item
void CNWNoteWnd::OnSetColors() 
{
	m_bInMenu = TRUE;

	m_Color.SetBkColor(m_pItem->m_rgbNoteBackground);
	m_Color.SetTextColor(m_pItem->m_rgbNoteText);
	m_Color.SetFont(m_fontText);
	m_Color.SetApplyHandler(ApplyHook);
	m_Color.SetThis(this);

	if(m_Color.Select(this) == IDOK)
	{
		m_pItem->m_rgbNoteBackground = m_Color.GetBkColor();
		m_pItem->m_rgbNoteText = m_Color.GetTextColor();

		::DeleteObject(m_hbrNote);
		m_hbrNote = NULL;

		CFont& fontNew = m_Color.GetFont();
		LOGFONT lf;

		fontNew.GetLogFont(&lf);
		m_pItem->m_lfNote = lf;

		m_fontText.DeleteObject();
		m_fontText.CreateFontIndirect(&lf);

		m_Edit.SetFont(&m_fontText);

		GETAPP()->GetDocument()->SetModified();
		AfxGetMainWnd()->PostMessage(UM_UPDATE,UPD_COLORS,(LPARAM) m_pItem->m_pos);

		Invalidate();
	}

	m_bInMenu = FALSE;
}

// Handles the "Always on top" menu item
void CNWNoteWnd::OnAlwaysontop() 
{
	m_pItem->m_sNoteTopMost = !m_pItem->m_sNoteTopMost;

	// Make the window topmost, if specified
	if(m_pItem->m_sNoteTopMost)
		SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	else SetWindowPos(&CWnd::wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	GETAPP()->GetDocument()->SetModified();
	AfxGetMainWnd()->PostMessage(UM_UPDATE,UPD_ALL,(LPARAM) m_pItem->m_pos);
}

void CNWNoteWnd::OnChangeText() 
{
	m_Edit.GetWindowText(m_pItem->m_strNoteText);

	GETAPP()->GetDocument()->SetModified();
	AfxGetMainWnd()->PostMessage(UM_UPDATE,UPD_ALL,(LPARAM) m_pItem->m_pos);
}

void CNWNoteWnd::PostNcDestroy() 
{
	delete this;
}

void CNWNoteWnd::OnSetSchedule() 
{
	CNWSchdDlg dlgSchedule(this);

	m_bInMenu = TRUE;

	dlgSchedule.SetItem(m_pItem);

	if(dlgSchedule.DoModal() == IDOK)
	{
	}

	m_bInMenu = FALSE;
}

LRESULT CNWNoteWnd::OnUpdate(WPARAM,LPARAM)
{
	::DeleteObject(m_hbrNote);
	m_hbrNote = NULL;

	m_fontText.DeleteObject();
	m_fontText.CreateFontIndirect(&m_pItem->m_lfNote);

	m_Edit.SetFont(&m_fontText);
	m_Edit.SetWindowText(m_pItem->m_strNoteText);
	Invalidate();

	return(0);
}

void CNWNoteWnd::OnTimer(UINT nIDEvent) 
{
	KillTimer(1);

	BringWindowToTop();
	SetForegroundWindow();
	m_Edit.SetFocus();
	m_Edit.SetSel(0,-1);
}

void CNWNoteWnd::OnClose() 
{
	m_pItem->m_pWnd = NULL;
	
	CWnd::OnClose();
}


void CNWNoteWnd::OnUndo() 
{
	// TODO: Add your command handler code here
	::SendMessage(m_Edit,EM_UNDO,ID_UNDO,0);
}

void CNWNoteWnd::OnCopy() 
{
	// TODO: Add your command handler code here
	::SendMessage(m_Edit,WM_COPY,ID_COPY,0);
}

void CNWNoteWnd::OnCut() 
{
	// TODO: Add your command handler code here
	::SendMessage(m_Edit,WM_CUT,ID_CUT,0);
}

void CNWNoteWnd::OnPaste() 
{
	// TODO: Add your command handler code here
	::SendMessage(m_Edit,WM_PASTE,ID_PASTE,0);
}

void CNWNoteWnd::OnSelall() 
{
	// TODO: Add your command handler code here
	::SendMessage(m_Edit,EM_SETSEL,ID_SELALL,0);
}

void CNWNoteWnd::OnHandleExit() 
{
	// TODO: Add your command handler code here
	PostMessage(WM_QUIT,1,0);
	//EndDialog(GETAPP()->m_pMainWnd->m_hWnd,0);
}

void CNWNoteWnd::OnNew() 
{
	// TODO: Add your command handler code here
	::SendMessage(GETAPP()->m_pMainWnd->m_hWnd,WM_COMMAND,ID_NEW,0);
}

void CNWNoteWnd::OnNotelist() 
{
	// TODO: Add your command handler code here
	::SendMessage(GETAPP()->m_pMainWnd->m_hWnd,WM_COMMAND,ID_NOTELIST,0);
}
//移动时sticky功能
void CNWNoteWnd::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanging(lpwndpos);
	CPoint pot;
	CRect rc ;
	CRect tmprc;
	int haswin=0;
	rc.left = lpwndpos->x;
	rc.top = lpwndpos->y;
	rc.right = lpwndpos->x + lpwndpos->cx;
	rc.bottom = lpwndpos->y + lpwndpos->cy;
	CNWDlg *pDlg = (CNWDlg *)GETAPP()->m_pMainWnd;
	if(pDlg->m_bStick){
		int i=0;
		for(i=rc.left-1;i>rc.left-11;i--){
			pot.x=i;
			for(int j=rc.top;j<rc.bottom;j++){
				pot.y=j;
				HWND tempwin = ::WindowFromPoint(pot);
				if(tempwin>0 && tempwin!=GetSafeHwnd()){
					::GetWindowRect(tempwin,tmprc);
					if(tmprc.right>=rc.left-10 && tmprc.right<=rc.left+10){
						lpwndpos->x=tmprc.right;
						//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
					if(tmprc.left>=rc.left-10 && tmprc.right<=rc.left+10){
						lpwndpos->x=tmprc.left;
						//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
				}
			}
			if(haswin==1){
				break;
			}
		}
		if(haswin==0){
			for(i=rc.left;i<rc.left+11;i++){
				pot.x=i;
				for(int j=rc.top-1;j<rc.bottom+1;j++){
					pot.y=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.right>=rc.left-10 && tmprc.right<=rc.left+10){
							lpwndpos->x=tmprc.right;
							//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.left>=rc.left-10 && tmprc.left<=rc.left+10){
							lpwndpos->x=tmprc.left;
							//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
		}
		haswin=0;
		for(i=rc.right-1;i>rc.right-11;i--){
			pot.x=i;
			for(int j=rc.top-20;j<rc.bottom+1;j++){
				pot.y=j;
				HWND tempwin = ::WindowFromPoint(pot);
				if(tempwin>0 && tempwin!=GetSafeHwnd()){
					::GetWindowRect(tempwin,tmprc);
					if(tmprc.left>=rc.right-10 && tmprc.left<=rc.right+10){
						lpwndpos->x=tmprc.left-lpwndpos->cx;
						//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
					if(tmprc.right>=rc.right-10 && tmprc.right<=rc.right+10){
						lpwndpos->x=tmprc.right-lpwndpos->cx;
						//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
				}
			}
			if(haswin==1){
				break;
			}
		}
		if(haswin==0){
			for(i=rc.right;i<rc.right+11;i++){
				pot.x=i;
				for(int j=rc.top-20;j<rc.bottom+1;j++){
					pot.y=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.right>=rc.right-10 && tmprc.right<=rc.right+10){
							lpwndpos->x=tmprc.right-lpwndpos->cx;
							//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.left>=rc.right-10 && tmprc.left<=rc.right+10){
							lpwndpos->x=tmprc.left-lpwndpos->cx;
							//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
		}
		haswin=0;
		for(i=rc.top-1;i>rc.top-11;i--){
			pot.y=i;
			for(int j=rc.left-1;j<rc.right+1;j++){
				pot.x=j;
				HWND tempwin = ::WindowFromPoint(pot);
				if(tempwin>0 && tempwin!=GetSafeHwnd()){
					::GetWindowRect(tempwin,tmprc);
					if(tmprc.bottom>=rc.top-10 && tmprc.bottom<=rc.top+10){
						lpwndpos->y=tmprc.bottom;
						//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
					if(tmprc.top>=rc.top-10 && tmprc.top<=rc.top+10){
						lpwndpos->y=tmprc.top;
						//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
				}
			}
			if(haswin==1){
				break;
			}
		}
		if(haswin==0){
			for(i=rc.top;i<rc.top+11;i++){
				pot.y=i;
				for(int j=rc.left-1;j<rc.right+1;j++){
					pot.x=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.bottom>=rc.top-10 && tmprc.bottom<=rc.top+10){
							lpwndpos->y=tmprc.bottom;
							//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.top>=rc.top-10 && tmprc.top<=rc.top+10){
							lpwndpos->y=tmprc.top;
							//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
		}
		haswin=0;
		for(i=rc.bottom-1;i>rc.bottom-11;i--){
			pot.y=i;
			for(int j=rc.left-1;j<rc.right+1;j++){
				pot.x=j;
				HWND tempwin = ::WindowFromPoint(pot);
				if(tempwin>0 && tempwin!=GetSafeHwnd()){
					::GetWindowRect(tempwin,tmprc);
					if(tmprc.bottom>=rc.bottom-10 && tmprc.bottom<=rc.bottom+10){
						lpwndpos->y=tmprc.bottom-lpwndpos->cy;
						//AfxMessageBox("test1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
					if(tmprc.top>=rc.bottom-10 && tmprc.top<=rc.bottom+10){
						lpwndpos->y=tmprc.top-lpwndpos->cy;
						//AfxMessageBox("test1-1",MB_ICONWARNING|MB_OK);
						haswin=1;
						break;
					}
				}
			}
			if(haswin==1){
				break;
			}
		}
		if(haswin==0){
			for(i=rc.bottom;i<rc.bottom+11;i++){
				pot.y=i;
				for(int j=rc.left-1;j<rc.right+1;j++){
					pot.x=j;
					HWND tempwin = ::WindowFromPoint(pot);
					if(tempwin>0 && tempwin!=GetSafeHwnd()){
						::GetWindowRect(tempwin,tmprc);
						if(tmprc.bottom>=rc.bottom-10 && tmprc.bottom<=rc.bottom+10){
							lpwndpos->y=tmprc.bottom-lpwndpos->cy;
							//AfxMessageBox("test2",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
						if(tmprc.top>=rc.bottom-10 && tmprc.top<=rc.bottom+10){
							lpwndpos->y=tmprc.top-lpwndpos->cy;
							//AfxMessageBox("test2-1",MB_ICONWARNING|MB_OK);
							haswin=1;
							break;
						}
					}
				}
				if(haswin==1){
					break;
				}
			}
		}
	}
	return;
}

/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's header control handler.
*/

#include "stdafx.h"
#include "notewhen.h"
#include "Header.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNWHeader

CNWHeader::CNWHeader()
{
}

CNWHeader::~CNWHeader()
{
}


BEGIN_MESSAGE_MAP(CNWHeader, CHeaderCtrl)
	//{{AFX_MSG_MAP(CNWHeader)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNWHeader message handlers

// Can we resize the specified column?
BOOL CNWHeader::CanResize(CPoint& point)
{
	HD_ITEM item;
	int nWidth = 0;

	item.mask = HDI_WIDTH;

	// Accumulate the width of the first two columns
	for(int i = 0; i < 2; i++)
	{
		GetItem(0,&item);
		nWidth += item.cxy;
	}

	// Don't allow resize if the mouse is anywhere in the first
	// two columns
	if(point.x >= 0 && point.x <= nWidth+13)
		return(FALSE);

	return(TRUE);
}

void CNWHeader::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(nFlags & MK_LBUTTON)
	{
		if(!CanResize(point))
			return;
	}
	
	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

BOOL CNWHeader::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint point;
	
	GetCursorPos(&point);
	ScreenToClient(&point);

	if(CanResize(point))
		return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
	else return(FALSE);	
}


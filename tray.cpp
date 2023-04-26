/*
	NoteWhen - PC Magazine note utility

	Copyright (c) 2001 Ziff-Davis Media, Inc.  All rights reserved.
	First published in PC Magazine, US Edition 2001.

	Written by Steven E. Sipe


	This file implements NoteWhen's tray handler.
*/


#include "stdafx.h"

#include <assert.h>

#include "notewhen.h"
#include "tray.h"

// Constructor - Sets the nofication window handle (if there is one) and
// the notification message id.  These can later be changed by calling
// the SetNotifyWindow() method
CZDTray::CZDTray(HWND hwndParent, int nMessageID) 
{
   // Save the receiving window and message ID if they are specified.
   // Note that the default constructor has NULL and 0 for these repectively.
   SetNotifyWindow(hwndParent,nMessageID);
} 

// Destructor
CZDTray::~CZDTray()
{
}

////////// Private Operations //////////

// Handles the "real" add/modify tray operation -- it is called by the public
// Add() and Modify() methods
BOOL CZDTray::DoAddModify(int nTrayIconID, LPCTSTR lpszTitle, int nNewIconID, 
                          int nOperation, HICON hIcon)
{
   // Make receiving window, message ID and tray icon are set
   assert(m_hWnd && m_nMessageID);
   assert(nTrayIconID);

   // Initialize the registration structure
   NOTIFYICONDATA notifyIcon;
   notifyIcon.cbSize = sizeof(notifyIcon);
   notifyIcon.uFlags = NIF_MESSAGE;
   notifyIcon.hWnd = m_hWnd;
   notifyIcon.uID = nTrayIconID;   
   notifyIcon.uCallbackMessage = m_nMessageID;

   // If setting a new icon...
   if(nNewIconID || hIcon)
   {
      notifyIcon.uFlags |= NIF_ICON;
		if(hIcon)
			notifyIcon.hIcon = hIcon;
      else notifyIcon.hIcon = AfxGetApp()->LoadIcon(nNewIconID);
   }

   // If setting a tooltip title...
   if(lpszTitle)
   {
      notifyIcon.uFlags |= NIF_TIP;
      strcpy(notifyIcon.szTip,lpszTitle);
   }

   // Register with the shell
   return(::Shell_NotifyIcon(nOperation,&notifyIcon));
}


////////// Public Operations //////////

// Adds a new notification object to the tray
BOOL CZDTray::Add(int nTrayIconID, LPCTSTR lpszTitle)
{
   return(DoAddModify(nTrayIconID,lpszTitle,nTrayIconID,NIM_ADD));
}

// Modifies the specified notification object
BOOL CZDTray::Modify(int nTrayIconID, int nNewIconID, LPCTSTR lpszTitle,
									HICON hIcon)
{
   return(DoAddModify(nTrayIconID,lpszTitle,nNewIconID,NIM_MODIFY,hIcon));
}

// Deletes the specified notification object
BOOL CZDTray::Delete(int nTrayIconID)
{
   // Make receiving window is set
   assert(m_hWnd);

     // Remove the notification icon from the taskbar
   NOTIFYICONDATA notifyIcon;
   notifyIcon.cbSize = sizeof(notifyIcon);
   notifyIcon.hWnd = m_hWnd;
   notifyIcon.uID = nTrayIconID;

   return(::Shell_NotifyIcon(NIM_DELETE,&notifyIcon));
}



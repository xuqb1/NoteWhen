
#ifndef __TRAY_H__
#define __TRAY_H__

class CZDTray 
{
// Private Attributes
private:
   int m_nMessageID;
   HWND m_hWnd;

// Private Operations
private:
   // Performs the actual Add/Modify operation
   BOOL DoAddModify(int nTrayIconID, LPCTSTR lpszTitle, int nNewIconID, 
								int nOperation, HICON hIcon=NULL);

// Public Construction/Destruction
public:
   // Constructor
   CZDTray(HWND hwndParent=NULL, int nMessageID=0);
   // Destructor
   virtual ~CZDTray();

// Public Operations
public:
   // Adds an item to the system tray
   BOOL Add(int nTrayIconID, LPCTSTR lpszTitle=NULL);

   // Changes the attributes of a system tray item
   BOOL Modify(int nTrayIconID, int nNewIconID=0, LPCTSTR lpszTitle=NULL, 
										HICON hIcon=NULL);

   // Removes an item from the system tray
   BOOL Delete(int nTrayIconID);

   // Allows another window to also receive notification messages
   void SetNotifyWindow(HWND hwndNotifyWindow, int nMessageID) 
         { m_hWnd = hwndNotifyWindow; m_nMessageID = nMessageID; }
};

/////////////////////////////////////////////////////////////////////////////
#endif

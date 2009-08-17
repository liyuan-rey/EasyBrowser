//   DownloadFile.h:   interface   for   the   CDownloadFile   class.   
//////////////////////////////////////////////////////////////////////    
#pragma   once    

#include <Afxinet.h>


#define   NOTIFY_MSG_WPARAM_GENDOWNFILEID                         0x01   
#define   NOTIFY_MSG_LOW_WPARAM_FULLSIZE                           0x10   
#define   NOTIFY_MSG_LOW_WPARAM_CURRENTSIZE                     0x20   
#define   NOTIFY_MSG_LOW_WPARAM_DOWNSIZE                           0x30   
#define   NOTIFY_MSG_LOW_WPARAM_DOWNSPEED                         0x40   

class   CDownloadFile       
{   
public:   
	BOOL   OpenRedirectHttpURL(CString   &strOldLocation,CInternetSession   &cSession);   
	BOOL   DownLoadFile(LPCTSTR   lpFileURL,LPCTSTR   lpSaveFile);   
	CDownloadFile();   
	virtual   ~CDownloadFile();   
	LPCTSTR   GetSavedFileName()   {   return   m_strSaveToFile;}   
	LPCTSTR   GetDownURL()   {   return   m_strFileURL;}   

public:   
	WORD   GenFileID();   
	void   RegisterNotifyWindow(DWORD   dwThreadID,HWND   hWnd,DWORD   dwMsg);   
	BOOL   GetUNCFile();   
	bool   m_bForceReload;   
	DWORD   m_TimeOut;   
	WORD   m_wFileID;

protected:   
	DWORD   m_dwThreadID;   
	//void   PostNotifyMessage(WPARAM   wParam,   LPARAM   lParam);   
	DWORD   m_dwMsgID;   
	HWND   m_hNotify;   
	BOOL   GetFtpFile(CInternetSession   &cSession);   
	BOOL   GetHttpFile(CInternetSession   &cSession);   
	CString     m_strTmpFileName;   
	CString     m_strFileURL;   
	CString     m_strSaveToFile;   
	CString     m_rawHeaders;   
	float         m_transferRate;   
	DWORD         m_infoStatusCode;   
};   


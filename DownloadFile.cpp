//   DownloadFile.cpp:   implementation   of   the   CDownloadFile   class.   
  //   
  //////////////////////////////////////////////////////////////////////   
    
  #include   "stdafx.h"
#include "EasyBrowser.h"
  #include   "DownloadFile.h"   
    
  #pragma   comment(   lib,"Wininet.lib"   )   
  #include   "shlwapi.h"   
  #pragma   comment(   lib,"shlwapi.lib"   )   
    
  #ifdef   _DEBUG   
  #undef   THIS_FILE   
  static   char   THIS_FILE[]=__FILE__;   
  #define   new   DEBUG_NEW   
  #endif   
    
  #define   BUFFER_SIZE   4095   
    
  const   TCHAR   szHeaders[]   =   _T("Accept:*/*\r\nUser-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)\r\n");   
    
  //////////////////////////////////////////////////////////////////////   
  //   Construction/Destruction   
  //////////////////////////////////////////////////////////////////////   
    
  CDownloadFile::CDownloadFile()   
  {   
          m_TimeOut   =   0;   
          m_bForceReload   =   true;   
          m_dwThreadID   =   0;   
          m_hNotify   =   NULL;   
          m_dwMsgID   =   0;   
          m_wFileID   =   0;   
		  m_pBuffer = NULL;
		  m_nBufferSize = 0L;
  }   
    
  CDownloadFile::~CDownloadFile()   
  {   
	ClearBuffer();
  }

  void CDownloadFile::ClearBuffer()
  {
	  if (m_pBuffer != NULL)
		  HeapFree(GetProcessHeap(), 0L, m_pBuffer);

	  m_pBuffer = NULL;
	  m_nBufferSize = 0L;
  }

  BOOL   CDownloadFile::DownLoadFile(LPCTSTR   lpFileURL,   LPCTSTR   lpSaveFile)   
  {   
          BOOL   bRet   =   FALSE;   
          if   (   !::PathIsURL(lpFileURL)   )   
          {   
                  return   bRet;   
          }   
          m_strSaveToFile   =   lpSaveFile;   
          m_strFileURL   =   lpFileURL;   
		  m_strTmpFileName   =   lpSaveFile;   
          m_strTmpFileName   +=   _T(".df!");   
		  bRet = Download(TRUE);
		  if   (bRet)   
		  {   
			  if   (!::MoveFileEx(m_strTmpFileName,m_strSaveToFile,MOVEFILE_REPLACE_EXISTING)   )   
			  {   
				  Sleep(1000);   
				  ::MoveFileEx(m_strTmpFileName,m_strSaveToFile,MOVEFILE_REPLACE_EXISTING);   
			  }   
		  }   
		  return   bRet;   
  }   

  BOOL   CDownloadFile::DownloadToBuffer(LPCTSTR   lpFileURL)
  {
	  BOOL bRet = FALSE;
	  if (!::PathIsURL(lpFileURL))
		  return   bRet;   

	  m_strFileURL = lpFileURL;

	  ClearBuffer();
	  bRet = Download(FALSE);

	  return bRet;
  }

  BOOL   CDownloadFile::Download(BOOL bSaveToFile)
  {
	  BOOL bRet = FALSE;
	  CString   strAgentCaption   =     _T("Update   Download   ")   ;   
	  strAgentCaption   +=   ::PathFindFileName(m_strSaveToFile);   
	  DWORD   dwFlags   =   0;   
	  InternetGetConnectedState(&dwFlags,   0);   
	  CInternetSession   session   (   strAgentCaption,   1,   
		  (dwFlags   &   INTERNET_CONNECTION_PROXY)   ==   INTERNET_CONNECTION_PROXY   ?   INTERNET_OPEN_TYPE_PRECONFIG   :   INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY,   
		  NULL,   NULL,   0);   
	  CString   strServer,strObject;
	  INTERNET_PORT   nPort;   
	  AfxParseURL(m_strFileURL,dwFlags,strServer,strObject,nPort);   

	  if   (m_TimeOut   !=   0)   
		  session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT,   m_TimeOut);   
	  if(   !m_wFileID   )   
		  m_wFileID   =   GenFileID();   
	  //PostNotifyMessage(NOTIFY_MSG_WPARAM_GENDOWNFILEID,m_wFileID);   

	  try   
	  {   
		  if   (   dwFlags==   AFX_INET_SERVICE_HTTP   )   
		  {   
			  if (bSaveToFile)
				  bRet   =   GetHttpFile(session);   
			  else
				  bRet   =   GetHttpFileToBuffer(session);
		  }   
		  else   if(   dwFlags==   AFX_INET_SERVICE_FTP   )   
		  {   
			  ASSERT(bSaveToFile); // save to buffer not implemented
			  bRet   =   GetFtpFile(session);   
		  }   
		  else   if(   dwFlags==   AFX_INET_SERVICE_FILE   )   
		  {   
			  ASSERT(bSaveToFile); // save to buffer not implemented
			  if(   UrlIsFileUrl(m_strFileURL)   )   
				  bRet   =   GetUNCFile();   
		  }   
		  else   
		  {   
			  ;   
		  }           
	  }   
	  catch   (CException*   pEx)   
	  {   
		  TCHAR   szErrorMsg[MAX_PATH]   =   {0};   
		  pEx->GetErrorMessage(szErrorMsg,   MAX_PATH);   
		  TRACE(   _T("Exception:   %s\n")   ,   szErrorMsg);   
		  pEx->Delete();   
	  }   

	  session.Close();   
	  m_wFileID   =   0;   
	  return bRet;
  }

  BOOL   CDownloadFile::GetHttpFile(CInternetSession   &cSession)   
  {   
	  BOOL   bRet   =   FALSE;   
	  CFile   m_TmpFile;   
	  CFileException   fileException;   

	  if   (   !m_TmpFile.Open   (m_strTmpFileName,     
		  CFile::modeCreate   |   CFile::modeNoTruncate     |   CFile::modeReadWrite   
		  |   CFile::shareDenyWrite         |   CFile::typeBinary,   
		  &fileException   )   )   
	  {   
		  TRACE(   _T("Open   File   failed:   %d\n"),   fileException.m_cause   );   
		  return   bRet;   
	  }   
	  CString   strRangeQuest;   

	  if   (m_TmpFile.GetLength()>0)   
	  {   
		  //PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_CURRENTSIZE,m_wFileID),m_TmpFile.GetLength());   
		  m_TmpFile.SeekToEnd();   
		  strRangeQuest.Format(   _T("%sRange:   bytes=%d-\r\n"),   szHeaders,m_TmpFile.GetLength());   
	  }   
	  else   
		  strRangeQuest   =   szHeaders;   

	  DWORD   dwCount   =   0;   
	  CHttpFile*   pFile   =   NULL;   
	  CString   strTmpURL   =   m_strFileURL;   
	  try   
	  {   
		  DWORD   dwFlags   =   INTERNET_FLAG_TRANSFER_BINARY     
			  |INTERNET_FLAG_DONT_CACHE   
			  |INTERNET_FLAG_PRAGMA_NOCACHE;   
		  if   (m_bForceReload)   {   
			  dwFlags   |=   INTERNET_FLAG_RELOAD;   
		  }   
		  //Here   Find   URLFile   Redirect.   
		  //              
		  OpenRedirectHttpURL(strTmpURL,cSession);   
		  pFile   =   (CHttpFile*)   cSession.OpenURL(strTmpURL,   1,   dwFlags,strRangeQuest,   -1);   
	  }   
	  catch   (CInternetException*   e)   
	  {   
		  TCHAR       szCause[MAX_PATH]   =   {0};   
		  e->GetErrorMessage(szCause,   MAX_PATH);   
		  e->Delete();   
		  delete   pFile;   
		  pFile   =   NULL;   
		  return   bRet;   
	  }

	  COleDateTime startTime = COleDateTime::GetCurrentTime();
	  DWORD dwHttpFileSize = 0;
	  if (pFile)
	  {      
		  BYTE buffer[BUFFER_SIZE+1] = {0}; 
		  try {     
			  UINT nRead = 0; 
			  pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,dwHttpFileSize);   
			  //PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_FULLSIZE,m_wFileID),dwHttpFileSize);  
			  TRACE( _T("Totoal Length is %d\n"), dwHttpFileSize ); 
			  dwCount = 0;    
			  do     
			  {        
				  nRead = pFile->Read(buffer, BUFFER_SIZE);   
				  if (nRead > 0)    
				  {              
					  buffer[nRead] = 0;     
					  m_TmpFile.Write(buffer,nRead);   
					  COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - startTime;  
					  double dSecs = elapsed.GetTotalSeconds();  
					  //PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSIZE,m_wFileID),dwCount);    
					  if (dSecs > 0.0)            
					  {                       
						  dwCount += nRead;        
						  m_transferRate = (float) ( dwCount / 1024.0 / dSecs );     
						  TRACE("Read %d bytes (%0.1f Kb/s)\n", dwCount, m_transferRate );  
					  }                  
					  else            
					  {               
						  TRACE("Read %d bytes\n", dwCount);   
						  m_transferRate = (float) ( dwCount / 1024.0 );     
					  }              
					  //PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSPEED,m_wFileID),(LPARAM)m_transferRate); 
				  }       
			  }         
			  while (nRead > 0);   
			  bRet = TRUE;     
		  }       
		  catch (CFileException *e)   
		  {         
			  TCHAR   szCause[MAX_PATH] = {0};  
			  e->GetErrorMessage(szCause, MAX_PATH); 
			  TRACE("ErrorMsg : %s\n", szCause);       
			  e->Delete();  
			  delete pFile;   
			  m_TmpFile.Close();   
			  return FALSE;   
		  }       
		  pFile->QueryInfoStatusCode(m_infoStatusCode);   
		  pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS ,m_rawHeaders);  
		  pFile->Close();    
		  m_TmpFile.Close();   
		  delete pFile; 
	  }       
	  return bRet;
}

BOOL   CDownloadFile::GetHttpFileToBuffer(CInternetSession   &cSession)   
{   
	CHttpFile*   pFile   =   NULL;   
	CString   strTmpURL   =   m_strFileURL;   
	try   
	{   
		DWORD   dwFlags   =   INTERNET_FLAG_TRANSFER_BINARY     
			|INTERNET_FLAG_DONT_CACHE   
			|INTERNET_FLAG_PRAGMA_NOCACHE;   
		if   (m_bForceReload)   {   
			dwFlags   |=   INTERNET_FLAG_RELOAD;   
		}   
		//Here   Find   URLFile   Redirect.   
		//              
		OpenRedirectHttpURL(strTmpURL,cSession);   
		pFile   =   (CHttpFile*)   cSession.OpenURL(strTmpURL,   1,   dwFlags,szHeaders,   -1);   
	}   
	catch   (CInternetException*   e)   
	{   
		TCHAR       szCause[MAX_PATH]   =   {0};   
		e->GetErrorMessage(szCause,   MAX_PATH);   
		e->Delete();   
		delete   pFile;   
		pFile   =   NULL;   
		return   FALSE;   
	}

	if (!pFile)
		return FALSE;

	////////////////////////
	COleDateTime startTime = COleDateTime::GetCurrentTime();

	try
	{     
		DWORD dwHttpFileSize = 0;
		pFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,dwHttpFileSize);   
		//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_FULLSIZE,m_wFileID),dwHttpFileSize);  
		TRACE( _T("Totoal Length is %d\n"), dwHttpFileSize ); 

		DWORD dwAllocSize = dwHttpFileSize;
		m_pBuffer = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAllocSize);
		if (!m_pBuffer)
			throw new CMemoryException();

		DWORD dwCount = 0;
		UINT nRead = 0;
		BYTE buffer[BUFFER_SIZE+1] = {0};

		do     
		{        
			nRead = pFile->Read(buffer, BUFFER_SIZE);   
			if (nRead > 0)    
			{
				buffer[nRead] = 0;

				if (dwCount + nRead > dwAllocSize)
				{
					m_pBuffer = (BYTE*) HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, m_pBuffer, dwAllocSize + nRead * 2);
					if (!m_pBuffer)
						throw new CMemoryException();
				}

				CopyMemory(m_pBuffer + dwCount, buffer, nRead);
				dwCount += nRead;

				COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - startTime;  
				double dSecs = elapsed.GetTotalSeconds();  
				//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSIZE,m_wFileID),dwCount);    
				if (dSecs > 0.0)            
				{                       
					m_transferRate = (float) ( dwCount / 1024.0 / dSecs );     
					TRACE("Read %d bytes (%0.1f Kb/s)\n", dwCount, m_transferRate );  
				}                  
				else            
				{               
					TRACE("Read %d bytes\n", dwCount);   
					m_transferRate = (float) ( dwCount / 1024.0 );     
				}              
				//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSPEED,m_wFileID),(LPARAM)m_transferRate); 
			}       
		}         
		while (nRead > 0);

		m_nBufferSize = dwCount;
	}       
	catch (CFileException *e)   
	{         
		TCHAR   szCause[MAX_PATH] = {0};  
		e->GetErrorMessage(szCause, MAX_PATH); 
		TRACE("ErrorMsg : %s\n", szCause);       
		e->Delete();  
		delete pFile;   
		return FALSE;   
	}   
	catch (CMemoryException *me)
	{
		TRACE("out of memory...\n");
		me->Delete();
		delete pFile;
		return FALSE;
	}

	pFile->QueryInfoStatusCode(m_infoStatusCode);   
	pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS ,m_rawHeaders);  
	pFile->Close();    
	delete pFile; 

	return TRUE;
}

BOOL CDownloadFile::OpenRedirectHttpURL(CString &strOldLocation,CInternetSession &cSession)
{  
	BOOL bRet = FALSE; 
	CHttpFile *pFile = NULL;  
	CHttpConnection* pServer = NULL;  
	CString strServerName,strObject;  
	INTERNET_PORT nPort = 0;  
	DWORD dwServiceType = 0;   
	if (!AfxParseURL(strOldLocation, dwServiceType, strServerName, strObject, nPort) ||        dwServiceType != INTERNET_SERVICE_HTTP)
	{      
		TRACE( _T("Not A Http Quest!\n") );  
		return bRet; 
	}     
	pServer = cSession.GetHttpConnection(strServerName, nPort); 
	pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,   
		strObject, NULL, 1, NULL, NULL,   
		INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT); 
	pFile->AddRequestHeaders(szHeaders);  
	pFile->SendRequest();    
	DWORD dwRet;  
	pFile->QueryInfoStatusCode(dwRet);  
	// if access was denied, prompt the user for the password 
	if (dwRet == HTTP_STATUS_DENIED) 
	{      
		DWORD dwPrompt; 
		dwPrompt = pFile->ErrorDlg(NULL, ERROR_INTERNET_INCORRECT_PASSWORD,    
			FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, NULL); 
		// if the user cancelled the dialog, bail out      
		if (dwPrompt != ERROR_INTERNET_FORCE_RETRY)     
		{        
			TRACE( _T("Access denied: Invalid password\n") );  
			// close up the redirected site  
			pFile->Close();     
			delete pFile;    
			pServer->Close();    
			delete pServer;   
			return bRet;    
		}              
		pFile->SendRequest();  
		pFile->QueryInfoStatusCode(dwRet); 
	}        
	// were we redirected? 
	// these response status codes come from WININET.H   
	if (dwRet == HTTP_STATUS_MOVED ||    
		dwRet == HTTP_STATUS_REDIRECT ||   
		dwRet == HTTP_STATUS_REDIRECT_METHOD)   
	{       
		CString strNewLocation; 
		pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);    
		int nPlace = strNewLocation.Find(_T("Location: ")); 
		if (nPlace == -1)  
		{        
			TRACE( _T("Error: Site redirects with no new location\n") );    
			// close up the redirected site 
			pFile->Close();     
			delete pFile;   
			pServer->Close();  
			delete pServer;     
			return bRet;    
		}          
		strNewLocation = strNewLocation.Mid(nPlace + 10);  
		nPlace = strNewLocation.Find('\n');
		if (nPlace > 0)  
			strNewLocation = strNewLocation.Left(nPlace); 
		strOldLocation = strNewLocation;   
	}   
	if ( dwRet == HTTP_STATUS_OK )  
	{     
		bRet = TRUE;    
	}  
	// close up the redirected site
	pFile->Close(); 
	delete pFile;  
	pServer->Close();
	delete pServer;  
	return bRet;
}

BOOL CDownloadFile::GetFtpFile(CInternetSession &cSession)
{  
	BOOL bRet = FALSE;  
	CFile m_TmpFile; 
	CFileException fileException;    
	if ( !m_TmpFile.Open (m_strTmpFileName,  
		CFile::modeCreate | CFile::modeNoTruncate  | CFile::modeReadWrite     
		| CFile::shareDenyWrite    | CFile::typeBinary,   
		&fileException ) ) 
	{     
		TRACE( _T("Open File failed: %d\n"), fileException.m_cause );     
		return bRet; 
	}   
	DWORD dwCount = 0; 
	CFtpConnection *pFtpConn = NULL; 
	CInternetFile *pFile = NULL; 
	try  
	{    
		CString strServerName,strObject,strUserName,strPassword; 
		INTERNET_PORT nPort = 0;  
		DWORD dwServiceType = 0;   
		CString strRestPointCommand;  
		if (!AfxParseURLEx(m_strFileURL, dwServiceType, strServerName, strObject, nPort,    
			strUserName, strPassword) ||   
			dwServiceType != INTERNET_SERVICE_FTP)  
		{         
			TRACE( _T("Not A Ftp Quest!\n") );  
		}      
		
		// CFtpConnection ERROR_INTERNET_NO_DIRECT_ACCESS CInternetSession      
		if (strUserName.IsEmpty()) 
			pFtpConn = cSession.GetFtpConnection(strServerName,NULL,NULL,nPort,m_bForceReload);   
		else       
			pFtpConn = cSession.GetFtpConnection(strServerName,strUserName,strPassword,nPort,m_bForceReload);   
		
		if (m_TmpFile.GetLength())   
		{         
			//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_CURRENTSIZE,m_wFileID),m_TmpFile.GetLength());    
			m_TmpFile.SeekToEnd();   
			strRestPointCommand.Format( _T("REST %d"), m_TmpFile.GetLength());     
			//strRestPointCommand.Format( _T("ls") );   
			
			if ( !FtpCommand((*pFtpConn), FALSE, FTP_TRANSFER_TYPE_ASCII,      
				strRestPointCommand, 0, 0) )   
			{              
				TRACE( _T("FtpCommand failed, error: %d\n"), GetLastError());       
				m_TmpFile.SeekToBegin();  
			}        
		}      
		if (pFtpConn)  
		{        
			pFile = pFtpConn->OpenFile(strObject);   
		}  
	}   
	catch (CInternetException* e)  
	{       
		TCHAR   szCause[MAX_PATH] = {0};   
		e->GetErrorMessage(szCause, MAX_PATH);  
		e->Delete();    
		delete pFile;    
		delete pFtpConn;  
		return bRet; 
	}       
	
	COleDateTime startTime = COleDateTime::GetCurrentTime();
	DWORD dwFtpFileSize = 0; 
	if (pFile)  
	{      
		BYTE buffer[BUFFER_SIZE+1] = {0};  
		try {    
			UINT nRead = 0;    
			dwFtpFileSize = FtpGetFileSize( (*pFile),0); 
			//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_FULLSIZE,m_wFileID),dwFtpFileSize);  
			TRACE( _T("Totoal Length is %d\n"), dwFtpFileSize ); 
			dwCount = 0;   
			do         
			{          
				nRead = pFile->Read(buffer, BUFFER_SIZE); 
				if (nRead > 0)      
				{             
					buffer[nRead] = 0;    
					m_TmpFile.Write(buffer,nRead);   
					COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - startTime;         
					double dSecs = elapsed.GetTotalSeconds(); 
					//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSIZE,m_wFileID),dwCount);     
					if (dSecs > 0.0)   
					{              
						dwCount += nRead;      
						m_transferRate = (float)(dwCount / 1024.0 / dSecs);         
						TRACE("Read %d bytes (%0.1f Kb/s)\n", dwCount, m_transferRate );     
					}         
					else    
					{       
						TRACE("Read %d bytes\n", dwCount);  
						m_transferRate = (float)(dwCount / 1024.0); 
					}                
					//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSPEED,m_wFileID),(LPARAM)m_transferRate);   
				}       
			}         
			while (nRead > 0);   
			bRet = TRUE;     
		}     
		catch (CFileException *e)   
		{          
			TCHAR   szCause[MAX_PATH] = {0};    
			e->GetErrorMessage(szCause, MAX_PATH); 
			TRACE("ErrorMsg : %s\n", szCause);  
			e->Delete();    
			delete pFile;  
			pFtpConn->Close();  
			delete pFtpConn;    
			m_TmpFile.Close();  
			return FALSE;     
		}       
		pFile->Close();   
		delete pFile;   
		m_TmpFile.Close();  
		pFtpConn->Close();  
		delete pFtpConn;  
	}   
	return bRet;
}

BOOL CDownloadFile::GetUNCFile()
{   
	BOOL bRet = FALSE; 
	CFile m_TmpFile,m_SrcFile; 
	CFileException fileException; 
	CString strOldLocation = m_strFileURL;
	if ( !m_TmpFile.Open (m_strTmpFileName,   
		CFile::modeCreate | CFile::modeNoTruncate  | CFile::modeReadWrite      
		| CFile::shareDenyWrite    | CFile::typeBinary,  
		&fileException ) )  
	{      
		TRACE( _T("Open File failed: %d\n"), fileException.m_cause );   
		return bRet; 
	}  
	strOldLocation.TrimLeft(); 
	strOldLocation.TrimRight();
	if( StrCmpNI(strOldLocation, _T("file:/"),6) == 0 )   
	{    
		strOldLocation = strOldLocation.Mid(8); 
		strOldLocation.Replace( _T('/'), _T('\\'));  
	} 
	if ( !m_SrcFile.Open ( strOldLocation,    
		CFile::modeRead | CFile::shareDenyWrite | CFile::typeBinary,    
		&fileException ) )  
	{       
		TRACE( _T("Open File failed: %d\n"), fileException.m_cause );    
		return bRet;  
	}  
	COleDateTime startTime = COleDateTime::GetCurrentTime(); 
	DWORD dwCount = 0;   
	try {       
		if (m_TmpFile.GetLength())    
		{       
			//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_CURRENTSIZE,m_wFileID),m_TmpFile.GetLength()); 
			m_TmpFile.SeekToEnd();   
			m_SrcFile.Seek(m_TmpFile.GetLength(), CFile::begin); 
		}       
		BYTE buffer[BUFFER_SIZE+1] = {0}; 
		UINT nRead = 0;  
		//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_FULLSIZE,m_wFileID),m_SrcFile.GetLength()); 
		TRACE( _T("Totoal Length is %d,left is %d\n"), m_SrcFile.GetLength() ,m_SrcFile.GetLength() - m_TmpFile.GetLength());  
		dwCount = 0;  
		do   
		{     
			nRead = m_SrcFile.Read(buffer, BUFFER_SIZE);  
			if (nRead > 0)    
			{           
				buffer[nRead] = 0;   
				m_TmpFile.Write(buffer,nRead);  
				COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - startTime;    
				double dSecs = elapsed.GetTotalSeconds();  
				//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSIZE,m_wFileID),dwCount);   
				if (dSecs > 0.0)    
				{                 
					dwCount += nRead;     
					m_transferRate = (float)(dwCount / 1024.0 / dSecs);   
					TRACE("Read %d bytes (%0.1f Kb/s)\n", dwCount, m_transferRate );       
				}        
				else  
				{     
					TRACE("Read %d bytes\n", dwCount);  
					m_transferRate = (float)(dwCount / 1024.0); 
				}       
				//PostNotifyMessage(MAKEWPARAM(NOTIFY_MSG_LOW_WPARAM_DOWNSPEED,m_wFileID),(LPARAM)m_transferRate);   
			}    
		}     
		while (nRead > 0);   
		bRet = TRUE; 
	}    
	catch (CFileException *e) 
	{      
		TCHAR   szCause[MAX_PATH] = {0};  
		e->GetErrorMessage(szCause, MAX_PATH);  
		TRACE("ErrorMsg : %s\n", szCause);   
		e->Delete();  
		m_TmpFile.Close(); 
		return FALSE;
	}   
	m_TmpFile.Close(); 
	return bRet;
}

void CDownloadFile::RegisterNotifyWindow(DWORD dwThreadID,HWND hWnd, DWORD dwMsg)
{  
	m_dwThreadID = dwThreadID; 
	m_hNotify = hWnd;  
	m_dwMsgID = dwMsg;
}

//void CDownloadFile::PostNotifyMessage(WPARAM wParam, LPARAM lParam)
//{  
//	if (m_hNotify)  
//	{      
//		::PostMessage(m_hNotify, m_dwMsgID, wParam,  lParam); 
//	} 
//	if ( m_dwThreadID )  
//	{      
//		::PostThreadMessage(m_dwThreadID,m_dwMsgID, wParam, lParam);  
//	}
//}

WORD CDownloadFile::GenFileID()
{   
	srand(GetTickCount()); 
	return rand() & 0xFFFF;
}


UINT ThreadDownSingleFile( LPVOID pParam )
{
	CDownloadFile m_DownFile;
	UINT uRet = 0;
	m_DownFile.m_wFileID = m_DownFile.GenFileID();
	//这里注册通知窗口和消息
	//m_DownFile.RegisterNotifyWindow
	CString path = theApp.programPath;
	path += "update.exe";
	if ( m_DownFile.DownLoadFile(theApp.updatefilePath, path))
	{
		uRet = 1;
		theApp.WriteProfileInt("System", "VersionNum", theApp.versionNum);
	}
	return uRet;
}

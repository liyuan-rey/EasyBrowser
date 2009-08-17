// EasyBrowser.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号

#define VERSION_NUM 1005

// CEasyBrowserApp:
// 有关此类的实现，请参阅 EasyBrowser.cpp
//

class CEasyBrowserApp : public CWinApp
{
public:
	CEasyBrowserApp();

	CString programPath;
	CString strPath1;
	CString strPath2;

	int versionNum;
	CString updatefilePath;

	CString path1Url;
	CString path2Url;

	int browserIndex;
// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CEasyBrowserApp theApp;

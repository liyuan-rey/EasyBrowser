// EasyBrowser.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������

#define VERSION_NUM 1005

// CEasyBrowserApp:
// �йش����ʵ�֣������ EasyBrowser.cpp
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
// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEasyBrowserApp theApp;

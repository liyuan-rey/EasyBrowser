// EasyBrowser.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "EasyBrowser.h"
#include "EasyBrowserDlg.h"
#include "DownloadFile.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEasyBrowserApp

BEGIN_MESSAGE_MAP(CEasyBrowserApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEasyBrowserApp 构造

CEasyBrowserApp::CEasyBrowserApp()
{
	// TODO: 在此处添加构造代码，
	versionNum = 0;
	updatefilePath  = "";
	browserIndex = 0;
}


// 唯一的一个 CEasyBrowserApp 对象

CEasyBrowserApp theApp;


// CEasyBrowserApp 初始化

BOOL CEasyBrowserApp::InitInstance()
{
	GetModuleFileName(NULL,programPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	programPath.ReleaseBuffer();
	int nPos = programPath.ReverseFind('\\');
	programPath = programPath.Left(nPos + 1);
	strPath1 = programPath;
	strPath1 += "url1.xml";

	strPath2 = programPath;
	strPath2 += "url2.xml";

	if(CoInitialize(NULL) != S_OK)
	{
		return FALSE;
	}
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControls()。否则，将无法创建窗口。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名

	SetRegistryKey(_T("EasyBrowser"));

	int KeyVersion = GetProfileInt("System", "VersionNum", 0);
	if(KeyVersion > VERSION_NUM)
	{
		CString strPathComplete = programPath;
		strPathComplete += "update.exe";

		if (_access(strPathComplete, 0) == 0)
		{
			int ret = AfxMessageBox("发现更新的版本，是否更新？", MB_YESNO, 0);
			if(ret == IDYES)
			{
				::ShellExecute(NULL, "open", strPathComplete, "", "", SW_SHOW);
				exit(0);
			}
		}
	}

	CDownloadFile m_DownFile;
	UINT uRet = 0;
	m_DownFile.m_wFileID = m_DownFile.GenFileID();
	//这里注册通知窗口和消息
	//m_DownFile.RegisterNotifyWindow
	//path1Url = GetProfileString("System", "PathUrl1", "http://123.66w.com/url.xml");
	//path2Url = GetProfileString("System", "PathUrl2", "http://www.66w.com/url.xml");
	path1Url = "http://www.m9797.com/url.xml";
	path2Url = "http://www.97pai.com/url.xml";

	m_DownFile.DownLoadFile(path1Url, strPath1);
	m_DownFile.DownLoadFile(path2Url, strPath2);


	CEasyBrowserDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用“确定”来关闭
		//对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	// 而不是启动应用程序的消息泵。
	return FALSE;
}

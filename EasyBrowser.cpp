// EasyBrowser.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "EasyBrowser.h"
#include "EasyBrowserDlg.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEasyBrowserApp

BEGIN_MESSAGE_MAP(CEasyBrowserApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEasyBrowserApp ����

CEasyBrowserApp::CEasyBrowserApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	versionNum = 0;
	updatefilePath  = "";
	browserIndex = 0;
}


// Ψһ��һ�� CEasyBrowserApp ����

CEasyBrowserApp theApp;


// CEasyBrowserApp ��ʼ��

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
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��

	SetRegistryKey(_T("EasyBrowser"));

	int KeyVersion = GetProfileInt("System", "VersionNum", 0);
	if(KeyVersion > VERSION_NUM)
	{
		CString strPathComplete = programPath;
		strPathComplete += "update.exe";

		if (_access(strPathComplete, 0) == 0)
		{
			int ret = AfxMessageBox("���ָ��µİ汾���Ƿ���£�", MB_YESNO, 0);
			if(ret == IDYES)
			{
				::ShellExecute(NULL, "open", strPathComplete, "", "", SW_SHOW);
				exit(0);
			}
		}
	}


	m_DownFile1.m_wFileID = m_DownFile1.GenFileID();
	m_DownFile2.m_wFileID = m_DownFile2.GenFileID();
	//����ע��֪ͨ���ں���Ϣ
	//m_DownFile.RegisterNotifyWindow
	//path1Url = GetProfileString("System", "PathUrl1", "http://123.66w.com/url.xml");
	//path2Url = GetProfileString("System", "PathUrl2", "http://www.66w.com/url.xml");
	path1Url = "http://www.m9797.com/url.xml";
	path2Url = "http://www.880325.com/url.xml";

	m_DownFile1.DownloadToBuffer(path1Url);
	m_DownFile2.DownloadToBuffer(path2Url);


	CEasyBrowserDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȷ�������ر�
		//�Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//�Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	// ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

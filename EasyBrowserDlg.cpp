// EasyBrowserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EasyBrowser.h"
#include "EasyBrowserDlg.h"
#include  <comdef.h>
#include ".\easybrowserdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern UINT ThreadDownSingleFile( LPVOID pParam );

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


IXMLDOMNode* CEasyBrowserDlg::GetFirstLevelNode(IXMLDOMElement* pRootNode, CComBSTR bstrChildNodeName)
{
	IXMLDOMNode* pResultNode = NULL;
	CComBSTR szName;
	for(pRootNode->get_firstChild(&pResultNode); pResultNode!= NULL; pResultNode->get_nextSibling(&pResultNode))
	{
		pResultNode->get_baseName(&szName);
		if(szName == bstrChildNodeName)
			return pResultNode;
	}
	return pResultNode;
}

IXMLDOMNode* CEasyBrowserDlg::GetChildNode(IXMLDOMNode* pParentNode, CComBSTR bstrChildNodeName)
{
	IXMLDOMNode* pResultNode = NULL;
	CComBSTR szName;
	for(pParentNode->get_firstChild(&pResultNode); pResultNode!= NULL; pResultNode->get_nextSibling(&pResultNode))
	{
		pResultNode->get_baseName(&szName);
		if(szName == bstrChildNodeName)
			return pResultNode;
	}
	return pResultNode;
}

BOOL CEasyBrowserDlg::ReadXMLFile(CString filePath)
{
	VARIANT_BOOL bResult;
	BOOL ret = FALSE;
	m_pXMLDoc->load((_variant_t)(filePath), &bResult);
	ret = bResult;
	if(bResult)
	{
		IXMLDOMElement *pRootNode;
		m_pXMLDoc->get_documentElement(&pRootNode);
		if(pRootNode == NULL)
		{
			ret = FALSE;
		}else
		{
			IXMLDOMNode* pBaseNode = GetFirstLevelNode(pRootNode, CComBSTR("urlnum"));
			if(pBaseNode == NULL)
			{
				ret = FALSE;
			}else
			{
				CComBSTR nodeStr;
				pBaseNode->get_text(&nodeStr);
				CString urlNumStr(nodeStr);
				int urlNum = atoi(urlNumStr.GetBuffer());
				for(int i = 0; i < urlNum; i++)
				{
					CString tmp;
					tmp.Format("url%d", i+1);
					pBaseNode = GetFirstLevelNode(pRootNode, CComBSTR(tmp.GetBuffer()));
					pBaseNode->get_text(&nodeStr);
					CString urlStr(nodeStr);
					m_vecUrls.push_back(urlStr);
				}
				if(m_vecUrls.size() > 0)
					ret = TRUE;
				else
					ret = FALSE;
			}

			pBaseNode = GetFirstLevelNode(pRootNode, CComBSTR("version"));
			if(pBaseNode != NULL)
			{
				CComBSTR bstrStr;
				IXMLDOMNode* pChildNode = GetChildNode(pBaseNode, CComBSTR("num"));
				pChildNode->get_text(&bstrStr);
				CString strversion(bstrStr) ;
				theApp.versionNum = atoi(strversion.GetBuffer());

				pChildNode = GetChildNode(pBaseNode, CComBSTR("fileurl"));
				pChildNode->get_text(&bstrStr);
				theApp.updatefilePath = bstrStr;
			}
		}
	}
	return ret;
}

// CEasyBrowserDlg 对话框



CEasyBrowserDlg::CEasyBrowserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEasyBrowserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwTabLen = 100;
	m_bInit = FALSE;
	curSel = 0;
}

void CEasyBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_EASYBW, m_ctrTab);
}

BEGIN_MESSAGE_MAP(CEasyBrowserDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_EASYBW, OnTcnSelchangeTabEasybw)
END_MESSAGE_MAP()


// CEasyBrowserDlg 消息处理程序

BOOL CEasyBrowserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将\“关于...\”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_ctrTab.pMyParent = this;

	HRESULT hr;
	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, 
		IID_IXMLDOMDocument, (void**)&m_pXMLDoc);

	BOOL ret = ReadXMLFile(theApp.strPath1);

	if(!ret)// 读第一个文件没有成功
	{
		ReadXMLFile(theApp.strPath2);
	}

	::DeleteFile(theApp.strPath1);
	::DeleteFile(theApp.strPath2);

	//下载更新文件
	if(theApp.versionNum > VERSION_NUM)
	{
		DWORD ThreadID;
		HANDLE hThread = ::CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ThreadDownSingleFile,
			NULL,
			0,
			&ThreadID);
		::CloseHandle(hThread);
	}

	for(int i = 0; i < m_vecUrls.size(); i++)
	{
		m_ctrTab.InsertItem(i, "空白页");
	}
	m_imageTab.Create( IDB_BITMAP_CLOSE, 16, 1, RGB(255,255,255) );
	//m_ctrTab.SetImageList(&m_imageTab);


	for(int i = 0; i < m_vecUrls.size(); i++)
	{
		CDlgBrowser* pBrowser = new CDlgBrowser;
		pBrowser->Create(i, this);
		CString urlStr = m_vecUrls[i];
		pBrowser->m_ctrWebBrowser.Navigate(urlStr, NULL, NULL, NULL, NULL);
		m_vecBrowser.push_back(pBrowser);
	}
	theApp.browserIndex = m_vecUrls.size();

	if(m_vecBrowser.size() > 0)
	{
		m_vecBrowser[0]->ShowWindow(SW_SHOW);
		//TC_ITEM tcItem;
		//tcItem.mask = TCIF_IMAGE;
		//tcItem.iImage = 0;
		//m_ctrTab.SetItem( 0, &tcItem );

		m_ctrTab.SetCurSel(0);
	}

	SetSize();

	m_ctrTab.SetMinTabWidth(96);
	//m_ctrTab.SetItemSize(CSize(240,0));


	m_bInit = TRUE;
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CEasyBrowserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEasyBrowserDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CEasyBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEasyBrowserDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
}

void CEasyBrowserDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CEasyBrowserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_bInit)
		SetSize();
}

void CEasyBrowserDlg::OnDestroy()
{
	CDialog::OnDestroy();

	for(int i = 0; i < m_vecBrowser.size(); i++)
	{
		if(m_vecBrowser[i] != NULL)
		{
			delete m_vecBrowser[i];
			m_vecBrowser[i] = NULL;
		}
	}
	if(m_pXMLDoc != NULL){
		m_pXMLDoc->Release();
		m_pXMLDoc = NULL;
	}
}

void CEasyBrowserDlg::SetSize()
{
	CRect rect;
	GetClientRect(&rect);

	m_ctrTab.MoveWindow(&rect);

	rect.top += 24;

	for(int i = 0; i < m_vecBrowser.size(); i++)
	{
		m_vecBrowser[i]->MoveWindow(&rect);
	}
}

void CEasyBrowserDlg::OnTcnSelchangeTabEasybw(NMHDR *pNMHDR, LRESULT *pResult)
{
	curSel = m_ctrTab.GetCurSel();
	for(int i = 0; i < m_vecBrowser.size(); i++)
	{
		if(i == curSel)
		{
			//TC_ITEM tcItem;
			//tcItem.mask = TCIF_IMAGE;
			//tcItem.iImage = 0;
			//m_ctrTab.SetItem( i, &tcItem );

			m_vecBrowser[i]->ShowWindow(SW_SHOW);
			SetWindowText(m_vecBrowser[i]->m_strTitle);
		}
		else
		{
			//TC_ITEM tcItem;
			//tcItem.mask = TCIF_IMAGE;
			//tcItem.iImage = -1;
			//m_ctrTab.SetItem( i, &tcItem );

			m_vecBrowser[i]->ShowWindow(SW_HIDE);
		}
	}

	//*pResult = 0;
}

BOOL CEasyBrowserDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == MY_MESSAGE)
	{
		char* title = (char*)pMsg->wParam;
		int num = pMsg->lParam;
		TC_ITEM tItem; 
		tItem.mask = TCIF_TEXT; 
		if(strlen(title) < 16)
			tItem.pszText = title;
		else
		{
			tItem.pszText = new char[16];
			memset(tItem.pszText, 0, 16);
			bool hzbegin = false;
			for(int i = 0; i < 12; i++)
			{
				tItem.pszText[i] = title[i];
				if(title[i] < 0)
					hzbegin = !hzbegin;
			}
			if(hzbegin)
				tItem.pszText[11] = 0;
			tItem.pszText = strcat(tItem.pszText, "...");
		}
		tItem.cchTextMax = strlen(tItem.pszText); 

		m_ctrTab.SetItem(num, &tItem);

		if(num == curSel)
		{
			SetWindowText(title);
		}
		return TRUE;
	}
	else if(pMsg->message == MY_HITTEST_MSG)
	{
		int index = int(pMsg->wParam);
		if(index >= 0 && m_vecBrowser.size() > 1 && m_vecBrowser.size() > index)
		{
			m_ctrTab.DeleteItem(index);
			CDlgBrowser* pBrowser = m_vecBrowser[index];
			for(vector<CDlgBrowser*>::iterator iter = m_vecBrowser.begin(); iter != m_vecBrowser.end(); iter++ )
			{
				if(*iter == pBrowser)
				{
					m_vecBrowser.erase(iter);
					delete pBrowser;
					pBrowser = NULL;
					break;
				}
			}
			//CString strUrl = m_vecUrls[index];
			//for(vector<CString>::iterator iter = m_vecUrls.begin(); iter != m_vecUrls.end(); iter++)
			//{
			//	if(strUrl == *iter)
			//	{
			//		m_vecUrls.erase(iter);
			//		break;
			//	}
			//}
		}
		m_ctrTab.SetCurSel(0);
		OnTcnSelchangeTabEasybw(NULL, NULL);
		UpdateData(FALSE);
		return TRUE;
	}
	else if(pMsg->message == MY_MESSAGE_NEW)
	{
		m_ctrTab.InsertItem(pMsg->lParam, "空白页");
		CDlgBrowser* pBrowser = (CDlgBrowser*)pMsg->wParam;
		m_vecBrowser.push_back(pBrowser);
		SetSize();
		m_ctrTab.SetCurSel(m_vecBrowser.size() - 1);
		OnTcnSelchangeTabEasybw(NULL, NULL);
	}

	return CDialog::PreTranslateMessage(pMsg);
}

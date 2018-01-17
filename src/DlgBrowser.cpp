// DlgBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "EasyBrowser.h"
#include "DlgBrowser.h"
#include ".\dlgbrowser.h"


// CDlgBrowser dialog

IMPLEMENT_DYNAMIC(CDlgBrowser, CDialog)
CDlgBrowser::CDlgBrowser(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgBrowser::IDD, pParent)
{
	m_strTitle = "";
	m_bInit = FALSE;
}

CDlgBrowser::~CDlgBrowser()
{
}

BOOL CDlgBrowser::Create(int num, CWnd* pParent /*=NULL*/) 
{
	// TODO: Add your specialized code here and/or call the base class
	pMyParent = pParent;
	m_myNum = num;
	return CDialog::Create(IDD, pParent);
}


void CDlgBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEB_BROWSER, m_ctrWebBrowser);
}


BEGIN_MESSAGE_MAP(CDlgBrowser, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgBrowser message handlers

void CDlgBrowser::OnBnClickedOk()
{
	//// TODO: Add your control notification handler code here
	//OnOK();
}

void CDlgBrowser::OnBnClickedCancel()
{
	//// TODO: Add your control notification handler code here
	//OnCancel();
}
BEGIN_EVENTSINK_MAP(CDlgBrowser, CDialog)
	ON_EVENT(CDlgBrowser, IDC_WEB_BROWSER, 113, TitleChangeWebBrowser, VTS_BSTR)
	ON_EVENT(CDlgBrowser, IDC_WEB_BROWSER, 251, NewWindow2WebBrowser, VTS_PDISPATCH VTS_PBOOL)
	ON_EVENT(CDlgBrowser, IDC_WEB_BROWSER, 273, NewWindow3WebBrowser, VTS_PDISPATCH VTS_PBOOL VTS_UI4 VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()

void CDlgBrowser::TitleChangeWebBrowser(LPCTSTR Text)
{
	m_strTitle = Text;
	pMyParent->PostMessage(MY_MESSAGE, (WPARAM)(m_strTitle.GetBuffer()), (LPARAM)m_myNum);
}

void CDlgBrowser::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	if(m_bInit)
	{
		m_ctrWebBrowser.MoveWindow(0, 0, cx, cy);
	}
	// TODO: Add your message handler code here
}

BOOL CDlgBrowser::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBrowser::NewWindow2WebBrowser(LPDISPATCH* ppDisp, BOOL* Cancel)
{
	// TODO: Add your message handler code here
	CDlgBrowser* pBrowser = new CDlgBrowser;
	pBrowser->Create(theApp.browserIndex, pMyParent);
	*ppDisp = pBrowser->m_ctrWebBrowser.get_Application();
	pMyParent->PostMessage(MY_MESSAGE_NEW, (WPARAM)pBrowser, (LPARAM)theApp.browserIndex);
	theApp.browserIndex++;
}

void CDlgBrowser::NewWindow3WebBrowser(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl)
{
	// TODO: Add your message handler code here
}

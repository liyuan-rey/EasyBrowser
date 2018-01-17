#pragma once
#include "explorer_web.h"

#define MY_MESSAGE WM_USER + 0x00ef
#define MY_MESSAGE_NEW WM_USER + 0x00e0

// CDlgBrowser dialog

class CDlgBrowser : public CDialog
{
	DECLARE_DYNAMIC(CDlgBrowser)

  public:
	CDlgBrowser(CWnd *pParent = NULL); // standard constructor
	virtual ~CDlgBrowser();

	BOOL Create(int num, CWnd *pParent = NULL);
	// Dialog Data
	enum
	{
		IDD = IDD_DIALOG_BROWSER
	};

  protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

	DECLARE_MESSAGE_MAP()
  public:
	CString m_strTitle;
	int m_myNum; // 编号
	CExplorer_web m_ctrWebBrowser;

	BOOL m_bInit;

	CWnd *pMyParent;

  public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	DECLARE_EVENTSINK_MAP()
	void TitleChangeWebBrowser(LPCTSTR Text);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	void NewWindow2WebBrowser(LPDISPATCH *ppDisp, BOOL *Cancel);
	void NewWindow3WebBrowser(LPDISPATCH *ppDisp, BOOL *Cancel, unsigned long dwFlags, LPCTSTR bstrUrlContext, LPCTSTR bstrUrl);
};

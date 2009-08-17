// EasyBrowserDlg.h : 头文件
//

#pragma once
#include "DlgBrowser.h"
#include "MyTabCtrl.h"
#include <vector>
#include "afxcmn.h"
using namespace std;
// CEasyBrowserDlg 对话框
class CEasyBrowserDlg : public CDialog
{
// 构造
public:
	CEasyBrowserDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_EASYBROWSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	DWORD m_dwTabLen;
	BOOL m_bInit;
	CMyTabCtrl m_ctrTab;
	vector<CString> m_vecUrls;
	vector<CDlgBrowser*> m_vecBrowser;

	int curSel;
	CImageList m_imageTab;

	// XML
	IXMLDOMDocument *m_pXMLDoc;

public:
	IXMLDOMNode* GetFirstLevelNode(IXMLDOMElement* pRootNode, CComBSTR bstrChildNodeName);
	IXMLDOMNode* GetChildNode(IXMLDOMNode* pParentNode, CComBSTR bstrChildNodeName);

	BOOL ReadXMLFile(CString filePath);

	void SetSize();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnTcnSelchangeTabEasybw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

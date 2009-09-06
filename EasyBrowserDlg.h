// EasyBrowserDlg.h : ͷ�ļ�
//

#pragma once
#include "DlgBrowser.h"
#include "MyTabCtrl.h"
#include <vector>
#include "afxcmn.h"
using namespace std;
// CEasyBrowserDlg �Ի���
class CEasyBrowserDlg : public CDialog
{
// ����
public:
	CEasyBrowserDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_EASYBROWSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

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

	BOOL ReadXMLFile(CString fileOrStream, BOOL bInMemory = FALSE);

	void SetSize();
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();

	void Buffer2String( CString& str, const BYTE* const buffer, DWORD size )
	{
		CString::PXSTR tmp = str.GetBuffer(size); //TODO: recount buffer size for wchar/unicode situation
		CopyMemory(tmp, buffer, size);
		str.ReleaseBuffer(size);
	}

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

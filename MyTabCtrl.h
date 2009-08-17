#pragma once

#include "Resource.h"
// CMyTabCtrl dialog
#define MY_HITTEST_MSG WM_USER+0xde
class CMyTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CMyTabCtrl)

public:
	CMyTabCtrl();
	virtual ~CMyTabCtrl();

// Dialog Data
	enum { IDD = IDC_TAB_EASYBW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CWnd* pMyParent;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

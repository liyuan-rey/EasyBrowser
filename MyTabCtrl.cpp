// MyTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MyTabCtrl.h"
#include ".\mytabctrl.h"


// CMyTabCtrl dialog

IMPLEMENT_DYNAMIC(CMyTabCtrl, CTabCtrl)
CMyTabCtrl::CMyTabCtrl()
	: CTabCtrl()
{
	pMyParent = NULL;
}

CMyTabCtrl::~CMyTabCtrl()
{
}

void CMyTabCtrl::DoDataExchange(CDataExchange* pDX)
{
	CTabCtrl::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMyTabCtrl, CTabCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CMyTabCtrl message handlers

BOOL CMyTabCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CTabCtrl::PreTranslateMessage(pMsg);
}

void CMyTabCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	TCHITTESTINFO info;
	info.pt = point;
	int index = HitTest(&info);
	if(pMyParent)
		pMyParent->PostMessage(MY_HITTEST_MSG, (WPARAM)index);

	CTabCtrl::OnLButtonDblClk(nFlags, point);
}

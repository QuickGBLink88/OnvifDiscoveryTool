// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ColorStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CColorStatic::CColorStatic()
{
	Brush.CreateSolidBrush(RGB(0, 0, 0));
	m_Back = RGB(0, 0, 0);
	m_Fore = RGB(0, 0, 0);
	m_bCustomFont = FALSE;
}

CColorStatic::~CColorStatic()
{
	Brush.DeleteObject();
	m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers

void CColorStatic::SetColors(COLORREF crFore, COLORREF crBack)
{
	m_Back = crBack;
	m_Fore = crFore;
	Brush.DeleteObject();
	Brush.CreateSolidBrush(crBack);
	Invalidate();
}

HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetBkColor(m_Back);
	pDC->SetTextColor(m_Fore);
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return (HBRUSH) Brush;
}

BOOL CColorStatic::OnEraseBkgnd(CDC* pDC) 
{
	RECT rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, m_Back);
	return TRUE;
}

void CColorStatic::OnPaint() 
{
	CPaintDC dc(this);
	DWORD dwFormat = 0;
	RECT rectWnd;
	CString strText;
	CWnd* pWndParent = NULL;
	GetWindowText(strText);

	if(!strText.IsEmpty())
	{
		pWndParent = GetParent();
		CFont* pOldFont;
		if(m_bCustomFont)
			pOldFont = dc.SelectObject(&m_font);
		else if (pWndParent)
			pOldFont = dc.SelectObject (GetFont());

		GetClientRect(&rectWnd);
		dwFormat = DT_END_ELLIPSIS;
		dc.SetBkColor(m_Back);
		dc.SetTextColor(m_Fore);
		::DrawTextEx(dc.m_hDC, strText.GetBuffer(0), strText.GetLength(), &rectWnd, dwFormat, NULL);
		strText.ReleaseBuffer();
		dc.SelectObject(pOldFont);
	}
}

void CColorStatic::SetFontAttr(int fontHeight,int fontWeight,bool bUnderline)
{
	// set up font
	LOGFONT lf;
	// GetFont()->GetObject(sizeof(lf), &lf);
	lstrcpy (lf.lfFaceName,"ËÎÌו");
	lf.lfCharSet=GB2312_CHARSET;
	lf.lfClipPrecision=0;
	lf.lfEscapement=0;
	lf.lfItalic=0;
	lf.lfOrientation=0;
	lf.lfOutPrecision=0;
	lf.lfPitchAndFamily=0;
	lf.lfQuality=10;
	lf.lfStrikeOut=0;
	lf.lfUnderline=0;
	lf.lfWidth=0;
	lf.lfHeight = fontHeight;
	lf.lfWeight=fontWeight;
	lf.lfUnderline = bUnderline;
	
	if(m_font.m_hObject)
			m_font.DeleteObject();

	m_font.CreateFontIndirect(&lf);
	// CFont *pFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	//SetFont(pFont);
	m_bCustomFont = TRUE;
}
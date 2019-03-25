#if !defined(AFX_J_EditEx_H__50185A3F_CCB7_4D83_89A4_F0107B23EF51__INCLUDED_)
#define AFX_J_EditEx_H__50185A3F_CCB7_4D83_89A4_F0107B23EF51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// J_EditEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJ_EditEx window

class CJ_EditEx : public CEdit
{
	DECLARE_DYNAMIC(CJ_EditEx)
// Construction
public:
	CJ_EditEx();

// Attributes
public:
	CString m_strTip;	//��ʾ������

// Operations
public://ע�⣺����ֵΪOldֵ
	bool JEnableTip(bool bl_tip=true,bool bl_tipByEdit=true);	//��ʾ��
	CString JSetStrTip(CString strTip);//������ʾ��Ϣ
	bool JSetOnlyDecStr(bool bl_DecStr=false,int nLengthLimit=0);//�Ƿ�ֻ��ʾ10������
	bool JSetOnlyHexStr(bool bl_HexStr=false,int nLengthLimit=0,bool bl_flag0x=false);//�Ƿ�ֻ��ʾ16������
	COLORREF JSetColor(COLORREF crText=RGB(0,0,0),COLORREF crBk=RGB(255,255,255),int iBkMode=TRANSPARENT);//������ʾ��ɫ
	
	BOOL J_IsMutilLine();//�Ƿ��Ƕ���Edit

private:
	CToolTipCtrl *pTip;
	bool m_bltip;
	bool m_bltipByEdit;

	bool m_blDecStr;
	bool m_blHexStr;
	int m_LenLimitDec;
	int m_LenLimitHex;
	bool m_blflag0x;

	COLORREF m_clrText;
	COLORREF m_clrBk;
	CBrush m_brushBack;
	int m_clrBkMode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJ_EditEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJ_EditEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CJ_EditEx)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_J_EditEx_H__50185A3F_CCB7_4D83_89A4_F0107B23EF51__INCLUDED_)

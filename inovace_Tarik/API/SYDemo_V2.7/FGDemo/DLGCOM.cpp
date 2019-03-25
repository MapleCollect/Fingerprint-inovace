// DLGCOM.cpp : implementation file
//

#include "stdafx.h"
#include "FGDemo.h"
#include "DLGCOM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLGCOM dialog


CDLGCOM::CDLGCOM(CWnd* pParent /*=NULL*/)
	: CDialog(CDLGCOM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDLGCOM)
	m_iCom = 0;
	//}}AFX_DATA_INIT
}


void CDLGCOM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDLGCOM)
	DDX_CBIndex(pDX, IDC_COMBO1, m_iCom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDLGCOM, CDialog)
	//{{AFX_MSG_MAP(CDLGCOM)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLGCOM message handlers

BOOL CDLGCOM::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDLGCOM::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	CDialog::OnOK();
}

void CDLGCOM::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(g_iLanguage==0x0804)
	{
		this->SetWindowText("���豸");
		GetDlgItem(IDC_SETTING)->SetWindowText("���ô��ڲ���");
		GetDlgItem(IDC_COM)->SetWindowText("���ں�:");
		GetDlgItem(IDC_INFO)->SetWindowText("�����������USB��UDISK�豸�������������ڣ�\r\n��Ҫ�����ô��ں�");
		GetDlgItem(IDOK)->SetWindowText("ȷ��");
		GetDlgItem(IDCANCEL)->SetWindowText("ȡ��");
	}
	else
	{
		this->SetWindowText("Open Device");
		GetDlgItem(IDC_SETTING)->SetWindowText("Select COM");
		GetDlgItem(IDC_COM)->SetWindowText("COM:");
		GetDlgItem(IDC_INFO)->SetWindowText("Software will first search for USB and UDISK equipment, and then search for the serial port.");
		GetDlgItem(IDOK)->SetWindowText("OK");
		GetDlgItem(IDCANCEL)->SetWindowText("Cancel");
	}
}

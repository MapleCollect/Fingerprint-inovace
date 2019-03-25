// DlgFGDB.cpp : implementation file
//

#include "stdafx.h"
#include "FGDemo.h"
#include "DlgFGDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFGDB dialog
#include "ToolEx\\PathDialog.h"
#define	  IMAGE_SIZE (256*288)

CDlgFGDB* pDlgFGDB;
BOOL g_bStopRun;
CString g_sFileName;
UINT J_gRunGenImg(LPVOID pParam);
UINT J_gSigGenImg(LPVOID pParam);

#include <shlwapi.h>
extern TCHAR szUserLogoPath[255];

#define		IMAGE_SIZE (256*288)
#define		IMAGE_W	256
#define		IMAGE_H	288
extern BOOL g_bImgpos;//ͼ������У��
////////////////////////////
CDlgFGDB::CDlgFGDB(HANDLE hHandle,UINT nAddr/*=0xFFFFFFFF*/,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFGDB::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFGDB)
	m_nNumFPin = 5;
	m_FGDBPath = _T("C:\\FGDB_BMP");
	m_RunNum = _T("000000");
	m_SinNum = _T("000000");
	//}}AFX_DATA_INIT
	m_hHandle = hHandle;
	m_nAddr	  = nAddr;
}


void CDlgFGDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFGDB)
	DDX_Text(pDX, IDC_EDIT1, m_nNumFPin);
	DDX_Text(pDX, IDC_EDIT2, m_FGDBPath);
	DDX_Text(pDX, IDC_EDIT3, m_RunNum);
	DDX_Text(pDX, IDC_EDIT4, m_SinNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFGDB, CDialog)
	//{{AFX_MSG_MAP(CDlgFGDB)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFGDB message handlers

void CDlgFGDB::J_bImgPosN(BYTE *pImg/*=NULL*/,BOOL bImgPN/*=0*/)//ͼ����������
{
	if(pImg==NULL || !bImgPN)
		return;
	BYTE pBuf[IMAGE_SIZE]={0};
	for(int i=0; i<IMAGE_H; i++)
	{
		memcpy(pBuf+i*IMAGE_W,pImg+(IMAGE_H-1-i)*IMAGE_W,IMAGE_W*sizeof(BYTE));
	}
	memcpy(pImg,pBuf,IMAGE_SIZE);
	return;
}

BOOL CDlgFGDB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(g_iLanguage==LGN_CHINA)//����
	{
		SetWindowText("�ɼ�����");
		GetDlgItem(IDC_STATIC1)->SetWindowText("ÿ����ָ��������: ");
		GetDlgItem(IDC_STATIC2)->SetWindowText("ָ�ƿ�·��:");
		GetDlgItem(IDC_STATIC3)->SetWindowText("�����ɼ�ͼ��");
		GetDlgItem(IDC_STATIC4)->SetWindowText("ͼ����:");
		GetDlgItem(IDC_STATIC5)->SetWindowText("�ɼ�����ͼ��");
		GetDlgItem(IDC_STATIC6)->SetWindowText("ͼ����:");
		GetDlgItem(IDC_BUTTON2)->SetWindowText("��ʼ�ɼ�");
		GetDlgItem(IDC_BUTTON3)->SetWindowText("ȡ��");
		GetDlgItem(IDC_BUTTON4)->SetWindowText("�ɼ�");
		GetDlgItem(IDC_BUTTON5)->SetWindowText("ȡ��");
		GetDlgItem(IDCANCEL)->SetWindowText("�˳�");
	}
	else
	{
		SetWindowText("Capture fingerprint");
		GetDlgItem(IDC_STATIC1)->SetWindowText("Each finger number: ");
		GetDlgItem(IDC_STATIC2)->SetWindowText("Fingerprint database path:");
		GetDlgItem(IDC_STATIC3)->SetWindowText("Continuous Capture");
		GetDlgItem(IDC_STATIC4)->SetWindowText("FG_Number:");
		GetDlgItem(IDC_STATIC5)->SetWindowText("Capture One");
		GetDlgItem(IDC_STATIC6)->SetWindowText("FG_Number:");
		GetDlgItem(IDC_BUTTON2)->SetWindowText("Start Capture");
		GetDlgItem(IDC_BUTTON3)->SetWindowText("Cancel");
		GetDlgItem(IDC_BUTTON4)->SetWindowText("Capture");
		GetDlgItem(IDC_BUTTON5)->SetWindowText("Cancel");
		GetDlgItem(IDCANCEL)->SetWindowText("Exit");
	}

	pDlgFGDB = this;
	g_bStopRun = TRUE;
	char path[255] = {0};
	J_GetPathName(path);
	SetDlgItemText(IDC_EDIT2,path);
	ShowDefaltImg();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CDlgFGDB::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(nCtlColor == CTLCOLOR_EDIT)//�༭��_��ɫ
	{
		pDC->SetTextColor(RGB(250,50,50));
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CDlgFGDB::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x, point.y));
	CDialog::OnLButtonDown(nFlags, point);
}
//---------------------------------------------------------------------
//��ȡ�ļ�����·��
//���룺iMode 0-�ļ�����·��	1-�ļ���	2-·��
//�����pstr[MAX_PATH]
int CDlgFGDB::J_GetPathName(char* pstr,int iMode/*=2*/)
{
	if(pstr==NULL || iMode<0 || iMode>2 )
		return -1;
	char pBuf[MAX_PATH] = {0};
	char *pName;
	int iLen=0,iTmp=0;
	GetModuleFileName(NULL,(LPTSTR)pBuf,MAX_PATH);
	if(0==iMode){
		iLen = strlen(pBuf);
		memcpy(pstr,pBuf,iLen+1);//0-�ļ�����·��
		return 0;
	}
	pName = strrchr(pBuf,(int)('\\'));
	iLen = strlen(pName);
	if(1==iMode){//1-�ļ���
		memcpy(pstr,(pName+1),iLen);
		return 0;
	}
	if(2==iMode){//2-·��
		iTmp = iLen;
		iLen = strlen(pBuf);
		memcpy(pstr,pBuf,iLen-iTmp);
		return 0;
	}
	return -1;
}
int CDlgFGDB::J_AfxMsgBox(CString str_CH, CString str_EN, UINT nType/* = MB_OK*/, UINT nIDHelp/* = 0*/)
{
	if(g_iLanguage==LGN_CHINA)//����
		return AfxMessageBox(str_CH,nType,nIDHelp);
	else
		return AfxMessageBox(str_EN,nType,nIDHelp);
	return 1;
}
void CDlgFGDB::J_EnableBtn(BOOL bEnable)
{
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON2))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON3))->EnableWindow(!bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON4))->EnableWindow(bEnable);
	((CButton*)GetDlgItem(IDC_BUTTON5))->EnableWindow(!bEnable);
	((CButton*)GetDlgItem(IDCANCEL))->EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////
void CDlgFGDB::OnButton1() //·��
{
	CString str1,str2,sPath;
	if(g_iLanguage==LGN_CHINA){//����
		str1="����ļ���";
		str2="��ѡ��һ���ļ���";
	}
	else{
		str1="Browse folder";
		str2="Please select a folder";
	}
	GetDlgItemText(IDC_EDIT2,sPath);
	CPathDialog pd(str1,str2,sPath,NULL);
	if(pd.DoModal()!=IDOK)
		return;
	sPath = pd.GetPathName();
	SetDlgItemText(IDC_EDIT2,sPath);
}

UINT J_gRunGenImg(LPVOID pParam)
{	
	int FingerCode;	//��ָ����
	int	SerialCode;	//��һ����ָ����ű���
	int nNumFGPin = (int)(pDlgFGDB->m_nNumFPin);
 	while(!g_bStopRun)
	{	
		SerialCode = 1;
		//������Ҫ�����ͼ�����·��
		SerialCode = atoi(pDlgFGDB->m_RunNum)%10;
  		FingerCode = atoi(pDlgFGDB->m_RunNum)/10;
		if( (SerialCode<0) || (SerialCode>=nNumFGPin ) )
		{
			pDlgFGDB->J_AfxMsgBox("�����ͼ����!","The image number is error!");
			pDlgFGDB->J_EnableBtn();
			return 1;
		}
		g_sFileName.Format("%s\\%s.bmp",pDlgFGDB->m_FGDBPath,pDlgFGDB->m_RunNum);
		SerialCode ++; 
		if(pDlgFGDB->J_GetImage()!=0){///?????????
			pDlgFGDB->J_EnableBtn();
			return 1;
		}
		if(g_bStopRun){
			pDlgFGDB->J_EnableBtn();
    		return 1;
		}
		if(SerialCode==nNumFGPin)
		{
			SerialCode = 0;
			FingerCode ++;
			if( (pDlgFGDB->J_AfxMsgBox("�뻻��ָ��","Please change a finger!",MB_OKCANCEL))==IDCANCEL){
				pDlgFGDB->J_EnableBtn();
				return 1;
			}
		}
 		pDlgFGDB->m_RunNum.Format("%05d%d",FingerCode,SerialCode);
		pDlgFGDB->SetDlgItemText(IDC_EDIT3,pDlgFGDB->m_RunNum);
	}

	return 0;
}
void CDlgFGDB::OnButton2() //��ʼ�ɼ�
{
	UpdateData();
	int ret=0;

	if(m_nNumFPin<1 || m_nNumFPin>10 ){
		J_AfxMsgBox("��������ȷ������������","Please the correct number");
		return ;
	}
	if( m_FGDBPath=="" ){
		J_AfxMsgBox("��������ȷ��ָ�ƿ�Ŀ¼��","Please select a fingerprint folder!");
		return;
	}
	J_EnableBtn(0);
	g_bStopRun = FALSE;	
 	AfxBeginThread( J_gRunGenImg,NULL);
}

void CDlgFGDB::OnButton3() 
{
	g_bStopRun = TRUE;
}

void CDlgFGDB::OnButton5() 
{
	g_bStopRun = TRUE;
}

int CDlgFGDB::J_GetImage() 
{ 
	int ret,i=0,ImgLen;
	BYTE ImgData[IMAGE_SIZE]={0}; 

	//�뽫��ָƽ���ڴ�������...
	ret = PSGetImage(m_hHandle,m_nAddr);  //��ȡͼ��  
	while(ret==PS_NO_FINGER)
	{
		if( (ret==-1) || (ret==-2) || (ret==1) )//�շ����ݰ�ʧ��ʱ�˳�
			return 1;
		ret=PSGetImage(m_hHandle,m_nAddr);
		if(g_bStopRun)
			return 1;
	}
	if(ret!=PS_OK){
		AfxMessageBox( PSErr2Str(ret) );
		return ret;
	}
	//ͼ�������ϴ�...
	ret = PSUpImage(m_hHandle,m_nAddr,ImgData,&ImgLen);  //�ϴ�ͼ��
	J_bImgPosN(ImgData,g_bImgpos);//######ͼ����������
	if(ret!=PS_OK){
		AfxMessageBox( PSErr2Str(ret) );
		return ret;
	}
	
	ret= PSImgData2BMP(ImgData,g_sFileName);
	
    if(ret!=PS_OK){
		AfxMessageBox( PSErr2Str(ret) );
		return ret;
	}
	//ͼ��¼��ɹ�!
	J_ShowImage();
	return PS_OK;//0
}

void CDlgFGDB::ShowDefaltImg() 
{
	HBITMAP hBitmapFP;
	if (PathFileExists(szUserLogoPath))
		hBitmapFP = (HBITMAP)LoadImage( NULL,szUserLogoPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	else
		hBitmapFP = LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP1));

	((CStatic*)GetDlgItem(IDC_BITMAP))->SetBitmap(hBitmapFP);
//	DeleteObject(hBitmapFP);
}

void CDlgFGDB::J_ShowImage() 
{
	HBITMAP hBitmapFP = (HBITMAP)LoadImage( GetModuleHandle(NULL),(LPCTSTR)g_sFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmapFP  == NULL)
		hBitmapFP  = (HBITMAP)LoadImage( NULL,(LPCTSTR)g_sFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
	((CStatic*)GetDlgItem(IDC_BITMAP))->SetBitmap(hBitmapFP);
	DeleteObject(hBitmapFP);	
}

UINT J_gSigGenImg(LPVOID pParam)
{	
	int ret;
	ret = pDlgFGDB->J_GetImage();
	pDlgFGDB->J_EnableBtn();
	g_bStopRun = TRUE;
	return ret;
}

void CDlgFGDB::OnButton4() 
{
	UpdateData();
	if( m_FGDBPath=="" ){
		J_AfxMsgBox("��������ȷ��ָ�ƿ�Ŀ¼��","Please select a fingerprint folder!");
		return;
	}
	//��Ҫ�����ͼ�����·��
	g_sFileName.Format("%s\\%s.bmp",m_FGDBPath,m_SinNum);
	J_EnableBtn(0);
	g_bStopRun = FALSE;
	AfxBeginThread( J_gSigGenImg,NULL);
}

void CDlgFGDB::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (g_bStopRun)
    {
		ShowDefaltImg(); 
    }
	// Do not call CDialog::OnPaint() for painting messages
}

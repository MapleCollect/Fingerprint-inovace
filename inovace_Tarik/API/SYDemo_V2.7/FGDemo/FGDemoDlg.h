// FGDemoDlg.h : header file
//

#if !defined(AFX_FGDEMODLG_H__E502968B_DE7D_4FDD_A4BF_254476CC1AE8__INCLUDED_)
#define AFX_FGDEMODLG_H__E502968B_DE7D_4FDD_A4BF_254476CC1AE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFGDemoDlg dialog
#include "ToolEx\\XInfoTip.h"
#include "ToolEx\\J_EditEx.h"
#include "ToolEx\\ShadeButtonST.h"

//���ݽṹ
typedef struct _JsDevHANDLE
{
	HANDLE hHandle;	//�豸���
	int nDevType;	//�豸����	USB-0	COM-1	UDisk-2
	int iCom;		//���ں�
	int iDevId;		//�豸Id
	DWORD dwPwd;	//�豸����	0x00000000
	DWORD dwAddr;	//�豸��ַ	0xFFFFFFFF
	CString	sDevName;	//�豸��Ϣ	USB_No.0	Dev_COM1	UDisk_No.0
	CString sMsg_CH;	//Ӳ����Ϣ_CH	����
	CString sMsg_EN;	//Ӳ����Ϣ_EN	Ӣ��
	int iBaud_x;		//������		$ 0 - 9600  1 - 19200   2 - 38400   3 - 57600   4 - 115200
	int iPacketSize_x;	//���ݰ���С	$ 0 - 32    1 - 64      2 - 128     3 - 256
	int iSecureLev_x;	//��ȫ�ȼ�		$ 0 - 1     1 - 2       2 - 3       3 - 4       4 - 5
	int iMbMax;	//ָ�ƿ��С
	_JsDevHANDLE *next;//ָ����һ����ָ��
//	int nPackSize;
}JsDevHandle,*pJsDevHandle;

#define LGN_CHINA	0x0804	//���� ����(����)
#define LGN_EN		0x0000	//���� Ӣ��
/////////////////////////////////////////////
class CFGDemoDlg : public CDialog
{
// Construction
public:
	CFGDemoDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CFGDemoDlg();

// Dialog Data
	//{{AFX_DATA(CFGDemoDlg)
	enum { IDD = IDD_FGDEMO_DIALOG };
	CComboBox	m_cbSensorType;
	CComboBox	m_ctlDevHandle;
	CListCtrl	m_list;
	CJ_EditEx	m_STctlAddr;
	CJ_EditEx	m_STctlPwd;
	CStatic	m_bmp;
	CString	m_strAddr;
	CString	m_strPwd;
	int		m_iDevType;
	int		m_iCom;
	int		m_iBaud;
	int		m_iDevHandle;
	int		m_iLG;
	BOOL	m_bShowBMP;
	BOOL	m_bReplaceMB;
	int		m_iBaud_x;
	int		m_iPackSize_x;
	int		m_iSLev_x;
	int		m_iDevID;
	int		m_iSensorType;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFGDemoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
////////////////////////////////////////////////////////////////
private:
	CXInfoTip m_tooltip;
	CShadeButtonST m_sbtn1;
	CShadeButtonST m_sbtn2;
	CShadeButtonST m_sbtn3;
	CShadeButtonST m_sbtn4;
	CShadeButtonST m_sbtn5;
protected:
	BOOL m_bSpBtn;		//�Ƿ���ʾ���⹦�ܰ�ť
	BOOL m_bDevOpen;	//�ж��豸�Ƿ��
	BOOL m_blShowTip;	//�Ƿ���ʾToolTip
	CFont m_font;	//����sMsg������
	BOOL m_bBlue;	//����sMsg����ɫ TRUE-BLUE FALSE=RED
	BOOL m_bRefBMP; //����ˢ����ʾͼ��
	
public:
	void SaveUserInfo();
	void LoadUserDefine(HANDLE hl=NULL,BOOL bInit=FALSE);//�����ļ�
	void J_MyBtn(CShadeButtonST* msbtn,UINT nID,UINT iIcon=0,UINT shadeID=CShadeButtonST::SHS_METAL,COLORREF rgb=RGB(50,50,50));
	void J_EnableBtn(BOOL bEnable=1,BOOL bEcl=1,BOOL bOpen=TRUE);
	void J_EnableSet(BOOL bEnable=1,int nType=DEVICE_USB);//�豸����
	void J_SetLanuage(int iLanguage=LGN_CHINA);//�������� ����/Ӣ��
	int  J_GetDevInfo(HANDLE hHandle,int nAddr, CString& str_CH, CString& str_EN,int* iMbMax);//��ȡ�豸Ӳ����Ϣ
	int  J_GetBaudMul(int imbo=0,int way=0);//��ȡ�����ʱ���N,����9600
	BOOL J_CheckDev(CString sDevInfo);//�ж��豸�Ƿ����Ѵ򿪣����ڱ���ͬһ�豸���ظ���
	BOOL J_GetDevHandle(pJsDevHandle& pDevHandle,CString sDevName);//��ȡ�豸���
	BOOL J_GetCurDevHandle(pJsDevHandle& pDevHandle);//��ȡ��ǰ��Ч�豸���
	BOOL J_CloseDev(CString sDevName);//�ر��豸
	void J_SetMsg1(CString str,BOOL bBlue=TRUE);
	void J_SetMsg2(CString str_CH,CString str_EN,BOOL bBlue=TRUE);//�Զ�ѡ������
	void ShowDefaltImg();
	void ShowImage(char *filename=NULL); //��ʾͼ��
	void J_LoadInfo2List();//����ָ��������List�б�
	BOOL J_IsExistId(int iIndex);
	void J_AfxMsgBox(CString str_CH, CString str_EN);
	void J_ShowDemoImage(int iMode=-1);// -1 ԭʼ(Ĭ��), 0 ��ֵ��ͼ, 1 ϸ��ͼ, 2 ������ϸ��ͼ 
	int  J_WriteReg(int iType,int nVal);
	void J_bImgPosN(BYTE *pImg=NULL, BOOL bImgPN=0);//ͼ����������
	void J_SetFocus(UINT nID);

	void J_SetIMGFILE(int type=0);//����Ĭ����ʱ�ļ�·��

	
	int J_GetBmpDataFromFile(const LPCTSTR lpFileName,BYTE* pBuf,int *iWidth,int *iHeigh);//��BMP�ļ���ȡͼ������
	int J_GetBMPSize(CString file,int* image_x,int* image_y);//��ͼ��ߴ�
	int J_SaveBmpDataToFileEx8(const LPCTSTR lpFileName,BYTE* pBuf,int iWidth,int iHeight);//BMPͼ�����ݱ���ΪBmp�ļ�_
////////////////////////////////////////////////////////////////
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFGDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeComboLg();
	virtual void OnOK();
	afx_msg void OnSelchangeCOMBODev();
	afx_msg void OnCloseDev();
	afx_msg void OnCloseDev2();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton9();
	afx_msg void OnButton10();
	afx_msg void OnButton11();
	afx_msg void OnButton12();
	afx_msg void OnButton13();
	afx_msg void OnButton24();
	afx_msg void OnButton25();
	afx_msg void OnButton26();
	afx_msg void OnButton18();
	afx_msg void OnButton16();
	afx_msg void OnButton14();
	afx_msg void OnButton15();
	afx_msg void OnButton19();
	afx_msg void OnButton20();
	afx_msg void OnButton21();
	afx_msg void OnButton22();
	afx_msg void OnButton23();
	afx_msg void OnButton17();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnSelchangeCombo3();
	afx_msg void OnButton27();
	afx_msg void OnButton28();
	afx_msg void OnSelchangeCombo4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FGDEMODLG_H__E502968B_DE7D_4FDD_A4BF_254476CC1AE8__INCLUDED_)

 
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "SYDevice.h"
#include "pusb.h"
#include "serial.h"
#include "pusb-linux.h"
//ԐȽ
#define ST_VENDOR  0x0453 //0x0fbc
#define ST_PRODUCT 0x9005//0x3090

//vendor=0204 ProdID=6025
#define SU_VENDOR  0x2109//0x0204
#define SU_PRODUCT 0x7638//0x6025

//ϞȽ
//#define SU_VENDOR  0x0fbc	
//#define SU_PRODUCT 0x3090
//#define ST_VENDOR  0x0AEC
//#define ST_PRODUCT 0x3080

//#define ST_VENDOR  0x071B
//#define ST_PRODUCT 0x3102
////////////////////////////////////////////////
/////           ���ڲ���                  //////
////////////////////////////////////////////////

 int       g_nComPort     = 0;     //���ں�
 int       g_nBaudRate    = 57600; //������

const int   WAITTIME_PER_BYTE =  100;   //��ÿ���ֽ�ʱ��(ms)
static pusb_device_t fdusb = NULL;
/**********************************************
  FUNCTION:   InitCom
  PURPOSE:    ��ʼ������
  ARGS:
       comPort:    ���ں�,��1��8
	   baudRate:   ������,0-9600,1-19200,2-38400   3-57600  4-115200  ȱʡΪ3-57600
  RETURN:
       0:    ʧ��
	   1:    �ɹ�
***********************************************/
BOOL   OpenCom(int comPort, int baudRate)
{
	int baud;
 
	baud=baudRate*9600; 

	//��¼������Ϣ
	g_nBaudRate = baud;
	g_nComPort = comPort;

	if( OpenComPort(comPort,baud,8,"2",0)==0)
       {
                ClearComPort();
                
		 return TRUE;
       }

	return FALSE;
}

/**********************************************
  FUNCTION:   ReleaseCom
  PURPOSE:    �رմ���
***********************************************/
BOOL   CloseCom()
{
	CloseComPort();
	return TRUE;
}
 
void ClearCom()
{
  ClearComPort();
}

/**********************************************
  FUNCTION:   GetByte
  PURPOSE:    �Ӵ��ڶ�һ���ֽ�
  ARGS:
       tranChar�� Ҫ���յ��ֽ�
  RETURN:
       0:    ʧ��
	   1:    �ɹ�
***********************************************/
BOOL   GetByte(unsigned char* tranChar)
{
	if (tranChar == NULL)
		return FALSE;

   if(ReadComPort(tranChar,1)==-1)
	   return FALSE;

    return TRUE;
}

/**********************************************
  FUNCTION:   SendByte
  PURPOSE:    �򴮿�дһ���ֽ�
  ARGS:
       tranChar�� Ҫ���͵��ֽ�
  RETURN:
       0:    ʧ��
	   1:    �ɹ�
***********************************************/
BOOL  SendByte(unsigned char tranChar)
{
   
    unsigned char pBuf[2]={0};
    pBuf[0]=tranChar;
    if(WriteComPort(pBuf,1)!=1)
		return FALSE;

    return TRUE;
} 



BOOL CloseUSB();

BOOL OpenUSB()
{
	CloseUSB();
	assert(fdusb == NULL);
	
	fdusb = pusb_search_open(ST_VENDOR,ST_PRODUCT);
	if (fdusb == NULL) {
		return 0;
	}
	return 1;
}

BOOL CloseUSB()
{
	if(fdusb != NULL) {
		pusb_close(fdusb);
		fdusb = NULL;
		return 1;
	}
	return 0;
}

void dDelay(int nTimes)
{
  usleep(nTimes);
  return;
 }
//----Send ctrl package to device for get result---------
BOOL SendCtrlPackage(int nRequestType,int nCmdLen)
{
   //send
  unsigned char pData[32]={0};
  int nSent = 0;
  int i;

  //send cmd request
  for (i=0; i<100; ++i)   {
    
    nSent = pusb_control_msg(fdusb,0xc0,nRequestType, 
		     nCmdLen, 0, pData, 2, 500);

    if (nSent >= 2) {
      break;
    dDelay(100);
    }
  }
  if(i >= 100) 
  {
     // printf("ctrl error\n");
      return  FALSE;
  }

  return TRUE;
}
BOOL SendCtrlPackageUDisk(int nReqTp, int nReq,int nCmdLen,unsigned char *pData)
{
   //send
//  unsigned char pData[32]={0};
  int nSent = 0;
  int i;

  //send cmd request
  for (i=0; i<10; ++i)   {
    
    nSent = pusb_control_msg(fdusb,nReqTp,nReq, 
		     nCmdLen, 0, pData, 31, 500);

    if (nSent >= 2) {
      break;
    dDelay(100);
    }
  }
  if(i >= 10) 
  {
      printf("ctrl error\n");
      return  FALSE;
  }

  return TRUE;
}
int BulkRecvPackage(unsigned char* DataBuf,int nLen,int nTimeOut)
{
	//printf("----into BulkRecvPackage function----\n");
	pusb_endpoint_t pedt;
        pedt=pusb_endpoint_open(fdusb,0x81,0);//0x01
	if(pedt==NULL) return -2;
       
        int nRecLen=pusb_endpoint_read(pedt,DataBuf,nLen,nTimeOut);
       // printf("bulk recv %d\n",nRecLen);

	if(nRecLen!=nLen)
	{//printf("bulk recv error\n");
		pusb_endpoint_close(pedt);
		return -3;
	}
        pusb_endpoint_close(pedt);

	return 0;

}
int BulkSendPackage(unsigned char* DataBuf,int nLen,int nTimeOut)
{
	//printf("----into BulkSendPackage function----\n");
	pusb_endpoint_t pedt;
        pedt=pusb_endpoint_open(fdusb,0x02,0);//0x82
	if(pedt==NULL) return -2;

         int nSendLen=pusb_endpoint_write(pedt,DataBuf,nLen,nTimeOut);

         //printf("Bulk send %d\n",nSendLen);

	if(nSendLen!=nLen)
	{//printf("bulk send error\n");
		pusb_endpoint_close(pedt);
		return -3;
	}
	pusb_endpoint_close(pedt);

	return 0;

}
//�հ�����Щ��һ�㶼��64�ֽڣ�
 
int USBGetData(unsigned char* DataBuf,int nLen)
{
	int nRet;

	/*if (!OpenUSB())
	{
		CloseUSB();
		return -1;
	}
	*/
 
   if(!SendCtrlPackage(1,nLen))
	   return -1;
   // Delay(100);
   
   nRet=BulkRecvPackage(DataBuf,nLen,10000);
	//CloseUSB();
 
	return nRet;	
}
//������������Щ��һ�㶼��64�ֽڣ���ͨ���������һ����Ҫ���صĿ��ư���Ȼ��ͨ��Bulk����64�ֽڵ�������ʽ�� 
int USBDownData(unsigned char *DataBuf,int nLen)
{
	int nRet;

	/*if (!OpenUSB())
	{
		CloseUSB();
		return -1;
	}
	*/
   if(!SendCtrlPackage(0,nLen))
	   return -1;
   //Delay(100);

   nRet=BulkSendPackage(DataBuf,nLen,10000);
	//CloseUSB();
 
	return nRet;	
}
//Down Char�Ƚ�����
int USBDownData1(unsigned char *image,int nLen)
{	
   int nRet;

	/*if (!OpenUSB())
	{
		CloseUSB();
		return -1;
	}
	*/
   if(!SendCtrlPackage(1,nLen))
	   return -1;

   nRet=BulkSendPackage(image,nLen,8000);
	//CloseUSB();
 
	return nRet;	
} 

int DeviceConnected()
{
	BOOL b = OpenUSB();
	CloseUSB();
	return b?0:-1;
}

//��ȡͼ�����ݣ�����λ�ȡ
int USBGetImage(unsigned char* DataBuf,int nLen)
{	
	int nRet;

	/*if (!OpenUSB())
	{
		CloseUSB();
		return -1;
	}*/
   if(!SendCtrlPackage(1,nLen))
	   return -1;

   nRet=BulkRecvPackage(DataBuf,nLen/2,8000);

   if(nRet!=0) return nRet;

   nRet=BulkRecvPackage(DataBuf+nLen/2,nLen/2,8000); 
 
	//CloseUSB();
 
	return nRet;	
}
//����ͼ�����ݣ����Ĵ��´�
int USBDownImage(unsigned char *DataBuf,int nLen)
{	
	int nRet;

	/*if (!OpenUSB())
	{
		CloseUSB();
		return -1;
	}*/
	

   if(!SendCtrlPackage(1,nLen))
	   return -1;

   nRet=BulkSendPackage(DataBuf,nLen/4,8000);

   if(nRet!=0) return nRet;

   nRet=BulkSendPackage(DataBuf+nLen/4,nLen/4,8000); 

   if(nRet!=0) return nRet;

    nRet=BulkSendPackage(DataBuf+nLen/2,nLen/4,8000); 

   if(nRet!=0) return nRet;

    nRet=BulkSendPackage(DataBuf+nLen*3/4,nLen/4,8000); 

	//CloseUSB();
 
	return nRet;		

}

///////////////////////////////////////////////////////////////////////////
//ϞȽ²¿·ԍ
///////////////
#include <errno.h>
#include <sys/ioctl.h>
struct tagUSB_device_t
{
	int fd;
};

struct tagUSB_endpoint_t
{
	int fd;
	int ep;
};
//´򿫉豸
BOOL OpenUDisk()
{
	//printf("=====into openudisk function=====\n");
	CloseUDisk();
	assert(fdusb == NULL);
	int ret=-1,interface = 1,i=0;
	
	fdusb = pusb_search_open(SU_VENDOR,SU_PRODUCT);
	if (fdusb == NULL) {
		return FALSE;
	}
	//printf("pusb_serch_open function is success\n");
	
	//1.Disconnect
	ret = pusb_ioctl(fdusb,0,USBDEVFS_DISCONNECT,NULL);
	//printf("pusb_ioctl function ret is %d\n", ret);
	perror("USBDEVFS_IOCTL DISCONNECT");
	usleep(10000);
	
	//2.Claim
	for(i = 0; i < interface; i++)
	{
		ret = ioctl(fdusb->fd, USBDEVFS_CLAIMINTERFACE, &i); 
		perror("USBDEVFS_CLAIMINTERFACE");
	}
	
	//3.Reset
	//如果程序遇到ioctl:devices or resource is busy 的错误请屏蔽此程序段
/*
	ret = ioctl(fdusb->fd, USBDEVFS_RESET);
	perror("USBDEVFS_RESET");
        printf("openudisk function is success,here\n");
*/

	return TRUE;
}

//¹ر։豸
BOOL CloseUDisk()
{
	if(fdusb != NULL) {
		pusb_close(fdusb);
		fdusb = NULL;
		return TRUE;
	}
	return TRUE;
}

int UDiskGetData(unsigned char* DataBuf, int nLen)
{//ģʽ DO_CBW + DI_DATA + DI_CSW
	int ret = -1,i=0;
	unsigned char do_CBW[33] = {0x55,0x53,0x42,0x43,
					'S','y','n','o',	//0xb0,0xfa,0x69,0x86,
								0x00,0x00,0x00,0x00,
								0x80,0x00,0x0a,0x85,
								0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
	unsigned char di_CSW[16] = {0x00};

	do_CBW[8] = nLen & 0xff;
	do_CBW[9] = (nLen>>8) & 0xff;
	do_CBW[10]= (nLen>>16)& 0xff;
	do_CBW[11]= (nLen>>24)& 0xff;

	//DO_CBW
	ret = BulkSendPackage(do_CBW,31,8000);
	if(ret!=0){
		printf("1...UDiskGetData fail!\n");
		return -311;
	}

	//DI_DATA
	ret = BulkRecvPackage(DataBuf,nLen,10000);
	if(ret!=0)
		return -312;

	//DI_CSW
	ret = BulkRecvPackage(di_CSW,13,10000);
	if(di_CSW[3]!=0x53 || di_CSW[12]!=0x00)
		return -313;//UDisk½Ԋ֊�񋈉di_CSW[3]=0x43;
	for(i=4; i<8; i++){
		if(di_CSW[i]!=do_CBW[i])
			return -313;
	}

	return 0;
}

//·¢̍˽¾ۍ
int UDiskDownData(unsigned char* pBuf, int nLen)
{//ģʽ DO_CBW + DO_CMD + DI_CSW
	printf("-----into UDiskDownData function----\n");
	int ret = -1,i;
	unsigned char do_CBW[33] = {0x55,0x53,0x42,0x43,
			'S','y','n','o',
			//	0xb0,0xfa,0x69,0x86,
								0x00,0x00,0x00,0x00,
								0x00,0x00,0x0a,0x86,
								0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};
	unsigned char di_CSW[16] = {0x00};

	do_CBW[8] = nLen & 0xff;
	do_CBW[9] = (nLen>>8) & 0xff;
	do_CBW[10]= (nLen>>16)& 0xff;
	do_CBW[11]= (nLen>>24)& 0xff;

	//DO_CBW
	ret = BulkSendPackage(do_CBW,31,10000);
	if(ret!=0){
		printf("1...UDiskDownData fail!\n");
		for(i=0; i<31; i++)
			printf("0x%02X ",do_CBW[i]);
		printf("\n");
		return -301;//UDisk·¢̍˽¾ݴ
	}
	//printf("BulkSendPackage is ok\n");

	//DO_CMD
	ret = BulkSendPackage(pBuf,nLen,10000);
	if(ret!=0)
		return -302;//UDisk·¢̍˽¾ݴ
	//printf("BUlkSendPackage two is ok\n");

	//DI_CSW
	ret = BulkRecvPackage(di_CSW,13,10000);
	if(di_CSW[3]!=0x53 || di_CSW[12]!=0x00)
		return -303;//UDisk·¢̍˽¾ݴ
	di_CSW[3]=0x43;
	for(i=0; i<12; i++){
		if(di_CSW[i]!=do_CBW[i])
			return -303;
	}

	return 0;
}


int mydata_prt(const void *data, int length)
{
    int i;
    unsigned char *dp = (unsigned char *)data;

    if(dp == NULL)
    {
        printf("Warning: data pointer is null!\n");
        return -1;
    }

    printf("Data list(size = %d):", length);

    for(i = 0; i < length; i++)
    {
        if(i%16 == 0)
            printf("\n[%04x] ", i);
        if(i%8 == 0)
            printf(" ");
        printf("%02x ", *dp++);
    }
    printf("\n");

    return 0;
}
int TestData()
{
	int ret = -1,i;
	//printf("====into TestData function====\n");

	unsigned char do_CBW[33] = {0x55,0x53,0x42,0x43,
					0xb0,0xfa,0x69,0x86,//0x5f,0xd9,0x00,0x00,	//
					0x10,0x00,0x00,0x00,//0x24,0x00,0x00,0x00,	//
					0x00,0x00,0x10,0xFF,	//0x00,0x00,0x0a,0x86,//
	0x13,0x00,0x10,0x00, 0x57,0x45,0x4C,0x4C,	//0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,//
 	0x43,0x4F,0x4D,0x39, 0x39,0x38,0xFF,0x00};//0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};//
	unsigned char di_CSW[16] = {0x00};

         unsigned char pBuf[17]={0xef,0x01,0xff,0xff, 0xff,0xff,0x01,0x00,
                                 0x07,0x13,0x00,0x00, 0x00,0x00,0x00,0x1b};

	if(!OpenUDisk()){
		printf("Open UDisk failed!\n");
	       return 0;
	}
	//printf("Open UDisk success!\n");

if(0)
{
struct usbdevfs_ioctl {
	    int ifno;       /* interface 0..N ; negative numbers reserved */
	    int ioctl_code; /* MUST encode size + direction of data so the
                         * macros in <asm/ioctl.h> give correct values */
	    void    *data;      /* param buffer (in, or out) */
};
	int ret;
	struct usbdevfs_ioctl  myctl;
	 myctl.ifno = 0;
	 myctl.ioctl_code = USBDEVFS_DISCONNECT;

	 ret = ioctl(fdusb->fd, USBDEVFS_IOCTL, &myctl);
	 if(ret < 0)
	   perror("USBDEVFS_IOCTL DISCONNECT");

	 printf("USBDEVFS_IOCTL DISCONNECT ret=%d \n", ret);
}
//usleep(10000);
//sleep(5);
if(0)
{
	int interface = 1;
	for(i = 0; i < interface; i++)
	{
	ret = ioctl(fdusb->fd, USBDEVFS_CLAIMINTERFACE, &i); 
	if(ret)
	         perror("USBDEVFS_CLAIMINTERFACE");

     printf("USBDEVFS_CLAIMINTERFACE ret=%d\n", ret);
	}
}

if(0)
{
	int ret;

	ret = ioctl(fdusb->fd, USBDEVFS_RESET);
	if(ret < 0)
		perror("reset");

	printf("USBDEVFS_RESET reset ret = %d\n", ret);
}

	ret = UDiskDownData(pBuf,16);
	if(ret!=0){
		return -1;
	}
	printf("Send Succ!\n");
	unsigned char pRev[64]={0};
	ret = UDiskGetData(pRev,16);
	if(ret!=0){
		printf("Recv err: %d\n",ret);
		return -1;
	}
	printf("Recv Succ!\n");
mydata_prt(pRev,16);

	ret = UDiskDownData(pBuf,16);
	if(ret!=0){
		return -1;
	}
	printf("Send Succ!\n");
	ret = UDiskGetData(pRev,16);
	if(ret!=0)
		return -1;
	printf("Recv Succ!\n");
mydata_prt(pRev,16);

CloseUDisk();
	return 0;
}
//»򈢍¼б
int UDiskGetImage(unsigned char* Img, int nLen)
{
	int iTmpLen = nLen;
	iTmpLen = nLen/2;
	if( UDiskGetData(Img,iTmpLen)!=0 )
		return -33;//»򈢍¼бʧ°ڍ
	return UDiskGetData(Img+iTmpLen,iTmpLen);
}

//Ђ՘ͼбµ½Flash
int UDiskDownImage(unsigned char* Img, int nLen)
{
	int iTmpLen = nLen;
	iTmpLen = nLen/2;
	if( UDiskDownData(Img,iTmpLen)!=0 )
		return -34;//Ђ՘ͼбʧ°ڍ
	return UDiskDownData(Img+iTmpLen,iTmpLen);
}

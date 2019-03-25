
#include "stdafx.h"
#include "ImageChangeURUtoFPC.h"
/*
**************************************************************************************
�� ��: void ImageChangeURUtoFPC(unsigned char *iopImage)
�� ��: URUͼ��ת��ΪFPC��ͼ���ʽ,225X313-->256X288
�� ��:  iopImage-���������ͼ������
����ֵ:	��
**************************************************************************************
*/
void ImageChangeURUtoFPC(unsigned char *iopImage)
{	
	unsigned char ImageNew[282*286];
	int OriX=225,OriY=313,OldX = 282,OldY = 286;
	int NewX = 256,NewY = 288;
	int i,j,m,n;
	int offX,offY;
	float ratioX,ratioY;
	offX = -(NewX-OldX)/2;
	offY = -(OldY-NewY)/2;
	
	ratioX = 152*1.0f/121;
	ratioY = 152*1.0f/166;
	
	//ͼ��ֱ��ʱ仯
	ImageZoom(iopImage,OriX,OriY,ImageNew,OldX,OldY,ratioX,ratioY);
	
	//ͼ��ü�Ϊ��׼�ߴ�
	memset(iopImage,FILL_COLOR,256*288);
	for(m=offY,i=0; i<OldY; i++,m++)
	{
		for(j=offX, n=0; n<NewX; j++,n++)
			iopImage[m*NewX+n] = ImageNew[i*OldX+j];
	}

	//��ֱ����
	VertMirror(iopImage,NewX,NewY);
}


/*
**************************************************************************************
�� ��: void VertMirror(unsigned char*iopImage,int iX, int iY)
�� ��: ͼ��ֱ����
�� ��:  iopImage-���������ͼ������
		iX-���룬ͼ����
		iY-���룬ͼ��߶�
����ֵ:	��
**************************************************************************************
*/
void VertMirror(unsigned char*iopImage,int iX, int iY)
{
	int i,j;
	unsigned char ctmp;
	
	for(i=0; i<iY/2; i++)
	{
		for(j=0; j<iX; j++)
		{
			ctmp = iopImage[i*iX+j];
			iopImage[i*iX+j] = iopImage[(iY-1-i)*iX+j];
			iopImage[(iY-1-i)*iX+j] = ctmp;
		}
	}
}

/*
**************************************************************************************
�� ��: unsigned char bilinear_Dou(unsigned char *pImage,long lWidth,long lHeight, double x,double y)
�� ��: ˫���Բ�ֵ
�� ��:  pImage-ԭʼͼ������
		lWidth-ԭʼͼ����
		lHeight-ԭʼͼ��߶�
		pImageNew-ԭʼ���ź��ͼ������
		x-�ο����غ�����λ��
		y-�ο�����������λ��
����ֵ:	��ֵ�����ػҶ�ֵ
**************************************************************************************
*/
unsigned char bilinear_Dou(unsigned char *pImage,long lWidth,long lHeight, double x,double y)
{
	double fx1,fx2,fy1,fy2;
	unsigned char pix11,pix12,pix21,pix22;
	long x1,y1;
	int gray;
	
	if( (x<0) || (x>=lWidth) || (y<0) || (y>=lHeight))
		return FILL_COLOR;	//����ɫ
	
	x1 = (long)x+1;
	y1 = (long)y+1;
	if(x1>=lWidth)
		x1 = lWidth-1;
	if(y1>=lHeight)
		y1 = lHeight-1;	
	
	pix11 = pImage[(long)y*lWidth+(long)x];
	pix12 = pImage[(long)y*lWidth+(long)x1];
	pix21 = pImage[(long)y1*lWidth+(long)x];
	pix22 = pImage[(long)y1*lWidth+(long)x1];
	
	fx2 = x-(long)x;
	fx1 = 1-fx2;
	fy2 = y-(long)y;
	fy1 = 1-fy2;
	
	gray = (int)( pix11*fy1*fx1 + pix12*fy1*fx2 + pix21*fy2*fx1 + pix22*fy2*fx2+0.5);
	if(gray >255)
		gray = 255;
	if(gray < 0)
		gray = 0;
	
	return (unsigned char)gray;
}


/*
**************************************************************************************
�� ��: void ImageZoom(unsigned char *pImageOld,int oldX,int oldY,unsigned char *pImageNew,int newX,int newY,float ratio)
�� ��: ͼ�����ţ����ݸ������ű�������ͼ�񣬲����ݸ��������ź�ͼ��ߴ���н�ȡ�������
�� ��:	pImageOld - ԭʼͼ�񻺳���
		oldX - ԭʼͼ����
		oldY - ԭʼͼ��߶�
		pImageNew - ���ź��ͼ�񻺳���
		newX - ���ź�ͼ����	
		newY - ���ź�ͼ���
		ratio - ͼ�����ű���			  
����ֵ:	  ��
**************************************************************************************
*/
void ImageZoom(unsigned char *pImageOld,int oldX,int oldY,unsigned char *pImageNew,
			   int newX,int newY,float ratioX,float ratioY)
{
	int i,j;
	int NewX_R,NewY_R;	//��ʵ��ͼ��Ŵ������С��Ŀ�Ⱥ͸߶�
	int m,n;
	int mstart,nstart,istart,jstart;
	
	memset(pImageNew,FILL_COLOR,sizeof(unsigned char)*newX*newY);
	if(ratioX < 0.0001 || ratioY < 0.0001)
		return ;
	//�µ�ͼ���С
	NewX_R = (int)(ratioX*oldX);
	NewY_R = (int)(ratioY*oldY);
	
	mstart = nstart = 0;
	istart = (NewY_R-newY)/2;
	jstart = (NewX_R-newX)/2;
	
	//istart��jstart����0����н�ȡ��С��0��������
	if(istart<0)
	{
		mstart = -istart;
		istart = 0;
	}
	if(jstart < 0)
	{
		nstart = -jstart;
		jstart = 0;
	}		
	
	for(i=istart,m=mstart ; i<NewY_R && m<newY; i++,m++)
	{
		for(j=jstart,n=nstart; j<NewX_R && n<newX; j++,n++)
		{
			pImageNew[m*newX+n] = bilinear_Dou(pImageOld,oldX,oldY,j/ratioX+0.5,i/ratioY+0.5);
		}
	}
}
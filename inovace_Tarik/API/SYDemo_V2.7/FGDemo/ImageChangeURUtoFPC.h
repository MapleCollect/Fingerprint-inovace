
//ImageChangeURUtoFPC.h

#ifndef ImageChangeURUtoFPC_H_2010
#define ImageChangeURUtoFPC_H_2010
///////////////////////////////////////////////////



#define FILL_COLOR 255	//���Ҷ�ֵ
unsigned char bilinear_Dou(unsigned char *pImage,long lWidth,long lHeight, double x,double y);
void ImageZoom(unsigned char *pImageOld,int oldX,int oldY,unsigned char *pImageNew,int newX,int newY,float ratioX,float ratioY);
void VertMirror(unsigned char*iopImage,int iX, int iY);


/*
**************************************************************************************
�� ��: void ImageChangeURUtoFPC(unsigned char *iopImage)
�� ��: URUͼ��ת��ΪFPC��ͼ���ʽ,225X313-->256X288
�� ��:  iopImage-���������ͼ������
����ֵ:	��
**************************************************************************************
*/
void ImageChangeURUtoFPC(unsigned char *iopImage);




///////////////////////////////////////////////////
#endif	//ImageChangeURUtoFPC_H_2010
#ifndef _SYDEVICE_H_
#define _SYDEVICE_H_
#include "vardef.h"

extern int       g_nComPort     ; //�˿ں� COM1~COM16
extern int       g_nBaudRate    ; //������ 57600

extern int OpenUDisk();
extern int CloseUDisk();
extern int UDiskGetData(unsigned char* DataBuf, int nLen);
extern int UDiskDownData(unsigned char* pBuf, int nLen);
extern int UDiskDownData1(unsigned char* pBuf, int nLen);
extern int UDiskGetImage(unsigned char* Img, int nLen);
extern int UDiskDownImage(unsigned char* Img, int nLen);





#endif
/***************************************************************************
 *   Copyright (C) 2006 by root   *
 *   root@localhost.localdomain   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include "SYDevice.h"
#include "SYProtocol.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "SYProtocol.cpp"

using namespace std;
#define DEV_ADDR 0xffffffff
extern int TestData();
int main(int argc, char *argv[])
{
//TestData();
//return 0;

//getchar();
//return 0;
   int nRet,iType=0;
//Open USB 
   if(PSOpenDevice( DEVICE_USB,0,0)){
	printf(">[OK]Open usb success\n");
	iType = DEVICE_USB;
	goto OPEN_scc;
   }
   printf("[ERR]Open usb fail!\n");


//Open UDisk
   if(PSOpenDevice( DEVICE_UDisk,0,0)){
	   printf(">[OK]Open UDisk success\n");
	   iType = DEVICE_UDisk;
	   goto OPEN_scc;
	}
   printf("[ERR]Open UDisk fail!\n");
//return 0;
         
//Open Com
  	if(PSOpenDevice( DEVICE_COM,0,57600/9600)){
	printf(">[OK]Open Com success!\n");
		iType = DEVICE_COM;
		goto OPEN_scc;
	}
	printf("[ERR]Open com fail!\n");

   	return 0;
 
OPEN_scc://Open dev success
 //  printf("Open success\n");
Delay(1000);
/*unsigned char ppp[64]={0};
nRet = PSReadInfo(DEV_ADDR,1,ppp);
if(nRet!=PS_OK)
       printf("Data Read error. nRet=%d\n",nRet);
   else
       printf("Read User info:%s\n",ppp);//*/

//验证密码
   unsigned char pPwd[5]={0};
   if(PSVfyPwd(DEV_ADDR,pPwd)!=PS_OK)
   {
       printf("[ERR]Verify pwd fail\n");
	   PSCloseDevice();
       return 0;
   }
   printf(">[OK]Verify pwd ok\n");  
  Delay(1000);


//测试PS_DownChar 和 PS_UpChar
   
//PSCloseDevice();
//return 0;
//写记事本
unsigned char pData[600]="test data to write 123456";
   
   if(PSWriteInfo(DEV_ADDR,1,pData)!=PS_OK)
      printf("[ERR]Data write error\n");
	else
		printf(">[OK]Write User info:%s\n",pData);
   memset(pData,0,512);
   Delay(100);
//读记事本
  if(PSReadInfo(DEV_ADDR,1,pData)!=PS_OK)
       printf("[ERR]Data Read error\n");
   else
       printf(">[OK]Read User info:%s\n",pData);
       
       BYTE pImage[256][288];
       int nImageLen;

//add by fujk
if( argc == 2 )
{
   if( strcmp( argv[1], "1" ) == 0 )
   {
       goto LabSearch;
   }
} 
//add end
LabRe://录入指纹
   Delay(10000);
   printf("Enroll: Please press finger1......\n");
 
   while((nRet=PSGetImage(DEV_ADDR))==PS_NO_FINGER) {
	if(DEVICE_USB==iType)
		PSReadInfo(DEV_ADDR,1,pData);
       Delay(30);
   }

    printf(">[OK]Get FingerPrint 1 OK,%d\n",nRet);
  /* if(PSUpImage(DEV_ADDR,pImage[0],&nImageLen)==PS_OK)
          printf("Upimage ok\n");
   else
         printf("Up Image Fail\n");
*/
   if(PSGenChar(DEV_ADDR,CHAR_BUFFER_A)!=PS_OK)
   {
         printf("[ERR]Gen Char fail!\n");
         goto LabRe;
   }
   Delay(1000);
   printf("Enroll: Please Repress finger2......\n");
/* add by fujk */
   while( ( PSGetImage(DEV_ADDR) ) == 0 )
   {
       Delay(10);
   }
/* add end */
   while(PSGetImage(DEV_ADDR)==PS_NO_FINGER){
	if(DEVICE_USB==iType)
		PSReadInfo(DEV_ADDR,1,pData);
       Delay(30);
   }

   printf(">[OK]Get FingerPrint 2 OK\n");

 Delay(10);
    if(PSGenChar(DEV_ADDR,CHAR_BUFFER_B)!=PS_OK)
   {
         printf("[ERR]Gen Char fail!\n");
         goto LabRe;
   }
   Delay(100);

   printf("generate the templet ...\n");
   nRet=PSRegModule(DEV_ADDR);
   if(nRet!=PS_OK)
   {
       printf("[ERR]Reg module fail!ErrCode=%d\n",nRet);
       goto LabRe;
   }
   if(PSStoreChar(DEV_ADDR,CHAR_BUFFER_A,1)!=PS_OK)
   {
      printf("[ERR]Store char fail\n");
      goto LabRe;
   }
printf(">[OK]Store char in PageID_1\n");
/////////////////////////////////////////////////
/*
LabDown:

 printf("\n[Test]Start UpChar...\n");
  Delay(1000);
  BYTE pTmpData[512]={0};
  int nTmpLen=512;
  nRet = PSUpChar(DEV_ADDR,CHAR_BUFFER_A,pTmpData,&nTmpLen);
  printf("PSUpChar nRet = %d\n",nRet);
  if(nRet!=PS_OK)
     return 0;
 FILE *fp = fopen("/root/1/a.dat","wb");
 if(!fp){
	printf("create file failed!\n");
  return 0;
}
  fwrite(pTmpData,1,nTmpLen,fp);
   fclose(fp);
    printf(">[OK]UpChar Char_Buf_A to PC(a.dat) success!\n");

//////////////
PSCloseDevice();
printf("!!!Please restart the device, than press any key to continue...\n");
getchar();
//////////////
if(!PSOpenDevice( DEVICE_COM,0,57600/9600)){
	printf("[ERR]Open com fail!\n");
	return 0;
	}
printf(">[OK]Open COM success\n");
	iType = DEVICE_COM;
//验证密码
   if(PSVfyPwd(DEV_ADDR,pPwd)!=PS_OK)
   {
       printf("[ERR]Verify pwd fail\n");
	   PSCloseDevice();
       return 0;
   }
   printf(">[OK]Verify pwd ok\n");  
///////////////////
 Delay(1000);
//int WINAPI     PSDownChar(int nAddr,int iBufferID, unsigned char* pTemplet, int iTempletLength);
printf("[Test]Start PSDownChar...\n");
  nRet = PSDownChar(DEV_ADDR,CHAR_BUFFER_B,pTmpData,nTmpLen);
printf("PSDownChar nRet=%d\n",nRet);
if(nRet!=PS_OK)
 return 0;
Delay(1000);//unsigned char pData[600]="test data to write 123456";
if(PSStoreChar(DEV_ADDR,CHAR_BUFFER_B,2)!=PS_OK)
   {
      printf("[ERR]Store char Char_Buf_B to PageID_2 fail\n");
	return 0;
   }
printf(">[OK]Down a.dat to PageID_2 success.\n");
  fp = fopen("/root/1/b.dat","rb");
if(!fp){
	printf("open file(b.dat) failed!\n");
	return 0;
}
memset(pTmpData,0,512);
fread(pTmpData,1,512,fp);
fclose(fp);

nRet = PSDownChar(DEV_ADDR,CHAR_BUFFER_A,pTmpData,512);
printf("PSDownChar nRet=%d\n",nRet);
if(nRet!=PS_OK)
 return 0;
Delay(1000);
if(PSStoreChar(DEV_ADDR,CHAR_BUFFER_A,1)!=PS_OK)
   {
      printf("Store char fail\n");
	return 0;
   }
printf(">[OK]Down b.dat to PageID_1 success.\n");

      
//return 0;

/////////////////////////////////////////////////
   printf("\nbegin test Search Finger....\n\n");
*/
LabSearch://搜索指纹
     Delay(1000);
     printf("Search: Please Repress finger......\n");
/* add by fujk */
   while( ( PSGetImage(DEV_ADDR) ) == 0 )
   {
       Delay(10);
   }
/* add end */
   while(PSGetImage(DEV_ADDR)==PS_NO_FINGER){
	if(DEVICE_USB==iType)
		PSReadInfo(DEV_ADDR,1,pData);
       Delay(10);
   }

   printf(">[OK]Get FingerPrint OK\n");

    if(PSGenChar(DEV_ADDR,CHAR_BUFFER_A)!=PS_OK)
   {
         printf("[ERR]Gen Char fail!\n");
         goto LabRe;
   }
   int nFinger;
   if(PSSearch(DEV_ADDR,CHAR_BUFFER_A,0,10,&nFinger)!=PS_OK)
   {
       printf("[ERR]Search Fail\n");
       goto LabSearch;
   }
   printf(">[OK]Search OK  nFinger=%d\n",nFinger);
   goto LabSearch;
Delay(2000);
//关闭设备
   PSCloseDevice();
printf("Close device ok!\n");
 
   return EXIT_SUCCESS;
}

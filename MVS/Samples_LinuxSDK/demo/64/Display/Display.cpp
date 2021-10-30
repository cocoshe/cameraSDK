#include <X11/Xlib.h> 
#include <assert.h>  
#include "math.h"
#include "pthread.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "MvCameraControl.h"

#define MAX_IMAGE_DATA_SIZE   (20*1024*1024)
#define NIL (0) 

// �ȴ��û�����enter��������ȡ�����������
// wait for user to input enter to stop grabbing or end the sample program
void PressEnterToExit(void)
{
	int c;
	while ( (c = getchar()) != '\n' && c != EOF );
    fprintf( stderr, "\nPress enter to exit.\n");
    while( getchar() != '\n');
	sleep(1);
}

bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("%s\n" , "The Pointer of pstMVDevInfoList is NULL!");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
		// ��ӡ��ǰ���ip���û��Զ�������
		// print current ip and user defined name
        printf("%s %x\n" , "nCurrentIp:" , pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp);                 
        printf("%s %s\n\n" , "chUserDefinedName:" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);   
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("UserDefinedName:%s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else
    {
        printf("Not support.\n");
    }
    return true;
}

int main()
{
	Window w;     
	Display *dpy; 

	memset(&w, 0, sizeof(Window));
	dpy    = NULL;
    printf("%d\n", sizeof(Window));

	// �����ӵ�X������������
	// open the connection to the display 0
	dpy = XOpenDisplay(NIL);

	int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 
		752, 480, 0, 0xffff00ff, 0xff00ffff);
	
	// ��ȡ�ı䴰�ڴ�С�¼� 
	// we want to get MapNotify events
	XSelectInput(dpy, w, StructureNotifyMask);

	// ʹ���ڿɼ�
	// "Map" the window (that is, make it appear on the screen)
	XMapWindow(dpy, w);

	// ����ͼ�������ĸ�����ͼ����������
	// Create a "Graphics Context"
	GC gc = XCreateGC(dpy, w, 0, NIL);

	// ����GCʹ�ð�ɫ
	// Tell the GC we draw using the white color
	XSetForeground(dpy, gc, whiteColor);

	// �ȴ��¼��ĵ���
	// Wait for the MapNotify event
	for(;;) 
	{
		XEvent e;
		XNextEvent(dpy, &e);
		if (e.type == MapNotify)
		{
			break;
		}	
	}		
	
    int nRet = MV_OK;

    void* handle = NULL;

    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // ö���豸
	// enum device
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
    if (MV_OK != nRet)
    {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
        return -1;
    }
    unsigned int nIndex = 0;
    if (stDeviceList.nDeviceNum > 0)
    {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                break;
            } 
            PrintDeviceInfo(pDeviceInfo);            
        }  
    } 
    else
    {
        printf("Find No Devices!\n");
        return -1;
    }

    scanf("%d", &nIndex);

    // ѡ���豸���������
	// select device and create handle
    nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
    if (MV_OK != nRet)
    {
        printf("MV_CC_CreateHandle fail! nRet [%x]\n", nRet);
        return -1;
    }

    // ���豸
	// open device
    nRet = MV_CC_OpenDevice(handle);
    if (MV_OK != nRet)
    {
        printf(" fail! nRet [%x]\n", nRet);
        return -1;
    }

    // ��ʼȡ��
	// start grab image
    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StartGrabbing fail! nRet [%x]\n", nRet);
        return -1;
    }

	nRet= MV_CC_Display(handle, (void*)w);
	if (MV_OK != nRet)
    {
        printf("MV_CC_Display fail! nRet [%x]\n", nRet);
        return -1;
    }
	printf("Display succeed\n");	
	
	PressEnterToExit();

    // ֹͣȡ��
	// end grab image
    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
        return -1;
    }

    // �ر��豸
	// close device
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_CloseDevice fail! nRet [%x]\n", nRet);
        return -1;
    }

    // ���پ��
	// destroy handle
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet)
    {
        printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
        return -1;
    }
	
	printf("exit\n");

    return 0;
}

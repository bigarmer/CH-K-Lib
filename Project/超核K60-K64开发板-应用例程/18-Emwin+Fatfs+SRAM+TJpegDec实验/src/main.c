/*
 * ʵ�����ƣ�Emwin_Fatfs_SRAM_TJpegDec
 * ʵ��ƽ̨����ѻ������
 * ����оƬ��MK60DN512ZVLQ10
 * ʵ�鲽�裺
 *				1.��../Pictures/hkk.jpg�ļ��ŵ��Ѿ���ʽ����SD���У�
 *				2.��LCD���������ͺŲ�ͬ��ili9320_get_id()�����ɻ���������ͺţ������̵�����Ϊ0x8989 (ssd1289)��
 *    			3.���������ͺŲ�ͬ���ο�������http://www.beyondcore.net/forum.php?mod=viewthread&tid=2650&extra=page%3D1&page=2 ������Ӧ���޸ģ�
 * ʵ��˵����
 *				1.GUI_DrawBitmap(&bma, 0, 0);��ʾ�����flash�е�bmpͼ��
 *				2.GUI_JPEG_Draw(jpgHankeku.picAddr,jpgHankeku.picSize,0,0);��ʾ�����flash�е�jpegͼ��
 *				3.sdhc_maintask();SD���Ķ�д�ļ�������
 *				4.jpegDispFromFile(filename);��SD���ж�ȡ��Ϊfilename��jpegͼƬ��
 *				5.����emwin���ڴ�ѡ�õ����ⲿ��512KB��SRAM��Ϊ��̬�ڴ棻
 *				6.JPGͼƬ�ı���/�������ChaN��TJpegDec���뷽ʽ������ѧϰ��������ذ�Ȩ�����߱������У�
 *				7.�����е�5��warning�����ڽ�int�ͷ��ظ�enum���ͣ�ǿ��ת����������warning���������ž͵��������Ѱɣ�
 */
#include "gpio.h"
#include "common.h"
#include "uart.h"

#include "GUI.h"				/* Emwin Header */
#include "IS61WV25616.h"		/* Extern SRAM Header */
#include "sd.h"					/* SD card Header */

/* defined by user for specific funcations */
#include "usrSdhc.h"
#include "usrJpegIO.h"
#include "usrPicture.h"

extern GUI_CONST_STORAGE GUI_BITMAP bma;

int main(void)
{
	TCHAR *filename = "0:/hkk.jpg";
	
	DelayInit();

    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
	
	/* initialize the sd card */
	if(SD_QuickInit(20*1000*1000))
    {
        printf("SD card init failed!\r\n");
        while(1);
    }
    printf("SD initialize successfully!Its size:%dMB\r\n", SD_GetSizeInMB());

	/* Initialize the external SRAM, its size is 512KB */
	SRAM_Init();
	/* Initialize the gui(LCD) */
	GUI_Init();	
	
	/* display the bitmap */
	GUI_DrawBitmap(&bma, 0, 0);
	DelayMs(2000);
	GUI_Clear();
	
	/* draw the jpeg file stored in flash */
	GUI_JPEG_Draw(jpgHankeku.picAddr,jpgHankeku.picSize,0,0);
	DelayMs(2000);
	GUI_Clear();
	
	/* create/read and write file on sd card */
	sdhc_maintask();
	DelayMs(2000);
	GUI_Clear();
	
	/* read the hkk.jpg from sd card and display on lcd with external sram */
	jpegDispFromFile(filename);
	
    while(1)
    {
        //led flashing
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(200);
    }
}

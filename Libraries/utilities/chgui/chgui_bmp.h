#ifndef _CHGUI_PIC_H_
#define _CHGUI_PIC_H_

#include "stdint.h"


typedef uint32_t (*GUI_GetDataCallbackType)(uint32_t, uint32_t, uint8_t ** ppData);


//BMP��Ϣͷ
typedef __packed struct
{
    uint32_t biSize ;		   	//˵��BITMAPINFOHEADER�ṹ����Ҫ��������
    long  biWidth ;		   	//˵��ͼ��Ŀ�ȣ�������Ϊ��λ 
    long  biHeight ;	   	//˵��ͼ��ĸ߶ȣ�������Ϊ��λ 
    uint16_t  biPlanes ;	   		//ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1 
    uint16_t  biBitCount ;	   	//˵��������/���أ���ֵΪ1��4��8��16��24����32
    uint32_t biCompression ;  	//˵��ͼ������ѹ�������͡���ֵ����������ֵ֮һ��
    //BI_RGB��û��ѹ����
    //BI_RLE8��ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��  
    //BI_RLE4��ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����
    //BI_BITFIELDS��ÿ�����صı�����ָ�������������
    uint32_t biSizeImage ;		//˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0  
    long  biXPelsPerMeter ;	//˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ
    long  biYPelsPerMeter ;	//˵����ֱ�ֱ��ʣ�������/�ױ�ʾ
    uint32_t biClrUsed ;	  	 	//˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
    uint32_t biClrImportant ; 	//˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ�� 
}BITMAPINFOHEADER ;
//BMPͷ�ļ�
typedef __packed struct
{
    uint16_t  bfType ;     //�ļ���־.ֻ��'BM',����ʶ��BMPλͼ����
    uint32_t  bfSize ;	  //�ļ���С,ռ�ĸ��ֽ�
    uint16_t  bfReserved1 ;//����
    uint16_t  bfReserved2 ;//����
    uint32_t  bfOffBits ;  //���ļ���ʼ��λͼ����(bitmap data)��ʼ֮��ĵ�ƫ����
}BITMAPFILEHEADER ;

//λͼ��Ϣͷ
typedef __packed struct
{ 
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bmiHeader;  
    uint32_t RGB_MASK[3];			//��ɫ�����ڴ��RGB����.
}BITMAPINFO; 


//ͼ������ѹ��������
#define BI_RGB	 		0  //û��ѹ��.RGB 5,5,5.
#define BI_RLE8 		1  //ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��
#define BI_RLE4 		2  //ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����
#define BI_BITFIELDS 	3  //ÿ�����صı�����ָ�������������  

//API funtctions

void GUI_BMP_Draw(uint16_t xPos, uint16_t yPos, const uint8_t *pData);
void GUI_BMP_DrawEx(uint16_t xPos, uint16_t yPos, GUI_GetDataCallbackType fpAppGetData);

#endif


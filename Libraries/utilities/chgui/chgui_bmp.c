#include "chgui.h"
#include "chgui_bmp.h"
#include "string.h"

static uint8_t *pBMPFileData;

void GUI_BMP_DrawEx(uint16_t xPos, uint16_t yPos, GUI_GetDataCallbackType fpAppGetData) 
{
    uint32_t br;
    uint32_t count;		    	   
    uint8_t  rgb ,color_byte;
    uint16_t x ,y,color;	 
    uint16_t image_width_in_pixel;	
    BITMAPINFO BitMapInfo1;
    uint32_t countpix=0;//��¼���� 	 
    //x,y��ʵ������	 
    uint8_t *databuf;    		//���ݶ�ȡ��ŵ�ַ
    uint32_t TotalDataSizeInBytes;
    uint8_t biCompression=0;		//��¼ѹ����ʽ
    uint32_t rowlen;	  		 	//ˮƽ�����ֽ���  
    uint32_t offx=0;
	  //Get Header File Info 
    while(offx < sizeof(BITMAPINFO))
    {
        br = fpAppGetData(sizeof(BITMAPINFO)-offx, offx, &databuf);
			  memcpy(((&BitMapInfo1) + offx), databuf, br);
        offx += br;
    }
		offx = BitMapInfo1.bmfHeader.bfOffBits;        	//����ƫ��,�õ����ݶεĿ�ʼ��ַ
		color_byte = BitMapInfo1.bmiHeader.biBitCount/8;	//��ɫλ 16/24/32  
		biCompression = BitMapInfo1.bmiHeader.biCompression;//ѹ����ʽ
    TotalDataSizeInBytes = BitMapInfo1.bmiHeader.biSizeImage ;
		y = BitMapInfo1.bmiHeader.biHeight;	//�õ�ͼƬ�߶�
		image_width_in_pixel = BitMapInfo1.bmiHeader.biWidth;
		//ˮƽ���ر�����4�ı���!!

		if((image_width_in_pixel * color_byte)%4)rowlen=((image_width_in_pixel*color_byte)/4+1)*4;
		else rowlen = image_width_in_pixel * color_byte;

		//��ʼ����BMP   
		color = 0;//��ɫ���	 													 
		x = 0;
		rgb = 0;      
    while(1)
    {				
        br = fpAppGetData(TotalDataSizeInBytes + BitMapInfo1.bmfHeader.bfOffBits - offx, offx, &databuf);
        offx += br;
        count = 0;
        while(count < br)
        {
            if(color_byte == 3)   //24λ��ɫͼ
            {
                switch (rgb) 
                {
                    case 0:				  
                        color = databuf[count]>>3; //B
                        break ;	   
                    case 1: 	 
                        color += ((uint16_t)databuf[count]<<3)&0X07E0;//G
                        break;	  
                    case 2 : 
                        color += ((uint16_t)databuf[count]<<8)&0XF800;//R	  
                        break;			
                }   
            }
						else if(color_byte == 2)  //16λ��ɫͼ
            {
                switch(rgb)
                {
                    case 0 : 
                        if(biCompression == BI_RGB)//RGB:5,5,5
                        {
                            color = ((uint16_t)databuf[count]&0X1F);	 	//R
                            color += (((uint16_t)databuf[count])&0XE0)<<1; //G
                        }
												else		//RGB:5,6,5
                        {
                            color = databuf[count];  			//G,B
                        }  
                        break ;   
                    case 1 : 			  			 
                        if(biCompression == BI_RGB)//RGB:5,5,5
                        {
                            color += (uint16_t)databuf[count]<<9;  //R,G
                        }
												else  		//RGB:5,6,5
                        {
                            color += (uint16_t)databuf[count]<<8;	//R,G
                        }  									 
                        break ;	 
                }		     
            }
						else if(color_byte == 4)//32λ��ɫͼ
            {
                switch (rgb)
                {
                    case 0:				  
                        color = databuf[count]>>3; //B
                        break ;	   
                    case 1: 	 
                        color += ((uint16_t)databuf[count]<<3)&0X07E0;//G
                        break;	  
                    case 2 : 
                        color += ((uint16_t)databuf[count]<<8)&0XF800;//R	  
                        break ;			
                    case 3 :
                        //alphabend=bmpbuf[count];//����ȡ  ALPHAͨ��
                        break ;  		  	 
                }	
            }
						else if(color_byte == 1)//8λɫ,��ʱ��֧��,��Ҫ�õ���ɫ��.
            {
            } 
            rgb++;	  
            count++ ;		  
            if(rgb == color_byte) //ˮƽ�����ȡ��1���������ݺ���ʾ
            {
                gpCHGUI->ops->ctrl_point(x + xPos, y + yPos, color);								    
                x++; //x������һ������ 
                color = 0x00; 
                rgb = 0;  		  
            }
            countpix++;//�����ۼ�
            if(countpix >= rowlen)//ˮƽ��������ֵ����.����
            {		 
                y--; 
                if(y == 0)break;			 
                x = 0; 
                countpix = 0;
                color = 0x00;
                rgb = 0;
            }	 
        } 		
    }   					   
}		 

static uint32_t AppGetData(uint32_t ReqestedByte, uint32_t Offset, uint8_t **ppData)
{
    *ppData = (pBMPFileData + Offset);
    return ReqestedByte;
}

void GUI_BMP_Draw(uint16_t xPos, uint16_t yPos, const uint8_t *pData)
{
    pBMPFileData = (uint8_t*)pData;
    GUI_BMP_DrawEx(xPos, yPos, AppGetData);
}

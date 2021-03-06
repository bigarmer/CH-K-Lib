#include "image_display.h"
#include <stdint.h>
#include <stdio.h>
#include "ili9320.h"

static void _DispBin(int x, int y, uint8_t data)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        if((data >> i) & 1)
        {
            ili9320_write_pixel(x+8-i, y, BLACK);
        }
        else
        {
            ili9320_write_pixel(x+8-i, y, WHITE);
        }
    }
}

void GUI_DispCCDImage(int x, int y, int xSize, int ySize, uint8_t* data)
{
    uint32_t h, v;

    GUI_HLine(GREEN, x, x+xSize+2, y-1);
    GUI_HLine(GREEN, x, x+xSize+2, y+ySize);
    GUI_VLine(GREEN, y, y+ySize, x);
    GUI_VLine(GREEN, y, y+ySize, x+xSize+1);
    
    for(v = 0; v < ySize; v++)
    {
        for(h = 0; h < (xSize/8); h++)
        {
            _DispBin(h*8+x, v+y, data[v*(xSize/8)+h+2]);
        }            
    }
}

void SerialDispCCDImage(int xSize, int ySize, uint8_t* data)
{
    int x,y;
    
    for(y = 0; y < ySize; y++)
    {
        for(x = 1; x < (xSize/8)+1; x++)
        {
            printf("%d",(data[y*(xSize/8)+x]>>7) & 0x01);
            printf("%d",(data[y*(xSize/8)+x]>>6) & 0x01);
            printf("%d",(data[y*(xSize/8)+x]>>5) & 0x01);
            printf("%d",(data[y*(xSize/8)+x]>>4) & 0x01);
            printf("%d",(data[y*(xSize/8)+x]>>3) & 0x01);
            printf("%d",(data[y*(xSize/8)+x]>>2) & 0x01);
            printf("%d",(data[y*(xSize/8)+x]>>1) & 0x01);
            printf("%d",(data[y*(xSize/8)+x]>>0) & 0x01);
            if(x == xSize/8)
                printf("\r\n");   
        }
        if(y == ySize -1)
        {
            printf("                                                                                ");
            printf("\r\n");  
        }				
    }
}



#include "shell.h"
#include "ili9320.h"
#include "ads7843.h"
#include "gpio.h"
#include "spi.h"
#include "board.h"
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     
#include "chgui_touch.h"
#include "sd.h"

//fat32
#include "znfat.h"
//�ײ�������ӽṹ Ŀǰ�İ汾ֻ��ʵ��LCD_DrawPoint�� LCD_Init�Ϳ��Թ�����������NULL�Ϳ���
CHGUI_CtrlOperation_TypeDef lcd_ops = 
{
	ili9320_Init,        //�ײ㺯����CHGUI�޹� LCD_Init��ʵ���� LCD_CHK60EVB.c�� ��ͬ
	NULL,
	LCD_DrawPoint,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	ILI9320_GetDeivceID,
};

    
static struct ads7843_device ads7843;
struct spi_bus bus; 
static void tp_init(void)
{
    spi_bus_init(&bus, BOARD_SPI_INSTANCE);
    PORT_PinMuxConfig(HW_GPIOD, 11, kPinAlt2); //SPI2_PCS0
    
    ads7843.bus = &bus;
    ads7843_init(&ads7843, BOARD_TP_SPI_PCSN, HW_CTAR0, 80*1000);
}

static uint32_t tp_get_x(void)
{
    uint16_t x = 0;
    uint32_t sum = 0;
    uint32_t i;
    for(i=0;i<7;i++)
    {
        ads7843.readX(&ads7843, &x);
        sum += x;
    }
    sum/=7;
    return sum;
}
static uint32_t tp_get_y(void)
{
    uint32_t sum = 0;
    uint16_t y = 0;
    uint32_t i;
    for(i=0;i<7;i++)
    {
        ads7843.readY(&ads7843, &y);
        sum += y;
    }
    sum/=7;
    return sum;
}
    
//����������������
CHGUI_TouchCtrlOperation_TypeDef tp_ops = 
{
    tp_init,
    tp_get_x,
    tp_get_y,
};
//CHGUI��ʼ���ṹ
CHGUI_InitTypeDef CHGUI_InitStruct1 = 
{
    "TFT_9320",
    0,
    LCD_X_MAX,
    LCD_Y_MAX,
    &lcd_ops,
    &tp_ops,
};

struct znFAT_Init_Args initArgSD; //��ʼ����������
struct znFAT_Init_Args initArgFlash; //��ʼ����������
struct FileInfo fileInfo1,fileInfo2; //�ļ���Ϣ����
struct DateTime dtInfo1; //����ʱ��ṹ�����


static uint8_t gBMPBuffer[512];
//BMP ��ʾ���� �û��ص����� ���ڶ�ȡ����
//ʵ���Ͼ��Ƕ�SD���е�����
static uint32_t AppGetData(uint32_t ReqestedByte, uint32_t Offset, uint8_t **ppData)
{
    //����ʵ�ʿ��Զ�ȡ�����ݳ���
    if(ReqestedByte > sizeof(gBMPBuffer))
    {
        ReqestedByte = sizeof(gBMPBuffer);
    }
    //��ȡ����
    znFAT_ReadData(&fileInfo1 ,Offset, ReqestedByte, gBMPBuffer);
    //��������ָ��λ��
    *ppData = gBMPBuffer;
    //����ʵ�ʶ����ĳ���
    return ReqestedByte;
}

int CMD_LCD(int argc, char * const * argv)
{
    uint8_t res;
    uint32_t i;
    uint32_t bmp_pic_cnt;
    SD_InitTypeDef SD_InitStruct1;
    CHGUI_PID_TypeDef State;
    SD_InitStruct1.SD_BaudRate = 2000000;
    //�ȴ�SD����ʼ���ɹ�
    while(SD_Init(&SD_InitStruct1) != ESDHC_OK);
    //��ʼ��FAT32
    znFAT_Device_Init(); //�豸��ʼ��
    znFAT_Select_Device(0,&initArgSD); //ѡ��SD���豸
    res = znFAT_Init();
    if(res == ERR_SUCC)
    {
        printf("Suc. to init FS\r\n");
        printf("BPB_Sector_No:%d\r\n",initArgSD.BPB_Sector_No);   
        printf("Total_SizeKB:%d\r\n",initArgSD.Total_SizeKB); 
        printf("BytesPerSector:%d\r\n",initArgSD.BytesPerSector); 
        printf("FATsectors:%d\r\n",initArgSD.FATsectors);  
        printf("SectorsPerClust:%d\r\n",initArgSD.SectorsPerClust); 
        printf("FirstFATSector:%d\r\n",initArgSD.FirstFATSector); 
        printf("FirstDirSector:%d\r\n",initArgSD.FirstDirSector); 
        printf("FSsec:%d\r\n",initArgSD.FSINFO_Sec);
        printf("Next_Free_Cluster:%d\r\n",initArgSD.Next_Free_Cluster);
        printf("FreenCluster:%d\r\n",initArgSD.Free_nCluster); 
    }
    else
    {
        GUI_printf("FAT32 Init failed CODE:%d",res);
        while(1);
    }
    
    ili9320_Init();
    shell_printf("LCD ID:0x%X\r\n", ILI9320_GetDeivceID());
    //��ʼ��GUI
    GUI_Init(&CHGUI_InitStruct1);
    GUI_Clear(BLACK);
    GUI_SetBkColor(BLACK);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetColor(LGRAY);
    GUI_SetFontFormName("FONT_CourierNew");
    GUI_printf("HelloWorld\r\n");
    GUI_printf("CHGUI_Version:%0.2f\r\n", (float)(GUI_VERSION/100));
    GUI_printf("ID:%X\r\n", GUI_GetDeivceID());
    while(1)
    {
        /*
        GUI_GotoXY(0, 0);
        GUI_TOUCH_GetState(&State);
        //��ӡ����AD����
        GUI_printf("Phy:X:%04d Y:%04d\r\n", GUI_TOUCH_GetxPhys(), GUI_TOUCH_GetyPhys());
        //��ӡ�߼�AD����
        GUI_printf("Log:X:%04d Y:%04d\r\n", State.x, State.y);	
        GUI_printf("State:%01d\r\n", State.Pressed);
        //LCD ���ʸ���
        GUI_DrawPoint(State.x, State.y);	
        GUI_TOUCH_Exec();
        */
        //ȷ���ж���BMP
        while(znFAT_Open_File(&fileInfo1, "/PIC/*.bmp", bmp_pic_cnt, 1) == ERR_SUCC)
        {
            bmp_pic_cnt++;
        }
        while(1)
        {
            //��SD���е�BMP�ļ�
            znFAT_Open_File(&fileInfo1, "/PIC/*.bmp", i, 1);
            //��ͼ
            GUI_BMP_DrawEx(0,0, AppGetData);
            //�ر��ļ�
            znFAT_Close_File(&fileInfo1);
            //ͣ��һ��
            DelayMs(1000);
            i++;
            if(i == bmp_pic_cnt) i = 0;
        }
        }
        return 0;

}

const cmd_tbl_t CommandFun_LCD = 
{
    .name = "LCD",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_LCD,
    .usage = "LCD",
    .complete = NULL,
    .help = "\r\n"
};

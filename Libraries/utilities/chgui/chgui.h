#ifndef _CHGUI_H_
#define _CHGUI_H_

#include <stdint.h>

#ifndef NULL
	#define NULL 0
#endif

#define GUI_COLOR uint16_t

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)




#define GUI_VERSION  100

typedef struct
{
    char* Name;
    uint8_t  FontXSize;
    uint8_t  FontYSize;
    const char* pFontDataAdress;
}CHGUI_FontSetup_TypeDef;

//LCD����������
typedef struct
{
    void (*ctrl_init)(void);
    void (*ctrl_deinit)(void *param);
    void (*ctrl_point)(uint16_t, uint16_t, GUI_COLOR);
    uint16_t (*ctrl_read_point)(uint16_t, uint16_t);	
    void (*ctrl_set_cursor)(uint16_t, uint16_t);
    void (*ctrl_draw_hline)(uint16_t, uint16_t, uint16_t, GUI_COLOR);
    void (*ctrl_draw_vline)(uint16_t, uint16_t, uint16_t, GUI_COLOR);
	  void (*ctrl_fill)      (uint16_t, uint16_t, uint16_t, uint16_t, GUI_COLOR);
    uint32_t (*ctrl_get_id)(void);

} CHGUI_CtrlOperation_TypeDef;

//��������������
typedef struct
{
    void (*ctrl_init)(void);
    uint32_t (*ctrl_get_touch_x)(void);
    uint32_t (*ctrl_get_touch_y)(void);
    uint32_t (*ctrl_read_touch_data)(uint8_t * pData);
    uint32_t (*ctrl_write_touch_data)(uint8_t * pData, uint32_t size);
}CHGUI_TouchCtrlOperation_TypeDef;


typedef struct
{
    uint32_t x;
    uint32_t y;
}CHGUI_Point_TypeDef;

typedef struct 
{
    char* name;
    uint32_t device_id;
    uint16_t x_max;
    uint16_t y_max;
    CHGUI_CtrlOperation_TypeDef *ops;           //!< Operations including the initialize, configure and read/write
    CHGUI_TouchCtrlOperation_TypeDef *tops;     //!< Operations including the touch system
} CHGUI_InitTypeDef;


extern CHGUI_InitTypeDef* gpCHGUI;
extern GUI_COLOR gGUI_ForntColor;
extern GUI_COLOR gGUI_BackColor;


//API funtctions
void GUI_SetColor(GUI_COLOR color);
void GUI_SetBkColor(GUI_COLOR color);
void GUI_Init(CHGUI_InitTypeDef* CHGUI_InitStruct);
void GUI_DrawPixel(uint16_t x, uint16_t y);
void GUI_HLine(uint16_t y0, uint16_t x0, uint16_t x1);
void GUI_VLine(uint16_t x0, uint16_t y0, uint16_t y1);
void GUI_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void GUI_LineWith(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int8_t with);
char* GUI_GetCurrentFontName(void);
void GUI_DispDeivceID(void);
void GUI_GotoXY(uint16_t x, uint16_t y);
void GUI_DrawPoint(uint16_t x, uint16_t y);


#endif 



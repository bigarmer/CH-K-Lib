#include "chgui.h"

//Global operation
 CHGUI_InitTypeDef* gpCHGUI = NULL;
 GUI_COLOR gGUI_ForntColor = 0xFFFF;
 GUI_COLOR gGUI_BackColor = 0;


void GUI_SetColor(GUI_COLOR color)
{
    gGUI_ForntColor = color;
}

void GUI_SetBkColor(GUI_COLOR color)
{
    gGUI_BackColor = color;
}


void GUI_Init(CHGUI_InitTypeDef* CHGUI_InitStruct)
{
    gpCHGUI = CHGUI_InitStruct;
    if(gpCHGUI->ops == NULL)
		{
        return;
		}
    gpCHGUI = CHGUI_InitStruct;
	  gpCHGUI->ops->ctrl_init();
		gpCHGUI->ops->ctrl_fill(0, 0, gpCHGUI->x_max, gpCHGUI->y_max, gGUI_BackColor);
		gpCHGUI->device_id = gpCHGUI->ops->ctrl_get_id();
		//Initalize Touch system
		if(gpCHGUI->tops == NULL)
		{
        return;
		}
		gpCHGUI->tops->ctrl_init();
}


void GUI_DrawPixel(uint16_t x, uint16_t y)
{
    gpCHGUI->ops->ctrl_point(x, y, gGUI_ForntColor);
}

void GUI_DrawPoint(uint16_t x, uint16_t y)
{	    
	GUI_DrawPixel(x,y);//���ĵ� 
	GUI_DrawPixel(x+1,y);
	GUI_DrawPixel(x,y+1);
	GUI_DrawPixel(x+1,y+1);	 	  	
}				

void GUI_HLine(uint16_t y0, uint16_t x0, uint16_t x1)
{
    gpCHGUI->ops->ctrl_draw_hline(y0, x0, x1, gGUI_ForntColor);
}

void GUI_VLine(uint16_t x0, uint16_t y0, uint16_t y1)
{
    gpCHGUI->ops->ctrl_draw_vline(x0, y0, y1, gGUI_ForntColor);
}


void GUI_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
   int32_t   dx;						// ֱ��x���ֵ����
   int32_t   dy;          			// ֱ��y���ֵ����
   int8_t    dx_sym;					// x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   int8_t    dy_sym;					// y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   int32_t   dx_x2;					// dx*2ֵ���������ڼӿ������ٶ�
   int32_t   dy_x2;					// dy*2ֵ���������ڼӿ������ٶ�
   int32_t   di;						// ���߱���
   
   
   dx = x1-x0;						// ��ȡ����֮��Ĳ�ֵ
   dy = y1-y0;
   
   /* �ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ��� */
   if(dx>0)							// �ж�x�᷽��
   {  dx_sym = 1;					// dx>0������dx_sym=1
   }
   else
   {  if(dx<0)
      {  dx_sym = -1;				// dx<0������dx_sym=-1
      }
      else
      {  // dx==0������ֱ�ߣ���һ��
         GUI_VLine(x0, y0, y1);
      	 return;
      }
   }
   
   if(dy>0)							// �ж�y�᷽��
   {  dy_sym = 1;					// dy>0������dy_sym=1
   }
   else
   {  if(dy<0)
      {  dy_sym = -1;				// dy<0������dy_sym=-1
      }
      else
      {  // dy==0����ˮƽ�ߣ���һ��
         GUI_HLine(y0, x0, x1);
      	 return;
      }
   }
    
   /* ��dx��dyȡ����ֵ */
   dx = dx_sym * dx;
   dy = dy_sym * dy;
 
   /* ����2����dx��dyֵ */
   dx_x2 = dx*2;
   dy_x2 = dy*2;
   
   /* ʹ��Bresenham�����л�ֱ�� */
   if(dx>=dy)						// ����dx>=dy����ʹ��x��Ϊ��׼
   {  di = dy_x2 - dx;
      while(x0!=x1)
      {  GUI_DrawPixel(x0, y0);
         x0 += dx_sym;
         if(di<0)
         {  di += dy_x2;			// �������һ���ľ���ֵ
         }
         else
         {  di += dy_x2 - dx_x2;
            y0 += dy_sym;
         }
      }
      GUI_DrawPixel(x0, y0);		// ��ʾ���һ��
   }
   else								// ����dx<dy����ʹ��y��Ϊ��׼
   {  di = dx_x2 - dy;
      while(y0!=y1)
      {  GUI_DrawPixel(x0, y0);
         y0 += dy_sym;
         if(di<0)
         {  di += dx_x2;
         }
         else
         {  di += dx_x2 - dy_x2;
            x0 += dx_sym;
         }
      }
      GUI_DrawPixel(x0, y0);		// ��ʾ���һ��
   } 
  
}


void GUI_LineWith(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int8_t with)
{ 
 	 int32_t   dx;						// ֱ��x���ֵ����
   int32_t   dy;          			// ֱ��y���ֵ����
   int8_t    dx_sym;					// x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   int8_t    dy_sym;					// y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   int32_t   dx_x2;					// dx*2ֵ���������ڼӿ������ٶ�
   int32_t   dy_x2;					// dy*2ֵ���������ڼӿ������ٶ�
   int32_t   di;						// ���߱���
   
   int32_t   wx, wy;					// �߿����
   int32_t   draw_a, draw_b;
   
   /* �������� */
   if(with==0) return;
   if(with>50) with = 50;
   
   dx = x1-x0;						// ��ȡ����֮��Ĳ�ֵ
   dy = y1-y0;
   
   wx = with/2;
   wy = with-wx-1;
   
   /* �ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ��� */
   if(dx>0)							// �ж�x�᷽��
   {  dx_sym = 1;					// dx>0������dx_sym=1
   }
   else
   {  if(dx<0)
      {  dx_sym = -1;				// dx<0������dx_sym=-1
      }
      else
      {  /* dx==0������ֱ�ߣ���һ�� */
         wx = x0-wx;
         if(wx<0) wx = 0;
         wy = x0+wy;
         
         while(1)
         {  x0 = wx;
            GUI_VLine(x0, y0, y1);
            if(wx>=wy) break;
            wx++;
         }
         
      	 return;
      }
   }
   
   if(dy>0)							// �ж�y�᷽��
   {  dy_sym = 1;					// dy>0������dy_sym=1
   }
   else
   {  if(dy<0)
      {  dy_sym = -1;				// dy<0������dy_sym=-1
      }
      else
      {  /* dy==0����ˮƽ�ߣ���һ�� */
         wx = y0-wx;
         if(wx<0) wx = 0;
         wy = y0+wy;
         
         while(1)
         {  y0 = wx;
            GUI_HLine(y0, x0, x1);
            if(wx>=wy) break;
            wx++;
         }
      	 return;
      }
   }
    
   /* ��dx��dyȡ����ֵ */
   dx = dx_sym * dx;
   dy = dy_sym * dy;
 
   /* ����2����dx��dyֵ */
   dx_x2 = dx*2;
   dy_x2 = dy*2;
   
   /* ʹ��Bresenham�����л�ֱ�� */
   if(dx>=dy)						// ����dx>=dy����ʹ��x��Ϊ��׼
   {  di = dy_x2 - dx;
      while(x0!=x1)
      {  /* x����������������y���򣬼�����ֱ�� */
         draw_a = y0-wx;
         if(draw_a<0) draw_a = 0;
         draw_b = y0+wy;
         GUI_VLine(x0, draw_a, draw_b);
         
         x0 += dx_sym;				
         if(di<0)
         {  di += dy_x2;			// �������һ���ľ���ֵ
         }
         else
         {  di += dy_x2 - dx_x2;
            y0 += dy_sym;
         }
      }
      draw_a = y0-wx;
      if(draw_a<0) draw_a = 0;
      draw_b = y0+wy;
      GUI_VLine(x0, draw_a, draw_b);
   }
   else								// ����dx<dy����ʹ��y��Ϊ��׼
   {  di = dx_x2 - dy;
      while(y0!=y1)
      {  /* y����������������x���򣬼���ˮƽ�� */
         draw_a = x0-wx;
         if(draw_a<0) draw_a = 0;
         draw_b = x0+wy;
         GUI_HLine(y0, draw_a, draw_b);
         y0 += dy_sym;
         if(di<0)
         {  di += dx_x2;
         }
         else
         {  di += dx_x2 - dy_x2;
            x0 += dx_sym;
         }
      }
      draw_a = x0-wx;
      if(draw_a<0) draw_a = 0;
      draw_b = x0+wy;
      GUI_HLine(y0, draw_a, draw_b);
   } 
}




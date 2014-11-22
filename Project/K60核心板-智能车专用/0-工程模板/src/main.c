#include "gpio.h"
#include "common.h"
#include "uart.h"

/* CH Kinetis固件库 V2.50 版本 */
/* 修改主频 请使用 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

/* UART 快速初始化结构所支持的引脚* 使用时还是推荐标准初始化 */
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
 
int main(void)
{
    DelayInit();
    /* 初始化一个引脚 设置为推挽输出 */
    GPIO_QuickInit(HW_GPIOC, 16, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOC, 17, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOC, 18, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOD, 1, kGPIO_Mode_OPP);
    /* 初始化一个串口 使用UART0端口的PTD6引脚和PTD7引脚作为接收和发送，默认设置 baud 115200 */
    UART_QuickInit(UART4_RX_PC14_TX_PC15, 115200);
    
    printf("HelloWorld!\r\n");
    
    while(1)
    {
        /* 闪烁小灯 */
        GPIO_ToggleBit(HW_GPIOC, 16);
        GPIO_ToggleBit(HW_GPIOC, 17);
        GPIO_ToggleBit(HW_GPIOC, 18);
        GPIO_ToggleBit(HW_GPIOD, 1);
        DelayMs(500);
    }
}



#include "sys.h"
#include "gpio.h"
#include "uart.h"
#include "minishell.h"
#include "systick.h"
#include "sdhc.h"
#include "clock.h"
#include "cpuidy.h"
//User defined functions
extern int CommandFun_SDInitTest(int argc, char *argv[]);
MINISHELL_CommandTableTypeDef cmd_tbl[] =
{
    { "info", 1, DisplayCPUInfo ,"list CPU info" },
};



#pragma weak configure_uart_pin_mux
extern void configure_uart_pin_mux(uint32_t instance);
int main(void)
{
	  uint8_t ch;
    uint32_t Req;
    uint8_t i;
    //����GPIO��ʼ���ṹ
    GPIO_InitTypeDef GPIO_InitStruct1;
  //  SystemClockSetup(kClockSource_EX50M,kCoreClock_200M);
	  DelayInit();
    UART_DebugPortInit(UART2_USER_DEFINE, 115200);
		PinMuxConfig(4,16,3);
		PinMuxConfig(4,17,3);
	//  PinMuxConfig(PTE,16,3);
	//  PinMuxConfig(PTE,17,3);
    UART_printf("HelloWorld!\r\n");
	  configure_uart_pin_mux(1);
	  MINISHELL_Register(cmd_tbl, ARRAY_SIZE(cmd_tbl));
    GPIO_InitStruct1.GPIOx = PTC;                             //C�˿�
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;                //��ʼ��������ߵ�ƽ
    GPIO_InitStruct1.GPIO_Pin = kGPIO_Pin_18;                  //PC16����
    GPIO_InitStruct1.GPIO_Mode = kGPIO_Mode_IPU;               //�������
    //ִ��GPIO��ʼ��
    GPIO_Init(&GPIO_InitStruct1);      
		GPIO_ITConfig(PTC, kGPIO_IT_Rising, kGPIO_Pin_18, ENABLE);
	  
    GPIO_InitStruct1.GPIOx = PTD;                             //C�˿�
    GPIO_InitStruct1.GPIO_InitState = Bit_SET;                //��ʼ��������ߵ�ƽ
    GPIO_InitStruct1.GPIO_Pin = kGPIO_Pin_1;                  //PC16����
    GPIO_InitStruct1.GPIO_Mode = kGPIO_Mode_OPP;               //�������
    //ִ��GPIO��ʼ��
    GPIO_Init(&GPIO_InitStruct1);     
		
		UART_printf("%d\r\n", &configure_uart_pin_mux);
    
		for(i=0;i<kClockNameCount;i++)
		{
    CLOCK_GetClockFrequency(i ,&Req);
		UART_printf("Req:%d\r\n", Req);
		}
    Req = 0;
		for(i=0;i<kMemNameCount;i++)
		{
    CPUIDY_GetMemSize(i ,&Req);
		UART_printf("Mem:%d\r\n", Req);
		}

	 // UART_ITConfig(UART4, kUART_IT_RDRF, ENABLE);
	  //UART_ITConfig(UART4, kUART_IT_TC, ENABLE);
	 // while(1);
    //GPIO_ITConfig(PTC,kGPIO_IT_Rising, kGPIO_Pin_18, ENABLE);
    UART_printf("FamilyID:%s\r\n", CPUIDY_GetFamID());
		CPUIDY_GetPinCount(&Req);
		UART_printf("PinCnt:%d\r\n", Req);
    //CommandFun_WDOGInitTest2();
		while(1)
		{
			DelayMs(300);
			DelayMs(300);
			UART_printf("!!\r\n");
		}
		
    while(1)
    {
			SHELL_CmdHandleLoop("SHELL>>");
    }
}





void SysTick_Handler(void)
{
	GPIO_ToggleBit(PTD, kGPIO_Pin_1);
}






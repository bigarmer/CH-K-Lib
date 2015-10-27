#include "gpio.h"
#include "common.h"
#include "uart.h"

#include <atp.h>


/*
��λ
AT+RST
���ò�����
AT+UART=115200,0,0
���й̼���Ϣ
AT+INFO
���ù���ģʽ
AT+CMODE=0
���ԭʼ����
AT+RAW
�������Ƶ��
AT+OUTFREQ=100
����У׼ֵ
AT+CAL(A/G/M)=10,33,55
��¼����������Ϣ
AT+SAVE

*/

int ATRST(int argc, char * const argv[])
{
    NVIC_SystemReset();
    return 0;
}

int AT(int argc, char * const argv[])
{
    return 0;
}

int INFO(int argc, char * const argv[])
{
    printf("UID(MAC):0x%X\r\n", GetUID());
    printf("Version:%d\r\n", 201);
    return 0;
}


ATCMD_t ATCMD[] = 
{
    {"",        AT      },
    {"+RST",    ATRST   },
    {"+INFO",   INFO    },
};



static void UART_RX_ISR(uint16_t byteReceived)
{
    ATP_Put(byteReceived);
}

 
int main(void)
{
    DelayInit();
    DelayMs(10);
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    
    UART_QuickInit(UART0_RX_PB16_TX_PB17 , 115200);
    
    printf("ATCMD test\r\n");

    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
    ATP_Init(ATCMD, ARRAY_SIZE(ATCMD));
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}



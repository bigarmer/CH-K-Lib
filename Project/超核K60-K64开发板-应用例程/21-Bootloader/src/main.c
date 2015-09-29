#include "common.h"
#include "uart.h"
#include "dma.h"
#include "flash.h"

#include "def.h"
#include "bootloader.h"


int main(void)
{
    DelayInit();
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 921600);
    
    BootloaderInit(2000);
    
    while(1)
    {
        BootloaderProc(); //ִ��Bootloader������
    }
}

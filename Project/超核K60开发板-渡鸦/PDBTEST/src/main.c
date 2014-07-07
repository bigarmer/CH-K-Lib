#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "pdb.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */


void PDB_ISR(void)
{
    printf("!!!\r\n");
}
 
int main(void)
{
    DelayInit();
    /* ��ʼ��һ������ ����Ϊ������� */
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* ��ʼ��һ������ ʹ��UART0�˿ڵ�PTD6���ź�PTD7������Ϊ���պͷ��ͣ�Ĭ������ baud 115200 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("HelloWorld!\r\n");
    
    PDB_InitTypeDef PDB_InitStruct1;
    PDB_InitStruct1.inputTrigSource = 15;
    PDB_InitStruct1.isContinuesMode = true;
    PDB_InitStruct1.srcClock = 96000000;
    PDB_Init(&PDB_InitStruct1);
    PDB_CallbackInstall(PDB_ISR);
    PDB_ITDMAConfig(kPDB_IT_CF, true);
    
    PDB_SoftwareTrigger();
    
    while(1)
    {
        /* ��˸С�� */
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}



#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include <stdint.h>
#include "uart.h"
#include "common.h"

/* 
    @fn:    BootloaderInit
    @bref:  ��ʼ��Bootloader
    @params: 
            UARTxMAP �� ѡ���Ӧ�Ĵ��ڼ��˴��ڶ�Ӧ��IO�����ã���MAIN����
            baudrate:   ���ڲ����� ���ֵ�Թ�2000000(2Mbps, ���ߵ�û��������)
            delayMs:    ����ȴ���ô��ʱ�����û��ִ�����ز���������ת��������ִ��
    @return: 1 �����ʼ���ɹ���0 �����ʼ��ʧ��
*/
extern uint8_t BootloaderInit(UART_Type *uartch, uint32_t baudrate, uint32_t delayMs);


/* 
    @fn:    BootloaderProc
    @bref:  Bootloader�Ĵ�����
    @params: ��
    @return: ��
*/
extern void BootloaderProc(void);



#endif

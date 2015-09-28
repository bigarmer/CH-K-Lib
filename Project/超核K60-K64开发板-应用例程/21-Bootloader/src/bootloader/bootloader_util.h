#ifndef __BOOTLOADER_UTIL_H__
#define __BOOTLOADER_UTIL_H__

#include <common.h>
#include "uart.h"

#define CMD_SERIALPORT      0xFD        /* ���Դ��ڵ����� */
#define CMD_CHIPINFO        0x80
#define CMD_APP_INFO        0x81
#define CMD_TRANS_DATA      0x82
#define CMD_VERIFICATION    0x83
#define CMD_APP_CHECK       0x84
#define CMD_SOFT_RESET      0x85


/* ���в��� */
typedef struct
{
    uint32_t write_addr;		        /* д��ַ */
    volatile uint32_t usart_timeout;	/* ��ʱ������ */
    uint32_t app_length;		        /* Ӧ�ó��򳤶� */
    volatile uint32_t delayms_cnt;	    /* ������ʱ������ */
    uint8_t  currentPkgNo;              /* ��ǰ���� */
    uint8_t  total_pkg;			        /* �ܰ��� */
    volatile uint32_t wdt_timeout;	    /* ���Ź���ʱ������ */
    uint8_t  op_state;
    uint8_t  bootloaderFlg;
    uint8_t  retryCnt;
} RunType_t;


extern RunType_t M_Control;
extern UART_Type *pUARTx;


extern void Fn_RxProcData(const uint8_t buf);
extern void Fn_SendResponse(uint8_t* content, uint8_t cipherFlg,  uint16_t len);




#endif

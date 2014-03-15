/**
  ******************************************************************************
  * @file    24l01.h
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __24L01_H
#define __24L01_H	 		  

#include <stdint.h>

//���س���
#define NRF_OK      (0x00)
#define NRF_ERR     (0x01)
#define MAX_TX  		(0x10)  //�ﵽ����ʹ����ж�
#define TX_OK   		(0x20)  //TX��������ж�
#define RX_OK   		(0x40)  //���յ������ж�

//********************************************************************************************************************// 
// SPI(nRF24L01) ָ��
#define READ_REG    0x00   // �����üĴ���
#define WRITE_REG   0x20   // д���üĴ���
#define RD_RX_PLOAD 0x61   // ��ȡRX FIFO�е�����
#define WR_TX_PLOAD 0xA0   // ��TX FIFO��д������
#define FLUSH_TX    0xE1   // ���TX FIFO�е����� Ӧ���ڷ���ģʽ��
#define FLUSH_RX    0xE2   // ���RX FIFO�е����� Ӧ���ڽ���ģʽ��
#define REUSE_TX_PL 0xE3   // ����ʹ����һ����Ч����
#define NOP         0xFF   // ����
//********************************************************************************************************************// 
// SPI(nRF24L01) �Ĵ���(��ַ)
#define NCONFIG      0x00  //���÷���״̬��CRCУ��ģʽ�Լ����շ�״̬��Ӧ��ʽ
#define EN_AA       0x01  //�Զ�Ӧ��������
#define EN_RXADDR   0x02  //�����ŵ�����
#define SETUP_AW    0x03  //�շ���ַ�������
#define SETUP_RETR  0x04  //�Զ��ط�����������
#define RF_CH       0x05  //����Ƶ���趨
#define RF_SETUP    0x06
#define NRF2401_STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17

#define STATUS_MAX_RT 0x10
#define STATUS_TX_DS  0x20
#define STATUS_RX_DR  0x40


//24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��




//<! API functions
uint8_t NRF2401_Init(void);
uint8_t NRF2401_SendPacket(uint8_t *txbuf, uint8_t len);
void NRF2401_SetTXMode(void);
void NRF2401_SetRXMode(void);
uint8_t NRF2401_SendPacket(uint8_t *txbuf, uint8_t len);
uint8_t NRF2401_RecPacket(uint8_t *rxbuf);

#endif












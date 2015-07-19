#include "gpio.h"
#include "uart.h"
#include "nrf24l01.h"
#include "ili9320.h"
#include "spi.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */
 
/*
     ʵ�����ƣ�NRF2401ʵ��
     ʵ��ƽ̨����ѻ������
     ����оƬ��MK60DN512ZVQ10
 ʵ��Ч����ͨ������ʵ���������ݵ��շ����ܣ�
      ����ͨ��֧�ֵ���NRF24L01ģ��
    ע�⣺�ײ������ı�д���2.4G��usb����ģ���д����Ҫ���USB-2.4Gģ��ʹ�� 
      TX_ADDRESS[5]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
      RX_ADDRESS[5]={0x34,0x43,0x10,0x10,0x01}; //���յ�ַ    
*/

static uint8_t NRF2401RXBuffer[32] = "HelloWorld\r\n";//���߽�������
static uint8_t* gpRevChar;


/* ���ڽ����ж� */
void UART_ISR(uint16_t ch)
{
    static uint8_t rev_ch;
    rev_ch = ch;
    gpRevChar = (uint8_t*)&rev_ch;
}

static uint32_t xfer(uint8_t *buf_in, uint8_t *buf_out, uint32_t len, uint8_t cs_state)
{
    uint8_t dummy;
    
    if(!buf_in)
        buf_in = &dummy;
    if(!buf_out)
        buf_out = &dummy;
    
    while(len--)
    {
        if(len == 0)
        {
            *buf_in = SPI_ReadWriteByte(HW_SPI1, HW_CTAR0, *buf_out, 0, !cs_state); 
        }
        else
        {
            *buf_in = SPI_ReadWriteByte(HW_SPI1, HW_CTAR0, *buf_out, 0, kSPI_PCS_KeepAsserted); 
        }
        if(buf_out != &dummy)
            buf_out++;
        if(buf_in != &dummy)
            buf_in++;
    }
}

static uint32_t get_reamin(void)
{
    return 0;
}

static void ce_control(uint8_t stat)
{
    PEout(0) = stat;
}

const struct nrf24xx_ops_t ops = 
{
    xfer,
    get_reamin,
    ce_control,
    DelayMs,
};

int main(void)
{
    uint32_t i;
    uint32_t len;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    UART_CallbackRxInstall(HW_UART0, UART_ISR);
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
    
    printf("NRF24L01 test\r\n");
    /* ��ʼ�� NRF2401ģ�� ��SPI�ӿڼ�Ƭѡ */
    PORT_PinMuxConfig(HW_GPIOE, 1, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 2, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 3, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOE, 4, kPinAlt2);
    /* ��ʼ��2401�����CE���� */
    GPIO_QuickInit(HW_GPIOE, 0 , kGPIO_Mode_OPP);
    /* ��ʼ��2401ģ��*/
    SPI_QuickInit(SPI1_SCK_PE02_SOUT_PE01_SIN_PE03, kSPI_CPOL0_CPHA0, 1*1000*1000);
    SPI_CTARConfig(HW_SPI1, HW_CTAR0, kSPI_CPOL0_CPHA0, 8, kSPI_MSB, 2*1000*1000);
    nrf24l01_init(&ops);
    
    //����Ƿ���������豸�������ý��պͷ��͵�ַ
    if(nrf24l01_probe())
    {
        printf("no nrf24xx!\r\n");
        while(1);
    }
    else
    {
        printf("nrf24xx ok!\r\n");   
    }
    
    /* ����Rxģʽ */
    nrf24l01_set_rx_mode();
    while(1)
    {
        /* ����յ������������� */
        if(gpRevChar != NULL)
        {
            nrf24l01_set_tx_mode();
            nrf24l01_write_packet(gpRevChar, 1);
            nrf24l01_set_rx_mode();
            gpRevChar = NULL;
        }
        /* ����յ�2401 ������ ���䵽���� */
        if(!nrf24l01_read_packet(NRF2401RXBuffer, &len))
        {
            i = 0;
            while(len--)
            {
                UART_WriteByte(HW_UART0, NRF2401RXBuffer[i++]);
            }
        }
    }
}



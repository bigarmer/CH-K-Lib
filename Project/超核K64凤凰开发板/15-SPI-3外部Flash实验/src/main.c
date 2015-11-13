#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "w25qxx.h"
#include "spi.h"
/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ���޸� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */
 
/*
     ʵ�����ƣ�SPI�ⲿFlashʵ��
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч��: ��ȡ��������SPI-Flash����Ϣͨ�����ڷ��ͳ�ȥ
      С����������˸����˸ʱ����500ms     
*/

#include <string.h>


static int w25qxx_test(uint32_t begin, uint32_t end)
{
    uint32_t i, total_sector, j;
    static uint8_t buf[W25QXX_SECTOR_SIZE];
    
    printf("spi flash id:0x%X\r\n", w25qxx_get_id());
    
    total_sector = (end - begin)/W25QXX_SECTOR_SIZE;
    
    /* erase chip */
    printf("erase all chips...\r\n");
    w25qxx_erase_chip();
    printf("erase complete\r\n");
    
    for(i=begin/W25QXX_SECTOR_SIZE; i<total_sector; i++)
    {
        printf("verify addr:0x%X(%d)...\r\n", i*W25QXX_SECTOR_SIZE, i);
        for(j=0;j<sizeof(buf);j++)
        {
            buf[j] = j % 0xFF;
        }
        w25qxx_write_sector(i*W25QXX_SECTOR_SIZE, buf, W25QXX_SECTOR_SIZE);
        memset(buf, 0, W25QXX_SECTOR_SIZE);
        w25qxx_read(i*W25QXX_SECTOR_SIZE, buf, W25QXX_SECTOR_SIZE);
        
        /* varify */
        for(j=0;j<sizeof(buf);j++)
        {
            if(buf[j] != (j%0xFF))
            {
                printf("%d error\r\n", j);
            }
        }
    }
    return 0;
}


static uint32_t _get_reamin(void)
{
    return 0;
}




static uint32_t xfer(uint8_t *buf_in, uint8_t *buf_out, uint32_t len, uint8_t cs_state)
{
    //GPIO_WriteBit(BOARD_SPI_CS_PORT, BOARD_SPI_CS_PIN, 0);
    while(len--)
    {
        if(len)
        {
            if(!buf_in)
            {
                SPI_ReadWriteByte(HW_SPI2 , HW_CTAR0, *buf_out++, 1, kSPI_PCS_KeepAsserted); 
            }
            else if(!buf_out)
            {
                *buf_in++ = SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xFF, 1, kSPI_PCS_KeepAsserted); 
            }
            else
            {
                *buf_in++ = SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, *buf_out++, 1, kSPI_PCS_KeepAsserted);   
            }
        }
        else
        {
            if(!buf_in)
            {
                SPI_ReadWriteByte(HW_SPI2 , HW_CTAR0, *buf_out++, 1, (SPI_PCS_Type)!cs_state); 
            }
            else if(!buf_out)
            {
                *buf_in++ = SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, 0xFF, 1, (SPI_PCS_Type)!cs_state); 
            }
            else
            {
                *buf_in++ = SPI_ReadWriteByte(HW_SPI2, HW_CTAR0, *buf_out++, 1, (SPI_PCS_Type)!cs_state);   
            }
        }

    }
    //GPIO_WriteBit(BOARD_SPI_CS_PORT, BOARD_SPI_CS_PIN, cs_state);
    return len;
}


    

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
    printf("w25qxx test\r\n");
    
    /* ��ʼ��SPI2�ӿ� */
    SPI_QuickInit(SPI2_SCK_PD12_SOUT_PD13_SIN_PD14, kSPI_CPOL0_CPHA0, 10*1000*1000);
    
    /* ��ʼ��w25qxx ʹ��CS1Ƭѡ */
    PORT_PinMuxConfig(HW_GPIOD, 15, kPinAlt2); /* SPI2_PCS1 */
    
    /* ��ȡSPI-Flash����Ϣ */
    struct w25qxx_init_t init;
    
    init.delayms = DelayMs;
    init.get_reamin = _get_reamin;
    init.xfer = xfer;
    
    if(w25qxx_init(&init))
    {
        printf("w25qxx device no found!\r\n");
    }
    else
    {
        printf("spi_flash found id:0x%X\r\n", w25qxx_get_id());
    }

    w25qxx_test(0, 2*1024*1024);
    
    printf("w252qxx test complete\r\n");
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOA, 9);
        DelayMs(500);  //С����˸
    }
}


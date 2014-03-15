#include "spi_abstraction.h"
#include "board.h"
#include "spi.h"
#include "clock.h"

static uint32_t gSPIInstance;

 /**
 * @brief  SPI ͨ�ó����ӿ� SPI ��ʼ��
 * 
 * @param  frameFormat: ֡��ʽ
 *         @arg kSPI_ABS_CPOL0_CPHA0:
 *         @arg kSPI_ABS_CPOL0_CPHA1:
 *         @arg kSPI_ABS_CPOL1_CPHA0:
 *         @arg kSPI_ABS_CPOL1_CPHA1:
 * @retval None
 */
int SPI_ABS_Init(int frameFormat, int baudrate)
{
    gSPIInstance = SPI_QuickInit(BOARD_SPI_MAP, frameFormat, baudrate);
    return kSPI_ABS_StatusOK;
}
 /**
 * @brief  SPI ����һ���ֽ�
 * 
 * @param  data: ���͵�����
 * @param  cs:   PCS ʹ���ź�ѡ��
 * @param  csState: �������ݺ�CS�ߵ�״̬
 *         @arg kSPI_ABS_CS_ReturnInactive: �ص�δѡ��״̬
 *         @arg kSPI_ABS_CS_KeepAsserted:   ��������ѡ��״̬
 * @retval ���յ�������
 */
int SPI_ABS_ReadWriteByte(uint8_t data, uint32_t cs, uint16_t csState)
{
    return SPI_ReadWriteByte(gSPIInstance, data, cs, csState);
}

int SPI_ABS_xfer(uint8_t *dataSend, uint8_t *dataReceived, uint32_t cs, uint16_t csState, uint32_t len)
{
    if(csState == kSPI_ABS_CS_KeepAsserted)
    {
        len--;
        while(len--)
        {
            *dataReceived++ = SPI_ReadWriteByte(gSPIInstance, *dataSend++, cs, kSPI_ABS_CS_KeepAsserted);
        }
        *dataReceived++ = SPI_ReadWriteByte(gSPIInstance, *dataSend++, cs, kSPI_ABS_CS_ReturnInactive);
    }
    else if(csState == kSPI_ABS_CS_ReturnInactive)
    {
        while(len--)
        {
            *dataReceived++ = SPI_ReadWriteByte(gSPIInstance, *dataSend++, cs, kSPI_ABS_CS_ReturnInactive);
        }
    }
    return kSPI_ABS_StatusOK;
}



int SPI_ABS_AbortTransfer(void)
{
    return kSPI_ABS_StatusUnsupported;
}

/*
void SPI_ABS_SetCSHigh(void)
{
    
}

void SPI_ABS_SetCSLow(void)
{
    
}
*/








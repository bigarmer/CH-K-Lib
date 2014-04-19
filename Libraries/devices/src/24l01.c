/**
  ******************************************************************************
  * @file    24l01.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "24l01.h"
#include "board.h"
#include "gpio.h"
#include "spi.h"
#include "spi_abstraction.h"

#define NRF24L01_DEBUG		0
#if ( NRF24L01_DEBUG == 1 )
#define NRF24L01_DEBUG	printf
#else
#define NRF24L01_DEBUG(...)
#endif

static struct spi_device device;

//���س���
#define NRF_OK          (0x00)
#define NRF_ERR         (0x01)
#define MAX_TX  		(0x10)  //�ﵽ����ʹ����ж�
#define TX_OK   		(0x20)  //TX��������ж�
#define RX_OK   		(0x40)  //���յ������ж�

//********************************************************************************************************************// 
// SPI(nRF24L01) ָ��
#define READ_REG        0x00   // �����üĴ���
#define WRITE_REG       0x20   // д���üĴ���
#define RD_RX_PLOAD     0x61   // ��ȡRX FIFO�е�����
#define WR_TX_PLOAD     0xA0   // ��TX FIFO��д������
#define FLUSH_TX        0xE1   // ���TX FIFO�е����� Ӧ���ڷ���ģʽ��
#define FLUSH_RX        0xE2   // ���RX FIFO�е����� Ӧ���ڽ���ģʽ��
#define REUSE_TX_PL     0xE3   // ����ʹ����һ����Ч����
#define R_RX_PL_WID     0x60
#define NOP             0xFF   // ����
//********************************************************************************************************************// 
/* register define */
#define NCONFIG             0x00  //���÷���״̬��CRCУ��ģʽ�Լ����շ�״̬��Ӧ��ʽ
#define EN_AA               0x01  //�Զ�Ӧ��������
#define EN_RXADDR           0x02  //�����ŵ�����
#define SETUP_AW            0x03  //�շ���ַ�������
#define SETUP_RETR          0x04  //�Զ��ط�����������
#define RF_CH               0x05  //����Ƶ���趨
#define RF_SETUP            0x06
#define STATUS              0x07
#define OBSERVE_TX          0x08
#define CD                  0x09
#define RX_ADDR_P0          0x0A
#define RX_ADDR_P1          0x0B
#define RX_ADDR_P2          0x0C
#define RX_ADDR_P3          0x0D
#define RX_ADDR_P4          0x0E
#define RX_ADDR_P5          0x0F
#define TX_ADDR             0x10
#define RX_PW_P0            0x11
#define RX_PW_P1            0x12
#define RX_PW_P2            0x13
#define RX_PW_P3            0x14
#define RX_PW_P4            0x15
#define RX_PW_P5            0x16
#define FIFO_STATUS         0x17
#define PYNPD               0x1C
#define FEATURE             0x1D

#define STATUS_MAX_RT 0x10
#define STATUS_TX_DS  0x20
#define STATUS_RX_DR  0x40


//24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5   	//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH  32  	//32�ֽڵ��û����ݿ��


#define CE_HIGH()     GPIO_WriteBit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN, 1)
#define CE_LOW()      GPIO_WriteBit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN, 0)

static uint8_t NRF2401_WriteBuffer(uint8_t reg, uint8_t *pData, uint8_t len)
{
    uint8_t status;
    status = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, reg, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    len--;
    while(len--)
    {
        SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, *pData++, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    }
    SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, *pData++, BOARD_SPI_NRF2401_CSn, kSPI_PCS_ReturnInactive);
    return(status);          // return nRF24L01 status byte
}

static uint8_t NRF2401_ReadWriteReg(uint8_t reg,uint8_t value)
{
    uint8_t status;
    status = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, reg, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, value, BOARD_SPI_NRF2401_CSn, kSPI_PCS_ReturnInactive);
    return status;
}

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���յ�ַ
unsigned char Bank0_Reg[][2]=
{
    {0,0x0F},//congig
    {1,0x0F},//en_aa
    {2,0x03},//en_rxaddr
    {3,0x03},//setup_aw
    {4,0x15},//setup_retr
    {5,40},//rf_ch
    {6,0X07},//rf_setup	//REG6,120517,0x0F or 0x2F:2Mbps; 0x07:1Mbps ; 0x27:250Kbps
    {7,0x07},//status
    {8,0x00},//observe_tx
    {9,0x00},//cd
    {12,0x11},//rx_addr_p2
    {13,0x12},//rx_addr_p3
    {14,0x13},//rx_addr_p4
    {15,0x14},//rx_addr_p5
    {17,0x20},//RX_PW_P0
    {18,0x20},//RX_PW_P1
    {19,0x20},//RX_PW_P2
    {20,0x20},//RX_PW_P3
    {21,0x20},//RX_PW_P4
    {22,0x20},//RX_PW_P5
    {23,0x00},//FIFO_STATUS,
    {28,0x3F},//DYNPD
    {29,0x04}, //FEATURE
};

static uint8_t NRF2401_ConfigStatus(void)
{
    uint8_t i;
	for(i = 22; i > 0; i--)
    {
        NRF2401_ReadWriteReg(WRITE_REG+Bank0_Reg[i][0], Bank0_Reg[i][1]);    //ˢ�½�������
    }
	
	NRF2401_WriteBuffer(WRITE_REG+RX_ADDR_P0, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH);//дTX�ڵ��ַ
	NRF2401_WriteBuffer(WRITE_REG+TX_ADDR, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH);//дRX�ڵ��ַ
	i = NRF2401_ReadWriteReg(29,0xFF); //��ȡ�Ĵ�����״̬ 
	if(i == 0) // i!=0 showed that chip has been actived.so do not active again.
    {
        NRF2401_ReadWriteReg(0X50,0x73);// Active
    }
	for(i = 22; i >= 21; i--)
    {
        NRF2401_ReadWriteReg((WRITE_REG|Bank0_Reg[i][0]), Bank0_Reg[i][1]);
    }
	return 0;
}


static uint8_t NRF2401_ReadBuffer(uint8_t reg, uint8_t *pData, uint32_t len)
{
    uint8_t status;
    /* Select register to write to and read status byte */
    status = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, reg, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    len--;
    while(len--)
    { 
        *pData++ = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, 0x00, BOARD_SPI_NRF2401_CSn, kSPI_PCS_KeepAsserted);
    }
    *pData++ = SPI_ReadWriteByte(HW_SPI0, HW_CTAR0, 0x00, BOARD_SPI_NRF2401_CSn, kSPI_PCS_ReturnInactive);
	return(status);                    // return nRF24L01 status byte
}

static int write_register(uint8_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t temp = WRITE_REG + addr;
    spi_write(&device, &temp, 1, false);
    spi_write(&device, buf, len, true);
}

static int read_register(uint8_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t temp = READ_REG + addr;
    spi_write(&device, &temp, 1, false);
    spi_read(&device, buf, len, true);
}

int nrf24l01_probe(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i = sizeof(buf);
    write_register(TX_ADDR, buf, sizeof(buf));
	for(i = 0; i < 5; i++)
	{
        buf[i] = 0;
	}
    read_register(TX_ADDR, buf, sizeof(buf));
	for(i = 0; i < 5; i++)
	{
        if(buf[i] != 0xA5) return 1;
	}
	return 0;
}


int nrf24l01_init(spi_bus_t bus, uint32_t cs)
{
    uint32_t ret;
    device.csn = cs;
    device.config.baudrate = 1*1000*1000;
    device.config.data_width = 8;
    device.config.mode = SPI_MODE_0 | SPI_MASTER | SPI_MSB;
    ret = spi_bus_attach_device(bus, &device);
    if(ret)
    {
        return ret;
    }
    else
    {
        ret = spi_config(&device);
    }
    return ret;
}

uint8_t NRF2401_Init(void)
{
	GPIO_QuickInit(BOARD_NRF2401_CE_PORT, BOARD_NRF2401_CE_PIN , kGPIO_Mode_OPP);
    GPIO_QuickInit(BOARD_NRF2401_IRQ_PORT, BOARD_NRF2401_IRQ_PIN , kGPIO_Mode_IPU);

    CE_LOW();
    
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x01, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
 //   NRF2401_ReadWriteReg(WRITE_REG + 0x1D, 0x07);
    NRF2401_ConfigStatus();
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x01, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1C, 0x3F);
  //  NRF2401_ReadWriteReg(WRITE_REG + 0x1D, 0x07);
    return 0;
}



//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF2401_SetTXMode(void)
{
    uint8_t value;
	NRF2401_ReadWriteReg(FLUSH_TX,  0x00);    //ˢ�½������� 
    CE_LOW();// chip enable
    value=NRF2401_ReadWriteReg(NCONFIG, 0xFF); //��ȡ�Ĵ�����״̬ 
    value |= 0xFE;//set bit 1
    NRF2401_ReadWriteReg(WRITE_REG+NCONFIG, value);// Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..
    CE_HIGH(); //CEΪ��,�������ģʽ 
}

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF2401_SetRXMode(void)
{
	uint8_t value;
	NRF2401_ReadWriteReg(FLUSH_RX, 0x00);    //ˢ�½�������
	value=NRF2401_ReadWriteReg(STATUS, 0xFF); //��ȡ�Ĵ�����״̬ 
	NRF2401_ReadWriteReg(WRITE_REG+STATUS, value);  //���״̬��־
	CE_LOW();
	value = NRF2401_ReadWriteReg(NCONFIG,0xFF); //��ȡ�Ĵ�����״̬ 
	value |= 0x01;//set bit 1
	NRF2401_ReadWriteReg(WRITE_REG+NCONFIG, value);// Set PWR_UP bit, enable CRC(2 length) & Prim:RX. RX_DR enabled..
	CE_HIGH(); //CEΪ��,�������ģʽ 
}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
uint8_t NRF2401_SendPacket(uint8_t *txbuf, uint8_t len)
{
    NRF2401_WriteBuffer(WR_TX_PLOAD,txbuf, len);//д���ݵ�TX BUF  32���ֽ�
    return 0;
}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������



int nrf24l01_read_packet(uint8_t *buf, uint32_t *len)
{
	uint8_t sta,rev_len;
	sta = NRF2401_ReadWriteReg(STATUS, 0xFF);
	if(sta & RX_OK)
	{
        write_register(STATUS, &sta, 1);//���TX_DS��MAX_RT�жϱ�־
        read_register(R_RX_PL_WID, &rev_len, 1);
        read_register(RD_RX_PLOAD, buf, rev_len);// read receive payload from RX_FIFO buffer
        *len = rev_len;
        if(rev_len > 32)
        {
            NRF2401_ReadWriteReg(FLUSH_RX,0xff);//���TX FIFO�Ĵ��� 
            return 2;
        }
        return 0;
	}
    *len = 0;
    return 1;
}



 













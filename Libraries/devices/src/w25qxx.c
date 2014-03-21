/**
  ******************************************************************************
  * @file    w25qxx.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "spi_abstraction.h"
#include "w25qxx.h"

#define W25X_PAGE_SIZE          (256)
#define W25X_SECTOR_SIZE        (4096)
#define W25X_PBLOCK_SIZE        (64*1024)

#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 


#ifndef USE_USER_MEM
uint8_t SPI_FLASH_BUFFER[4096];	
#endif

typedef struct
{
    const char* name;
    uint32_t size;
    uint16_t id;
}w25qxx_attr_t;

static const w25qxx_attr_t w25qxx_attr_table[] = 
{
    {"W25Q10",    128*1024, 0xEF10},
    {"W25Q20",    256*1024, 0xEF11},
    {"W25Q40",    512*1024, 0xEF12},
    {"W25Q80",   1024*1024, 0xEF13},
    {"W25Q16",   2048*1024, 0xEF14},
    {"W25Q32",   4096*1024, 0xEF15},
    {"W25Q64",   8192*1024, 0xEF16},
    {"W25Q128", 16384*1024, 0xEF17}, 
};

static spi_status w25qxx_power_up(struct w25qxx_device * device)
{
    volatile uint32_t i;
    uint8_t buf[1];
    buf[0] = W25X_ReleasePowerDown;
    device->bus->write(device->bus, &device->spi_device, buf, sizeof(buf), true);
    //delay 3us
    for(i=0;i<1000;i++);
    return kspi_status_ok;
}

static uint8_t w25qxx_read_sr(struct w25qxx_device * device)
{
    uint8_t buf[1];
    buf[0] = W25X_ReadStatusReg;
    device->bus->write(device->bus, &device->spi_device, buf, 1, false); //false = ����Ƭѡ,��������
    device->bus->read(device->bus, &device->spi_device, buf, 1, true);
    return buf[0];
}


static spi_status w25qxx_probe(struct w25qxx_device * device)
{
    uint32_t i;
    uint16_t id;
    uint8_t buf[4];
    buf[0] = W25X_ManufactDeviceID;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
    //read id
    device->bus->write(device->bus, &device->spi_device, buf, 4, false);
    device->bus->read(device->bus, &device->spi_device, buf, 2, true);
    id = ((buf[0]<<8) + buf[1]);
    //see if we find a match
    for(i = 0; i< ARRAY_SIZE(w25qxx_attr_table);i++)
    {
        if(w25qxx_attr_table[i].id == id)
        {
            // find a match
            device->name = w25qxx_attr_table[i].name;
            device->id = w25qxx_attr_table[i].id;
            device->size = w25qxx_attr_table[i].size;
            w25qxx_power_up(device);
            return kspi_status_ok; 
        }
    }
    return kspi_status_error;
}

static spi_status w25qxx_read(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t buf_send[4];
    buf_send[0] = W25X_ReadData;
    buf_send[1] = (uint8_t)((addr)>>16);
    buf_send[2] = (uint8_t)((addr)>>8);
    buf_send[3] = (uint8_t)addr;
    if(device->bus->write(device->bus, &device->spi_device, buf_send, sizeof(buf_send), false))
    {
        return kspi_status_error;
    }
    device->bus->read(device->bus, &device->spi_device, buf, len, true);
    return kspi_status_ok;
}

static spi_status w25qxx_write_enable(struct w25qxx_device * device)
{
    uint8_t buf[1];
    buf[0] = W25X_WriteEnable;
    device->bus->write(device->bus, &device->spi_device, buf, sizeof(buf), true);
    return kspi_status_ok;
}

static spi_status w25qxx_write_page(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t buf_send[4];
    w25qxx_write_enable(device);
    buf_send[0] = W25X_PageProgram;
    buf_send[1] = (uint8_t)((addr)>>16);
    buf_send[2] = (uint8_t)((addr)>>8);
    buf_send[3] = (uint8_t)addr;
    if(device->bus->write(device->bus, &device->spi_device, buf_send, sizeof(buf_send), false))
    {
        return kspi_status_error;
    }
    device->bus->write(device->bus, &device->spi_device, buf, len, true);
    // wait busy
    while((w25qxx_read_sr(device) & 0x01) == 0x01);
    return kspi_status_ok;
}

spi_status w25qxx_write_no_check(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)  
{ 			 		 
	uint16_t pageremain;	   
    #if DEBUG
    printf("w25qxx - write_no_check: addr:%d len:%d\r\n", addr, len);
    #endif
	pageremain = W25X_PAGE_SIZE-(addr%W25X_PAGE_SIZE); //��ҳʣ����ֽ���		 	    
	if(len <= pageremain) pageremain = len;//������256���ֽ�
	while(1)
	{	   
        if(w25qxx_write_page(device, addr, buf, pageremain))
        {
            return kspi_status_error;
        }
		if(len == pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			buf += pageremain;
			addr += pageremain;	
			len -= pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(len > 256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain = len; 	  //����256���ֽ���
		}
	}
    return kspi_status_ok;
} 

static spi_status w25qxx_erase_sector(struct w25qxx_device * device, uint32_t addr)
{
    uint8_t buf_send[4];
    addr /= W25X_SECTOR_SIZE;
    addr *= W25X_SECTOR_SIZE; //round addr to N x W25X_SECTOR_SIZE
    buf_send[0] = W25X_SectorErase;
    buf_send[1] = (uint8_t)((addr)>>16);
    buf_send[2] = (uint8_t)((addr)>>8);
    buf_send[3] = (uint8_t)addr;
    w25qxx_write_enable(device);
    while((w25qxx_read_sr(device) & 0x01) == 0x01);
    device->bus->write(device->bus, &device->spi_device, buf_send, sizeof(buf_send), true);
    while((w25qxx_read_sr(device) & 0x01) == 0x01);
    return kspi_status_ok;
}

spi_status w25qxx_write(struct w25qxx_device * device, uint32_t addr, uint8_t *buf, uint32_t len)  
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * mem_pool;
    mem_pool = SPI_FLASH_BUFFER;
 	secpos = addr/4096;//������ַ  
	secoff = addr%4096;//�������ڵ�ƫ��
	secremain = 4096-secoff;//����ʣ��ռ��С   

 	if(len <= secremain)secremain = len;//������4096���ֽ�
	while(1) 
	{	
        w25qxx_read(device, secpos*4096, mem_pool, 4096); //������������������
		for(i = 0; i < secremain; i++)//У������
		{
			if(mem_pool[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
            w25qxx_erase_sector(device, secpos*4096); //�����������
			for(i=0;i<secremain;i++)	   //����ԭ��������
			{
				mem_pool[i+secoff]=buf[i];	  
			}
            w25qxx_write_no_check(device, secpos*4096, mem_pool,4096); //д���������� 
		}else 
        {
            w25qxx_write_no_check(device, addr, buf, secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 
        }			   
		if(len == secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	buf += secremain;  //ָ��ƫ��
			addr += secremain;//д��ַƫ��	   
		   	len -= secremain;				//�ֽ����ݼ�
			if(len > 4096) secremain = 4096;	//��һ����������д����
			else secremain = len;			//��һ����������д����
		}	 
	}
    return kspi_status_ok;
}






spi_status w25qxx_init(struct w25qxx_device * device, uint32_t csn, uint32_t bus_chl, uint32_t baudrate)
{
    if(!device)
    {
        return kspi_status_error;
    }
    device->spi_device.csn = csn;
    device->spi_device.bus_chl = bus_chl;
    device->spi_device.cs_state = kspi_cs_keep_asserted;
    if(!device->bus)
    {
        return kspi_status_error;
    }
    if(!device->bus->init)
    {
        return kspi_status_error;
    }
    if(!device->bus->bus_config)
    {
        return kspi_status_error;
    }
    //init 
    if(device->bus->init(device->bus, device->bus->instance))
    {
        return kspi_status_error;
    }
    // bus chl config
    if(device->bus->bus_config(device->bus, device->spi_device.bus_chl, kspi_cpol0_cpha1, baudrate))
    {
        return kspi_status_error;
    }
    // link ops
    device->init = w25qxx_init;
    device->probe = w25qxx_probe;
    device->read = w25qxx_read;
    device->write = w25qxx_write;
    device->erase_sector = w25qxx_erase_sector;
    return kspi_status_ok;
}










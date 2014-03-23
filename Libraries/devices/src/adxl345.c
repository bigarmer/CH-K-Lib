/**
  ******************************************************************************
  * @file    adxl345.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "i2c_abstraction.h"
#include "adxl345.h"
#include "common.h"
#include <math.h>

#define DEVICE_ID		0X00 	//����ID,0XE5
#define THRESH_TAP		0X1D   	//�û���ֵ
#define OFSX			0X1E
#define OFSY			0X1F
#define OFSZ			0X20
#define DUR				0X21
#define Latent			0X22
#define Window  		0X23
#define THRESH_ACK		0X24
#define THRESH_INACT	0X25
#define TIME_INACT		0X26
#define ACT_INACT_CTL	0X27
#define THRESH_FF		0X28
#define TIME_FF			0X29
#define TAP_AXES		0X2A
#define ACT_TAP_STATUS  0X2B
#define BW_RATE			0X2C
#define POWER_CTL		0X2D

#define INT_ENABLE		0X2E
#define INT_MAP			0X2F
#define INT_SOURCE  	0X30
#define DATA_FORMAT	    0X31
#define DATA_X0			0X32
#define DATA_X1			0X33
#define DATA_Y0			0X34
#define DATA_Y1			0X35
#define DATA_Z0			0X36
#define DATA_Z1			0X37
#define FIFO_CTL		0X38
#define FIFO_STATUS		0X39

static i2c_status adxl345_write_register(adxl345_device_t device, uint8_t addr, uint8_t value)
{
    uint8_t buf[1];
    device->i2c_device.subaddr = addr;
    device->i2c_device.subaddr_len = 1;
    buf[0] = value;
    if(device->bus->write(device->bus, &device->i2c_device, buf, 1))
    {
        return ki2c_status_error;
    }
    return ki2c_status_ok;
}

/*
static uint8_t adxl345_read_register(struct adxl345_device * device, uint8_t addr)
{
    uint8_t buf[1];
    device->i2c_device.subaddr = addr;
    device->i2c_device.subaddr_len = 1;
    device->bus->read(device->bus, &device->i2c_device, buf, 1);
    return buf[0];
}
*/

i2c_status adxl345_probe(adxl345_device_t device)
{
    uint8_t buf[3];
    device->i2c_device.subaddr = DEVICE_ID;
    device->bus->read(device->bus, &device->i2c_device, buf, 1);
    if(buf[0] == 0XE5)
    {
        // init sequence
        //�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���� 
        adxl345_write_register(device, DATA_FORMAT, 0x2B);
        //��������ٶ�Ϊ100Hz
        adxl345_write_register(device, BW_RATE, 0x0A);
        //����ʹ��,����ģʽ
        adxl345_write_register(device, POWER_CTL, 0x28);
        //��ʹ���ж�
        adxl345_write_register(device, INT_ENABLE, 0x00);
        //����FIFO
        //adxl345_write_register(device, FIFO_CTL, 0x9F);
        // OFSX = OFSY = OFSZ = 0
        buf[0] = 0x00;
        buf[1] = 0x00;
        buf[2] = 0x00;
        device->i2c_device.subaddr = OFSX;
        device->bus->write(device->bus, &device->i2c_device, buf, 3);
        return ki2c_status_ok; 
    }
    return ki2c_status_error; 
}

i2c_status adxl345_readXYZ(adxl345_device_t device, short *x, short *y, short *z)
{
    uint8_t buf[6];
    //uint8_t fifo_cnt;
    device->i2c_device.subaddr = DATA_X0;
    device->bus->read(device->bus, &device->i2c_device, buf, 6); 
    device->bus->read(device->bus, &device->i2c_device, buf, 6); 
   // fifo_cnt = adxl345_read_register(device, FIFO_STATUS) & 0x3F;
   // if(fifo_cnt)
    *x=(short)(((uint16_t)buf[1]<<8)+buf[0]); 	    
    *y=(short)(((uint16_t)buf[3]<<8)+buf[2]); 	    
    *z=(short)(((uint16_t)buf[5]<<8)+buf[4]); 
    return ki2c_status_ok; 
}

i2c_status adxl345_calibration(adxl345_device_t device)
{
	short tx,ty,tz;
	uint8_t i;
	short offx=0,offy=0,offz=0; 
    adxl345_write_register(device, POWER_CTL, 0x00);//�Ƚ�������ģʽ.
	DelayMs(40);
	adxl345_write_register(device, DATA_FORMAT, 0X2B);	//�͵�ƽ�ж����,13λȫ�ֱ���,��������Ҷ���,16g���� 
	adxl345_write_register(device, BW_RATE, 0x0A);		//��������ٶ�Ϊ100Hz
	adxl345_write_register(device, POWER_CTL, 0x28);	   	//����ʹ��,����ģʽ
	adxl345_write_register(device, INT_ENABLE, 0x00);	//��ʹ���ж�
 //   adxl345_write_register(device, FIFO_CTL, 0x9F); //����FIFO
	DelayMs(12);
	for(i=0;i<10;i++)
	{
		device->readXYZ(device, &tx, &ty, &tz);
        DelayMs(10);
		offx += tx;
		offy += ty;
		offz += tz;
	}	 	
	offx /= 10;
	offy /= 10;
	offz /= 10;
	offx = -offx/4;
	offy = -offy/4;
	offz = -(offz-256)/4;	
#if DEBUG
    printf("OFFX:%d OFFY:%d OFFZ:%d \r\n" ,offx, offy, offz);
#endif
    
 	adxl345_write_register(device, OFSX, offx);
	adxl345_write_register(device, OFSY, offy);
	adxl345_write_register(device, OFSZ, offz);
    return ki2c_status_ok; 
}

//�õ��Ƕ�
//x,y,z:x,y,z������������ٶȷ���(����Ҫ��λ,ֱ����ֵ����)
//dir:Ҫ��õĽǶ�.0,��Z��ĽǶ�;1,��X��ĽǶ�;2,��Y��ĽǶ�.
//����ֵ:�Ƕ�ֵ.��λ0.1��.
static short adxl345_convert_angle(short x, short y, short z, short *ax, short *ay, short *az)
{
	float temp;
    float fx,fy,fz;
    fx = (float)x; fy = (float)y;  fz = (float)z; 

    temp = sqrt((fx*fx+fy*fy))/fz;
    *az = atan(temp)*1800/3.14;
    temp = fx/sqrt((fy*fy+fz*fz));
    *ax = atan(temp)*1800/3.14;;
    temp = fy/sqrt((fx*fx+fz*fz));
    *ay = atan(temp)*1800/3.14;;
	return 0;
}

i2c_status adxl345_init(struct adxl345_device * device, uint8_t chip_addr)
{
    if(!device->bus)
    {
        return ki2c_status_error;
    }
    if(!device->bus->init)
    {
        return ki2c_status_error;
    }
    if(device->bus->init(device->bus, device->bus->instance, device->bus->baudrate))
    {
        return ki2c_status_error;
    }
    //link param
    device->i2c_device.chip_addr = chip_addr;
    device->i2c_device.subaddr_len = 1;
    // link ops
    device->init = adxl345_init;
    device->probe = adxl345_probe;
    device->readXYZ = adxl345_readXYZ;
    device->calibration = adxl345_calibration;
    device->convert_angle = adxl345_convert_angle;
    return ki2c_status_ok;
}






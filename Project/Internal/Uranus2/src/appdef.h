#ifndef __APPDEF_H__
#define __APPDEF_H__


#define VERSION             (205)
#define MPU9250_INT_PIN     (18)

#define DMA_TX_CH      (HW_DMA_CH0)
#define DMA_RX_CH      (HW_DMA_CH1)


extern uint8_t gRevBuf[];

enum 
{
    kMSG_CMD_TIMER,
    kMSG_CMD_SENSOR_DATA_READY,
    kMSG_CMD_DATA_REV,
};


#endif


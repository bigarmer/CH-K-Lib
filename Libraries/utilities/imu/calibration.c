/**
  ******************************************************************************
  * @file    calibration.c
  * @author  YANDLD
  * @version V2.5
  * @date    2015.3.26
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  * @note    dymalic caliberation impletmentation
  ******************************************************************************
  */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "calibration.h"

#define M_MAX       (500)
#define M_MIN       (-500)

static struct dcal_t dcal;

#define     CAL_MAGIC           (0x5ACB)

static int is_mval_ok(int16_t data)
{
    if((data > M_MAX) || (data < M_MIN))
    {
        return 1;
    }
    return 0;
}
    
static void reset_dcal_data(void)
{
    int i;
    for(i=0;i<3;i++)
    {
        dcal.m_min[i] = 100;
        dcal.m_max[i] = -100;
        dcal.mg[i] = 1.000;
        dcal.mo[i] = 0;
    } 
}

void dcal_init(struct dcal_t *dc)
{

    if((dc) && (dc->magic == CAL_MAGIC))
    {
        memcpy(&dcal, dc, sizeof(struct dcal_t));
        printf("load dcal value!\r\n");
    }
    else
    {
        printf("no initial dcal value\r\n");
        reset_dcal_data();
    }
}

/* this function must be called every 100 ms */
void dcal_input(int16_t *mdata)
{
    int i;
    static float last_gain;
    dcal.need_update = false;
    for(i=0;i<3;i++)
    {
        if(is_mval_ok(mdata[i]))
        {
            /* strong mangetic distornation found */
            for(i=0;i<3;i++)
            {
                dcal.m_max[i] = (dcal.m_max[i]*4)/5;
                dcal.m_min[i] = (dcal.m_min[i]*4)/5;
            }
            printf("data of out rangle!\r\n");
            return;
        }
        
        if(dcal.m_max[i] < mdata[i])
        {
            dcal.m_max[i] = mdata[i];
        }
            
        if(dcal.m_min[i] > mdata[i])
        {
            dcal.m_min[i] = mdata[i];
        }
        
        dcal.mo[i] = (dcal.m_max[i] + dcal.m_min[i])/2;
    }
    
    dcal.mg[0] = 1.000;
    dcal.mg[1] = (float)(dcal.m_max[1] - dcal.m_min[1])/(float)(dcal.m_max[0] - dcal.m_min[0]);
    dcal.mg[2]  = (float)(dcal.m_max[2] - dcal.m_min[2])/(float)(dcal.m_max[0] - dcal.m_min[0]);
        
    /* constant val */
    if(dcal.mg[1] != last_gain)
    {
        if((dcal.mg[1] < 1.1) && (dcal.mg[1] > 0.9) && (dcal.mg[2] < 1.1) && (dcal.mg[2] > 0.9) && (dcal.mg[1] != last_gain))
        {
            dcal.need_update = true;
        }
        last_gain = dcal.mg[1];
    }
}

void dcal_output(struct dcal_t *dc)
{
    memcpy(dc, &dcal, sizeof(struct dcal_t));
}


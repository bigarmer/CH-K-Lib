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

#define M_MAX           (1200)
#define M_MIN           (-1200)
#define G_MAX           (900)
#define G_MIN           (-900)
#define CAL_MAGIC       (0x5ACB)

static struct dcal_t dcal;
static struct dcal_t inital_dcal;


static int is_mval_ok(int16_t data)
{
    if((data > M_MAX) || (data < M_MIN))
    {
        return 1;
    }
    return 0;
}
   
void dcal_print(struct dcal_t * dc)
{
    printf("GO:%d %d %d \r\n", dc->go[0], dc->go[1], dc->go[2]);
    printf("AO:%d %d %d \r\n", dc->ao[0], dc->ao[1], dc->ao[2]);
    printf("MO:%d %d %d \r\n", dc->mo[0], dc->mo[1], dc->mo[2]);
    printf("MG:%f %f %f \r\n",    dc->mg[0], dc->mg[1], dc->mg[2]);
    printf("MX:%d %d %d \r\n",     dc->m_max[0], dc->m_max[1], dc->m_max[2]);
    printf("MI:%d %d %d \r\n",     dc->m_min[0], dc->m_min[1], dc->m_min[2]);
}

void dcal_reset_mag(struct dcal_t *dc)
{
    int i;
    for(i=0;i<3;i++)
    {
        dc->m_min[i] = 0;
        dc->m_max[i] = 0;
        dc->mg[i] = 1.000;
        dc->mo[i] = 0;
    } 
}

void dcal_init(struct dcal_t *dc)
{
    memcpy(&dcal, dc, sizeof(struct dcal_t));
    memcpy(&inital_dcal, dc, sizeof(struct dcal_t));
}


/* this function must be called every 100 ms */
void dcal_minput(int16_t *mdata)
{
    int i;
    for(i=0;i<3;i++)
    {
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
        
}

void dcal_output(struct dcal_t *dc)
{
    dcal.magic = CAL_MAGIC;
    memcpy(dc, &dcal, sizeof(struct dcal_t));
}


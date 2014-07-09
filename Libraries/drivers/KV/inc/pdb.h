/**
  ******************************************************************************
  * @file    pdb.h
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.24
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_PDB_H__
#define __CH_LIB_PDB_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "common.h"
#include <stdint.h>

//!< PIT CallBack Type
typedef void (*PDB_CallBackType)(void);

typedef struct
{
    uint32_t                srcClock;           // 时钟源频率
    uint32_t                inputTrigSource;    // 输入输入触发源
    bool                    isContinuesMode;    //是否是连续触发模式
    uint32_t                clkDiv;             //定时时间
    uint32_t                mod;                //计数最大值
}PDB_InitTypeDef;

/*!< 中断及DMA配置 */
typedef enum
{
    kPDB_IT_CF,
    kPDB_DMA_CF,
}PDB_ITDMAConfig_Type;


//!< API functions
void PDB_Init(PDB_InitTypeDef * PDB_InitStruct);
void PDB_SoftwareTrigger(void);
void PDB_ITDMAConfig(PDB_ITDMAConfig_Type config, bool status);
void PDB_CallbackInstall(PDB_CallBackType AppCBFun);
uint32_t PDB_GetModValue(void);

#ifdef __cplusplus
}
#endif

#endif


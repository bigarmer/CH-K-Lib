#ifndef __LPTMR_H__
#define __LPTMR_H__

#include "sys.h"

//PIT ��ʼ���ṹ
typedef struct
{
//PIT_MapSelect_TypeDef  PITxMap;             //!< PIT Module and pinmux select 
    uint32_t LPTMR_PeriodInUs;                    //!< Tick Period Us
}LPTMR_InitTypeDef;


#endif


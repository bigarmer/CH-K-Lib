#ifndef _MAIN_H
#define _MAIN_H

#include "gpio.h"
#include "common.h"
#include "uart.h"

/* DSP lib */
#define ARM_MATH_CM4
#include "arm_math.h"
#include "arm_const_structs.h"

/**	���Բ���*/
/**	�����ʣ�1KHz
	fs = 1 KHz
	ts = 0.001 s
	����������100
	ns = 100
	���Σ����Ҳ�
	���ڣ�50Hz
	��λ��0
	���ֵ��2V��
	���ɷ�ʽ�� Matlab
	�ź�ά�� 1άʱ���źţ����鲿��
	n*/
float32_t test_signal[32] = {
0		,0.3090	,0.5878	,0.8090	,0.9511	,1.0000	,0.9511	,0.8090,  
0.5878	,0.3090	,0.0000	,-0.3090,-0.5878,-0.8090,-0.9511,-1.0000,
-0.9511	,-0.8090,-0.5878,-0.3090,-0.0000,0.3090	,0.5878	,0.8090,
0.9511 	,1.0000	,0.9511	,0.8090	,0.5878	,0.3090	,0.0000	,-0.3090   
};

float32_t test_input[64] = {0};
float32_t test_output[32] = {0};

#endif

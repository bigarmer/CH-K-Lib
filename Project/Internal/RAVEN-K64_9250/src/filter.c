

#include <math.h>

#include "filter.h"



#define M_PI 3.141592653f
#define ACC_LPF_CUT 10.0f		//���ٶȵ�ͨ�˲�����ֹƵ��10Hz


/**
 * @brief  һ�ڵ�ͨ�˲���ϵ������
 * @param  time: ����ʱ�� delataT
 * @param  f_cut: ��ֹƵ��
 * @retval ����ֵ
 */
float lpf_1st_factor_cal(float time, float f_cut)
{
    return time / (time + 1 / (2 * M_PI * f_cut));
}


float lpf_1st(float old_data, float new_data, float factor)
{
	return old_data * (1 - factor) + new_data * factor; 
}





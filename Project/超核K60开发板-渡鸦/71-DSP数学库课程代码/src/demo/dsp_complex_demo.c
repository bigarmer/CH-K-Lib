#include "dsp_complex_demo.h"

 /**
 * @brief  ��ʾarm_cmplx_conj_f32����
 * @code
 * @endcode
 * @param  None
 * @retval None
 * @author Rein
 * @update 2014-12-2
 */
 
void arm_cmplx_conj_f32_demo (void) {
	float32_t complex_1 [2*2],		/**����һ����������Ϊ�˱��渴����ʵ�����鲿*/
			  real_1 = 12.5,		/**Ϊ�˱�����⣬�ֱ𴴽�����������ʵ�����鲿*/
			  image_1 = 3.3,
			  real_2 = 5,
			  image_2 = 2.2,
			  complex_2 [2*2];		/**����һ�����鱣���乲�����ʵ�����鲿*/
	complex_1[0] = real_1;			/**����Ҫ����ĸ������鸳ֵ*/
	complex_1[1] = image_1;
	complex_1[2] = real_2;
	complex_1[3] = image_2;
	arm_cmplx_conj_f32(complex_1,complex_2,2);/**�����*/
	/**��ӡ��Ҫ������ĸ�������*/
	printf("real_1 = %f   image_1 = %fj\n\r",real_1,image_1);
	printf("conj_real_1 = %f   conj_image_1 = %fj\n\r",complex_2[0],complex_2[1]);
	/**��ӡ������ɵĸ�������*/
	printf("real_2 = %f   image_2 = %fj\n\r",real_2,image_2);
	printf("conj_real_2 = %f   conj_image_2 = %fj\n\r",complex_2[2],complex_2[3]);
}

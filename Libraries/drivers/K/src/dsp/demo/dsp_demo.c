#include "chlib_k.h"
#include "arm_math.h"

 /**
 * @brief  ��ʾarm_abs_f32����
 * @code
 * @endcode
 * @param  None
 * @retval None
 */
void arm_abs_f32_demo(void) {
	/**	
	*	���ȴ���һ������������A(0.0,6.0)
	*	������Կ�����һ��һ�����е�����ʽ��������ʾһ����ά����
	*	��ȻҲ����д��src_vector[1][2] = {{0.0},{6.0}}; ������ע����ʹ�ù�����ע������ָ���ά����
	*/
	float32_t src_vector[2] = {0.0f,-6.0f};	
	/**�ٴ���һ������������������������Ľ��,ϰ���Խ��ֲ�����������ʼ��Ϊ0*/
	float32_t dst_vector[2] = {0.0f,0.0f};
	/**���ڴ�ӡ��ʼ�����*/
	printf("arm_abs_f32_demo\n\r");
	printf("src_vector[0] = %f src_vector[1] = %f\n\r",src_vector[0],src_vector[1]);
	printf("dst_vector[0] = %f dst_vector[1] = %f\n\r",dst_vector[0],dst_vector[1]);
	/**������ѧ��*/
	printf("start arm_abs...\n\r");
	/**sizeof(src_vector)/sizeof(float32_t) ��ʾ�����������ռ���ֽ���Ȼ�����ÿ��Ԫ��ռ���ֽ��� ���������Ԫ�س���*/
	arm_abs_f32(src_vector,dst_vector,sizeof(src_vector)/sizeof(float32_t));
	printf("arm_abs completed\n\r");
	/**��ӡ���������бȽ�*/
	printf("dst_vector[0] = %f dst_vector[1] = %f\n\r",dst_vector[0],dst_vector[1]);
}


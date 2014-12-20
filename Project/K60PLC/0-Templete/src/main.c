#include "main.h"


#define TEST_LENGTH_SAMPLES 2048 
/* ------------------------------------------------------------------ 
* Global variables for FFT Bin Example 
* ------------------------------------------------------------------- */ 
uint32_t fftSize = 1024; 
uint32_t ifftFlag = 0; 
uint32_t doBitReverse = 1; 
 
/* Reference index at which max energy of bin ocuurs */ 
uint32_t refIndex = 213, testIndex = 0; 

extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES]; 
static float32_t testOutput[TEST_LENGTH_SAMPLES/2];  


int main(void) {
    DelayInit();
    /* ��ʼ��һ������ ����Ϊ������� */
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* ��ʼ��һ������ ʹ��UART0�˿ڵ�PTD6���ź�PTD7������Ϊ���պͷ��ͣ�Ĭ������ baud 115200 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);	

	/**��ӡ���������� Matlab��ͼ����*/
	printf("x = 1:2048\n\r");
	printf("y = [ ");
	for(int i=0;i<TEST_LENGTH_SAMPLES;i++) {
		if(i == TEST_LENGTH_SAMPLES -1)
			printf(" %f",testInput_f32_10khz[i]);
		else
			printf(" %f,",testInput_f32_10khz[i]);	
	}
	printf(" ]\n\r");
	
	arm_status status; 
	float32_t maxValue; 
	status = ARM_MATH_SUCCESS; 
    
	arm_cfft_f32(&arm_cfft_sR_f32_len1024, testInput_f32_10khz, ifftFlag, doBitReverse);   
	arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftSize);  
	arm_max_f32(testOutput, fftSize, &maxValue, &testIndex); 
	
	if(testIndex !=  refIndex) { 
		status = ARM_MATH_TEST_FAILURE; 
	} 
	
   
  if( status != ARM_MATH_SUCCESS) 
  { 
    while(1); 
  } 

  while(1);                             /* main function does not return */
}



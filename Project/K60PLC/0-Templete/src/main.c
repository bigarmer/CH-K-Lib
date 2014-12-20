#include "main.h"


#define TEST_LENGTH_SAMPLES 2048 

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
	
    while(1) {

    }
}



#include "main.h"


#define TEST_LENGTH_SAMPLES 2048 

extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES]; 
static float32_t testOutput[TEST_LENGTH_SAMPLES/2]; 
 
int main(void) {
    DelayInit();
    /* 初始化一个引脚 设置为推挽输出 */
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    /* 初始化一个串口 使用UART0端口的PTD6引脚和PTD7引脚作为接收和发送，默认设置 baud 115200 */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);	

	/**打印白噪声数据 Matlab绘图代码*/
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



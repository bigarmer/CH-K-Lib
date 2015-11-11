/*
 * ʵ������: uc/os-II �¼���־�����
 * ʵ��ƽ̨����ѻ������
 * ����оƬ��MK60DN512ZVLQ10
 * ʵ��Ч����
 *  				1.�򿪴��ڵ������� 115200 ������1234 �س�,Ȼ���ͣ����ǲ��Ƿ���password is correct, password = 1234
 *					2.���¶�ѻ�������KEY1��ť����ʱ����ӡtaskLedOn
 * ˵����  
 * 					1.����ͬʱ����  ������ȷ��1234���Ͱ���KEY1���Ż����taskLedOn����ʵ����Ե���uc/os-II���¼���־�����event flag��
 */
#include "includes.h"
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "systick.h"

#define TASK_STK_SIZE 512

OS_STK taskInitStk[TASK_STK_SIZE];
OS_STK taskLedOnStk[TASK_STK_SIZE];
OS_STK taskPasswordStk[TASK_STK_SIZE];

void taskInit(void *pdata);
void taskLedOn(void *pdata);
void taskPassword(void *pdata);
static void PTE26_EXTI_ISR(uint32_t pinxArray);

/* Global vars */
INT8U err;
OS_FLAG_GRP *EventFlag;

int main(void)
{
	/* Ӳ����ʼ�� */
	uint32_t instance;	
	instance = GPIO_QuickInit(HW_GPIOE, 26, kGPIO_Mode_IPU);
    GPIO_ITDMAConfig(HW_GPIOE, 26, kGPIO_IT_FallingEdge, true);
    GPIO_CallbackInstall(instance, PTE26_EXTI_ISR);
	UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
	
	/* ��ʼ�� uc/os-ii */ 
	OSInit();
	/* create an event flag */	
	EventFlag = OSFlagCreate(0,&err);
	OSTaskCreate(taskInit,(void *)0,&taskInitStk[TASK_STK_SIZE - 1],4);
		
	OSStart();
}

void taskLedOn(void *pdata)
{
	(void)pdata;
	
	while(1)
	{
		/* OS_FLAG_CONSUME������������ÿ�εȴ����¼�����֮�󣬻��Զ�������������Ӧ���¼���־λ */
		/* OSFlagPend�����������0λ�͵�1λ�ź�(1��Ч)ͬʱ����λ */
		OSFlagPend(EventFlag, (OS_FLAGS)3,OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME,0,&err);
		printf("taskLedOn\n");
		OSTimeDlyHMSM(0, 0, 0, 100);	
	}
}

static void PTE26_EXTI_ISR(uint32_t pinxArray)
{
	/* �ж���ֻ����OSFlagPost()��OSFlagAccept()��OSFlagQuery()���������ʵĺ��� */
	/* ��1λ��־λ��1 */
	OSFlagPost(EventFlag, (OS_FLAGS)2,OS_FLAG_SET,&err);
}

void taskPassword(void *pdata)
{
	(void)pdata;
	char str[10] = {0};
	char pwCorrect[] = "1234";
	printf("%d\n",sizeof(pwCorrect));
	uint8_t pwCheckedFlag = 0;
	while(1)
	{
		if(pwCheckedFlag == 0)
		{
			gets(str);
			if(((strncmp(str, pwCorrect,4) == 0)) && (str[4] == 0x0d))
			{
				printf("password is correct, password = %s\n",str);
				/* ��0λ��־λ��1 */
				OSFlagPost(EventFlag, (OS_FLAGS)1,OS_FLAG_SET,&err);
			}
			else
			{
				printf("password is wrong %s\n",str);
			}
				OSTimeDlyHMSM(0, 0, 0, 20);
		}
	}
}


void taskInit(void *pdata)
{
	(void)pdata;
    		
	SYSTICK_Init((1000*1000)/OS_TICKS_PER_SEC);
	SYSTICK_ITConfig(true);
	SYSTICK_Cmd(true);
	printf("enter the initialization task\n");
	
	OSTaskCreate(taskPassword,(void *)0,&taskPasswordStk[TASK_STK_SIZE - 1],6);
	OSTaskCreate(taskLedOn,(void *)0,&taskLedOnStk[TASK_STK_SIZE - 1],5);
	
	while(1)
	{		
		/* ����ǰ���� */
		OSTaskSuspend(OS_PRIO_SELF);
	}
}

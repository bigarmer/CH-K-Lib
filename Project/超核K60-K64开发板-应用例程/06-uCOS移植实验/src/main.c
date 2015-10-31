/*
 * ʵ�����ƣ�uc/os-II ��ֲʵ��
 * ʵ��ƽ̨����ѻ������
 * ����оƬ��MK60DN512ZVQ10
 * ʵ��Ч����
 *  				1.�ڴ��������������ֵ�LED1�����LED2����
 *					2.��ʾ��ǰuc/os-II�İ汾
 * 					3.ÿ����ʾCPU��ʹ����(%),��ҿ�����AppLED1Task������printf�ĸ������۲�CPUռ���ʵ����
 * �޸ļ�¼��
 *					1.2015.10.31 FreeXc ��ʱ�ӽ����ж�����OSStart()����֮�������˰汾��ʾ�Լ�CPUռ���ʵ���ʾ
 */

#include "gpio.h"
#include "common.h"
#include "systick.h"
#include "uart.h"
//uCOS
#include "includes.h"



//�����ջ��С
#define TASK_STK_SIZE              (128)
//�����������ȼ�
#define APP_START_TASK_PRIO        (4)
#define APP_LED1_TASK_PRIO         (5)
#define APP_LED0_TASK_PRIO         (7)
//���������ջ
OS_STK  APP_START_STK[TASK_STK_SIZE];
OS_STK  APP_LED1_STK[TASK_STK_SIZE];
OS_STK  APP_LED0_STK[TASK_STK_SIZE];
//LEDС������
void AppLED1Task(void *pdata)
{
    pdata = pdata; //��ֹ���������� ��ʵ������
	while(1)
	{
		printf("LED1 Task!\r\n");
		printf("LED1 Task!\r\n");
		printf("LED1 Task!\r\n");
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
}

void AppLED0Task(void *pdata)
{
    pdata = pdata; //��ֹ���������� ��ʵ������
	while(1)
	{
		printf("LED2 Task!\r\n");
		OSTimeDlyHMSM(0, 0, 0, 500);
	}
}

void TaskStart(void *pdata)
{
	pdata = pdata;
	//�����ʼ��������ʱ�ӽ��ģ���OSStart()֮������ʱ�ӽ�������Ϊͨ������£��û���ϣ���ڶ�����û�п�ʼʱ
	//�ͽ��յ�ʱ�ӽ����ж�
	SYSTICK_Init((1000*1000)/OS_TICKS_PER_SEC);
  SYSTICK_ITConfig(true);
  SYSTICK_Cmd(true);
	//��ʼ��ͳ������
	OSStatInit();
  while(1)
	{
		//delay 1 second
		OSTimeDlyHMSM(0, 0, 1, 0);
		printf("Occupancy rate = %d%% \r\n",OSCPUUsage);
		printf("%d\n",OSIdleCtrMax);
		printf("%d\n",OSIdleCtrRun);
		printf("%d",(INT8S)(100 - OSIdleCtrRun/OSIdleCtrMax));
	}		
}

int main(void)
{
		INT16U versionNum;  
		DelayInit();
    
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    
		versionNum = OSVersion();
    printf("uCOSII test��the current uc/os-II version is v%d.%d%d\r\n",versionNum/100,versionNum/10%10,versionNum%10);

    OSInit();  //OS��ʼ��
	
		OSTaskCreate(TaskStart,(void *)0,
                &APP_START_STK[TASK_STK_SIZE - 1],
                APP_START_TASK_PRIO); //create the first task

    OSTaskCreate(AppLED1Task,(void *)0,
                &APP_LED1_STK[TASK_STK_SIZE - 1],
                APP_LED1_TASK_PRIO); //����LED1 ����
    OSTaskCreate(AppLED0Task,(void *)0,
                &APP_LED0_STK[TASK_STK_SIZE - 1],
                APP_LED0_TASK_PRIO); //����LED0 ����
        
    /* ����Ȩ��������ϵͳ,��ִ��OSStart()֮�����ϵͳ��ʼ����������� */
    OSStart();
		/* ������Զ�������е��� */
}

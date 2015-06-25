#include <rtthread.h>
#include "chlib_k.h"
#include "rtt_drv.h"
#include "shell.h"

/* �߳�1 ��� */
void t1_thread_entry(void* parameter)
{
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}

/* �߳�2 ��� */
void t2_thread_entry(void* parameter)
{
    GPIO_QuickInit(HW_GPIOE, 7, kGPIO_Mode_OPP);
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 7);
        DelayMs(500);
    } 
}

/*  RTT ��ں��� �൱���޲���ϵͳ�µ� main ����
    RTT �ɹ������� ���Զ�ִ�� init_thread_entry ����߳�
*/
void init_thread_entry(void* parameter)
{

    rt_thread_t tid;
    rt_system_heap_init((void*)0x1FFF0000, (void*)(0x10000+0x1FFF0000));
    rt_system_comonent_init();
    /* �����߳� t1 ��ջ��С256 ���ȼ�0x24 ʱ��ƬΪ20ms */
    tid = rt_thread_create("t1", t1_thread_entry, RT_NULL, 256, 0x24, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
  
    /* �����߳� t2 ��ջ��С256 ���ȼ�0x24 ʱ��ƬΪ20ms*/
    tid = rt_thread_create("t2", t2_thread_entry, RT_NULL, 256, 0x24, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
   
    /* ����shell ����̨ϵͳ ��ѡ */

    rt_hw_uart_init("uart0", HW_UART0);
    rt_console_set_device("uart0");
    
    rt_kprintf("hello rt-thread!\r\n");
    
    /* ������ϵͳ���� */
    finsh_system_init();

    /* ɾ�� init_thread_entry ����߳� */
    tid = rt_thread_self();
    rt_thread_delete(tid); 
}


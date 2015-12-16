#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "cpuidy.h"
#include "pit.h"

/* CH Kinetis�̼��� V2.50 �汾 */
/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� startup_MKxxxx.c �е� CLOCK_SETUP �� */

/* UART ���ٳ�ʼ���ṹ��֧�ֵ�����* ʹ��ʱ�����Ƽ���׼��ʼ�� */
/*
 UART1_RX_PE01_TX_PE00   
 UART0_RX_PF17_TX_PF18   
 UART3_RX_PE05_TX_PE04   
 UART5_RX_PF19_TX_PF20   
 UART5_RX_PE09_TX_PE08   
 UART2_RX_PE17_TX_PE16   
 UART4_RX_PE25_TX_PE24   
 UART0_RX_PA01_TX_PA02   
 UART0_RX_PA15_TX_PA14   
 UART3_RX_PB10_TX_PB11   
 UART0_RX_PB16_TX_PB17   
 UART1_RX_PC03_TX_PC04   
 UART4_RX_PC14_TX_PC15   
 UART3_RX_PC16_TX_PC17   
 UART2_RX_PD02_TX_PD03   
 UART0_RX_PD06_TX_PD07   
 UART2_RX_PF13_TX_PF14   
 UART5_RX_PD08_TX_PD09   
*/
 
 /*
     ʵ�����ƣ�UART��ӡ��Ϣ
     ʵ��ƽ̨����˿�����
     ����оƬ��MK64DN512ZVQ10
 ʵ��Ч����ʹ�ô���UART��оƬ�ĳ�����Ϣ��оƬ�ϵ���ͳ�ȥ
        ������Ϻ󣬽���while�У�ִ��С����˸Ч��
*/
#define KEY3  PAin(7)  //����PTA�˿ڵ�7����Ϊ����
#define LED_R  PAout(9)  //����PTA�˿ڵ�9�����������
 
 /* ��������״̬ 0δ�� 1���� */
#define NO_KEY          (0x00)
#define KEY_SINGLE      (0x01)
 /* ��������ֵ */
static uint8_t gRetValue;

#define REV_IDLE        (0x01)
#define REV_CNT1 		    (0x02)
#define REV_CNT2  		  (0x03)
#define REV_CNT3  		  (0x04)
#define REV_WIDE        (0x05)
#define REV_WIDE2       (0x06)
#define REV_RATE        (0x07)
#define REV_WAIT				(0x08)

uint32_t PulseCnt = 20;
uint32_t CurrentPulseCnt = 0;
uint32_t CurrentCnt = 0;
uint32_t PulseWide = 6; //us
uint8_t PulseRate = 5; // speed rate
uint8_t CurrentLevel = 0;
uint8_t CurrentRate = 0;
uint32_t CurrentPulseWide = 0;
uint32_t old_CurrentCnt = 0;
uint8_t i = 0;
uint8_t timer_on =0;

/* ״̬���� */
typedef enum
{
    kKEY_Idle,          /*����̬ */
    kKEY_Debounce,      /*ȷ��������̬ */
    kKEY_Confirm,       /*ȷ�ϰ���״̬*/
}KEY_Status;


/* PIT0�жϷ����� */
//�˺����б�д�û��ж���Ҫ��������
static void PIT_ISR(void)
{
	CurrentCnt++; //record how low this state last
	
    //static uint32_t i;
    //�ж�������������
    //printf("enter PIT interrupt! %d\r\n", i++); 
    /* ��˸С�� */
    //GPIO_ToggleBit(HW_GPIOA, 9);
	if (CurrentLevel == 1)
	{
		PAout(4)=1;
	}
	else
	{
		PAout(4)=0;
	}
		
}

/*
static void PIT_ISR(void)
{
    static uint32_t i;
    //�ж�������������
    printf("enter PIT interrupt! %d\r\n", i++); 
    // ��˸С��
    GPIO_ToggleBit(HW_GPIOA, 9);
}
*/

static void UART_RX_ISR(uint16_t ch)
{
    static uint32_t State = REV_IDLE;
		//printf("recive");
    switch(State)
    {
        case REV_IDLE:
            if(ch == 0x55)
            {
                State = REV_CNT1;
            }
        break;
        case REV_CNT1:
            PulseCnt = ch;
            State = REV_CNT2;
        break;
				case REV_CNT2:
            PulseCnt = ch+(PulseCnt<<8);
            State = REV_CNT3;
        break;
				case REV_CNT3:
            PulseCnt = ch+(PulseCnt<<8);
            State = REV_WIDE;
        break;
				case REV_WIDE:
            PulseWide = ch;
            State = REV_WIDE2;
        break;
				case REV_WIDE2:
            PulseWide = ch+(PulseWide<<8);
            State = REV_RATE;
        break;
        case REV_RATE:
            PulseRate = ch;
            State = REV_IDLE;
				printf("Param Configuration: PulseCnt:%d  PulseWide:%d PulseRate:%d\r\n", PulseCnt, PulseWide, PulseRate);
        break;
    }

}

/* ����ɨ����� */
static void KEY_Scan(void)
{
    static KEY_Status status = kKEY_Idle;
    switch(status)
    {
        case kKEY_Idle:
            gRetValue = NO_KEY;
            if(KEY3 == 0) /* ������������� ����ȷ��������̬ */
            {
                status = kKEY_Debounce;
            }
            break;
        case kKEY_Debounce: /* ȷ��������̬ */
            if(KEY3 == 0)
            {
                status = kKEY_Confirm;
            }
            else
            {
                status = kKEY_Idle;
                gRetValue = NO_KEY;
            }
            break;
        case kKEY_Confirm: /* ȷ��̬ */
            if(KEY3 == 1) /* �����ɿ� */
            {
                gRetValue = KEY_SINGLE;
                status = kKEY_Idle;
            }
            break;
        default:
            break;
    }
}
 
 void GenPulse(uint32_t cnt, uint32_t us)
 {
	 uint8_t Rate = 0;

	
	 cnt = cnt - 2*PulseRate;
	 Rate = PulseRate;
	 

	  do
		{
			PAout(4)=1;
			DelayUs(us*Rate);
			PAout(4)=0;
			DelayUs(us*Rate);
			
			Rate--;
		
		}while(Rate > 0x00);
		
		while(cnt--)
		{
			//GPIO_WriteBit(HW_GPIOA, 9, 1);
			//GPIO_WriteBit(HW_GPIOA, 4, 1);
			PAout(4)=1;
			DelayUs(us);
			PAout(4)=0;
			DelayUs(us);
			//GPIO_WriteBit(HW_GPIOA, 9, 0);
			//GPIO_WriteBit(HW_GPIOA, 4, 0);	
		}
		
		do
		{
			Rate++;
			PAout(4)=1;
			DelayUs(us*Rate);
			PAout(4)=0;
			DelayUs(us*Rate);
		
		}while(PulseRate>Rate);

 }
 
 void CalPulse(uint32_t PulseCnt, uint32_t PulseWide)
 {
	 //uint8_t Rate = 0;
	 //uint16_t PulseWide = 0;
	/*
	 if (old_CurrentCnt == CurrentCnt)
	{
		if (CurrentLevel == 0)
		{
		CurrentLevel = 1; //high vol
		//CurrentCnt=0;
		}
		else
		{
		CurrentLevel = 0; //low vol
		//CurrentCnt= 0;
		}
	
	}
	old_CurrentCnt = CurrentCnt;
	*/
	 
	CurrentPulseWide = PulseWide * CurrentRate;
	 
	if (CurrentPulseCnt<=PulseCnt && timer_on==1) //still have pulse to sent out
	{
		if (old_CurrentCnt == CurrentCnt)// less than 1us
		{}
		else
		{
			if (CurrentCnt < CurrentPulseWide)//reach 1us and not long enough
			{}
			else//reach 1us and long enough 
			{
				if (CurrentLevel == 0)
				{
					CurrentLevel = 1; //high vol
					CurrentCnt=0;
				}
				else
				{
					CurrentLevel = 0; //low vol
					CurrentCnt= 0;
					
					if(CurrentRate>1 && CurrentPulseCnt<=(PulseRate)) //start up process 
					{
						CurrentRate--;//reach 1us and long enough than move to next pulse
						CurrentPulseCnt++;
					}
					else 
					{
						if (CurrentPulseCnt>=(PulseCnt-PulseRate) && CurrentPulseCnt<=PulseCnt) // slow down process
						{
							CurrentRate++;
							CurrentPulseCnt++;
						}
						else
						{
							CurrentRate = 1;//normal running process
							CurrentPulseCnt++;
						}
					}
				}
				
			}
			
		}
		
	}
	else
	{
		timer_on = 0;
		PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);
	}
	old_CurrentCnt = CurrentCnt;

 }

 
int main(void)
{
    uint32_t clock;
    DelayInit();
	
    GPIO_QuickInit(HW_GPIOA, 9, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 4, kGPIO_Mode_OPP);
    GPIO_QuickInit(HW_GPIOA, 5, kGPIO_Mode_OPP);
    //GPIO_QuickInit(PULSE_PORT, PULSE_PIN, kGPIO_Mode_OPP);
    /* ��GPIO����Ϊ����ģʽ оƬ�ڲ��Զ������������� */
    GPIO_QuickInit(HW_GPIOA, 7, kGPIO_Mode_IPU);
    /* ����Ϊ��� */
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);  
    /*  ����UART �ж����� �򿪽����ж� ��װ�жϻص����� */
    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR);
    /* �򿪴��ڽ����жϹ��� IT �����жϵ���˼*/
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
	
		/* ��ʼ��PITģ�� */
    PIT_InitTypeDef PIT_InitStruct1;  //����ṹ�����
    PIT_InitStruct1.chl = HW_PIT_CH0; /* ʹ��0�Ŷ�ʱ�� */
    PIT_InitStruct1.timeInUs = 1; /* ��ʱ����1uS */
    PIT_Init(&PIT_InitStruct1); //pitģ���ʼ��
    /* ע��PIT �жϻص����� */
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR); //0�Ŷ�ʱ�����жϴ���

		
    /* ��ӡʱ��Ƶ�� */
    clock = GetClock(kCoreClock);
    printf("core clock:%dHz\r\n", clock);
    clock = GetClock(kBusClock);
    printf("bus clock:%dHz\r\n", clock);
    GPIO_WriteBit(HW_GPIOA, 5, 0);
	
    CurrentRate = PulseRate;
    while(1)
    {
        KEY_Scan(); //���ð���ɨ�����  
        /* ��˸С�� */
        //GPIO_ToggleBit(HW_GPIOA, 9);
        if(gRetValue == KEY_SINGLE) //�������£�С�Ʒ���
        {
            LED_R = !LED_R;
            /* ����PIT0��ʱ���ж� */
            PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);
            timer_on = 1;
            CurrentPulseCnt = 0;
            CurrentRate = PulseRate;

            //GenPulse(PulseCnt, PulseWide);  
        }
            CalPulse(PulseCnt, PulseWide);  
				
    }
}



#include "common.h"
#include "gpio.h"

/* gloabl vars */
static GPIO_Type * const GPIOCLKTbl[] = GPIO_BASES;
static PORT_Type * const PORT_IPTbl[] = PORT_BASES;

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTA_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTB_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTC_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTD_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTE_MASK, SIM_SCGC5_PORTE_SHIFT},
};

#if defined(MKL27Z4)
static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)PORTA_IRQn,
    (IRQn_Type)0,
    (IRQn_Type)PORTCD_IRQn,
    (IRQn_Type)PORTCD_IRQn,
};
#else
static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)(PORTA_IRQn+0),
    (IRQn_Type)(PORTA_IRQn+1),
    (IRQn_Type)(PORTA_IRQn+2),
    (IRQn_Type)(PORTA_IRQn+3),
    (IRQn_Type)(PORTA_IRQn+4),
};
#endif


//! @defgroup CHKinetis
//! @{


//! @defgroup GPIO
//! @brief GPIO API functions
//! @{


 /**
 * @brief  ��������Ϊ���뻹���������  �û�һ�㲻�ص���
 * @note   ֻ�е�������ΪGPIOʱ��������
 * @code
 *      // ��PORTB�˿ڵ�3����������������
 *      GPIO_PinConfig(HW_GPIOB, 3, kInpput);
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @param  pinIndex  :�˿��ϵ����ź� 0~31
 * @param  mode      :��������������
 *         @arg kInpput  :���빦��ѡ��
 *         @arg kOutput  :�������ѡ��
 * @retval None
 */
void GPIO_SetPinDir(uint32_t instance, uint32_t pin, uint32_t dir)
{
    CLK_EN(CLKTbl, instance);
    (dir == 1) ? (GPIOCLKTbl[instance]->PDDR |= (1 << pin)):
    (GPIOCLKTbl[instance]->PDDR &= ~(1 << pin));
}

 /**
 * @brief  ���ٳ�ʼ��һ��GPIO���� ʵ������GPIO_Init���������
 * @code
 *      //��ʼ������PORTB�˿ڵ�10����Ϊ�����������
 *      GPIO_QuickInit(HW_GPIOB, 10, kGPIO_Mode_OPP);
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @param  pinIndex  :�˿��ϵ����ź� 0~31
 * @param  mode  :���Ź���ģʽ
 *         @arg kGPIO_Mode_IFT :��������
 *         @arg kGPIO_Mode_IPD :��������
 *         @arg kGPIO_Mode_IPU :��������
 *         @arg kGPIO_Mode_OOD :��©���
 *         @arg kGPIO_Mode_OPP :�������
 * @retval None
 */
uint32_t GPIO_Init(uint32_t instance, uint32_t pin, GPIO_Mode_t mode)
{
    int i;
    if(pin < 32)
    {
        switch(mode)
        {
            case kGPIO_Mode_IFT:
                SetPinPull(instance, pin, 0xFF);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_IPD:
                SetPinPull(instance, pin, 0);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_IPU:
                SetPinPull(instance, pin, 1);
                GPIO_SetPinDir(instance, pin, 0);
                break;
            case kGPIO_Mode_OPP:
                SetPinPull(instance, pin, 0xFF);
                GPIO_SetPinDir(instance, pin, 1);
                break;
            default:
                break;					
        }
        /* config pinMux */
        SetPinMux(instance, pin, 1);
    }
    else
    {
        for(i=0; i<31; i++)
        {
            if(pin & (1<<i))
            {
                switch(mode)
                {
                    case kGPIO_Mode_IFT:
                        SetPinPull(instance, i, 0xFF);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_IPD:
                        SetPinPull(instance, i, 0);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_IPU:
                        SetPinPull(instance, i, 1);
                        GPIO_SetPinDir(instance, i, 0);
                        break;
                    case kGPIO_Mode_OPP:
                        SetPinPull(instance, i, 0xFF);
                        GPIO_SetPinDir(instance, i, 1);
                        break;
                    default:
                        break;					
                }
            }
        SetPinMux(instance, i, 1);
        }
    }
    return instance;
}


 /**
 * @brief  ����ָ����������ߵ�ƽ���ߵ͵�ƽ
 * @note   �������������ó��������
 * @code
 *      //����PORTB�˿ڵ�10��������ߵ�ƽ
 *      GPIO_WriteBit(HW_GPIOB, 10, 1);
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @param  pinIndex  :�˿��ϵ����ź� 0~31
 * @param  data   : ���ŵĵ�ƽ״̬  
 *         @arg 0 : �͵�ƽ 
 *         @arg 1 : �ߵ�ƽ
 * @retval None
 */
void GPIO_PinWrite(uint32_t instance, uint32_t pin, uint8_t data)
{
    (data) ? (GPIOCLKTbl[instance]->PSOR |= (1 << pin)):
    (GPIOCLKTbl[instance]->PCOR |= (1 << pin));
}
 /**
 * @brief  ��ȡһ�������ϵĵ�ƽ״̬
 * @code
 *      //��ȡPORTB�˿ڵ�10���ŵĵ�ƽ״̬
 *      uint8_t status ; //���ڴ洢���ŵ�״̬
 *      status = GPIO_ReadBit(HW_GPIOB, 10); //��ȡ���ŵ�״̬���洢��status��
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @param  pinIndex  :�˿��ϵ����ź� 0~31
 * @retval 
 *         @arg 0 : �͵�ƽ
 *         @arg 1 : �ߵ�ƽ
 */
uint32_t GPIO_PinRead(uint32_t instance, uint32_t pin)
{
    return ((GPIOCLKTbl[instance]->PDIR >> pin) & 0x01);
}

 /**
 * @brief  ��תһ�����ŵĵ�ƽ״̬
 * @code
 *      //��תPORTB�˿ڵ�10���ŵĵ�ƽ״̬
 *      GPIO_ToggleBit(HW_GPIOB, 10); 
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @param  pinIndex  :�˿��ϵ����ź� 0~31
 * @retval None
 */
void GPIO_PinToggle(uint32_t instance, uint8_t pin)
{
    GPIOCLKTbl[instance]->PTOR |= (1 << pin);
}

/**
 * @brief  ��ȡһ���˿�32λ������
 * @code
 *      //��ȡPORTB�˿ڵ��������ŵĵ�ƽ״̬
 *      uint32_t status ; //���ڴ洢���ŵ�״̬
 *      status = GPIO_ReadPort(HW_GPIOB); //��ȡ���ŵ�״̬���洢��status��
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @retval �˿ڵ�32λ����
 */
uint32_t GPIO_GetPort(uint32_t instance)
{
    return (GPIOCLKTbl[instance]->PDIR);
}
/**
 * @brief  ��һ���˿�д��32λ����
 * @code
 *      //��PORTB�˿�д��0xFFFFFFFF
 *      GPIO_WriteByte(HW_GPIOB, 0xFFFFFFFF); 
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @param  data  :32λ����
 * @retval None
 */
void GPIO_SendPort(uint32_t instance, uint32_t data)
{
    GPIOCLKTbl[instance]->PDOR = data;
}

/**
 * @brief  ����GPIO�����ж����ͻ���DMA����
 * @retval None
 */
int GPIO_IntConfig(uint32_t instance, uint32_t pin, GPIO_Int_t config)
{
    CLK_EN(CLKTbl, instance);
    if(!config)
    {
        if(GPIO_IrqTbl[instance] == 0)
        {
            return 1;
        }
        NVIC_DisableIRQ(GPIO_IrqTbl[instance]);
    }
    PORT_IPTbl[instance]->PCR[pin] &= ~PORT_PCR_IRQC_MASK;
    switch(config)
    {
        case kGPIO_Int_RE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_Int_FE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_Int_EE:
            PORT_IPTbl[instance]->PCR[pin] |= PORT_PCR_IRQC(3);
            break;
    }
    return 0;
}

//void GPIO_ITDMAConfig(uint32_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config, bool status)
//{
//    CLK_EN(CLKTbl, instance);
////    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_IRQC_MASK;
//    
//    if(!status)
//    {
//        NVIC_DisableIRQ(GPIO_IrqTbl[instance]);
//        return;
//    }
//    
//    switch(config)
//    {
//        case kGPIO_DMA_RisingEdge:
//           // PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(1);
//            break;
//        case kGPIO_DMA_FallingEdge:
//          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(2);
//            break;
//        case kGPIO_DMA_RisingFallingEdge:
//          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(3);
//            break;
//        case kGPIO_IT_Low:
//          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(8);
//            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
//            break;
//        case kGPIO_IT_RisingEdge:
//         //   PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(9);
//            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
//            break;
//        case kGPIO_IT_FallingEdge:
//        //    PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(10);
//            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
//            break;
//        case kGPIO_IT_RisingFallingEdge:
//         //   PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(11);
//            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
//            break;
//        case kGPIO_IT_High:
//        //    PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(12);
//            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
//            break;
//        default:
//            break;
//    }
//}


//! @}

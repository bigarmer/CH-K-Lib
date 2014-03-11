/**
  ******************************************************************************
  * @file    gpio.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   CH KinetisLib: http://github.com/yandld   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "gpio.h"


//!< Leagacy Support for Kineis Z Version(Inital Version)
#if (!defined(GPIO_BASES))

    #if (defined(MK60DZ10))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #endif
    #if (defined(MK10D5))
        #define GPIO_BASES {PTA, PTB, PTC, PTD, PTE}
        #define PORT_BASES {PORTA, PORTB, PORTC, PORTD, PORTE}
    #endif
#endif

//!< Gloabl Const Table Defination
static GPIO_Type * const GPIO_InstanceTable[] = GPIO_BASES;
static PORT_Type * const PORT_InstanceTable[] = PORT_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(PORT_InstanceTable)] = {NULL};
static const uint32_t SIM_GPIOClockGateTable[] =
{
    SIM_SCGC5_PORTA_MASK,
    SIM_SCGC5_PORTB_MASK,
    SIM_SCGC5_PORTC_MASK,
    SIM_SCGC5_PORTD_MASK,
    SIM_SCGC5_PORTE_MASK,
};
static const IRQn_Type GPIO_IRQnTable[] = 
{
    PORTA_IRQn,
    PORTB_IRQn,
    PORTC_IRQn,
    PORTD_IRQn,
    PORTE_IRQn,
};

//! @defgroup CHKinetis
//! @{


//! @defgroup GPIO
//! @brief GPIO API functions
//! @{

 /**
 * @brief  �������Ÿ��ù��� ��������ᱻ�ܶ���������ģ�������������
 * 
 * @code
 *      // ��һ��IO�ڸ��ù�������ΪMux1
 *      PORT_PinMuxConfig(HW_GPIOA, 3, kPinAlt1);
 * @endcode
 * @param  instance: GPIOģ��� ��ѡ����
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: ���ź�0-31
 * @param  pinMux:   ���ù���ѡ�� ��ͬ�ĸ���ֵ����ͬ�Ĺ��� ���� kPinAlt1 һ���������GPIO
 *         @arg kPinAlt0
 *         @arg ...
 *         @arg kPinAlt7
 * @retval None
 */
void PORT_PinMuxConfig(uint8_t instance, uint8_t pinIndex, PORT_PinMux_Type pinMux)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~(PORT_PCR_MUX_MASK);
    PORT_InstanceTable[instance]->PCR[pinIndex] |=  PORT_PCR_MUX(pinMux);
}
 /**
 * @brief  ����һ�����ŵ����� ������������ ����ѡ�� �� �û�һ�㲻�ص���
 *
 * @code
 *      // ��һ����������Ϊ ������������ ��©�������
 *      PORT_PinMuxConfig(HW_GPIOA, 3, kPullUp, ENABLE);
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31 ����
 * @param  pull: ģʽѡ��
 *         @arg kPullDisabled:��ֹ������
 *         @arg kPullUp: ����
 *         @arg kPullDown:����
 * @param  newState: �Ƿ�����©���ģʽ ���ѡ��ֻ��Kinetis Kϵ������Ч��.
 * @retval None
 */
void PORT_PinPullConfig(uint8_t instance, uint8_t pinIndex, PORT_Pull_Type pull)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    switch(pull)
    {
        case kPullDisabled:
            PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_PE_MASK;
            break;
        case kPullUp:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PS_MASK;
            break;
        case kPullDown:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_PE_MASK;
            PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_PS_MASK;
            break;
        default:
            break;
    }
}

void PORT_PinOpenDrainConfig(uint8_t instance, uint8_t pinIndex, FunctionalState newState)
{
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (newState == ENABLE) ? (PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_ODE_MASK):(PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_ODE_MASK);
}

 /**
 * @brief  config ��������Ϊ���뻹����� ֻ�е�������ΪGPIOʱ���������
 * @param  instance: GPIO ģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31���ź�
 * @param  mode: ����������
 *         @arg kInpput
 *         @arg kOutput
 * @retval None
 */
void GPIO_PinConfig(uint8_t instance, uint8_t pinIndex, GPIO_PinConfig_Type mode)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    SIM->SCGC5 |= SIM_GPIOClockGateTable[instance];
    (mode == kOutput) ? (GPIO_InstanceTable[instance]->PDDR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PDDR &= ~(1 << pinIndex));
}

 /**
 * @brief  ͨ�������ʼ���ṹ�� ��ʼ��GPIO
 *
 * @code
 *       //��ʼ��������ΪGPIO����(PTB10) ������� ����ת�����ŵĵ�ƽ
 *  GPIO_InitTypeDef GPIO_InitStruct1;
 *  GPIO_InitStruct1.instance = HW_GPIOB;
 *  GPIO_InitStruct1.mode = kGPIO_Mode_OPP; //�������
 *  GPIO_InitStruct1.pinx = 10;
 *  //��ʼ��GPIO 
 *  GPIO_Init(&GPIO_InitStruct1);
 *  while(1)
 *  {
 *      DelayMs(200); //��ʱ200m
 *      //��ת�����ŵ�ƽ
 *      GPIO_ToggleBit(HW_GPIOB, 10);
 *  }
 * @endcode
 * @param  GPIO_InitStruct: GPIO��ʼ���ṹ
 *         contains the configuration information for the specified GPIO peripheral.
 * @retval None
 */
void GPIO_Init(GPIO_InitTypeDef * GPIO_InitStruct)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(GPIO_InitStruct->instance));
    assert_param(IS_PORT_ALL_INSTANCE(GPIO_InitStruct->instance));
    assert_param(IS_GPIO_ALL_PIN(GPIO_InitStruct->pinx));
    //config state
    switch(GPIO_InitStruct->mode)
    {
        case kGPIO_Mode_IFT:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDown);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_IPU:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kInput);
            break;
        case kGPIO_Mode_OOD:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullUp);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, ENABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        case kGPIO_Mode_OPP:
            PORT_PinPullConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPullDisabled);
            PORT_PinOpenDrainConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, DISABLE);
            GPIO_PinConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kOutput);
            break;
        default:
            break;					
    }
    //config pinMux
    PORT_PinMuxConfig(GPIO_InitStruct->instance, GPIO_InitStruct->pinx, kPinAlt1);
}

 /**
 * @brief  ���ٳ�ʼ��һ��GPIO���� ʵ������GPIO_Init���������
 * @code
 *      // ��ʼ��һ��GPIO��Ϊ��� ����ʼ��Ϊ�ߵ�ƽ
 *      GPIO_QuickInit(HW_GPIOA, 3, kGPIO_Mode_OPP);
 *      GPIO_WriteBit(HW_GPIOA, 3, 1);
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31
 * @param  mode: pin type
 *         @arg kGPIO_Mode_IFT:��������
 *         @arg kGPIO_Mode_IPD:��������
 *         @arg kGPIO_Mode_IPU:��������
 *         @arg kGPIO_Mode_OOD:��©���
 *         @arg kGPIO_Mode_OPP:�������
 * @retval None
 */
uint8_t GPIO_QuickInit(uint8_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
    GPIO_InitStruct1.instance = instance;
    GPIO_InitStruct1.mode = mode;
    GPIO_InitStruct1.pinx = pinx;
    GPIO_Init(&GPIO_InitStruct1);
    return  instance;
}

 /**
 * @brief  ����ߵ�ƽ���ߵ͵�ƽ(���������Ϊ�������)
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31
 * @param  data:
 *         @arg 0 : �͵�ƽ 
 *         @arg 1 : �ߵ�ƽ
 * @retval None
 */
void GPIO_WriteBit(uint8_t instance, uint8_t pinIndex, uint8_t data)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    (data) ? (GPIO_InstanceTable[instance]->PSOR |= (1 << pinIndex)):(GPIO_InstanceTable[instance]->PCOR |= (1 << pinIndex));
}
 /**
 * @brief  ��ȡһ�������ϵĵ�ƽ״̬
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31
 * @retval 
 *         @arg 0 : �͵�ƽ
 *         @arg 1 : �ߵ�ƽ
 */
uint8_t GPIO_ReadBit(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    //!< input or output
    if(((GPIO_InstanceTable[instance]->PDDR) >> pinIndex) & 0x01)
    {
        return ((GPIO_InstanceTable[instance]->PDOR >> pinIndex) & 0x01);
    }
    else
    {
        return ((GPIO_InstanceTable[instance]->PDIR >> pinIndex) & 0x01);
    }
}
 /**
 * @brief  Toggle a GPIO single bit
 * @param  instance: GPIO instance
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param  pinIndex: 0-31
 * @retval None
 */
void GPIO_ToggleBit(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    GPIO_InstanceTable[instance]->PTOR |= (1 << pinIndex);
}


/**
 * @brief  Read 32 bit data form whole GPIO port
 * @param  instance: GPIO instance
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @retval prot data(32 bit)
 */
uint32_t GPIO_ReadByte(uint8_t instance, uint8_t pinIndex)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    return (GPIO_InstanceTable[instance]->PDIR);
}
/**
 * @brief  ��һ��GPIO�˿�д��32λ����
 * @param  instance: GPIO ģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param data: 32λ����
 * @retval None
 */
void GPIO_WriteByte(uint8_t instance, uint8_t pinIndex, uint32_t data)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    GPIO_InstanceTable[instance]->PDOR = data;
}

/**
 * @brief  ����GPIO�����ж� ���� ����DMA����
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param config: ����ģʽ
 * @param newState: ʹ�ܻ��߽�ֹ
 * @retval None
 */
void GPIO_ITDMAConfig(uint8_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    assert_param(IS_GPIO_ALL_PIN(pinIndex));
    
    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_IRQC_MASK;
    switch(config)
    {
        case kGPIO_IT_Disable:
            NVIC_DisableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_DMA_Disable:
            break;
        case kGPIO_DMA_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_DMA_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_DMA_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(3);
            break;
        case kGPIO_IT_Low:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(8);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(9);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_FallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(10);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_RisingFallingEdge:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(11);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        case kGPIO_IT_High:
            PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(12);
            NVIC_EnableIRQ(GPIO_IRQnTable[instance]);
            break;
        default:
            break;
    }
}
/**
 * @brief  ע���жϻص�����
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA
 *         @arg HW_GPIOB
 *         @arg HW_GPIOC
 *         @arg HW_GPIOD
 *         @arg HW_GPIOE
 * @param AppCBFun: �ص�����ָ��
 * @retval None
 */
void GPIO_CallbackInstall(uint8_t instance, GPIO_CallBackType AppCBFun)
{
    //param check
    assert_param(IS_GPIO_ALL_INSTANCE(instance));
    assert_param(IS_PORT_ALL_INSTANCE(instance));
    if(AppCBFun != NULL)
    {
        GPIO_CallBackTable[instance] = AppCBFun;
    }
}

//! @}



void PORTA_IRQHandler(void)
{
    uint32_t ISFR;
    //safe copy
    ISFR = PORT_InstanceTable[HW_GPIOA]->ISFR;
    //clear IT pending bit
    PORT_InstanceTable[HW_GPIOA]->ISFR = 0xFFFFFFFF;
    if(GPIO_CallBackTable[HW_GPIOA])
    {
        GPIO_CallBackTable[HW_GPIOA](ISFR);
    }	
}

void PORTB_IRQHandler(void)
{
    uint32_t ISFR;
    //safe copy
    ISFR = PORT_InstanceTable[HW_GPIOB]->ISFR;
    //clear IT pending bit
    PORT_InstanceTable[HW_GPIOB]->ISFR = 0xFFFFFFFF;
    if(GPIO_CallBackTable[HW_GPIOB])
    {
        GPIO_CallBackTable[HW_GPIOB](ISFR);
    }	
}

void PORTC_IRQHandler(void)
{
    uint32_t ISFR;
    //safe copy
    ISFR = PORT_InstanceTable[HW_GPIOC]->ISFR;
    //clear IT pending bit
    PORT_InstanceTable[HW_GPIOC]->ISFR = PORT_ISFR_ISF_MASK;
    if(GPIO_CallBackTable[HW_GPIOC])
    {
        GPIO_CallBackTable[HW_GPIOC](ISFR);
    }	
}

void PORTD_IRQHandler(void)
{
    uint32_t ISFR;
    //safe copy
    ISFR = PORT_InstanceTable[HW_GPIOD]->ISFR;
    //clear IT pending bit
    PORT_InstanceTable[HW_GPIOD]->ISFR = PORT_ISFR_ISF_MASK;
    if(GPIO_CallBackTable[HW_GPIOD])
    {
        GPIO_CallBackTable[HW_GPIOD](ISFR);
    }	
}

void PORTE_IRQHandler(void)
{
    uint32_t ISFR;
    //safe copy
    ISFR = PORT_InstanceTable[HW_GPIOE]->ISFR;
    //clear IT pending bit
    PORT_InstanceTable[HW_GPIOE]->ISFR = PORT_ISFR_ISF_MASK;
    if(GPIO_CallBackTable[HW_GPIOE])
    {
        GPIO_CallBackTable[HW_GPIOE](ISFR);
    }	
}

#if (defined(MK70F12))
void PORTF_IRQHandler(void)
{
    uint32_t ISFR;
    //safe copy
    ISFR = PORT_InstanceTable[HW_GPIOF]->ISFR;
    //clear IT pending bit
    PORT_InstanceTable[HW_GPIOF]->ISFR = PORT_ISFR_ISF_MASK;
    if(GPIO_CallBackTable[HW_GPIOF])
    {
        GPIO_CallBackTable[HW_GPIOF](ISFR);
    }
}
#endif


//! @}

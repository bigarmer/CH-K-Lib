#include "common.h"
#include "gpio.h"

/* gloabl vars */
static GPIO_Type * const GPIOCLKTbl[] = GPIO_BASES;
static GPIO_CallBackType GPIO_CallBackTable[ARRAY_SIZE(GPIOCLKTbl)] = {NULL};

static const Reg_t CLKTbl[] =
{
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTA_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTB_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTC_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTD_SHIFT},
    {(void*)&(SIM->SCGC5), SIM_SCGC5_PORTE_MASK, SIM_SCGC5_PORTE_SHIFT},
};

static const IRQn_Type GPIO_IrqTbl[] = 
{
    (IRQn_Type)(PORTA_IRQn+0),
    (IRQn_Type)(PORTA_IRQn+1),
    (IRQn_Type)(PORTA_IRQn+2),
    (IRQn_Type)(PORTA_IRQn+3),
    (IRQn_Type)(PORTA_IRQn+4),
};



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
 * @brief  GPIO��ʼ������
 * @code
 *    //��ʼ������PORTB�˿ڵ�10����Ϊ�����������
 *    GPIO_InitTypeDef GPIO_InitStruct1;      //����һ���ṹ����
 *    GPIO_InitStruct1.instance = HW_GPIOB;   //ѡ��PORTB�˿�
 *    GPIO_InitStruct1.mode = kGPIO_Mode_OPP; //�������
 *    GPIO_InitStruct1.pinx = 10;             //ѡ��10����
 *    //���ó�ʼ��GPIO���� 
 *    GPIO_Init(&GPIO_InitStruct1);
 * @endcode
 * @param  GPIO_InitStruct: GPIO��ʼ���ṹ�壬����������״̬����  
           GPIO_InitStruct.instance   :�˿ں� HW_GPIOA ~ HW_GPIOE
 * @retval None
 */
void GPIO_Init(GPIO_InitTypeDef * Init)
{
    /* config state */
    switch(Init->mode)
    {
        case kGPIO_Mode_IFT:
            SetPinPull(Init->instance, Init->pinx, 0xFF);
            GPIO_SetPinDir(Init->instance, Init->pinx, 0);
            break;
        case kGPIO_Mode_IPD:
            SetPinPull(Init->instance, Init->pinx, 0);
            GPIO_SetPinDir(Init->instance, Init->pinx, 0);
            break;
        case kGPIO_Mode_IPU:
            SetPinPull(Init->instance, Init->pinx, 1);
            GPIO_SetPinDir(Init->instance, Init->pinx, 0);
            break;
        case kGPIO_Mode_OPP:
            SetPinPull(Init->instance, Init->pinx, 0xFF);
            GPIO_SetPinDir(Init->instance, Init->pinx, 1);
            break;
        default:
            break;					
    }
    /* config pinMux */
    SetPinMux(Init->instance, Init->pinx, kPinAlt1);
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
uint8_t GPIO_QuickInit(uint32_t instance, uint32_t pinx, GPIO_Mode_Type mode)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
    GPIO_InitStruct1.instance = instance;
    GPIO_InitStruct1.mode = mode;
    GPIO_InitStruct1.pinx = pinx;
    GPIO_Init(&GPIO_InitStruct1);
    return  instance;
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
void GPIO_WriteBit(uint32_t instance, uint8_t pinIndex, uint8_t data)
{
    (data) ? (GPIOCLKTbl[instance]->PSOR |= (1 << pinIndex)):(GPIOCLKTbl[instance]->PCOR |= (1 << pinIndex));
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
uint8_t GPIO_ReadBit(uint32_t instance, uint8_t pinIndex)
{
    /* input or output */
    if(((GPIOCLKTbl[instance]->PDDR) >> pinIndex) & 0x01)
    {
        return ((GPIOCLKTbl[instance]->PDOR >> pinIndex) & 0x01);
    }
    else
    {
        return ((GPIOCLKTbl[instance]->PDIR >> pinIndex) & 0x01);
    }
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
void GPIO_ToggleBit(uint32_t instance, uint8_t pinIndex)
{
    GPIOCLKTbl[instance]->PTOR |= (1 << pinIndex);
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
uint32_t GPIO_Get(uint32_t instance)
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
void GPIO_Send(uint32_t instance, uint32_t data)
{
    GPIOCLKTbl[instance]->PDOR = data;
}

/**
 * @brief  ����GPIO�����ж����ͻ���DMA����
 * @code
 *      //����PORTB�˿ڵ�10����Ϊ�½��ش����ж�
 *      GPIO_ITDMAConfig(HW_GPIOB, 10, kGPIO_IT_FallingEdge, true); 
 * @endcode
 * @param  instance: GPIOģ���
 *         @arg HW_GPIOA :оƬ��PORTA�˿�
 *         @arg HW_GPIOB :оƬ��PORTB�˿�
 *         @arg HW_GPIOC :оƬ��PORTC�˿�
 *         @arg HW_GPIOD :оƬ��PORTD�˿�
 *         @arg HW_GPIOE :оƬ��PORTE�˿�
 * @param  pinIndex  :�˿��ϵ����ź� 0~31
 * @param config: ����ģʽ
 *         @arg kGPIO_DMA_RisingEdge DMA�����ش���
 *         @arg kGPIO_DMA_FallingEdge DMA�½��ش���
 *         @arg kGPIO_DMA_RisingFallingEdge DMA�������½��ض�����
 *         @arg kGPIO_IT_Low �͵�ƽ�����ж�
 *         @arg kGPIO_IT_RisingEdge �����ش����ж�
 *         @arg kGPIO_IT_FallingEdge �½��ش����ж�
 *         @arg kGPIO_IT_RisingFallingEdge �������½��ض������ж�
 *         @arg kGPIO_IT_High �ߵ�ƽ�����ж�
 * @retval None
 */
void GPIO_ITDMAConfig(uint32_t instance, uint8_t pinIndex, GPIO_ITDMAConfig_Type config, bool status)
{
    CLK_EN(CLKTbl, instance);
//    PORT_InstanceTable[instance]->PCR[pinIndex] &= ~PORT_PCR_IRQC_MASK;
    
    if(!status)
    {
        NVIC_DisableIRQ(GPIO_IrqTbl[instance]);
        return;
    }
    
    switch(config)
    {
        case kGPIO_DMA_RisingEdge:
           // PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(1);
            break;
        case kGPIO_DMA_FallingEdge:
          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(2);
            break;
        case kGPIO_DMA_RisingFallingEdge:
          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(3);
            break;
        case kGPIO_IT_Low:
          //  PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(8);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_RisingEdge:
         //   PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(9);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_FallingEdge:
        //    PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(10);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_RisingFallingEdge:
         //   PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(11);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        case kGPIO_IT_High:
        //    PORT_InstanceTable[instance]->PCR[pinIndex] |= PORT_PCR_IRQC(12);
            NVIC_EnableIRQ(GPIO_IrqTbl[instance]);
            break;
        default:
            break;
    }
}
/**
 * @brief  ע���жϻص�����
 * @param  instance: GPIOģ���ж���ں�
 *         @arg HW_GPIOA :оƬ��PORTA�˿��ж����
 *         @arg HW_GPIOB :оƬ��PORTB�˿��ж����
 *         @arg HW_GPIOC :оƬ��PORTC�˿��ж����
 *         @arg HW_GPIOD :оƬ��PORTD�˿��ж����
 *         @arg HW_GPIOE :оƬ��PORTE�˿��ж����
 * @param AppCBFun: �ص�����ָ�����
 * @retval None
 * @note ���ڴ˺����ľ���Ӧ�������Ӧ��ʵ��
 */
void GPIO_CallbackInstall(uint32_t instance, GPIO_CallBackType AppCBFun)
{
    CLK_EN(CLKTbl, instance);
    if(AppCBFun != NULL)
    {
        GPIO_CallBackTable[instance] = AppCBFun;
    }
}

//! @}


void PORTA_IRQHandler(void)
{
    uint32_t ISFR;
    /* safe copy */
   // ISFR = PORT_InstanceTable[HW_GPIOA]->ISFR;
    /* clear IT pending bit */
  //  PORT_Inst/ceTable[HW_GPIOA]->ISFR = 0xFFFFFFFF;
    if(GPIO_CallBackTable[HW_GPIOA])
    {
        GPIO_CallBackTable[HW_GPIOA](ISFR);
    }	
}




//! @}

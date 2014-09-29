#include "usb.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "message_manage.h"

//BDT��������������ֻҪ����512�ֽڣ��������ڻ�������ַ�ĵ�9λ����Ϊ0����ȻUSBģ���޷�ʹ�ã�
//��������1024�����ҳ���ַ��9λΪ0�Ļ�������ַ��Ϊ�׵�ַ��
#ifdef __ICCARM__ 
#pragma data_alignment = 512
tBDT tBDTtable[16];
#endif

#ifdef __CC_ARM
__align(512) tBDT tBDTtable[16];													//�ڲ�SRAM�ڴ��
#endif


//�����˵�����ݻ�����
uint8_t guint8_tEP0_OUT_ODD_Buffer[EP0_SIZE];
uint8_t guint8_tEP0_OUT_EVEN_Buffer[EP0_SIZE];
uint8_t guint8_tEP0_IN_ODD_Buffer[EP0_SIZE];
uint8_t guint8_tEP0_IN_EVEN_Buffer[EP0_SIZE];
uint8_t guint8_tEP1_OUT_ODD_Buffer[EP1_SIZE];
uint8_t guint8_tEP1_OUT_EVEN_Buffer[EP1_SIZE];
uint8_t guint8_tEP1_IN_ODD_Buffer[EP1_SIZE];
uint8_t guint8_tEP1_IN_EVEN_Buffer[EP1_SIZE];
uint8_t guint8_tEP2_OUT_ODD_Buffer[EP2_SIZE];
uint8_t guint8_tEP2_OUT_EVEN_Buffer[EP2_SIZE];
uint8_t guint8_tEP2_IN_ODD_Buffer[EP2_SIZE];
uint8_t guint8_tEP2_IN_EVEN_Buffer[EP2_SIZE];
uint8_t guint8_tEP3_OUT_ODD_Buffer[EP3_SIZE];
uint8_t guint8_tEP3_OUT_EVEN_Buffer[EP3_SIZE];
uint8_t guint8_tEP3_IN_ODD_Buffer[EP3_SIZE];
uint8_t guint8_tEP3_IN_EVEN_Buffer[EP3_SIZE];
//ָ������������ĵ�ַָ��
uint8_t *BufferPointer[]=
{
    guint8_tEP0_OUT_ODD_Buffer,
    guint8_tEP0_OUT_EVEN_Buffer,
    guint8_tEP0_IN_ODD_Buffer,
    guint8_tEP0_IN_EVEN_Buffer,
    guint8_tEP1_OUT_ODD_Buffer,
    guint8_tEP1_OUT_EVEN_Buffer,
    guint8_tEP1_IN_ODD_Buffer,
    guint8_tEP1_IN_EVEN_Buffer,
    guint8_tEP2_OUT_ODD_Buffer,
    guint8_tEP2_OUT_EVEN_Buffer,
    guint8_tEP2_IN_ODD_Buffer,
    guint8_tEP2_IN_EVEN_Buffer,
    guint8_tEP3_OUT_ODD_Buffer,
    guint8_tEP3_OUT_EVEN_Buffer,
    guint8_tEP3_IN_ODD_Buffer,
    guint8_tEP3_IN_EVEN_Buffer
};
//ÿ����������С
const uint8_t cEP_Size[]=
{
    EP0_SIZE,    
    EP0_SIZE,    
    EP0_SIZE,    
    EP0_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP3_SIZE,
    EP3_SIZE,
    EP3_SIZE,
    EP3_SIZE
};
//StringDesc������ָ��
const uint8_t* String_Table[4]=
{
    String_Descriptor0,
    String_Descriptor1,
    String_Descriptor2,
    String_Descriptor3
};
//SETUP������ֻ�ܸ�DATA0
//�˵�IN(2)״̬
uint8_t vEP2State = kUDATA1;
//�˵�OUT(3)״̬
uint8_t vEP3State = kUDATA0;

//USBģ���ڲ�ȫ�ֱ���
uint8_t guint8_tUSBClearFlags;            //�ڲ�ʹ��
uint8_t *puint8_tIN_DataPointer;          //�ڲ�ʹ��
uint8_t guint8_tIN_Counter;               //�ڲ�ʹ��
uint8_t guint8_tUSB_Toogle_flags;         //�ڲ�ʹ��
uint8_t guint8_tUSB_State;                //����USB�˵�0��״̬
tUSB_Setup *Setup_Pkt;             //ָ��˵�0OUT�����׵�ַ


/***********************************************************************************************
 ���ܣ��ȴ�USBģ�鱻ö�ٳɹ� 
 �βΣ�0          
 ���أ�0
 ��⣺����ʼ��USB�豸�󣬵��ô˺�����������һֱ�ȴ� USBö�ٳɹ�
************************************************************************************************/
void USB_WaitDeviceEnumed(void)
{
  while(guint8_tUSB_State != uENUMERATED);//�ȴ�USB�豸��ö��
}
/***********************************************************************************************
 ���ܣ��ж�USBģ��ʱ�򱻳ɹ�ö��
 �βΣ�0          
 ���أ�0 ö���Ѿ��ɹ�  1 ö��ʧ�� ���� ��δö�����
 ��⣺
************************************************************************************************/
uint8_t USB_IsDeviceEnumed(void)
{
	if(guint8_tUSB_State == uENUMERATED) 
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
/***********************************************************************************************
 ���ܣ��˵������жϷ������ݺ���
 �βΣ�uint8_tEP: �˵��
       puint8_tDataPointer: �������ݻ�����
       uint8_tDataSize: ��Ҫ���͵����ݳ���
 ���أ�0 
 ��⣺
*   �˵�IN���ͣ��ڲ�ͬUSBģʽ��IN�ĺ��岻ͬ
*     ��USB����ģʽ�£�USB host��  IN��ʾ  USB Host���� USB Device�����ݰ�
*     ��USB�豸ģʽ�£�USB Device��IN��ʾ  USB Host�� USB Device�������ݰ�
*   ��ǰ��USB Deviceģʽ�£��ú�����������Device�豸�������ݰ�
************************************************************************************************/
void USB_EP_IN_Transfer(uint8_t uint8_tEP,uint8_t *puint8_tDataPointer,uint8_t uint8_tDataSize)
{
  uint8_t *puint8_tEPBuffer;
  uint8_t uint8_tEPSize;     //�˵�����ݳ���
  uint16_t uint16_tLenght=0;    
  uint8_t uint8_tEndPointFlag;    
  /*������ǰ��������λ��*/
  uint8_tEndPointFlag=uint8_tEP;
  if(uint8_tEP)   //������Ƕ˵�0,
    uint8_tEP=(uint8_t)(uint8_tEP<<2);
  uint8_tEP+=2; //EPֵ��2
  puint8_tEPBuffer=BufferPointer[uint8_tEP];   //����EP��BUFFER��ַ��puint8_tEPBuffer
  
  if(BIT_CHK(fIN,guint8_tUSBClearFlags)) //���guint8_tUSBClearFlags = 1
  {
      puint8_tIN_DataPointer = puint8_tDataPointer; //���û����ݴ洢�����׵�ַ��puint8_tIN_DataPointer
      guint8_tIN_Counter = uint8_tDataSize;         //���û����ݳ��ȸ�guint8_tIN_Counter

      uint16_tLenght=(Setup_Pkt->wLength_h<<8)+Setup_Pkt->wLength_l ;//��setup���ݵĳ��ȸ�uint16_tLenght
      if((uint16_tLenght < uint8_tDataSize) && (uint8_tEP==2)) //��������͵����ݳ��ȴ���setup���趨�ĳ��� ͬʱ �˵�ֵ == 2
      {
          guint8_tIN_Counter=Setup_Pkt->wLength_l; //ֻ����setup�еĵ�8λ����
      }
  }
  /*��鷢�ͳ���*/
  if(guint8_tIN_Counter > cEP_Size[uint8_tEP]) //����������ݰ��ĳ��� ����32�ֽ�ʱ
  {
      uint8_tEPSize = cEP_Size[uint8_tEP];     //����ʱ�˵�ĳ��������ڶ˵��Ĭ�ϳ���
      guint8_tIN_Counter-=cEP_Size[uint8_tEP]; //�����ݰ��ĳ��ȼ���EP_Size
      BIT_CLR(fIN,guint8_tUSBClearFlags);//��guint8_tUSBClearFlags����
  }
  else
  { 
      uint8_tEPSize = guint8_tIN_Counter;      //���С��
      guint8_tIN_Counter=0;            
      BIT_SET(fIN,guint8_tUSBClearFlags);//��guint8_tUSBClearFlags��һ
  }
  /*���û����������ֵ���Ƶ�EP ��������׼������*/
  tBDTtable[uint8_tEP].Cnt=(uint8_tEPSize);    //������Ҫ���͵����ݳ���
  while(uint8_tEPSize--)
       *puint8_tEPBuffer++=*puint8_tIN_DataPointer++; //���û������ݸ�ֵ��EP�洢��                                                        
  if(BIT_CHK(uint8_tEndPointFlag,guint8_tUSB_Toogle_flags)) //�����Ӧ�˵��guint8_tUSB_Toogle_flags == 1
  {
      tBDTtable[uint8_tEP].Stat._byte= kUDATA0;         
      BIT_CLR(uint8_tEndPointFlag,guint8_tUSB_Toogle_flags);//������Ӧ�˵��guint8_tUSB_Toogle_flags
  } 
  else
  {
      tBDTtable[uint8_tEP].Stat._byte= kUDATA1;          
      BIT_SET(uint8_tEndPointFlag,guint8_tUSB_Toogle_flags);//��λguint8_tUSB_Toogle_flags
  }
}
/***********************************************************************************************
 ���ܣ����OUT�˵���յ���������
 �βΣ�uint8_tEP: �˵��
 ���أ����յ������ݳ���
 ��⣺���OUT�˵���յ���������
************************************************************************************************/
uint16_t USB_EP_OUT_SizeCheck(uint8_t uint8_tEP)
{
  uint8_t uint8_tEPSize; 
   /* ��ȡ�������ĳ��� */
  uint8_tEPSize = tBDTtable[uint8_tEP<<2].Cnt;
  return(uint8_tEPSize & 0x03FF);
}
//=========================================================================
//������: hw_usb_ep0_stall
//��  ��: �˵�0����һ��STALL��
//��  ��: ��  
//��  ��: ��
//=========================================================================

void hw_usb_ep0_stall(void)
{
	// ����һ��STALL��  
	#if (DEBUG_PRINT == 1)
	UART_printf("����һ��stall��\r\n");
	#endif
	BIT_SET(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT);
	//ENDPT0_EP_STALL = 1;                      
	tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA0; 
	tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;       
}
/***********************************************************************************************
 ���ܣ�����SIE(K60USB���нӿ�����)��EP0 ��Ķ˵�����
 �βΣ�0
 ���أ�0
 ��⣺0
************************************************************************************************/
void USB_EnableInterface(void)
{
    // ʹ�ܶ˵�
    // �˵� Register ����
//	USB0->ENDPOINT[1].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;              
	USB0->ENDPOINT[2].ENDPT=_EP_IN  | USB_ENDPT_EPHSHK_MASK;    //ʹ��RX ����
	USB0->ENDPOINT[3].ENDPT=_EP_OUT | USB_ENDPT_EPHSHK_MASK;    //ʹ��TX ����
	//USB0->ENDPOINT[4].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;   
	//USB0->ENDPOINT[5].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;   
	//USB0->ENDPOINT[6].ENDPT=EP1_VALUE | USB_ENDPT_EPHSHK_MASK;   
	                   
  
    // ���� 1 BDT ����
    // �ѿ���Ȩ����MCU 
    //tBDTtable[bEP1IN_ODD].Stat._byte= kUDATA1;                //����DATA1���ݰ�
    //tBDTtable[bEP1IN_ODD].Cnt = 0x00;                         //���������
    //tBDTtable[bEP1IN_ODD].Addr =(uint32_t)guint8_tEP1_IN_ODD_Buffer;    //��ַָ���Ӧ�Ļ�����

    // ���� 2 BDT ����
    // �ѿ���Ȩ����MCU 
    tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA1;
    tBDTtable[bEP2IN_ODD].Cnt = 0x00;
    tBDTtable[bEP2IN_ODD].Addr =(uint32_t  )guint8_tEP2_IN_ODD_Buffer;            

    // ���� 3 BDT ����
    // �ѿ���Ȩ����MCU 
    tBDTtable[bEP3OUT_ODD].Stat._byte= kUDATA0;
    tBDTtable[bEP3OUT_ODD].Cnt = 0xFF;
    tBDTtable[bEP3OUT_ODD].Addr =(uint32_t)guint8_tEP3_OUT_ODD_Buffer;            
}

/***********************************************************************************************
 ���ܣ�USB�ж�ɢת-EP0_OUTɢת-Host��ȡ�������жϴ���
 �βΣ�0
 ���أ�0
 ��⣺0
************************************************************************************************/
void USB_GetDescHandler(void)
{
	switch((Setup_Pkt->wValue_h) & 0xFF)
	{
		case DEVICE_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			UART_printf("�豸������\r\n");
			#endif
			USB_EP_IN_Transfer(EP0,(uint8_t*)Device_Descriptor,sizeof(Device_Descriptor));//�����豸������
			break;
		case CONFIGURATION_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			UART_printf("����������\r\n");
			#endif
			USB_EP_IN_Transfer(EP0,(uint8_t*)Configuration_Descriptor,sizeof(Configuration_Descriptor)); //��������������
			break;
		case STRING_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			UART_printf("�ַ���������-%x ",Setup_Pkt->wValue_l);
			#endif
			switch(Setup_Pkt->wValue_l)  //����wValue�ĵ��ֽڣ�����ֵ��ɢת
			{
				case 0:  //��ȡ����ID
					#if (DEBUG_PRINT == 1)
					UART_printf("-����ID\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 1:  //�����ַ���������ֵΪ1����������Ϊ�����ַ���
					#if (DEBUG_PRINT == 1)
					UART_printf("-�����ַ���\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 2:  //��Ʒ�ַ���������ֵΪ2����������Ϊ��Ʒ�ַ���
					#if (DEBUG_PRINT == 1)
					UART_printf("-��Ʒ�ַ���\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 3:  //��Ʒ���кŵ�����ֵΪ3����������Ϊ���к�
					#if (DEBUG_PRINT == 1)
					UART_printf("-���к�\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				default: 
					#if (DEBUG_PRINT == 1)
					UART_printf("-λ�õ�����ֵ\r\n");
					#endif
					break; 
			}
			break;
				case REPORT_DESCRIPTOR:
					#if (DEBUG_PRINT == 1)
					UART_printf("-����������\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(uint8_t*)Report_Descriptor,sizeof(Report_Descriptor));
					break;
		default:
			#if (DEBUG_PRINT == 1)
			UART_printf("����������,����������:0x%x\r\n",Setup_Pkt->wValue_h);
			#endif
			break;
	}
}

/***********************************************************************************************
 ���ܣ�USB�ж�ɢת-EP0��������ж�
 �βΣ�0
 ���أ�0
 ��⣺��SETUP������ʹ���
		(1)ֻ�н��յ�SETUP���ŵ��øú���
		(2)SETUP����8�ֽ�����
			bmRequestType:1
			bRequest:1
			wValue.H:1 :������������
			wValue.L:1 :������������
			wIndex:2
			wLength:2
************************************************************************************************/
void USB_EP0_OUT_Handler(void)
{
	uint8_t *p =(uint8_t*)Setup_Pkt;
	#if (DEBUG_PRINT == 1)
	uint8_t i;
	#endif

	*p = *p;
	// ��DATA0 ��ʼ���䡣
	BIT_CLR(0,guint8_tUSB_Toogle_flags);
	//��ӡ���յ�������
	#if (DEBUG_PRINT == 1)
	for(i=0;i<8;i++,p++) UART_printf("0x%x ",*p); 	UART_printf("\r\n");
	#endif
	if((Setup_Pkt->bmRequestType & 0x80) == 0x80)
	{
		//����bmRequestType��D6-5λɢת��D6-5λ��ʾ���������
		//0Ϊ��׼����1Ϊ������2Ϊ��������
	  switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB��׼��������-");
				#endif
				//USBЭ�鶨���˼�����׼��������ʵ����Щ��׼�������󼴿�
				//����Ĵ�����bRequest�У��Բ�ͬ������������ɢת
				switch(Setup_Pkt->bRequest)
				{
					case mGET_CONFIG:
						#if (DEBUG_PRINT == 1)
						UART_printf("��ȡ����\r\n");
						#endif
						break;	
					case mGET_DESC:
						#if (DEBUG_PRINT == 1)
						UART_printf("��ȡ������-");
						#endif
						USB_GetDescHandler(); //ִ�л�ȡ������
						break;
					case mGET_INTF:
						#if (DEBUG_PRINT == 1)
						UART_printf("��ȡ�ӿ�\r\n");
						#endif
						break;
					case mGET_STATUS:
						#if (DEBUG_PRINT == 1)
						UART_printf("��ȡ״̬\r\n");
						#endif
						break;
					case mSYNC_FRAME:
						#if (DEBUG_PRINT == 1)
						UART_printf("ͬ��֡\r\n");
						#endif
						break;
						default:
						#if (DEBUG_PRINT == 1)
						UART_printf("����δ����ı�׼��������\r\n");
						#endif
						break;
				}
				break;
			case 1:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB����������\r\n");
				#endif
				break;
			case 2:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB������������\r\n");
				#endif
				break;
			default:
				#if (DEBUG_PRINT == 1)
				UART_printf("����δ�������������\r\n");
				#endif
				break;
		}	
	}
	else
	{
		//����bmRequestType��D6-5λɢת,D6-5λ��ʾ���������
		//0Ϊ��׼����1Ϊ������ 2Ϊ��������
		switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				#if (DEBUG_PRINT == 1)
				UART_printf("USB ��׼�������-");
				#endif
				switch(Setup_Pkt->bRequest)
				{
					case mCLR_FEATURE:
						#if (DEBUG_PRINT == 1)
						UART_printf("�������\r\n");
						#endif
						break;
					case mSET_ADDRESS:
						#if (DEBUG_PRINT == 1)
						UART_printf("���õ�ַ ��ַ:%x\r\n",Setup_Pkt->wValue_l);
						#endif
						guint8_tUSB_State=uADDRESS;
						USB_EP_IN_Transfer(EP0,0,0); //Ϊʲô������
						break;
					case mSET_CONFIG:
						#if (DEBUG_PRINT == 1)
						UART_printf("��������\r\n");
						#endif
            if(Setup_Pkt->wValue_h+Setup_Pkt->wValue_l) 
            {
                //ʹ��1 ��2 ��3 �˵� 
							USB_EnableInterface();
							USB_EP_IN_Transfer(EP0,0,0);
							guint8_tUSB_State=uENUMERATED;
            }
						break;
					case  mSET_DESC:
						#if (DEBUG_PRINT == 1)
						UART_printf("����������\r\n");
						#endif
						break;
					case mSET_INTF:
						#if (DEBUG_PRINT == 1)
						UART_printf("���ýӿ�\r\n");
						#endif
						break;
					default:
						#if (DEBUG_PRINT == 1)
						UART_printf("����δ����ı�׼�������\r\n");
						#endif
					break;
				}
				break;
				case 1:
					#if (DEBUG_PRINT == 1)
					UART_printf("USB���������-\r\n");
					#endif
				/*
					switch(Setup_Pkt->bRequest)
					{
						case 0x0A:
							USB_EP_IN_Transfer(EP0,0,0); //�ȴ�������0���ݰ�����
							UART_printf("���ÿ���\r\n");
							break;
						default:
							UART_printf("δ֪������\r\n");
							break;
					}
				*/
					break;
				case 2:
					#if (DEBUG_PRINT == 1)
					UART_printf("USB�����������r\n");
					#endif
					break;
				default:
					#if (DEBUG_PRINT == 1)
					UART_printf("����δ������������\r\n");
					#endif
				break;
		}
	}
	tBDTtable[bEP0OUT_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL); // Ϊ0 ʱ: SIE ������������
}


static int USB_SetClockDiv(uint32_t srcClock)
{
    uint8_t usbfrac_max, usbdiv_max,frac,div;
    usbfrac_max = 1;
    usbdiv_max = 7;
    
    /* clear all divivder */
    SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBDIV_MASK;
    SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBFRAC_MASK;
    
    for(frac=0;frac<usbfrac_max;frac++)
    {
        for(div=0;div<usbdiv_max;div++)
        {
            if((srcClock*(frac+1))/(div+1) == 48000000)
            {
                SIM->CLKDIV2 |= SIM_CLKDIV2_USBDIV(div);
                (frac)?(SIM->CLKDIV2 |= SIM_CLKDIV2_USBFRAC_MASK):(SIM->CLKDIV2 &= ~SIM_CLKDIV2_USBFRAC_MASK);
                LIB_TRACE("USB clock OK src:%d frac:%d div:%d 0x%08X\r\n", srcClock, frac, div, SIM->CLKDIV2);
                return 0;
            }
        }
    }
    return 1;
}

uint8_t USB_Init(void)
{
    Setup_Pkt=(tUSB_Setup*)BufferPointer[bEP0OUT_ODD];
    guint8_tUSB_State=uPOWER;      

    /* disable flash protect */
	FMC->PFAPR |= (FMC_PFAPR_M4AP_MASK);
    
    /* enable USB reguator */
	SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    
    /* disable memory protection */
    MPU->CESR=0;
    
    /* clock config */
    SIM->SOPT2 |= SIM_SOPT2_USBSRC_MASK;
    uint32_t clock;
    CLOCK_GetClockFrequency(kCoreClock, &clock);
    if(USB_SetClockDiv(clock))
    {
        LIB_TRACE("USB clock setup fail\r\n");
    }
    (MCG->C6 & MCG_C6_PLLS_MASK)?
    (SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK):   /* PLL */
    (SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK);  /* FLL */

    
    /* enable USB clock */
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    
    USB0->USBTRC0 = 0x40; 

	//����BDT��ַ�Ĵ���
	//( ��9 λ��Ĭ��512 �ֽڵ�ƫ��) 512 = 16 * 4 * 8 ��
	//8 λ��ʾ: 4 ���ֽڵĿ���״̬��4 ���ֽڵĻ�������ַ
        
#if 0
    
    /* reset USB moudle */
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while(BIT_CHK(USB_USBTRC0_USBRESET_SHIFT,USB0->USBTRC0)){};
    
    /* set BDT table address */
	USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);
        
    /* clear all IT bit */
    USB0->ISTAT |= 0xFF;
    
    /* enable USB reset IT */
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;       //D-  D+ ����
	USB0->USBTRC0 |= 0x40;                      //ǿ�����õ�6λΪ1  ���Ǿ��ᣬDS�Ͼ���ôд��

    /* enable USB moudle */
	USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    
    /* enable pull down reisger */
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
#endif
    NVIC_EnableIRQ(USB0_IRQn); //ʹ��USBģ��IRQ�ж�
	return 0;
}


void USB_DisConnect(void)
{
	USB0->CONTROL &= ~USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void USB_Connect(void)
{
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
}


void USB_ResetHandler(void)
{
    //���־
    guint8_tUSBClearFlags=0xFF;
    guint8_tUSB_Toogle_flags=0;
    //��ֹ���ж˵� 0�˵����
    USB0->ENDPOINT[1].ENDPT=0x00;
	USB0->ENDPOINT[2].ENDPT=0x00;
	USB0->ENDPOINT[3].ENDPT=0x00;
	USB0->ENDPOINT[4].ENDPT=0x00;
	USB0->ENDPOINT[5].ENDPT=0x00;
	USB0->ENDPOINT[6].ENDPT=0x00;
    /*�˵�0 BDT �����˵�����*/
    tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;   // EP0 OUT BDT ����
    tBDTtable[bEP0OUT_ODD].Addr =(uint32_t)guint8_tEP0_OUT_ODD_Buffer;
    tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA1;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                              //ʹ��USB-FSȥ�������ݷ�תͬ��
                                              //����DATA1�����ͻ��߽���        
   
    tBDTtable[bEP0OUT_EVEN].Cnt = EP0_SIZE; // EP0 OUT BDT ����
    tBDTtable[bEP0OUT_EVEN].Addr =(uint32_t)guint8_tEP0_OUT_EVEN_Buffer;
    tBDTtable[bEP0OUT_EVEN].Stat._byte = kUDATA1;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                               //ʹ��USB-FSȥ�������ݷ�תͬ��
                                               //����DATA1�����ͻ��߽���       
   
    tBDTtable[bEP0IN_ODD].Cnt = EP0_SIZE;   // EP0 IN BDT ����     
    tBDTtable[bEP0IN_ODD].Addr =(uint32_t)guint8_tEP0_IN_ODD_Buffer;      
    tBDTtable[bEP0IN_ODD].Stat._byte = kUDATA0;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                             //ʹ��USB-FSȥ�������ݷ�תͬ��
                                             //����DATA0�����ͻ��߽��� 
   
    tBDTtable[bEP0IN_EVEN].Cnt = EP0_SIZE;  // EP0 IN BDT ����            
    tBDTtable[bEP0IN_EVEN].Addr =(uint32_t)guint8_tEP0_IN_EVEN_Buffer;      
    tBDTtable[bEP0IN_EVEN].Stat._byte = kUDATA0;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                              //ʹ��USB-FSȥ�������ݷ�תͬ��
                                              //����DATA0�����ͻ��߽���          
    USB0->ENDPOINT[0].ENDPT=0x0D; // ʹ�� EP0 �������� ����ʱ��
    USB0->ERRSTAT=0xFF;           // ������еĴ���
	USB0->ISTAT=0xFF;             // ������е��жϱ�־
	USB0->ADDR=0x00;  					  // USBö��ʱ��Ĭ���豸��ַ0
	USB0->ERREN=0xFF;             // ʹ�����еĴ����ж�
	// USBģ���ж�ʹ��
	BIT_SET(USB_INTEN_TOKDNEEN_SHIFT,USB0->INTEN);
	BIT_SET(USB_INTEN_SOFTOKEN_SHIFT,USB0->INTEN);
	BIT_SET(USB_INTEN_ERROREN_SHIFT,USB0->INTEN); 
	BIT_SET(USB_INTEN_USBRSTEN_SHIFT,USB0->INTEN);    
}

/***********************************************************************************************
 ���ܣ�USB�ж�ɢת-EP0�����ж�����
 �βΣ�0          
 ���أ�0 
 ��⣺
************************************************************************************************/
void USB_EP0_IN_Handler(void)
{
    if(guint8_tUSB_State==uADDRESS)
    {
        USB0->ADDR = Setup_Pkt->wValue_l; //д���ַ
        guint8_tUSB_State=uREADY;              //��ΪReady״̬
        BIT_SET(fIN,guint8_tUSBClearFlags);   //
				#if (DEBUG_PRINT == 1)
				UART_printf("�µ�ַ��%d\r\n",USB0->ADDR);
				#endif
    }
		USB_EP_IN_Transfer(EP0,0,0); 
}

/***********************************************************************************************
 ���ܣ�USB�ж�ɢת-EP2�����ж�����
 �βΣ�0          
 ���أ�0 
 ��⣺
************************************************************************************************/
static  MessageType_t m_Msg;
void USB_EP2_IN_Handler(void)
{
	m_Msg.m_Command = USB_DEVICE_CLASS;
	m_Msg.m_MessageType = fIN;
	fn_msg_push(m_Msg); //����һ����Ϣ
	// tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL);
}
/***********************************************************************************************
 ���ܣ�USB�ж�ɢת-EP2����ж�����
 �βΣ�0          
 ���أ�0 
 ��⣺
************************************************************************************************/
void USB_EP2_OUT_Handler(void)
{
	
}
/***********************************************************************************************
 ���ܣ�USB�ж�ɢת-EP3�����ж�����
 �βΣ�0          
 ���أ�0 
 ��⣺
************************************************************************************************/
void USB_EP3_IN_Handler(void)
{
	
}
/***********************************************************************************************
 ���ܣ�USB�ж�ɢת-EP3����ж�����
 �βΣ�0          
 ���أ�0 
 ��⣺
************************************************************************************************/
void USB_EP3_OUT_Handler(void)
{
	m_Msg.m_Command = USB_DEVICE_CLASS;
	m_Msg.m_MessageType = fOUT;
	m_Msg.m_MsgLen = USB_EP_OUT_SizeCheck(EP3);
	m_Msg.pMessage = BufferPointer[bEP3OUT_ODD];
	fn_msg_push(m_Msg); //����һ����Ϣ
    tBDTtable[EP3<<2].Stat._byte= kSIE;
    tBDTtable[bEP3OUT_ODD].Cnt = EP3_SIZE;
}
/***********************************************************************************************
 ���ܣ���USB0_IRQHandler Ӳ���ж�ɢת����������ɢת
 �βΣ�0
 ���أ�0
 ��⣺��USB0_IRQHandler Ӳ���ж�ɢת��������������ɢת
************************************************************************************************/
void USB_Handler(void)
{
	uint8_t uint8_tEndPoint;
	uint8_t uint8_tIN;
    uint8_tIN = USB0->STAT & 0x08;    //��õ�ǰ�Ĵ���״̬��1���ͣ�0����
    uint8_tEndPoint = USB0->STAT >> 4;//��õ�ǰ�������ƵĶ˵��ַ
	if(uint8_tEndPoint == 0) //�˵�0
	{
		if(uint8_tIN) //IN
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("�˵�0�����жϴ���-");
			#endif
			USB_EP0_IN_Handler(); //�˵�0���Ͱ�
		}
		else    //OUT
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("�˵�0����жϴ���-");
			#endif
			USB_EP0_OUT_Handler();
		}
	}
	if(uint8_tEndPoint == 2)//�˵�2
	{
		if(uint8_tIN) //IN
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("�˵�2�����жϴ���\r\n");
			#endif
			USB_EP2_IN_Handler();
		}
		else //OUT
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("�˵�2����жϴ���\r\n");
			#endif
			USB_EP2_OUT_Handler();
		}	
	}
	if(uint8_tEndPoint == 3)//�˵�3
	{
		if(uint8_tIN) //IN
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("�˵�3�����жϴ���\r\n");
			#endif
			USB_EP3_IN_Handler();
		}
		else //OUT
		{
			#if (DEBUG_PRINT == 1)
			UART_printf("�˵�3�����жϴ���\r\n");
			#endif
			USB_EP3_OUT_Handler();
		}	
	}
}


void USB0_IRQHandler2(void)
{
	uint8_t err = 0;
	err = err;
    if(USB0->ISTAT & USB_ISTAT_USBRST_MASK)
    {
    	USB0->ISTAT = USB_ISTAT_USBRST_MASK;

        /* Handle RESET Interrupt */
        //USB_Bus_Reset_Handler();

        /* Notify Device Layer of RESET Event */
        //(void)USB_Device_Call_Service(USB_SERVICE_BUS_RESET, &event);

        /* Clearing this bit allows the SIE to continue token processing and clear suspend condition */
        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
        LIB_TRACE("bus USBRST\r\n");
        /* No need to process other interrupts */
        return;
    }
    if(USB0->ISTAT & USB_ISTAT_ERROR_MASK)
    {
        LIB_TRACE("bus ERROR\r\n");
    }
    if(USB0->ISTAT & USB_ISTAT_SOFTOK_MASK)
    {
        LIB_TRACE("bus SOFTOK\r\n");
    }
    if(USB0->ISTAT & USB_ISTAT_TOKDNE_MASK)
    {
        LIB_TRACE("bus TOKDNE\r\n");
    }
    if(USB0->ISTAT & USB_ISTAT_SLEEP_MASK)
    {
        /* Clear RESUME Interrupt if Pending */
    	USB0->ISTAT = USB_ISTAT_RESUME_MASK;

        /* Clear SLEEP Interrupt */
    	USB0->ISTAT = USB_ISTAT_SLEEP_MASK;

        /* Notify Device Layer of SLEEP Event */
        //(void)USB_Device_Call_Service(USB_SERVICE_SLEEP, &event);

        /* Set Low Power RESUME enable */
        USB0->USBTRC0 |= USB_USBTRC0_USBRESMEN_MASK;

        /* Set SUSP Bit in USB_CTRL */
        USB0->USBCTRL |= USB_USBCTRL_SUSP_MASK;

        /* Enable RESUME Interrupt */
        USB0->INTEN |= USB_INTEN_RESUMEEN_MASK;
    }
    if(USB0->ISTAT & USB_ISTAT_RESUME_MASK)
    {
        LIB_TRACE("bus RESUME\r\n");
    } 
    if(USB0->ISTAT & USB_ISTAT_ATTACH_MASK)
    {
        LIB_TRACE("bus ATTACH\r\n");
    } 
    if(USB0->ISTAT & USB_ISTAT_STALL_MASK)
    {
        LIB_TRACE("bus STALL\r\n");
    } 
    
    
    
    #if 0
    
	//���USB ģ���Ƿ��������Ч�ĸ�λ��
	if(BIT_CHK(USB_ISTAT_USBRST_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		UART_printf("USB���߸�λ\r\n");
		#endif
		USB_ResetHandler();
	}
	//�յ�SOF��
	if(BIT_CHK(USB_ISTAT_SOFTOK_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		//UART_printf("�յ�SOF��\r\n");
		#endif
		USB0->ISTAT = USB_ISTAT_SOFTOK_MASK;   
	}
	//�յ�STALL��
	if(BIT_CHK(USB_ISTAT_STALL_SHIFT,USB0->ISTAT))
	{
		#if (DEBUG_PRINT == 1)
		UART_printf("�յ�STALL��\r\n");
		#endif
		if(BIT_CHK(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT))
		BIT_CLR(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT);
    BIT_SET(USB_ISTAT_STALL_SHIFT,USB0->ISTAT);
	}
	//��������ж�
	if(BIT_CHK(USB_ISTAT_TOKDNE_SHIFT,USB0->ISTAT)) 
	{
		BIT_SET(USB_CTL_ODDRST_SHIFT,USB0->CTL);//ָ��BDT EVEN ��
		USB_Handler(); //����USB Handler
		//�����������ж�
		BIT_SET(USB_ISTAT_TOKDNE_SHIFT,USB0->ISTAT);
	}
	//SLEEP 
	if(BIT_CHK(USB_ISTAT_SLEEP_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		UART_printf("SLEEP�ж�\r\n");
		#endif
		BIT_SET(USB_ISTAT_SLEEP_SHIFT,USB0->ISTAT);        
	}
	// ����
	if(BIT_CHK(USB_ISTAT_ERROR_SHIFT,USB0->ISTAT))
	{
		err = USB0->ERRSTAT;
		#if (DEBUG_PRINT == 1)
		UART_printf("���� �������:%d\r\n",err);
		#endif
		BIT_SET(USB_ISTAT_ERROR_SHIFT,USB0->ISTAT);
		USB0->ERRSTAT=0xFF; //��������жϴ���
	}
    #endif
}




#include "usb.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "message_manage.h"

/* BDT RAM */
ALIGN(512) tBDT tBDTtable[16];


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

void USB_WaitDeviceEnumed(void)
{
  while(guint8_tUSB_State != uENUMERATED);//�ȴ�USB�豸��ö��
}


uint32_t USB_GetEPNum(void)
{
    return ARRAY_SIZE(USB0->ENDPOINT);
}

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
  
  
  if(guint8_tUSBClearFlags & (1<<kUSB_IN)) //���guint8_tUSBClearFlags = 1
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
      BIT_CLR(kUSB_IN,guint8_tUSBClearFlags);//��guint8_tUSBClearFlags����
  }
  else
  { 
      uint8_tEPSize = guint8_tIN_Counter;      //���С��
      guint8_tIN_Counter=0;            
      BIT_SET(kUSB_IN,guint8_tUSBClearFlags);//��guint8_tUSBClearFlags��һ
  }
  /*���û����������ֵ���Ƶ�EP ��������׼������*/
  tBDTtable[uint8_tEP].Cnt=(uint8_tEPSize);    //������Ҫ���͵����ݳ���
  while(uint8_tEPSize--)
       *puint8_tEPBuffer++=*puint8_tIN_DataPointer++; //���û������ݸ�ֵ��EP�洢��     

 
  if(guint8_tUSB_Toogle_flags & (1<<uint8_tEndPointFlag))  //�����Ӧ�˵��guint8_tUSB_Toogle_flags == 1
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


uint16_t USB_EP_OUT_SizeCheck(uint8_t uint8_tEP)
{
  uint8_t uint8_tEPSize; 
   /* ��ȡ�������ĳ��� */
  uint8_tEPSize = tBDTtable[uint8_tEP<<2].Cnt;
  return(uint8_tEPSize & 0x03FF);
}

void hw_usb_ep0_stall(void)
{
	// ����һ��STALL��  
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("send a stall packet\r\n"));
    
	BIT_SET(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT);
	//ENDPT0_EP_STALL = 1;                      
	tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA0; 
	tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;       
}


void USB_EnableInterface(void)
{
    uint32_t i, ep_num;
    
    ep_num = USB_GetEPNum();
    for(i = 0; i < ep_num;i++)
    {
        USB0->ENDPOINT[i].ENDPT |= (USB_ENDPT_EPHSHK_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK);
    }

    // ���� 1 BDT ����
    // �ѿ���Ȩ����MCU 
    //tBDTtable[bEP1IN_ODD].Stat._byte= kUDATA1;                //����DATA1���ݰ�
    //tBDTtable[bEP1IN_ODD].Cnt = 0x00;                         //���������
    //tBDTtable[bEP1IN_ODD].Addr =(uint32_t)guint8_tEP1_IN_ODD_Buffer;    //��ַָ���Ӧ�Ļ�����

    // ���� 2 BDT ����
    // �ѿ���Ȩ����MCU 
    tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA1;
    tBDTtable[bEP2IN_ODD].Cnt = 0x00;
    tBDTtable[bEP2IN_ODD].bufAddr =(uint32_t  )guint8_tEP2_IN_ODD_Buffer;            

    // ���� 3 BDT ����
    // �ѿ���Ȩ����MCU 
    tBDTtable[bEP3OUT_ODD].Stat._byte= kUDATA0;
    tBDTtable[bEP3OUT_ODD].Cnt = 0xFF;
    tBDTtable[bEP3OUT_ODD].bufAddr =(uint32_t)guint8_tEP3_OUT_ODD_Buffer;            
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
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("device desc\r\n"));
        
			USB_EP_IN_Transfer(EP0,(uint8_t*)Device_Descriptor,sizeof(Device_Descriptor));//�����豸������
			break;
		case CONFIGURATION_DESCRIPTOR:
			USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("config desc\r\n"));
        
			USB_EP_IN_Transfer(EP0,(uint8_t*)Configuration_Descriptor,sizeof(Configuration_Descriptor)); //��������������
			break;
		case STRING_DESCRIPTOR:

			switch(Setup_Pkt->wValue_l)  //����wValue�ĵ��ֽڣ�����ֵ��ɢת
			{
				case 0:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: language\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 1:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 2:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender string\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 3:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender serail number\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				default: 
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: unknow:%d\r\n", Setup_Pkt->wValue_l));
					break; 
			}
			break;
				case REPORT_DESCRIPTOR:
					USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report desc\r\n"));
                
					USB_EP_IN_Transfer(EP0,(uint8_t*)Report_Descriptor,sizeof(Report_Descriptor));
					break;
		default:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("desc:unknow%d\r\n", Setup_Pkt->wValue_h));
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
    int i;
	uint8_t *p =(uint8_t*)Setup_Pkt;
	// ��DATA0 ��ʼ���䡣
	BIT_CLR(0,guint8_tUSB_Toogle_flags);
    
    for(i=0;i<8;i++)
    {
        //USB_DEBUG_LOG(USB_DEBUG_EP0, ("0x%X ", *p++));
    }
    //USB_DEBUG_LOG(USB_DEBUG_EP0, ("\r\n"));
    
	if((Setup_Pkt->bmRequestType & 0x80) == 0x80)
	{
		//����bmRequestType��D6-5λɢת��D6-5λ��ʾ���������
		//0Ϊ��׼����1Ϊ������2Ϊ��������
        switch((Setup_Pkt->bmRequestType>>5) & 0x03)
        {
			case 0:
                
				//USBЭ�鶨���˼�����׼��������ʵ����Щ��׼�������󼴿�
				//����Ĵ�����bRequest�У��Բ�ͬ������������ɢת
				switch(Setup_Pkt->bRequest)
				{
					case mGET_CONFIG:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get config\r\n"));
						break;	
					case mGET_DESC:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get desc - "));
						USB_GetDescHandler(); //ִ�л�ȡ������
						break;
					case mGET_INTF:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get interface\r\n"));
						break;
					case mGET_STATUS:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get status\r\n"));
						break;
					case mSYNC_FRAME:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: sync frame\r\n"));
						break;
                    default:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: unknown\r\n"));
						break;
				}
				break;
			case 1:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb request\r\n"));
				break;
			case 2:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb vender request\r\n"));
				break;
			default:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("unknown request\r\n"));
				break;
		}	
	}
	else
	{
		switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				switch(Setup_Pkt->bRequest)
				{
					case mCLR_FEATURE:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: clear feature\r\n"));
						break;
					case mSET_ADDRESS:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: get addr:%d\r\n", Setup_Pkt->wValue_l));
						guint8_tUSB_State=uADDRESS;
						USB_EP_IN_Transfer(EP0,0,0); //Ϊʲô������
						break;
					case mSET_CONFIG:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set config\r\n"));
                        if(Setup_Pkt->wValue_h+Setup_Pkt->wValue_l) 
                        {
                            //ʹ��1 ��2 ��3 �˵� 
							USB_EnableInterface();
							USB_EP_IN_Transfer(EP0,0,0);
							guint8_tUSB_State=uENUMERATED;
                        }
						break;
					case  mSET_DESC:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set desc\r\n"));
						break;
					case mSET_INTF:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set interface\r\n"));
						break;
					default:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: unknown\r\n"));
					break;
				}
				break;
				case 1:
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: usb request\r\n"));
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
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: usb vender request\r\n"));
					break;
				default:
					USB_DEBUG_LOG(USB_DEBUG_EP0, ("set request: unknown request\r\n"));
				break;
		}
	}
	tBDTtable[bEP0OUT_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL); // Ϊ0 ʱ: SIE ������������
}


uint8_t USB_Init(void)
{
    Setup_Pkt=(tUSB_Setup*)BufferPointer[bEP0OUT_ODD];                       //��Setup���ݽṹ��ָ���Ӧ�Ļ�����
    guint8_tUSB_State = uPOWER;
    
    /* enable USB clock */
    SIM->SCGC4 |= SIM_SCGC4_USBOTG_MASK;
    
    /* disable flash protect */
	FMC->PFAPR |= (FMC_PFAPR_M4AP_MASK);
    
    /* enable USB reguator */
	SIM->SOPT1 |= SIM_SOPT1_USBREGEN_MASK;
    
    /* disable memory protection */
    MPU->CESR=0;

    /* reset USB moudle */
	USB0->USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
	while(USB0->USBTRC0 & USB_USBTRC0_USBRESET_MASK){};
        
	//����BDT��ַ�Ĵ���
	//( ��9 λ��Ĭ��512 �ֽڵ�ƫ��) 512 = 16 * 4 * 8 ��
	//8 λ��ʾ: 4 ���ֽڵĿ���״̬��4 ���ֽڵĻ�������ַ ��
	USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);

    
    USB0->USBTRC0 = 0x40; 

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
    
	//��ʼ����Ϣ����
	fn_queue_init(); //��ʼ������
	return 0;
}


void USB_BusResetHandler(void)
{
    uint32_t i, ep;
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb reset\r\n"));
	//���־
	guint8_tUSBClearFlags=0xFF;
	guint8_tUSB_Toogle_flags=0;
    
    ep = USB_GetEPNum();
    
    for(i=1;i<ep;i++)
    {
        USB0->ENDPOINT[i].ENDPT=0x00;
    }

    /*�˵�0 BDT �����˵�����*/
    tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;   // EP0 OUT BDT ����
    tBDTtable[bEP0OUT_ODD].bufAddr =(uint32_t)guint8_tEP0_OUT_ODD_Buffer;
    tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA1;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                              //ʹ��USB-FSȥ�������ݷ�תͬ��
                                              //����DATA1�����ͻ��߽���        
   
    tBDTtable[bEP0OUT_EVEN].Cnt = EP0_SIZE; // EP0 OUT BDT ����
    tBDTtable[bEP0OUT_EVEN].bufAddr =(uint32_t)guint8_tEP0_OUT_EVEN_Buffer;
    tBDTtable[bEP0OUT_EVEN].Stat._byte = kUDATA1;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                               //ʹ��USB-FSȥ�������ݷ�תͬ��
                                               //����DATA1�����ͻ��߽���       
   
    tBDTtable[bEP0IN_ODD].Cnt = EP0_SIZE;   // EP0 IN BDT ����     
    tBDTtable[bEP0IN_ODD].bufAddr =(uint32_t)guint8_tEP0_IN_ODD_Buffer;      
    tBDTtable[bEP0IN_ODD].Stat._byte = kUDATA0;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                             //ʹ��USB-FSȥ�������ݷ�תͬ��
                                             //����DATA0�����ͻ��߽��� 
   
    tBDTtable[bEP0IN_EVEN].Cnt = EP0_SIZE;  // EP0 IN BDT ����            
    tBDTtable[bEP0IN_EVEN].bufAddr =(uint32_t)guint8_tEP0_IN_EVEN_Buffer;      
    tBDTtable[bEP0IN_EVEN].Stat._byte = kUDATA0;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                              //ʹ��USB-FSȥ�������ݷ�תͬ��
                                              //����DATA0�����ͻ��߽���  

    /* reset all BDT to even */
    USB0->CTL |= USB_CTL_ODDRST_MASK;

    USB0->ENDPOINT[0].ENDPT=0x0D; // ʹ�� EP0 �������� ����ʱ��
    
    /* enable and clear all error states */
    USB0->ERRSTAT=0xFF;
    USB0->ERREN=0xFF;
    
    /* reset USB moudle addr */
    USB0->ADDR=0x00;
    
    /* clear all interrupts status */
    USB0->ISTAT=0xFF;
    
    /* enable USB module interrupts */
    USB0->INTEN |= USB_INTEN_TOKDNEEN_MASK;
    //USB0->INTEN |= USB_INTEN_SOFTOKEN_MASK;
    USB0->INTEN |= USB_INTEN_ERROREN_MASK;
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;  
}


void USB_EP0_IN_Handler(void)
{
    if(guint8_tUSB_State==uADDRESS)
    {
        USB0->ADDR = Setup_Pkt->wValue_l; //д���ַ
        guint8_tUSB_State=uREADY;              //��ΪReady״̬
        BIT_SET(kUSB_IN, guint8_tUSBClearFlags);
        USB_DEBUG_LOG(USB_DEBUG_EP0, ("new addr:%d\r\n", USB0->ADDR));
    }
    USB_EP_IN_Transfer(EP0,0,0); 
}


static  MessageType_t m_Msg;
void USB_EP2_IN_Handler(void)
{
	m_Msg.m_Command = USB_DEVICE_CLASS;
	m_Msg.m_MessageType = kUSB_IN;
	fn_msg_push(m_Msg); //����һ����Ϣ
	// tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL);
}


void USB_EP2_OUT_Handler(void)
{
	
}


void USB_EP3_IN_Handler(void)
{
	
}

void USB_EP3_OUT_Handler(void)
{
    m_Msg.m_Command = USB_DEVICE_CLASS;
    m_Msg.m_MessageType = kUSB_OUT;
    m_Msg.m_MsgLen = USB_EP_OUT_SizeCheck(EP3);
    m_Msg.pMessage = BufferPointer[bEP3OUT_ODD];
    fn_msg_push(m_Msg); //����һ����Ϣ
    tBDTtable[EP3<<2].Stat._byte= kSIE;
    tBDTtable[bEP3OUT_ODD].Cnt = EP3_SIZE;
}


void USB_Handler(void)
{
    uint8_t ep;
    uint8_t uint8_tIN;
    uint8_tIN = USB0->STAT & 0x08;
    
    /* get endpoint direction */
    
    /* get endpoint num */
    ep = (USB0->STAT >> 4);
    
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("EP%d-DIR:%d - ", ep, uint8_tIN));
    
	if(ep == 0)
	{
		if(uint8_tIN)
		{
			USB_EP0_IN_Handler();
		}
		else
		{
			USB_EP0_OUT_Handler();
		}
	}
	if(ep == 2)
	{
		if(uint8_tIN)
		{
			USB_EP2_IN_Handler();
		}
		else
		{
			USB_EP2_OUT_Handler();
		}	
	}
	if(ep == 3)
	{
		if(uint8_tIN)
		{
			USB_EP3_IN_Handler();
		}
		else
		{
			USB_EP3_OUT_Handler();
		}	
	}
}


void USB0_IRQHandler(void)
{
	uint8_t v1 = USB0->ISTAT;
	uint8_t v2 = USB0->INTEN;
    uint8_t status = (v1 & v2);
    
    if(status & USB_ISTAT_USBRST_MASK)
    {
        USB_BusResetHandler();
    }

	if(status & USB_ISTAT_SOFTOK_MASK) 
	{
        USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb sof\r\n"));
		USB0->ISTAT = USB_ISTAT_SOFTOK_MASK;   
	}
	
	if(status & USB_ISTAT_STALL_MASK)
	{
		USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb stall\r\n"));
        
        if(USB0->ENDPOINT[0].ENDPT & USB_ENDPT_EPSTALL_MASK)
        {
            USB0->ENDPOINT[0].ENDPT &= ~USB_ENDPT_EPSTALL_MASK;
        }
    
        USB0->ISTAT |= USB_ISTAT_STALL_MASK;
	}

	if(status & USB_ISTAT_TOKDNE_MASK) 
	{
        
		USB_Handler();

        USB0->ISTAT |= USB_ISTAT_TOKDNE_MASK;
	}

	if(status & USB_ISTAT_SLEEP_MASK) 
	{
		USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb sleep\r\n"));
        USB0->ISTAT |= USB_ISTAT_SLEEP_MASK;      
	}

	if(status & USB_ISTAT_ERROR_MASK)
	{
        uint8_t err = USB0->ERRSTAT;
        USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb error:%0x%X\r\n", err));
        
        /* clear */
        USB0->ISTAT |= USB_ISTAT_ERROR_MASK;
        USB0->ERRSTAT = 0xFF;
	}
}





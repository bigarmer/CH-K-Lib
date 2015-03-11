#include "usb.h"
#include "usb_desc.h"
#include "usb_hid.h"
#include "message_manage.h"

/* BDT RAM */
ALIGN(512) tBDT tBDTtable[16];
//USBģ���ڲ�ȫ�ֱ���
static uint32_t gData1 = 0x55555555;
uint8_t guint8_tUSBClearFlags;            //�ڲ�ʹ��

uint8_t guint8_tIN_Counter;               //�ڲ�ʹ��
uint8_t gDataFlag;         //�ڲ�ʹ��
tUSB_Setup *Setup_Pkt;             //ָ��˵�0OUT�����׵�ַ

static USB_DEVICE gUSBDevice;

//�����˵�����ݻ�����
uint8_t EP0_OUT_ODD_Buffer[EP0_SIZE];
uint8_t EP0_OUT_EVEN_Buffer[EP0_SIZE];
uint8_t EP0_IN_ODD_Buffer[EP0_SIZE];
uint8_t EP0_IN_EVEN_Buffer[EP0_SIZE];
uint8_t EP1_OUT_ODD_Buffer[EP1_SIZE];
uint8_t EP1_OUT_EVEN_Buffer[EP1_SIZE];
uint8_t EP1_IN_ODD_Buffer[EP1_SIZE];
uint8_t EP1_IN_EVEN_Buffer[EP1_SIZE];
uint8_t EP2_OUT_ODD_Buffer[EP2_SIZE];
uint8_t EP2_OUT_EVEN_Buffer[EP2_SIZE];
uint8_t EP2_IN_ODD_Buffer[EP2_SIZE];
uint8_t EP2_IN_EVEN_Buffer[EP2_SIZE];
uint8_t EP3_OUT_ODD_Buffer[EP3_SIZE];
uint8_t EP3_OUT_EVEN_Buffer[EP3_SIZE];
uint8_t EP3_IN_ODD_Buffer[EP3_SIZE];
uint8_t EP3_IN_EVEN_Buffer[EP3_SIZE];
//ָ������������ĵ�ַָ��
uint8_t *BufferPointer[]=
{
    EP0_OUT_ODD_Buffer,
    EP0_OUT_EVEN_Buffer,
    EP0_IN_ODD_Buffer,
    EP0_IN_EVEN_Buffer,
    EP1_OUT_ODD_Buffer,
    EP1_OUT_EVEN_Buffer,
    EP1_IN_ODD_Buffer,
    EP1_IN_EVEN_Buffer,
    EP2_OUT_ODD_Buffer,
    EP2_OUT_EVEN_Buffer,
    EP2_IN_ODD_Buffer,
    EP2_IN_EVEN_Buffer,
    EP3_OUT_ODD_Buffer,
    EP3_OUT_EVEN_Buffer,
    EP3_IN_ODD_Buffer,
    EP3_IN_EVEN_Buffer
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




void USBD_DecodeSetupPacket(uint8_t *data, SETUP_PACKET *packet)
{
    /* Fill in the elements of a SETUP_PACKET structure from raw data */
    packet->bmRequestType.dataTransferDirection = (data[0] & 0x80) >> 7;
    packet->bmRequestType.Type = (data[0] & 0x60) >> 5;
    packet->bmRequestType.Recipient = data[0] & 0x1f;
    packet->bRequest = data[1];
    packet->wValue = (data[2] | (uint16_t)data[3] << 8);
    packet->wIndex = (data[4] | (uint16_t)data[5] << 8);
    packet->wLength = (data[6] | (uint16_t)data[7] << 8);
}


uint32_t USB_GetPhyEPNumber(void)
{
    return ARRAY_SIZE(USB0->ENDPOINT);
}


void USB_EP_IN_Transfer(uint8_t ep, uint8_t *buf, uint8_t len)
{
    uint8_t *p;
    uint8_t uint8_tEPSize;     //�˵�����ݳ���
    uint16_t uint16_tLenght=0;    
    static uint8_t uint8_tEndPointFlag;
    static uint8_t *puint8_tIN_DataPointer;          //�ڲ�ʹ��
    /*������ǰ��������λ��*/
    uint8_tEndPointFlag = ep;
    
    ep *= 4;
    ep += 2; //EPֵ��2
    p = BufferPointer[ep];   //����EP��BUFFER��ַ��puint8_tEPBuffer
  
  
    if(guint8_tUSBClearFlags & (1<<kUSB_IN)) //���guint8_tUSBClearFlags = 1
    {
        puint8_tIN_DataPointer = buf; //���û����ݴ洢�����׵�ַ��puint8_tIN_DataPointer
        guint8_tIN_Counter = len;         //���û����ݳ��ȸ�guint8_tIN_Counter

        uint16_tLenght=(Setup_Pkt->wLength_h<<8)+Setup_Pkt->wLength_l ;//��setup���ݵĳ��ȸ�uint16_tLenght
        if((uint16_tLenght < len) && (ep == 2)) //��������͵����ݳ��ȴ���setup���趨�ĳ��� ͬʱ �˵�ֵ == 2
        {
            guint8_tIN_Counter = Setup_Pkt->wLength_l; //ֻ����setup�еĵ�8λ����
        }
    }
    
    /*��鷢�ͳ���*/
    if(guint8_tIN_Counter > cEP_Size[ep]) //����������ݰ��ĳ��� ����32�ֽ�ʱ
    {
        uint8_tEPSize = cEP_Size[ep];     //����ʱ�˵�ĳ��������ڶ˵��Ĭ�ϳ���
        guint8_tIN_Counter-=cEP_Size[ep]; //�����ݰ��ĳ��ȼ���EP_Size
        BIT_CLR(kUSB_IN,guint8_tUSBClearFlags);//��guint8_tUSBClearFlags����
    }
    else
    { 
        uint8_tEPSize = guint8_tIN_Counter;      //���С��
        guint8_tIN_Counter=0;            
        BIT_SET(kUSB_IN,guint8_tUSBClearFlags);//��guint8_tUSBClearFlags��һ
    }
    
    /*���û����������ֵ���Ƶ�EP ��������׼������*/
    tBDTtable[ep].Cnt=(uint8_tEPSize);
    while(uint8_tEPSize--)
    {
        *p++ =* puint8_tIN_DataPointer++; //���û������ݸ�ֵ��EP�洢��     
    }

    
    if(gDataFlag & (1<<uint8_tEndPointFlag))
    {
        tBDTtable[ep].Stat._byte= kUDATA0;         
        BIT_CLR(uint8_tEndPointFlag, gDataFlag);
    }
    else
    {
        tBDTtable[ep].Stat._byte= kUDATA1;          
        BIT_SET(uint8_tEndPointFlag, gDataFlag);
    }
}


uint16_t USB_GetPacketSize(uint8_t ep)
{
    uint8_t size; 
    size = tBDTtable[ep << 2].Cnt;
    return(size & 0x03FF);
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
    
    /* enable all endpoint */
    ep_num = USB_GetPhyEPNumber();
    for(i = 0; i < ep_num;i++)
    {
        USB0->ENDPOINT[i].ENDPT |= (USB_ENDPT_EPHSHK_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK);
    }

    tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA1;
    tBDTtable[bEP2IN_ODD].Cnt = 0x00;
    tBDTtable[bEP2IN_ODD].bufAddr =(uint32_t  )EP2_IN_ODD_Buffer;            

    tBDTtable[bEP3OUT_ODD].Stat._byte= kUDATA0;
    tBDTtable[bEP3OUT_ODD].Cnt = 0xFF;
    tBDTtable[bEP3OUT_ODD].bufAddr =(uint32_t)EP3_OUT_ODD_Buffer;            
}


void USB_GetDescHandler(SETUP_PACKET *packet)
{
    switch(DESCRIPTOR_TYPE(packet->wValue))
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
            switch (DESCRIPTOR_INDEX(packet->wValue))
			{
				case STRING_OFFSET_LANGID:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: language\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[STRING_OFFSET_LANGID],sizeof(String_Table[STRING_OFFSET_LANGID]));
					break;
				case STRING_OFFSET_IMANUFACTURER:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[STRING_OFFSET_IMANUFACTURER],sizeof(String_Table[STRING_OFFSET_IMANUFACTURER]));
					break;
				case STRING_OFFSET_IPRODUCT:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender string\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[STRING_OFFSET_IPRODUCT],sizeof(String_Table[STRING_OFFSET_IPRODUCT]));
					break;
				case STRING_OFFSET_ISERIAL:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: vender serail number\r\n"));
					USB_EP_IN_Transfer(EP0,(uint8_t*)String_Table[STRING_OFFSET_ISERIAL],sizeof(String_Table[STRING_OFFSET_ISERIAL]));
					break;
				case STRING_OFFSET_ICONFIGURATION:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: congiuration\r\n"));
					break;
				case STRING_OFFSET_IINTERFACE:
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: interface\r\n"));
					break;
				default: 
                    USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("string desc: unknow:%d\r\n",DESCRIPTOR_INDEX(packet->wValue)));
					break; 
			}
			break;
        case REPORT_DESCRIPTOR:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("report desc\r\n"));
            USB_EP_IN_Transfer(EP0,(uint8_t*)Report_Descriptor,sizeof(Report_Descriptor));
            break;
		default:
            USB_DEBUG_LOG(USB_DEBUG_DESCRIPTOR, ("desc:unknow%d\r\n",  DESCRIPTOR_TYPE(packet->wValue)));
			break;
    }
}


void USB_EP0_OUT_Handler(SETUP_PACKET *packet)
{
    int i;
	// ��DATA0 ��ʼ���䡣
	BIT_CLR(0,gDataFlag);
    
    //SETUP_PACKET packet;

    //USBD_DecodeSetupPacket((uint8_t*)BufferPointer[bEP0OUT_ODD], &packet);
    
    //memcpy(&packet, packet1, sizeof(SETUP_PACKET));
    //packet.bmRequestType = 
    for(i=0;i<8;i++)
    {
        //USB_DEBUG_LOG(USB_DEBUG_EP0, ("0x%X ", *p++));
    }
    //USB_DEBUG_LOG(USB_DEBUG_EP0, ("~~0x%X\r\n", packet.bmRequestType));
    
	if(packet->bmRequestType.dataTransferDirection == DEVICE_TO_HOST)
	{   
        switch(packet->bmRequestType.Type)
        {
			case STANDARD_TYPE:
				switch(packet->bRequest)
				{
					case GET_CONFIGURATION:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get config\r\n"));
						break;	
					case GET_DESCRIPTOR:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get desc - "));
						USB_GetDescHandler(packet);
						break;
					case GET_INTERFACE:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get interface\r\n"));
						break;
					case GET_STATUS:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: get status\r\n"));
						break;
                    default:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("standrd request: unknown\r\n"));
						break;
				}
				break;
			case CLASS_TYPE:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb request\r\n"));
				break;
			case VENDOR_TYPE:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("usb vender request\r\n"));
				break;
			default:
				USB_DEBUG_LOG(USB_DEBUG_EP0, ("unknown request\r\n"));
				break;
		}	
	}
	else
	{
		switch(packet->bmRequestType.Type)
		{
			case STANDARD_TYPE:
				switch(packet->bRequest)
				{
					case CLEAR_FEATURE:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: clear feature\r\n"));
						break;
					case SET_ADDRESS:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: get addr:%d\r\n", packet->wValue));
                        gUSBDevice.state = ADDRESS;
						USB_EP_IN_Transfer(EP0, 0, 0);
						break;
					case SET_CONFIGURATION:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set config\r\n"));
                        USB_EnableInterface();
                        USB_EP_IN_Transfer(EP0, 0, 0);
                        gUSBDevice.state = CONFIGURED;
						break;
					case SET_DESCRIPTOR:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set desc\r\n"));
						break;
					case SET_INTERFACE:
						USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: set interface\r\n"));
						break;
					default:
                        USB_DEBUG_LOG(USB_DEBUG_EP0, ("set standrd request: unknown\r\n"));
					break;
				}
				break;
				case CLASS_TYPE:
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
				case VENDOR_TYPE:
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
    Setup_Pkt=(tUSB_Setup*)BufferPointer[bEP0OUT_ODD];   //��Setup���ݽṹ��ָ���Ӧ�Ļ�����
    gUSBDevice.state = DEFAULT;
    
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

    /* BDT adddress */
	USB0->BDTPAGE1=(uint8_t)((uint32_t)tBDTtable>>8);
	USB0->BDTPAGE2=(uint8_t)((uint32_t)tBDTtable>>16);
	USB0->BDTPAGE3=(uint8_t)((uint32_t)tBDTtable>>24);

    /* clear all IT bit */
    USB0->ISTAT |= 0xFF;
    
    /* enable USB reset IT */
    USB0->INTEN |= USB_INTEN_USBRSTEN_MASK;

	USB0->USBCTRL = USB_USBCTRL_PDE_MASK;
	USB0->USBTRC0 |= 0x40;

    /* enable USB moudle */
	USB0->CTL |= USB_CTL_USBENSOFEN_MASK;
    
    /* enable pull down reisger */
	USB0->CONTROL |= USB_CONTROL_DPPULLUPNONOTG_MASK;
    
	fn_queue_init();
    
	return 0;
}


void USB_BusResetHandler(void)
{
    uint32_t i, phy_ep;
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("usb reset\r\n"));

	guint8_tUSBClearFlags=0xFF;
	gDataFlag=0;
    
    phy_ep = USB_GetPhyEPNumber();
    
    for(i = 1; i < phy_ep; i++)
    {
        USB0->ENDPOINT[i].ENDPT=0x00;
    }

    /*�˵�0 BDT �����˵�����*/
    tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;   // EP0 OUT BDT ����
    tBDTtable[bEP0OUT_ODD].bufAddr =(uint32_t)EP0_OUT_ODD_Buffer;
    tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA1;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                              //ʹ��USB-FSȥ�������ݷ�תͬ��
                                              //����DATA1�����ͻ��߽���        
   
    tBDTtable[bEP0OUT_EVEN].Cnt = EP0_SIZE; // EP0 OUT BDT ����
    tBDTtable[bEP0OUT_EVEN].bufAddr =(uint32_t)EP0_OUT_EVEN_Buffer;
    tBDTtable[bEP0OUT_EVEN].Stat._byte = kUDATA1;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                               //ʹ��USB-FSȥ�������ݷ�תͬ��
                                               //����DATA1�����ͻ��߽���       
   
    tBDTtable[bEP0IN_ODD].Cnt = EP0_SIZE;   // EP0 IN BDT ����     
    tBDTtable[bEP0IN_ODD].bufAddr =(uint32_t)EP0_IN_ODD_Buffer;      
    tBDTtable[bEP0IN_ODD].Stat._byte = kUDATA0;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                             //ʹ��USB-FSȥ�������ݷ�תͬ��
                                             //����DATA0�����ͻ��߽��� 
   
    tBDTtable[bEP0IN_EVEN].Cnt = EP0_SIZE;  // EP0 IN BDT ����            
    tBDTtable[bEP0IN_EVEN].bufAddr =(uint32_t)EP0_IN_EVEN_Buffer;      
    tBDTtable[bEP0IN_EVEN].Stat._byte = kUDATA0;//USB-FS(Ӳ��USBģ��) ��ר��Ȩ���� BD
                                              //ʹ��USB-FSȥ�������ݷ�תͬ��
                                              //����DATA0�����ͻ��߽���  

    /* reset all BDT to even */
    USB0->CTL |= USB_CTL_ODDRST_MASK;

    USB0->ENDPOINT[0].ENDPT = 0x0D; // ʹ�� EP0 �������� ����ʱ��
    
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



void USB_EP0_IN_Handler(SETUP_PACKET *packet)
{
    if(gUSBDevice.state == ADDRESS)
    {
        USB0->ADDR = packet->wValue & 0xFF;
        gUSBDevice.state = CONFIGURED;
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

    USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
}



void USB_EPCallback(uint8_t ep, uint8_t dir)
{
    if((ep == 2) && (dir == kUSB_IN))
    {
        m_Msg.m_Command = USB_DEVICE_CLASS;
        m_Msg.m_MessageType = kUSB_IN;
        fn_msg_push(m_Msg); //����һ����Ϣ
        // tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA0;
        USB0->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
    }
    if((ep == 3) && (dir == kUSB_OUT))
    {
        m_Msg.m_Command = USB_DEVICE_CLASS;
        m_Msg.m_MessageType = kUSB_OUT;
        m_Msg.m_MsgLen = USB_GetPacketSize(EP3);
        m_Msg.pMessage = BufferPointer[bEP3OUT_ODD];
        fn_msg_push(m_Msg); //����һ����Ϣ
        tBDTtable[EP3<<2].Stat._byte= kSIE;
        tBDTtable[bEP3OUT_ODD].Cnt = EP3_SIZE;
    }
    
}


void USB_Handler(void)
{
    uint8_t ep;
    uint8_t dir;

    /* get endpoint direction */
    (USB0->STAT & USB_STAT_TX_MASK)?(dir = kUSB_IN):(dir = kUSB_OUT);
    
    /* get endpoint num */
    ep = ((USB0->STAT & USB_STAT_ENDP_MASK )>> USB_STAT_ENDP_SHIFT);
    
    USB_DEBUG_LOG(USB_DEBUG_MIN, ("EP%d-DIR:%d - ", ep, dir));
    
	if(ep == 0)
	{
        SETUP_PACKET packet;
        USBD_DecodeSetupPacket((uint8_t*)BufferPointer[bEP0OUT_ODD], &packet);
        
		if(dir == kUSB_IN)
		{
			USB_EP0_IN_Handler(&packet);
		}
		else
		{
			USB_EP0_OUT_Handler(&packet);
		}
	}
    
    /* other ep callback */
    USB_EPCallback(ep, dir);
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



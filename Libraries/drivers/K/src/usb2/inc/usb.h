#ifndef __USB_H_
#define	__USB_H_
#include "common.h"
#include "message_manage.h"
#include <stdio.h>

#ifdef USB0

#if !defined(ARRAY_SIZE)
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN
/* Compiler Related Definitions */
#ifdef __CC_ARM                         /* ARM Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #define PRAGMA(x)                   _Pragma(#x)
    #define ALIGN(n)                    PRAGMA(data_alignment=n)
#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define ALIGN(n)                    __attribute__((aligned(n)))
#endif /* Compiler Related Definitions */
#endif

#define USB_DEBUG

#ifdef USB_DEBUG

/* Turn on some of these (set to non-zero) to debug kernel */
#ifndef USB_DEBUG_MIN
#define USB_DEBUG_MIN                   1
#endif

#ifndef USB_DEBUG_EP0
#define USB_DEBUG_EP0                   1
#endif

#ifndef USB_DEBUG_DESCRIPTOR
#define USB_DEBUG_DESCRIPTOR            1
#endif




#define USB_DEBUG_LOG(type, message)                                            \
do                                                                              \
{                                                                               \
    if (type)                                                                   \
        printf message;                                                         \
}                                                                               \
while (0)

#define USB_ASSERT(EX)                                                      \
if (!(EX))                                                                  \
{                                                                           \
    volatile char dummy = 0;                                                \
    printf("(%s) assert failed at %s:%d \n", #EX, __FUNCTION__, __LINE__);  \
    while (dummy == 0);                                                     \
}

    
#else
#define USB_ASSERT(EX)
#define USB_DEBUG_LOG(type, message)
#endif


#define USB_DEVICE_CLASS USB_DEVICE_CLASS_HID

//���ò����궨��
#define BIT_SET(BitNumber, Register)        (Register |=(1<<BitNumber))
#define BIT_CLR(BitNumber, Register)        (Register &=~(1<<BitNumber))



//EP0����������
#define EP0_SIZE            32

//EP1����
#define EP1_VALUE           _EP_IN
#define EP1_TYPE            INTERRUPT
#define EP1_SIZE            32
#define EP1_BUFF_OFFSET     0x18

//EP2����
#define EP2_VALUE           _EP_IN
#define EP2_TYPE            BULK
#define EP2_SIZE            32
#define EP2_BUFF_OFFSET     0x20

//EP3����
#define EP3_VALUE           _EP_OUT
#define EP3_TYPE            BULK
#define EP3_SIZE            32
#define EP3_BUFF_OFFSET     0x28



//DBT��MCU����
#define kMCU      0x00
//DBT��SIE(USB)ģ�����
#define kSIE      0x80

//BDT������DATA0��ʽ
#define kUDATA0   0x88
//BDT������DATA1��ʽ
#define kUDATA1   0xC8

//USB�����У�����PID����
#define mSETUP_TOKEN    0x0D
#define mOUT_TOKEN      0x01
#define mIN_TOKEN       0x09


/* Standard requests */
#define GET_STATUS        (0)
#define CLEAR_FEATURE     (1)
#define SET_FEATURE       (3)
#define SET_ADDRESS       (5)
#define GET_DESCRIPTOR    (6)
#define SET_DESCRIPTOR    (7)
#define GET_CONFIGURATION (8)
#define SET_CONFIGURATION (9)
#define GET_INTERFACE     (10)
#define SET_INTERFACE     (11)

/* bmRequestType.dataTransferDirection */
#define HOST_TO_DEVICE (0)
#define DEVICE_TO_HOST (1)

/* bmRequestType.Type*/
#define STANDARD_TYPE  (0)
#define CLASS_TYPE     (1)
#define VENDOR_TYPE    (2)
#define RESERVED_TYPE  (3)

/* bmRequestType.Recipient */
#define DEVICE_RECIPIENT    (0)
#define INTERFACE_RECIPIENT (1)
#define ENDPOINT_RECIPIENT  (2)
#define OTHER_RECIPIENT     (3)

/* Descriptors */
#define DESCRIPTOR_TYPE(wValue)  (wValue >> 8)
#define DESCRIPTOR_INDEX(wValue) (wValue & 0xff)


#define DEVICE_DESCRIPTOR         1
#define CONFIGURATION_DESCRIPTOR  2
#define STRING_DESCRIPTOR         3
#define INTERFACE_DESCRIPTOR      4
#define ENDPOINT_DESCRIPTOR       5
#define REPORT_DESCRIPTOR         0x22


#define USB_DEVICE_CLASS_AUDIO        1
#define USB_DEVICE_CLASS_CDC          2
#define USB_DEVICE_CLASS_HID          3
#define USB_DEVICE_CLASS_PHY          4
#define USB_DEVICE_CLASS_IMAGE        5
#define USB_DEVICE_CLASS_MASS_STORAGE 6
#define USB_DEVICE_CLASS_HUB          7
#define USB_DEVICE_CLASS_CDC_DATA     8
#define USB_DEVICE_CLASS_SMARTCARD    9

/*string offset*/
#define STRING_OFFSET_LANGID            (0)
#define STRING_OFFSET_IMANUFACTURER     (1)
#define STRING_OFFSET_IPRODUCT          (2)
#define STRING_OFFSET_ISERIAL           (3)
#define STRING_OFFSET_ICONFIGURATION    (4)
#define STRING_OFFSET_IINTERFACE        (5)


enum
{
    EP0,
    EP1,
    EP2,
    EP3,
    EP4,
    EP5,
    DUMMY,
    LOADER
};

typedef enum {ATTACHED, POWERED, DEFAULT, ADDRESS, CONFIGURED} DEVICE_STATE;

typedef struct {
    volatile DEVICE_STATE state;
    uint8_t configuration;
    bool suspended;
} USB_DEVICE;

enum
{
    kUSB_IN,
    kUSB_OUT
};

enum
{
    bEP0OUT_ODD,
    bEP0OUT_EVEN,
    bEP0IN_ODD,
    bEP0IN_EVEN,
    bEP1OUT_ODD,
    bEP1OUT_EVEN,
    bEP1IN_ODD,
    bEP1IN_EVEN,
    bEP2OUT_ODD,
    bEP2OUT_EVEN,
    bEP2IN_ODD,
    bEP2IN_EVEN,
    bEP3OUT_ODD,
    bEP3OUT_EVEN,
    bEP3IN_ODD,
    bEP3IN_EVEN
};

typedef union _tBDT_STAT
{
    uint8_t _byte;
	//���͵�MCU�����ֶ�
    struct{
        uint8_t :1;
        uint8_t :1;
        uint8_t BSTALL:1;              //OTGģ�鷢��һ������Э��
        uint8_t DTS:1;                 //
        uint8_t NINC:1;                //�������ݻ�����ʱ��DMA���治���������ĵ�ַ
        uint8_t KEEP:1;                //
        uint8_t DATA:1;                //���ͻ������DATA0/DATA1����USBģ�鲻�ı��λ
        uint8_t UOWN:1;                //BDT����Ȩ UOWN=1 USBģ��ӵ�У�UOWN=0 ΢������ӵ��
    }McuCtlBit;
    //���ܿ����ֶ� 
    struct{
        uint8_t    :2;
        uint8_t PID:4;                 //����־
        uint8_t    :2;
    }RecPid;
} tBDT_STAT,*ptBDT_STAT;                            //��������������ṹ��


typedef struct _tBDT
{
    tBDT_STAT Stat;
    uint8_t  dummy;
    uint16_t Cnt;     //���ܵ����ֽ���
    uint32_t bufAddr;    //��������ַ         
} tBDT,*ptBDT;


typedef struct {
    struct {
        uint8_t dataTransferDirection;
        uint8_t Type;
        uint8_t Recipient;
    } bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} SETUP_PACKET;

typedef struct _tUSB_Setup 
{
       uint8_t bmRequestType; 
       uint8_t bRequest;      
       uint8_t wValue_l;      //�ִ�С�ֶ�,��������Ĳ�ͬ����ͬ
       uint8_t wValue_h;      
       uint8_t wIndex_l;      //�ִ�С�ֶ�,��������Ĳ�ͬ����ͬ,ͨ���Ǵ���������λ����
       uint8_t wIndex_h;
       uint8_t wLength_l;     //��������ݽ׶Σ������ʾ��Ҫ������ֽڴ�С
       uint8_t wLength_h;
}tUSB_Setup;


//������ʵ�ֵĽӿں���

void USB_EP_IN_Transfer(uint8_t uint8_tEP,uint8_t *puint8_tDataPointer,uint8_t uint8_tDataSize);
uint16_t USB_EP_OUT_SizeCheck(uint8_t uint8_tEP);
void USB_EnableInterface(void);
uint8_t USB_Init(void);

#endif

#endif

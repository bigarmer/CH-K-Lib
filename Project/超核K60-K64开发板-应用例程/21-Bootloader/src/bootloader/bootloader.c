#include "bootloader.h"
#include "def.h"
#include "flash.h"
#include "message_manage.h"
#include "bootloader_util.h"
#include <string.h>
#include "common.h"

#define RCV_OK              0xA5
#define RCV_ERR             0x5A
#define RCV_SKIP            0x5B

typedef  void (*pFunction)(void);



//����֡��ʽ
#pragma pack(1)
typedef struct
{
    uint8_t  cmd;
    uint16_t currentPkgNo;
    uint8_t content[8189];
} DataFrame_t;


//оƬ��Ϣ֡���ݲ��ָ�ʽ
#pragma pack(1)
typedef struct
{
    uint8_t cmd;
    uint32_t FCFG1;
    uint32_t FCFG2;
    uint32_t SDID;
    uint8_t  Reserved[3];
} ChipInfo_t;

//��Ӧ֡���ݲ��ָ�ʽ
#pragma pack(1)
typedef struct
{
    uint8_t cmd;
    uint16_t pkg_no;
    uint8_t status;
//    uint8_t Reserved[3];
} ResponseFrame_t;


//ͨ�����ݰ��ĸ�ʽ
#pragma pack(1)
typedef struct
{
    uint8_t cmd;
    uint8_t content[8191];
} GenericRecvFrame_t;

//���յ�������У��֡��ʽ
#pragma pack(1)
typedef struct
{
    uint8_t cmd;
    uint32_t fcs;
} VerificationFrame_t;


/* Ӧ�ó���ṹ�� */
typedef struct
{
    uint32_t 	app_size;       /* Ӧ�ó��򳤶� */
    uint32_t 	app_crc;        /* Ӧ��CRC����ֵ */
    uint32_t	app_start_addr; /* ������ʼ��ַ */
} AppType_t;


//Ӧ�ó�����Ϣ�ṹ��
#pragma pack(1)
typedef struct
{
    uint8_t  cmd;
    uint32_t app_len;
    uint16_t total_pkg;
    uint8_t Reserved[1];
} AppInfoType_t;


//������Ϣ����Ļص�����
typedef void(*pFuncCallback)(MessageType_t *pMsg);



static MessageType_t* pMsg;         /* ��Ϣָ�� */
static pFuncCallback pExecFun;      /* �ص��������� */
static uint8_t CacheFirstPg[FLASH_PAGE_SIZE];/* ��һҳ�����ݻ��� */
 uint32_t runAferDelayMs;


/* Systick�������ĳ�ʼ�� */
static void SysTick_Cfg(uint32_t ticks);

/* ����������� */
static pFuncCallback MsgCallbackFind(MessageType_t* pMsg);

/* ������Ϣ������ */
static void ProcessUartMsg(MessageType_t* pMsg);

/* оƬ��Ϣ������ */
static void ProcessChiMsg(MessageType_t* pMsg);

/* Ӧ�ó�����Ϣ������ */
static void ProcessAppInfoMsg(MessageType_t* pMsg);

/* ����������Ϣ������ */
static void ProcessTransDataMsg(MessageType_t* pMsg);

/* Ӧ�ü�����Ϣ������ */
static void ProcessAppVerificationMsg(MessageType_t* pMsg);

/* Ӧ�ó����麯�� */
static void ProccessAppCheckMsg(MessageType_t* pMsg);

/* ��ʱ�¼������� */
 void ProcessTimeOutEvt(void);

/* Tick������ */
 void TickProcess(void);

static void ProcessChipInfoMsg(MessageType_t* pMsg);
static uint32_t Fn_AppVerification(uint8_t* firstPgStart, uint16_t pkgNo);
void Fn_GoToUserApplication(__IO uint32_t app_start_addr);
static uint8_t Fn_IsAppValid(void);


/*
    Ѱ�Һ��ʵ���Ϣ�ص������������ظú���
*/
static pFuncCallback MsgCallbackFind(MessageType_t* pMsg)
{
    pFuncCallback pCallBack = NULL;

    switch(pMsg->m_Command)
    {
    case CMD_SERIALPORT:
        pCallBack = ProcessUartMsg;
        break;

    case CMD_CHIPINFO:
        pCallBack = ProcessChipInfoMsg;
        break;

    case CMD_APP_INFO:
        pCallBack = ProcessAppInfoMsg;
        break;

    case CMD_TRANS_DATA:
        pCallBack = ProcessTransDataMsg;
        break;

    case CMD_VERIFICATION:
        pCallBack = ProcessAppVerificationMsg;
        break;

    case CMD_APP_CHECK:
        pCallBack = ProccessAppCheckMsg;
        break;

    default:
        pCallBack = NULL;
        break;
    }

    return pCallBack;
}


static void ProcessUartMsg(MessageType_t* pMsg)
{
    GenericRecvFrame_t *pRcvFrame;
    MessageType_t m_Msg;

    pRcvFrame = (GenericRecvFrame_t *)(((uint8_t*)pMsg->pMessage)) ;
    M_Control.bootloaderFlg = 1;

    m_Msg.m_Command = pRcvFrame->cmd;
    m_Msg.pMessage = pRcvFrame;
		
    fn_msg_push(m_Msg);
}



static void ProcessChipInfoMsg(MessageType_t* pMsg)
{
    ChipInfo_t infoFrame;

    infoFrame.cmd = CMD_CHIPINFO;
    infoFrame.FCFG1 = SIM->FCFG1;
    infoFrame.FCFG2 = SIM->FCFG2;
    infoFrame.SDID  = SIM->SDID;

    Fn_SendResponse((uint8_t*)&infoFrame, 0, sizeof(infoFrame));
}

static void ProcessAppInfoMsg(MessageType_t* pMsg)
{
    AppInfoType_t*pAppInfo;
    ResponseFrame_t rspFrame = {CMD_APP_INFO, 0, RCV_OK};

    pAppInfo = (AppInfoType_t*)pMsg->pMessage;

    M_Control.app_length = pAppInfo->app_len;
    M_Control.total_pkg = pAppInfo->total_pkg;
    M_Control.write_addr = APP_START_ADDR;
    M_Control.currentPkgNo = 0;
    M_Control.retryCnt = 0;

    Fn_SendResponse((uint8_t*)&rspFrame, 0, sizeof(rspFrame));
}

static void ProcessTransDataMsg(MessageType_t* pMsg)
{
    DataFrame_t* pDataFrame;
    ResponseFrame_t rspFrame;
    uint8_t needWrite = 0;

    pDataFrame = (DataFrame_t*)pMsg->pMessage;
    rspFrame.cmd = CMD_TRANS_DATA;
    //�������һ���������ظ���
    if((M_Control.currentPkgNo == (pDataFrame->currentPkgNo-1)) || (M_Control.currentPkgNo == pDataFrame->currentPkgNo))
    {
        if(M_Control.currentPkgNo != pDataFrame->currentPkgNo)
        {
            needWrite = 1;
            M_Control.write_addr = APP_START_ADDR + (pDataFrame->currentPkgNo-1)*FLASH_PAGE_SIZE;
        }
        else if(M_Control.op_state != RCV_OK)
        {
            needWrite = 1;
            M_Control.write_addr = APP_START_ADDR + pDataFrame->currentPkgNo*FLASH_PAGE_SIZE;
        }
        if(needWrite)
        {
            FLASH_EraseSector(M_Control.write_addr);
            //�����һҳ��ֱ������������֮���ٴ��һҳ���������Է�ֹд��һ����������벻�������������
            if(M_Control.write_addr == APP_START_ADDR)
            {
                memcpy(CacheFirstPg, pDataFrame->content, FLASH_PAGE_SIZE);
                M_Control.op_state = RCV_OK;

            }

            else if(FLASH_WriteSector(M_Control.write_addr, pDataFrame->content, FLASH_PAGE_SIZE) == FLASH_OK)
            {
							
                if(memcmp((void*)M_Control.write_addr, pDataFrame->content, FLASH_PAGE_SIZE) == 0)
                {
                    M_Control.op_state = RCV_OK;
                }
                else
                {
                    M_Control.op_state = RCV_ERR;
                    M_Control.retryCnt++;
                }
                M_Control.op_state = RCV_OK;
            }

            if(M_Control.op_state == RCV_OK)
            {
                M_Control.retryCnt = 0;
                M_Control.currentPkgNo = pDataFrame->currentPkgNo;
            }
            else
            {
                M_Control.retryCnt++;
            }

            rspFrame.status = M_Control.op_state;
        }
        else
        {
            rspFrame.status = RCV_OK;
        }

        rspFrame.pkg_no = pDataFrame->currentPkgNo;
        if(M_Control.retryCnt<3)
        {
            Fn_SendResponse((uint8_t*)&rspFrame, 0, sizeof(rspFrame));
        }
    }
    //����յ��İ��Ų�����
    else
    {
    }
    for(volatile uint32_t delayCnt = 26000; delayCnt; delayCnt--);
}

/* 
    ��֤Ӧ�ó����Ƿ�Ϸ�
    ������õļ򵥵İ������ֽڼ���������һ��uint32_t���͵����ݵķ���
    һ�����������Ӧ��û������
*/
static uint32_t Fn_AppVerification(uint8_t* firstPgStart, uint16_t pkgNo)
{
    uint16_t pkgOffset = 0;
    uint32_t endOfVerificationAddr;
    uint32_t i;
    uint32_t fcs = 0;

    for(i=0; i< FLASH_PAGE_SIZE; i++)
    {
        fcs += firstPgStart[i];
    }
    pkgOffset = 1;

    if(pkgNo > pkgOffset)
    {
        endOfVerificationAddr = APP_START_ADDR + FLASH_PAGE_SIZE * pkgNo;
        for(i = APP_START_ADDR + FLASH_PAGE_SIZE; i < endOfVerificationAddr; i++)
        {
            fcs += (*(uint8_t*)i);
        }
    }

    return fcs;
}

/* ��֤Ӧ�ó����Ƿ������� */
static void ProcessAppVerificationMsg(MessageType_t* pMsg)
{
    VerificationFrame_t *pVFrame ;
    ResponseFrame_t rspFrame;
    MessageType_t m_Msg = {CMD_APP_CHECK, 0, 0, &m_Msg};

    pVFrame = (VerificationFrame_t *)pMsg->pMessage;

    if(M_Control.currentPkgNo == M_Control.total_pkg)
    {
        rspFrame.cmd = CMD_VERIFICATION;
        rspFrame.status = RCV_ERR;

        if(pVFrame->fcs == Fn_AppVerification(CacheFirstPg, M_Control.total_pkg))
        {
            if(FLASH_WriteSector(M_Control.write_addr, CacheFirstPg, FLASH_PAGE_SIZE) == FLASH_OK)
            {
                rspFrame.status = RCV_OK;
            }
        }

        Fn_SendResponse((uint8_t*)&rspFrame, 0, sizeof(rspFrame));

        fn_msg_push(m_Msg);
    }
}

/* �������Ƿ���Ч�������Ч�� ����ת��Ӧ�ó���ִ�� */
static void ProccessAppCheckMsg(MessageType_t* pMsg)
{
	uint32_t i;
	uint8_t* p;
    if(Fn_IsAppValid())
    {
        for(volatile uint32_t i=0; i<26000; i++);
        Fn_GoToUserApplication( APP_START_ADDR);
    }
}

static void ProcessTimeOutEvt(void)
{
    MessageType_t m_Msg = {CMD_APP_CHECK,0, 0, &m_Msg};

    M_Control.usart_timeout = 0;
    fn_msg_push(m_Msg);
}


static void SysTick_Cfg(uint32_t ticks)
{
    if (ticks > SysTick_LOAD_RELOAD_Msk)  return;            /* Reload value impossible */

    SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
    SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */                                                 /* Function successful */
}






static void TickProcess(void)
{
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
    {
        ++M_Control.usart_timeout;
    }
}

static uint8_t Fn_IsAppValid(void)
{
    if(*(uint32_t*)APP_START_ADDR != 0xFFFFFFFFU)
    {
        return 1;
    }
    return 0;
}




 void Fn_GoToUserApplication(__IO uint32_t app_start_addr)
{
    pFunction jump_to_application;
    uint32_t jump_addr;
    jump_addr = *(__IO uint32_t*)(app_start_addr + 4);  //RESET�ж�
    
    //���ڲ�����bootloader, �ʳ����jump_addr��ַӦ���� (0x5000, END_ADDR] ��Χ��
    if(app_start_addr != 0xfffffffful && (jump_addr > 0x5000 && jump_addr < END_ADDR))
    {

        jump_to_application = (pFunction)jump_addr;
        __set_MSP(*(__IO uint32_t*)app_start_addr); //ջ��ַ
        jump_to_application();
    }
}

uint8_t BootloaderInit(UART_Type *uartch, uint32_t buadrate, uint32_t delayMs)
{
    FLASH_Init();
    SysTick_Cfg(GetClock(kBusClock)/1000);
    pUARTx = uartch;
    fn_queue_init();
    
    runAferDelayMs = delayMs; 
    
    //�ȴ������ȶ�
    for(volatile uint32_t delayCnt = 2600; delayCnt; delayCnt--);
    
    if(pUARTx != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//���ڽ��ջ�����
uint8_t UARTRxBuf[32];
void BootloaderWaitUARTData(void)
{
				while(1)
				{
                    
					if(UART_ReadByte(0, (uint16_t*)UARTRxBuf) == 0) //����0���յ�����
					{
						pUARTx = UART0;
						Fn_RxProcData(UARTRxBuf[0]);
						break;
					}

					if (fn_msg_exist())
					{
						pMsg =  fn_msg_pop();

						//�ҵ����ʵ���Ϣ������
						pExecFun = MsgCallbackFind(pMsg);

						if(pExecFun != NULL)
						{
							pExecFun(pMsg);
							pExecFun = NULL;
						}
					}
							//��������趨�ĵȴ�ʱ����û������λ��ͨ�� 
					if((M_Control.usart_timeout >= runAferDelayMs) && (!M_Control.bootloaderFlg))
					{
							ProcessTimeOutEvt();
					}
					TickProcess();
				}
}

void BootloaderProc(void)
{
    uint16_t i,RecCnt;
    
    if(UART_ReadByte(0, (uint16_t*)UARTRxBuf) == 0)
    {
        Fn_RxProcData(UARTRxBuf[0]);
    }
    if (fn_msg_exist())
    {
        pMsg =  fn_msg_pop();
        pExecFun = MsgCallbackFind(pMsg);
        if(pExecFun != NULL)
        {
            pExecFun(pMsg);
            pExecFun = NULL;
        }
    }

    if((M_Control.usart_timeout >= runAferDelayMs) && (!M_Control.bootloaderFlg))
    {
        ProcessTimeOutEvt();
    }
    TickProcess();
}


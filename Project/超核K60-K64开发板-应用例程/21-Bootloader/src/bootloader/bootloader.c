#include "bootloader.h"
#include "def.h"
#include "flash.h"
#include "mq.h"
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
    uint8_t content[4096];
} DataFrame_t;


//оƬ��Ϣ֡���ݲ��ָ�ʽ
#pragma pack(1)
typedef struct
{
    uint8_t cmd;
    uint32_t FCFG1;
    uint32_t FCFG2;
    uint32_t SDID;
    uint32_t FlashPageSize;
} ChipInfo_t;

//��Ӧ֡���ݲ��ָ�ʽ
#pragma pack(1)
typedef struct
{
    uint8_t cmd;
    uint16_t pkg_no;
    uint8_t status;
} ResponseFrame_t;


//ͨ�����ݰ��ĸ�ʽ
#pragma pack(1)
typedef struct
{
    uint8_t cmd;
    uint8_t content[8191];
} GenericRecvFrame_t;


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
typedef void(*pFuncCallback)(msg_t *pMsg);


static msg_t* pMsg;         /* ��Ϣָ�� */
static pFuncCallback pExecFun;      /* �ص��������� */
uint32_t SysTimeOut;


/* Systick�������ĳ�ʼ�� */
static void SysTick_Cfg(uint32_t ticks);

/* ����������� */
static pFuncCallback MsgCallbackFind(msg_t* pMsg);

/* ������Ϣ������ */
static void ProcessUartMsg(msg_t* pMsg);

/* Ӧ�ó�����Ϣ������ */
static void ProcessAppInfoMsg(msg_t* pMsg);

/* ����������Ϣ������ */
static void ProcessTransDataMsg(msg_t* pMsg);

/* Ӧ�ü�����Ϣ������ */
static void ProcessAppVerificationMsg(msg_t* pMsg);

/* Ӧ�ó����麯�� */
static void ProccessAppCheckMsg(msg_t* pMsg);

/* ��ʱ�¼������� */
 void ProcessTimeOutEvt(void);

/* Tick������ */
 void TickProcess(void);

static void ProcessChipInfoMsg(msg_t* pMsg);
void GoToUserApp(__IO uint32_t app_start_addr);


/*
    Ѱ�Һ��ʵ���Ϣ�ص������������ظú���
*/
static pFuncCallback MsgCallbackFind(msg_t* pMsg)
{
    pFuncCallback pCallBack = NULL;

    switch(pMsg->cmd)
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

static void ProcessUartMsg(msg_t* pMsg)
{
    GenericRecvFrame_t *pRcvFrame;
    msg_t m_Msg;

    pRcvFrame = (GenericRecvFrame_t *)(((uint8_t*)pMsg->pMessage)) ;
    M_Control.IsBootMode = true;

    m_Msg.cmd = pRcvFrame->cmd;
    m_Msg.pMessage = pRcvFrame;

    mq_push(m_Msg);
}

static void ProcessChipInfoMsg(msg_t* pMsg)
{
    ChipInfo_t info;

    info.cmd = CMD_CHIPINFO;
    info.FCFG1 = SIM->FCFG1;
    info.FCFG2 = SIM->FCFG2;
    info.SDID  = SIM->SDID;
    info.FlashPageSize = FLASH_GetSectorSize();
    SendResp((uint8_t*)&info, 0, sizeof(info));
}

static void ProcessAppInfoMsg(msg_t* pMsg)
{
    ResponseFrame_t Resp = {CMD_APP_INFO, 0, RCV_OK};

    M_Control.write_addr = APP_START_ADDR;
    M_Control.currentPkgNo = 0;
    M_Control.retryCnt = 0;

    SendResp((uint8_t*)&Resp, 0, sizeof(Resp));
}

static void ProcessTransDataMsg(msg_t* pMsg)
{
    DataFrame_t* pDataFrame;
    ResponseFrame_t Resp;
    uint8_t needWrite = 0;

    pDataFrame = (DataFrame_t*)pMsg->pMessage;
    Resp.cmd = CMD_TRANS_DATA;
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
            if(FLASH_WriteSector(M_Control.write_addr, pDataFrame->content, FLASH_PAGE_SIZE) == FLASH_OK)
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

            Resp.status = M_Control.op_state;
        }
        else
        {
            Resp.status = RCV_OK;
        }

        Resp.pkg_no = pDataFrame->currentPkgNo;
        if(M_Control.retryCnt < 3)
        {
            SendResp((uint8_t*)&Resp, 0, sizeof(Resp));
        }
    }
    //����յ��İ��Ų�����
    else
    {
        
    }
}



static void ProcessAppVerificationMsg(msg_t* pMsg)
{
    ResponseFrame_t Resp = {CMD_VERIFICATION, 0, RCV_OK};
    msg_t m_Msg = {CMD_APP_CHECK, 0, 0, &m_Msg};

    SendResp((uint8_t*)&Resp, 0, sizeof(Resp));
    mq_push(m_Msg);
}

/* �������Ƿ���Ч�������Ч�� ����ת��Ӧ�ó���ִ�� */
static void ProccessAppCheckMsg(msg_t* pMsg)
{
    ResponseFrame_t Resp = {CMD_VERIFICATION, 0, RCV_OK};
    SendResp((uint8_t*)&Resp, 0, sizeof(Resp));
    if(*(uint32_t*)APP_START_ADDR != 0xFFFFFFFF)
    {
        GoToUserApp( APP_START_ADDR);
    }
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
        ++M_Control.timeout;
    }
}


void GoToUserApp(__IO uint32_t app_start_addr)
{
    pFunction jump_to_application;
    uint32_t jump_addr;
    jump_addr = *(__IO uint32_t*)(app_start_addr + 4);  //RESET�ж�
    
    //���ڲ�����bootloader, �ʳ����jump_addr��ַӦ���� (0x5000, END_ADDR] ��Χ��
    if(app_start_addr != 0xFFFFFFFFUL && (jump_addr > APP_START_ADDR))
    {

        jump_to_application = (pFunction)jump_addr;
        __set_MSP(*(__IO uint32_t*)app_start_addr); //ջ��ַ
        SCB->VTOR = app_start_addr;
        jump_to_application();
    }
}

uint32_t BootloaderInit(uint32_t timeOut)
{
    FLASH_Init();
    SysTick_Cfg(GetClock(kBusClock)/1000);
    pUARTx = UART0;
    mq_init();
    
    SysTimeOut = timeOut; 
    return 0;
}

void BootloaderProc(void)
{
    uint8_t data;
    if(UART_ReadByte(0, (uint16_t*)&data) == 0)
    {
        Fn_RxProcData(data);
    }
    if (mq_exist())
    {
        pMsg =  mq_pop();
        pExecFun = MsgCallbackFind(pMsg);
        if(pExecFun != NULL)
        {
            pExecFun(pMsg);
            pExecFun = NULL;
        }
    }

    if((M_Control.timeout >= SysTimeOut) && (!M_Control.IsBootMode))
    {
        msg_t m_Msg = {CMD_APP_CHECK,0, 0, &m_Msg};
        M_Control.timeout = 0;
        mq_push(m_Msg);
    }
    
    TickProcess();
}


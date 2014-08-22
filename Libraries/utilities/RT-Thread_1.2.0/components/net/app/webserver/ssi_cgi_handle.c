#include "ssi_cgi_handle.h"
#include "httpd.h"
#include "string.h"
#include "uart.h"
#include "stdio.h"
#include "fs.h"



#define INDEX_PAGE_SET_CGI_RSP_URL        "/index.html"

#define RESPONSE_PAGE_SET_CGI_RSP_URL     "/response.ssi"


#define NUM_CONFIG_CGI_URIS     (sizeof(ppcURLs ) / sizeof(tCGI))
#define NUM_CONFIG_SSI_TAGS     (sizeof(ppcTags) / sizeof (char *))


static char *LED_RED_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *LED_GREEN_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static char *Orther_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );


static int SSIHandler ( int iIndex, char *pcInsert, int iInsertLen );


// typedef struct{
//  unsigned char *bufptr;   //ָ����Ӧ��������ָ��
//  int buf_user;   //ָ�򻺳�����ʹ�����
// }RESPONSE_BUF;

// RESPONSE_BUF  response_buf;

static const tCGI ppcURLs[] =
{

    { "/led_red.cgi",      LED_RED_CGIHandler },  
    { "/led_green.cgi",    LED_GREEN_CGIHandler },    
    { "/orther.cgi",       Orther_CGIHandler },     
};

static const char *ppcTags[] =
{
    "onetree",
	  "filest"
};

enum ssi_index_s
{
    SSI_INDEX_ONETREE_GET = 0, //�ñ��ӦppcTags[]������
    SSI_INDEX_FILEST_GET

} ;



//����->�ַ���ת������
//��num����(λ��Ϊlen)תΪ�ַ���,�����buf����
//num:����,����
//buf:�ַ�������
//len:����
void num2str(uint16_t num, uint8_t *buf,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	{
//		buf[i]=(num/LCD_Pow(10,len-i-1))%10+'0';
	}
}
//��ȡSTM32�ڲ��¶ȴ��������¶�
//temp:����¶��ַ������׵�ַ.��"28.3";
//temp,���ٵ���5���ֽڵĿռ�!
void get_temperature(uint8_t*temp)
{			  
	uint16_t t;
	float temperate;		   
//	temperate=Get_Adc_Average(ADC_CH_TEMP,10);			 
	temperate=temperate*(3.3/4096);			    											    
	temperate=(1.43-temperate)/0.0043+25;	//�������ǰ�¶�ֵ
	t=temperate*10;//�õ��¶�
	num2str(t/10,temp,2);							   
	temp[2]='.';temp[3]=t%10+'0';temp[4]=0;	//�����ӽ�����
}
//��ȡRTCʱ��
//time:���ʱ���ַ���,����:"2012-09-27 12:33:00"
//time,���ٵ���17���ֽڵĿռ�!
void get_time(uint8_t*time)
{	
	//RTC_Get();
	time[4]='-';time[7]='-';time[10]=' ';
	time[13]=':';time[16]=':';time[19]=0;			//�����ӽ�����
//	num2str(calendar.w_year,time,4);	//���->�ַ���
//	num2str(calendar.w_month,time+5,2); //�·�->�ַ���	 
//	num2str(calendar.w_date,time+8,2); 	//����->�ַ���
//	num2str(calendar.hour,time+11,2); 	//Сʱ->�ַ���
//	num2str(calendar.min,time+14,2); 	//����->�ַ���		
//  num2str(calendar.sec,time+17,2); 	//��->�ַ���		
}

//��ʼ��ssi��cgi
void init_ssi_cgi(void){
	
 http_set_cgi_handlers(ppcURLs , NUM_CONFIG_CGI_URIS);	
 http_set_ssi_handler (SSIHandler, ppcTags, NUM_CONFIG_SSI_TAGS );


}

//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests iocontrol.cgi.
//
//*****************************************************************************
static int FindCGIParameter(const char *pcToFind, char *pcParam[], int iNumParams)
{
    int iLoop;

    for(iLoop = 0; iLoop < iNumParams; iLoop++)
    {
        if(strcmp(pcToFind, pcParam[iLoop]) == 0)
        {
            return(iLoop);
        }
    }

    return(-1);
}

//���������������
void  clear_response_bufer(unsigned char *buffer){
  memset(buffer,0,strlen((const char*)buffer));
}
int num=100;

//��ƴ�����
static char *LED_RED_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
    int  index;
    index = FindCGIParameter ( "red", pcParam, iNumParams );
    if(index != -1)
    {
			clear_response_bufer(data_response_buf);      //���������������
            rt_kprintf("LED0 = !LED0\r\n");
//			LED0 = !LED0;
		//	if(!LED0){
	//				strcat((char *)(data_response_buf),"/img/red.gif");
  //    }else{
  //        strcat((char *)(data_response_buf),"/img/black.gif");
  //    }
        
    }
    return RESPONSE_PAGE_SET_CGI_RSP_URL;
}


//�̵ƴ�����
static char *LED_GREEN_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
    int  index;
    index = FindCGIParameter ( "green", pcParam, iNumParams );
    if(index != -1)
    {
			// num++;
			 //snprintf((char *)(data_response_buf),4,"%d",num);
			// printf("data_response_buf:%s\r\n",data_response_buf);
			 //printf("green:%s\r\n",pcValue[index]);	
			
			 clear_response_bufer(data_response_buf);      //���������������
                    rt_kprintf("LED0 = !LED0\r\n");
//       LED1 = !LED1;
//			 if(!LED1){
//					strcat((char *)(data_response_buf),"/img/green.gif");
//       }else{
//          strcat((char *)(data_response_buf),"/img/black.gif");
//       }
        
    }
    return RESPONSE_PAGE_SET_CGI_RSP_URL;
}


//�̵ƴ�����
static char *Orther_CGIHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
    uint8_t buf[20];
    clear_response_bufer(data_response_buf);      //���������������

	  get_temperature(data_response_buf);
	  strcat((char *)(data_response_buf),";");
	  get_time(buf);
	  strcat((char *)(data_response_buf),buf);
    return RESPONSE_PAGE_SET_CGI_RSP_URL;
}


//*****************************************************************************
//
// This function is called by the HTTP server whenever it encounters an SSI
// tag in a web page.  The iIndex parameter provides the index of the tag in
// the ppcTags array. This function writes the substitution text
// into the pcInsert array, writing no more than iInsertLen characters.
//
//*****************************************************************************
static int SSIHandler ( int iIndex, char *pcInsert, int iInsertLen )
{   
	
	 
    switch(iIndex)
    {
        case SSI_INDEX_ONETREE_GET:

            break;
            
        case SSI_INDEX_FILEST_GET:
            
            break;       
        default:
            strcpy( pcInsert , "??" );
            
    }
 
    return strlen ( pcInsert );
}

static void webserver_thread(void *parameter)
{
    httpd_init();				//��ʼ��webserver
    init_ssi_cgi();
    while(1)
    {
        rt_thread_delay(5);
    }
}

static void webserver_start(void)
{
	rt_thread_t tid;

	rt_kprintf("\n\n\tNow, Initializing The WEB File System...\n");

	tid = rt_thread_create("webserver",
		webserver_thread, RT_NULL,
		4096, 30, 5);

	if (tid != RT_NULL)
		rt_thread_startup(tid);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(webserver_start, start web server)
#endif

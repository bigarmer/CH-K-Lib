#include "test_common.h"

#include "stdint.h"

 uint32_t Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	   

 uint8_t CHS_Str2Num(uint8_t *str,uint32_t *res)
{
	uint32_t t;
	uint8_t bnum=0;	 //���ֵ�λ��
	uint8_t *p;		  
	uint8_t hexdec=10;//Ĭ��Ϊʮ��������
	p=str;
	*res=0;//����.
	while(1)
	{
		if((*p<='9'&&*p>='0')||(*p<='F'&&*p>='A')||(*p=='X'&&bnum==1))//�����Ϸ�
		{
			if(*p>='A')hexdec=16;	//�ַ����д�����ĸ,Ϊ16���Ƹ�ʽ.
			bnum++;					//λ������.
		}else if(*p=='\0')break;	//����������,�˳�.
		else return 1;				//��ȫ��ʮ���ƻ���16��������.
		p++; 
	} 
	p=str;			    //���¶�λ���ַ�����ʼ�ĵ�ַ.
	if(hexdec==16)		//16��������
	{
		if(bnum<3)return 2;			//λ��С��3��ֱ���˳�.��Ϊ0X��ռ��2��,���0X���治������,������ݷǷ�.
		if(*p=='0' && (*(p+1)=='X'))//������'0X'��ͷ.
		{
			p+=2;	//ƫ�Ƶ�������ʼ��ַ.
			bnum-=2;//��ȥƫ����	 
		}else return 3;//��ʼͷ�ĸ�ʽ����
	}else if(bnum==0)return 4;//λ��Ϊ0��ֱ���˳�.	  
	while(1)
	{
		if(bnum)bnum--;
		if(*p<='9'&&*p>='0')t=*p-'0';	//�õ����ֵ�ֵ
		else t=*p-'A'+10;				//�õ�A~F��Ӧ��ֵ	    
		*res+=t*Pow(hexdec,bnum);		   
		p++;
		if(*p=='\0')break;//���ݶ�������.	
	}
	return 0;//�ɹ�ת��
}
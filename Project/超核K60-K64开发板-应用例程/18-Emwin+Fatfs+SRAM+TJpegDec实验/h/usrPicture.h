#ifndef _USRPICTURE_H
#define _USRPICTURE_H

#include "stdint.h"

/**
 * \struct Picture �ṹ��
 * \brief ���ڴ��ͼƬ�ĵ�ַ�ʹ�С
 */
typedef struct{
	const unsigned char *picAddr;
	uint32_t picSize;
}Picture;

/* C�ļ������ͼƬ��Ϣ */
extern Picture jpgHankeku;

#endif

#ifndef _USRJPEGIO_H
#define _USRJPEGIO_H

	#include "tjpgd.h"
	#include "ff.h"
	#include "uart.h"
	#include "GUI.h"

	#if 0
		#define JPEG_TRACE printf
	#else
		#define JPEG_TRACE(...)
	#endif
	
	typedef struct {
		FIL *fp;      	/* ָ�������ļ���ָ�루JPEG�ļ���*/  
		BYTE *fbuf;    	/* ָ�������������ָ��  */
		UINT wfbuf;    	/* ����������Ŀ��[pix] */
	} IODEV;
	
	void jpegDispFromFile(const char *filename);
	static UINT JpegInFunc(JDEC *jd, BYTE *buff, UINT num);
	static UINT JpegOutFunc(JDEC *jd, void * bitmap, JRECT *rect);
	
#endif

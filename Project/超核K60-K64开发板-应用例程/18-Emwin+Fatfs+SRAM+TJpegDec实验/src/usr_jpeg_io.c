#include "usrJpegIO.h"

BYTE jpgBuff[1024*5] __attribute__((aligned(4))); 	/* ǿ�Ʊ�����4�ֽڶ��� */

/**
 * @brief  Read JPEG file from the file system fatfs
 * @param[in]  filename		Pointer to the object path
 * @retval	NONE
 */
void jpegDispFromFile(const TCHAR *filename)
{
	JDEC jdec;
	JRESULT res_Tjpg;
	IODEV devid;
	BYTE scale;
	
	FIL fnew;
	FATFS fs;
	FRESULT res;
	
	JPEG_TRACE("enter the jpeg_maintask\n");
	/* open a file of jpeg */
	res = f_mount(&fs, "0:", 0);
	res = f_open(&fnew, filename, FA_READ);
	devid.fp = &fnew;
	JPEG_TRACE("devid.fp = %p %p\n", devid.fp, &devid);
	
	if(res != FR_OK)
	{
		JPEG_TRACE("failed to open the %s ErrprID = %d\n", filename, res);
		f_close(&fnew);
	}
	else
	{
		JPEG_TRACE("open the %s successfully! State = %d\n", filename, res);
	}

	/* ִ�н����׼������������jd_prepare���� */
	res_Tjpg = jd_prepare(&jdec, JpegInFunc, jpgBuff, sizeof(jpgBuff), &devid);
	if(res_Tjpg == JDR_OK)
	{
		for (scale = 0; scale < 3; scale++) //ȷ�����ͼ��ı�������
        { 
            if ((jdec.width >> scale) <= 240 && (jdec.height >> scale) <= 320)  
            { 
                break; 
            } 
        } 
		//ִ�н��빤��������TjpgDecģ���jd_decomp����
		res_Tjpg = jd_decomp(&jdec, JpegOutFunc, scale);  
	}
	else
	{
		f_close(&fnew);
		JPEG_TRACE("failed to decomp the jpg file,errorId = %d\n", res_Tjpg);
	}
	f_close(&fnew);
	res = f_mount(NULL, "0:", 0);
}

/**
 * @brief  Read JPEG data from the input stream
 * @param[in]  jd 		Pointer to the decompression object
 * @param[in]  buff  	Pointer to buffer to store the read data
 * @param[in]  num  	Number of bytes to read
 * @retval 				Returns number of bytes read/removed
 */
static UINT JpegInFunc(JDEC *jd, BYTE *buff, UINT num)
{
	UINT rb;
	IODEV *devTmp = (IODEV*)jd->device;
	FIL *fil = devTmp->fp;
	if(buff)
	{
		FRESULT res;
		res = f_read(fil, buff, num, &rb);
		if(res == FR_OK)
			return rb;		/* ���ض�ȡ�����ֽ��� */
		else 
			return 0;
	}
	else
	{
		return (f_lseek(fil, f_tell(fil) + num) == FR_OK) ? num : 0;
	}
}


/**
 * @brief  				write decompressed pixels to the output device
 * @param[in]  jd 		Pointer to the decompression object
 * @param[in]  bitmap  	RGB bitmap to be output
 * @param[in]  rect  	Rectangular region to output
 * @retval 				Normally returns 1
 * \attention 			It lets TJpgDec to continue the decompressing process. When a 0 is returned, 
 * 						the jd_decomp function aborts with JDR_INTR. This is useful to interrupt the
 * 						decompression process.
 */
static UINT JpegOutFunc(JDEC *jd, void *bitmap, JRECT *rect)
{
	GUI_BITMAP bmp;
	
	jd = jd;

	bmp.XSize = rect->right - rect->left + 1;
	bmp.YSize = rect->bottom - rect->top + 1;
	bmp.BytesPerLine = bmp.XSize*2;		/* ��Ϊ��RGB565�ı��뷽ʽ������ÿһ�е��ֽ����ǿ�ȵ����� */
	bmp.BitsPerPixel = 16;				/* ��Ϊ��RGB565�ı��뷽ʽ������ÿһ�����ص��λ����16bits */
	bmp.pData = (U8 *)bitmap;  			/* ָ��ָ��Ķ�����ֻ���ģ�����ָ����Ը�ֵ */
	bmp.pPal = NULL;
	bmp.pMethods = GUI_DRAW_BMP565;	
	/* ������õ���������ת��ΪemWin�����ܵ��õ����ͣ������п��Բο�GUI_BITMAP���͵Ķ��巽ʽ */
	GUI_DrawBitmap(&bmp, rect->left, rect->top);
	
	return 1;
}

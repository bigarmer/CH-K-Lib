#include <rtthread.h>
#include <rthw.h>

/* DFS */
#include <dfs_fs.h>
#include <dfs_posix.h>
#include <dfs_elm.h>

#include <drivers/spi.h>

extern void gui_thread_entry(void* parameter);
extern void led_thread_entry(void* parameter);

void init_thread_entry(void* parameter)
{
    struct rt_spi_device *spi_device;
    rt_thread_t thread;
    rt_err_t result;
    
    rt_kprintf("start init thread @ LWIP:\r\n");
#ifdef RT_USING_LWIP
	/* initialize lwip stack */
    eth_system_device_init();
	/* register ethernetif device */
    rt_hw_ksz8041_init();
	/* initialize lwip system */
	lwip_system_init();
	rt_kprintf("TCP/IP initialized!\n");
#endif  /* RT_USING_LWIP */
    
    rt_kprintf("start init thread @ DFS:\r\n");
#ifdef RT_USING_DFS
	/* initialize the device file system */
	dfs_init();
    
#ifdef RT_USING_DFS_ELMFAT
	/* initialize the elm chan FatFS file system*/
	elm_init();
#endif
    
#ifdef RT_USING_DFS_YAFFS2
	dfs_yaffs2_init();
#endif
    
#ifdef RT_USING_DFS_UFFS
	dfs_uffs_init();
#endif 

#if defined(RT_USING_DFS_NFS) && defined(RT_USING_LWIP)
	/* initialize NFSv3 client file system */
    rt_kprintf("init nfs\r\n");
	nfs_init();
#endif

#endif /* RT_USING_DFS */

    /* check device */
    spi_device = (struct rt_spi_device *)rt_device_find("spi21");
    if(spi_device == RT_NULL)
    {
        rt_kprintf("no spi device(bus2 - device1) found\r\n");
        rt_thread_suspend(thread);
    }
    spi_device = (struct rt_spi_device *)rt_device_find("spi20");
    if(spi_device == RT_NULL)
    {
        rt_kprintf("no spi device(bus2 - device0) found\r\n");
        rt_thread_suspend(thread);
    }
    /* attacted ads7843 to spi20 */
    if(touch_ads7843_init("ads7843", "spi20") != RT_EOK)
    {
        rt_kprintf("init touch failed\r\n");
        rt_thread_suspend(thread); 
    }
    /* attacted ads7843 to spi21 */
    if(w25qxx_init("spi_flash", "spi21")!= RT_EOK)
    {
        rt_kprintf("init spi flash failed\r\n");
        rt_thread_suspend(thread); 
    }
    /* mount spi_flash */
    if (dfs_mount("spi_flash", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("spi_flash mount to / OK \n");
    }
    else
    {
        rt_kprintf("spi_flash mount to / failed!\n");
        rt_kprintf("try to format disk...\r\n");
        mkfs("elm", "spi_flash");
        dfs_mount("spi_flash", "/", "elm", 0, 0);
    }
    if (dfs_mount("sd0", "/SD", "elm", 0, 0) == 0)
    {
        rt_kprintf("sd0 mount to /SD \n");
    }
    else
    {
        rt_kprintf("sd0 mount to /SD failed!\n");
    }
   /* gui thread */
    thread = rt_thread_create("gui", gui_thread_entry, RT_NULL, 1024*8, 0x23, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    /* led thread */
    thread = rt_thread_create("led", led_thread_entry, RT_NULL, 1024*8, 0x21, 20);                                                      
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);		
    }
    /* supend me */
    thread = rt_thread_self();
    rt_thread_suspend(thread); 
}


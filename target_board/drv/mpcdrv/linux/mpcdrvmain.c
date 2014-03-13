/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvmain.c
*
* Description : P2020 mpc linux driver kernel module init 
*
* Author      : Konstantin Shiluaev..
*
******************************************************************************
******************************************************************************
*
* Module's Description Record:..
* ============================
*
* $ProjectRevision: 0.0.1 $
* $Source: 
* $State: Debug$
* $Revision: 0.0.1 $ $Name: $
* $Date: 2012/09/21 10:40:51 $recieve_matrica_commutacii_packet.state=true;
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrvmain.c $
* --------- Initial revision
******************************************************************************/


/*-----------------------------------------------------------------------------
GENERAL NOTES
-----------------------------------------------------------------------------*/

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/
/*
 * mdelay(unsigned long millisecond)
 * udelay(unsigned long microsecond)
 * ndelay(unsigned long nanosecond);
 * 
 */


/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
/*needed by kernel 2.6.18*/
#ifndef CONFIG_HZ
#include <linux/autoconf.h>
#endif

#include <linux/sched.h> //все предусмотренные версией ядра примитивы синхронизации
#include <linux/wait.h> 
//Механизмы синхронизации 
# if 0 
#include <linux/sched.h> //все предусмотренные версией ядра примитивы синхронизации
#include <linux/percpu.h> //переменные, локальные для каждого процессора (per-CPU variables)
#include <linux/spinlock.h> //спин-блокировки
#include <linux/seqlock.h> //сериальные (последовательные) блокировки


#include <linux/rwsem.h> //семафоры чтения и записи 
#include <linux/semaphore.h> //семафоры
#include <linux/rtmutex.h>   //мьютексы реального времени


#include <linux/completion.h> //механизмы ожидания завершения
#endif

#include <linux/module.h> // Needed by all modules
#include <linux/kernel.h> // Needed for KERN_ALERT
#include <linux/init.h>   // Needed for the macros
#include <linux/delay.h>  // mdelay ,msleep,
#include <linux/timer.h>  // timer 
#include <linux/ktime.h>  // hardware timer (ktime)

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/cpufreq.h>
#include <linux/of_platform.h>



#include <asm/irq.h>
#include <asm/hardirq.h>
#include <asm/prom.h>
#include <asm/time.h>
#include <asm/reg.h>
#include <asm/io.h>

#include <sysdev/fsl_soc.h>

//////////Net Filter Functions//////////

#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/if_ether.h>
#include <linux/socket.h>
#include <linux/ioctl.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/if_packet.h>

#include <linux/icmp.h>
#include <linux/udp.h>
#include <linux/if_arp.h>

#include <linux/etherdevice.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>



#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
//#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcP2020drvreg.x"
/*driver interface */

//#include "mpcdrvtest.h"
//#include "mpcdrvlbcnor.h"
#include "mpcdrvlbcCyclone.h"  //P2020 Local bus <->PLIS  API function
#include "mpcdrv_gianfar.h"    //P2020 Ethernet tsec <->  API function
#include "mpcdrvnbuf.h"        //FIFO buffer
//#include "mpcdrvngraf.h"       //Create p2020 graf functions for dynamic protocol
//#include "mpcdrveth_ipv4.h"


/******************************IP and UDP DEFINE****************************/

//Protocol value on IANA
//http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
//IP Protocol value
#define ICMP    0x1  //decimal 1
#define IPv4    0x4  //decimal 4
#define TCP     0x6  //decimal 6
#define UDP     0x11 //decimal 17

#define BROADCAST_FRAME 0xFFFF           //
#define IPv4_HEADER_LENGTH         20    //20 bait
#define UDP_HEADER_LENGTH          8     //8 bait
 

/*Cтруктура длинны UDP пакета как мы его видим в WIRESHARK */
//Ethernet заголовок = 14 байт = 0xE 
//IPv4 заголовок     = 20 байт = 0x14
//UDP заголовок      = 8  байт = 0x8

/*в IP части*/
//Поле :Headerlength = 20 ,байт
//Поле :Totallength  =общая длинна ip пакета без Ethernet части ;Totallength = all_length-Ethernet заголовок
/*в UDP части*/
//Поле Length = data_length+udp заголовок.


///////////////////////////////////////////DEFINE IP and MAC address//////////////////////////
/*IP Addrress*/
 //#define SEVA_NMS_IP_ADDR 0xC0A88261  //192.168.130.97
//#define NMS3_IP_ADDR     0xC0A8784C  //192.168.120.76
#define NMS3_IP_ADDR     0xC0A9784C  //192.169.120.76


//UINT32 CUR_KYS_IP_ADDR =0x00000000; //
UINT32 g_my_kys_ip_addres=0x00000000; //эту переменную вытащить наверхх


 
/*Первый стартовый информационный пакет от КУ-S содержит информация о IP и моего КУ-S
 *обратно отвечаю пакетом с поменённым MAC_oм */
char    kys_information_packet_da_mac         [18]=  {"01:ff:ff:ff:ff:00"};
UINT16  kys_information_packet_mac_last_word = 0xff00;


/*пакеты предназначенные моему МПС здесь содержиться  пакет информации о структуре
 * сети для Дейкстры  строю или изменяю граф*/
char    kys_deicstra_mps_packet_da_mac         [18]=  {"01:ff:ff:ff:ff:11"};
UINT16  kys_deicstra_packet_mac_last_word=0xff11;

/* с этим DA MAC адресом будут служебные данные от КУ-S ,замена 
   ppp который работает в данный момент на скорости 2400 бит/c у нас будет
   64 К/бит (1 тайм-слот)
   + здесь будет ледать информация о алгоритме декстры предназначенная другим МПС
   (сетевым элементам)
*/
//char kys_service_channel_packet_da_mac     [18]=  {"01:ff:ff:ff:22:00"};
 char   kys_service_channel_packet_da_mac     [16]=  {"01:ff:ff:ff:22"}; 
 UINT8  kys_service_channel_packet_pre_last_byte =0x22; 

  
/////////DEFINE P2020 HARDWARE REGISTERS  Implementayions ////
//#define PORPLLSR	0x50D4
//#define LBCR      0x50D0  
//#define PMJCR		0x50D4
//#define PMJCR		0x50D0   //
//#define PMJCR     0xE00A0  //PVR—Processor version register 0x80211040
 #define PMJCR     0xE0070  //DEVICE Disable Registers  
 
 const char * lbc_ready_toread     =    "data_read_ready_OK";
 const char * lbc_notready_to_read =    "data_read_ready_NOT";

 const char * lbc_ready_towrite    =    "data_write_ready_OK";
 const char * lbc_notready_to_write =   "data_write_ready_NOT";

 
 
/*****************************************************************************/
/***********************	PRIVATE STATIC FUNCTION DEFENITION****************/
/*****************************************************************************/
//static void Hardware_p2020_set_configuartion();
//static inline bool get_ethernet_packet(const u16 *in_buf ,const u16 in_size,const u16 priznak_packet);
static inline bool get_ethernet_packet(u16 *in_buf ,const u16 in_size,const u16 priznak_packet);  

/*****************************************************************************/
/***********************	EXTERN FUNCTION DEFENITION************			*/
/*****************************************************************************/
/*функция для обработки пакета от Гришы содержащей граф сети для моего МПС*/
//extern bool ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size);
/*функция для передачи пакета в матрицу коммутации для определния куда его направить*/
extern void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii,u32 priznak_nms3_ot_arp_sa_addr,u8 tdm_input_read_direction);
/*устанавливаем MAC моего KY-S */
extern void ngraf_get_ip_mac_my_kys (UINT8 state,UINT32 ip_addres,UINT8 *mac_address);

//extern void nbuf_set_datapacket_dir0  (const u16 *in_buf ,const u16 in_size);
//extern void nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size);
//extern void p2020_get_recieve_packet_and_setDA_MAC (const u16 *in_buf ,const u16 in_size,const u16 *mac_heder);
//extern void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);

/*Функции события готовность к чтению из ПЛИС */
extern void Event_TDM1_direction_READ_READY(void);
extern void Event_TDM2_direction_READ_READY(void);
extern void Event_TDM3_direction_READ_READY(void);
extern void Event_TDM4_direction_READ_READY(void);
extern void Event_TDM5_direction_READ_READY(void);
extern void Event_TDM6_direction_READ_READY(void);
extern void Event_TDM7_direction_READ_READY(void);
extern void Event_TDM8_direction_READ_READY(void);
/*Функции события готовность к записи в ПЛИС */
extern void Event_TDM1_direction_WRITE_READY(void);
extern void Event_TDM2_direction_WRITE_READY(void);
extern void Event_TDM3_direction_WRITE_READY(void);
extern void Event_TDM4_direction_WRITE_READY(void);
extern void Event_TDM5_direction_WRITE_READY(void);
extern void Event_TDM6_direction_WRITE_READY(void);
extern void Event_TDM7_direction_WRITE_READY(void);
extern void Event_TDM8_direction_WRITE_READY(void);



/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/

/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
////NET FILTER STRUCTURE///////////////////////////////////////////////
//// Структура для регистрации функции перехватчика входящих ip пакетов
struct nf_hook_ops bundle;
struct nf_hook_ops arp_bundle;


//Приоритеты задач
struct sched_param priority;
struct sched_param priority1;

/////////////////////////Timer structures//////////////////////////////////////
struct timer_list timer1_read,timer2_write;          //default timer   
static struct hrtimer hr_timer;           //high resolution timer 

///////////////////TASK _STRUCTURE///////////////
struct task_struct *r_t1,*r_t2,*test_thread_tdm_read_1,*test_thread_tdm_write_1;
//Task на чтение
struct task_struct  *test_thread_tdm_read_2,*test_thread_tdm_read_3,*test_thread_tdm_read_4,*test_thread_tdm_read_5,*test_thread_tdm_read_6,*test_thread_tdm_read_7,*test_thread_tdm_read_8;
//Task на запись
struct task_struct  *test_thread_tdm_write_2,*test_thread_tdm_write_3,*test_thread_tdm_write_4,*test_thread_tdm_write_5,*test_thread_tdm_write_6,*test_thread_tdm_write_7,*test_thread_tdm_write_8;


static int N=16;



//Очередь ожидания потока для чтения
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm1_read);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm2_read);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm3_read);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm4_read);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm5_read);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm6_read);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm7_read);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm8_read);
//Очередь ожидания потока для записи
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm1_write);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm2_write);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm3_write);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm4_write);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm5_write);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm6_write);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm7_write);
static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue_tdm8_write);


//Критические секции для чтения
rwlock_t myevent_read_tdm1_lock,myevent_read_tdm2_lock,myevent_read_tdm3_lock,myevent_read_tdm4_lock,
myevent_read_tdm4_lock,myevent_read_tdm5_lock,myevent_read_tdm6_lock,myevent_read_tdm7_lock,myevent_read_tdm8_lock;
//Критические секции на запись
rwlock_t myevent_write_tdm1_lock,myevent_write_tdm2_lock,myevent_write_tdm3_lock,myevent_write_tdm4_lock,
myevent_write_tdm4_lock,myevent_write_tdm5_lock,myevent_write_tdm6_lock,myevent_write_tdm7_lock,myevent_write_tdm8_lock;



/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/

static inline ktime_t ktime_now(void);
/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
//////////////////////////////////////////////////////////////////////////////////

///////////////////////THREAD TEST FUNCTIONS FOR MPC P2020///////////////
static char *sj( void ) { static char s[ 40 ];sprintf( s, "%08ld :", jiffies );return s;}
static char *st( int lvl ) { static char s[ 40 ];sprintf( s, "%skthread [%05d:%d]",ktime_now(),current->pid, lvl );return s;}
/**************************************************************************************************
Syntax:      	    static inline ktime_t ktime_now(void)
Parameters:     	void 
Remarks:			timer functions 
***************************************************************************************************/
static inline ktime_t ktime_now(void)
{
struct timespec ts;
ktime_get_ts(&ts);
printk("%lld.%.9ld->>>", (long long)ts.tv_sec, ts.tv_nsec);
return timespec_to_ktime(ts);


}


/**************************************************************************************************
Syntax:      	    void timer1_routine(unsigned long data)
Parameters:     	void data
Remarks:			timer functions 
***************************************************************************************************/
void timer1_routine(unsigned long data)
{
 //UINT16  lbcread_state=0;
 //UINT16  out_buf[1518];//1518 bait;
   //printk(KERN_ALERT"+timer1_routine+\n\r");
   TDM1_direction_READ_READY();   
   TDM2_direction_READ_READY();
   TDM3_direction_READ_READY();
   TDM4_direction_READ_READY();  
   TDM5_direction_READ_READY();
   TDM6_direction_READ_READY();
   TDM7_direction_READ_READY();
   TDM8_direction_READ_READY();

   
   mod_timer(&timer1_read, jiffies + msecs_to_jiffies(100)); // restarting timer 100 тиковов
 //ktime_now();     
}

/**************************************************************************************************
Syntax:      	    void timer2_routine(unsigned long data)
Parameters:     	void data
Remarks:			timer functions 
***************************************************************************************************/
void timer2_routine(unsigned long data)
{

	TDM1_direction_WRITE_READY();
	TDM2_direction_WRITE_READY();
	TDM3_direction_WRITE_READY();
	TDM4_direction_WRITE_READY();
	TDM5_direction_WRITE_READY();
	TDM6_direction_WRITE_READY();
	TDM7_direction_WRITE_READY();
	TDM8_direction_WRITE_READY();

	mod_timer(&timer2_write, jiffies + msecs_to_jiffies(100)); // restarting timer 2sec or 2000msec
	//ktime_now();

}


/**************************************************************************************************
Syntax:      	    void Event_TDM1_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM1_direction_READ_READY(void)
{
	  wake_up(&myevent_waitqueue_tdm1_read);
	  //printk("ok_event_tdm_read1\n\r");	
}
/**************************************************************************************************
Syntax:      	    void Event_TDM2_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM2_direction_READ_READY(void)
{
	
	wake_up(&myevent_waitqueue_tdm2_read);
	//printk("ok_event_tdm_read2\n\r");
}
/**************************************************************************************************
Syntax:      	    void Event_TDM3_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM3_direction_READ_READY(void)
{
	wake_up(&myevent_waitqueue_tdm3_read);
	//printk("ok_event_tdm_read3\n\r");
}

/**************************************************************************************************
Syntax:      	    void Event_TDM4_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM4_direction_READ_READY(void)
{
	wake_up(&myevent_waitqueue_tdm4_read);
	//printk("ok_event_tdm_read4\n\r");
}
/**************************************************************************************************
Syntax:      	    void Event_TDM5_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM5_direction_READ_READY(void)
{
	wake_up(&myevent_waitqueue_tdm5_read);
	//printk("ok_event_tdm_read5\n\r");
}


/**************************************************************************************************
Syntax:      	    void Event_TDM6_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM6_direction_READ_READY(void)
{
	  wake_up(&myevent_waitqueue_tdm6_read);
	  //printk("ok_event_tdm_read6\n\r");
}

/**************************************************************************************************
Syntax:      	    void Event_TDM7_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM7_direction_READ_READY(void)
{
	wake_up(&myevent_waitqueue_tdm7_read);
	//printk("ok_event_tdm_read7\n\r");
}

/**************************************************************************************************
Syntax:      	    void Event_TDM8_direction_READ_READY(void)
Parameters:     	void *ptr
Remarks:			 
Return Value:	   
***************************************************************************************************/
void Event_TDM8_direction_READ_READY(void)
{
	wake_up(&myevent_waitqueue_tdm8_read);
	//printk("ok_event_tdm_read8\n\r");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************
Syntax:      	    void Event_TDM1_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM1_direction_WRITE_READY(void)
{
	    wake_up(&myevent_waitqueue_tdm1_write);
	  //printk("ok_event_tdm_write1\n\r");
	
}
/**************************************************************************************************
Syntax:      	    void Event_TDM2_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM2_direction_WRITE_READY(void)
{
	  wake_up(&myevent_waitqueue_tdm2_write);
	  //printk("ok_event_tdm_write2\n\r");
	
}
/**************************************************************************************************
Syntax:      	    void Event_TDM3_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM3_direction_WRITE_READY(void)
{
	    wake_up(&myevent_waitqueue_tdm3_write);
	 // printk("ok_event_tdm_write3\n\r");
	
}


/**************************************************************************************************
Syntax:      	    void Event_TDM4_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM4_direction_WRITE_READY(void)
{
	    wake_up(&myevent_waitqueue_tdm4_write);
	//  printk("ok_event_tdm_write4\n\r");
	
}


/**************************************************************************************************
Syntax:      	    void Event_TDM5_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM5_direction_WRITE_READY(void)
{
	  wake_up(&myevent_waitqueue_tdm5_write);
	//printk("ok_event_tdm_write5\n\r");
	
}
/**************************************************************************************************
Syntax:      	    void Event_TDM6_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM6_direction_WRITE_READY(void)
{
	    wake_up(&myevent_waitqueue_tdm6_write);
	//  printk("ok_event_tdm_write6\n\r");
	
}
/**************************************************************************************************
Syntax:      	    void Event_TDM7_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM7_direction_WRITE_READY(void)
{
	    wake_up(&myevent_waitqueue_tdm7_write);
	    //printk("ok_event_tdm_write7\n\r");
	
}
/**************************************************************************************************
Syntax:      	    void Event_TDM8_direction_WRITE_READY(void)
Parameters:     	void *ptr
Remarks:			    
***************************************************************************************************/
void Event_TDM8_direction_WRITE_READY(void)
{
	
	  wake_up(&myevent_waitqueue_tdm8_write);
	 /// printk("ok_event_tdm_write8\n\r");
}



///////////////////////////////////////////Read_Thread_TASK////////////////////////////////////
///////////////////////////////////////////Read_Thread_TASK////////////////////////////////////
///////////////////////////////////////////Read_Thread_TASK////////////////////////////////////
/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_one(void *data)
{
	printk( "%s r_start_is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read1, current);	
	add_wait_queue (&myevent_waitqueue_tdm1_read, &wait_tdm_read1);

	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
           
        //критическая секция
		read_lock (&myevent_read_tdm1_lock);
	 	TDM1_dierction_read();
	 	read_unlock (&myevent_read_tdm1_lock); 
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm1_read, &wait_tdm_read1);
	
printk( "%s r_exit_find signal!\n", st( N ) );	
return 0;	
}

/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_two(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_two(void *data)
{
	printk( "%s r_start_is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read2, current);	
	add_wait_queue (&myevent_waitqueue_tdm2_read, &wait_tdm_read2);
	
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
	
		read_lock (&myevent_read_tdm2_lock);
		TDM2_dierction_read();
		read_unlock (&myevent_read_tdm2_lock); 
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm2_read, &wait_tdm_read2);
	
printk( "%s r_exit_find signal!\n", st( N ) );
return 0;
}

/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_three(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_three(void *data)
{
	printk( "%s r_start_is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read3, current);	
	add_wait_queue (&myevent_waitqueue_tdm3_read, &wait_tdm_read3);
	
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
	
		read_lock (&myevent_read_tdm3_lock);
		TDM3_dierction_read();
		read_unlock (&myevent_read_tdm3_lock); 
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm3_read, &wait_tdm_read3);
	
printk( "%s r_exit_find signal!\n", st( N ) );
return 0;
}

/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_four(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_four(void *data)
{
	printk( "%s r_start_is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read4, current);	
	add_wait_queue (&myevent_waitqueue_tdm4_read, &wait_tdm_read4);
	
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
	
		read_lock (&myevent_read_tdm4_lock);
		TDM4_dierction_read();
		read_unlock (&myevent_read_tdm4_lock); 
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm4_read, &wait_tdm_read4);
	
printk( "%s r_exit_find signal!\n", st( N ) );
return 0;
}

/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_five(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_five(void *data)
{
	printk( "%s r_start_is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read5, current);	
	add_wait_queue (&myevent_waitqueue_tdm5_read, &wait_tdm_read5);
	
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
	
		read_lock (&myevent_read_tdm5_lock);
		TDM5_dierction_read();
		read_unlock (&myevent_read_tdm5_lock); 
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm5_read, &wait_tdm_read5);
	
printk( "%s r_exit_find signal!\n", st( N ) );
return 0;
}

/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_six(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_six(void *data)
{
	printk( "%s r_start_is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read6, current);	
	add_wait_queue (&myevent_waitqueue_tdm6_read, &wait_tdm_read6);
	
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
	
		read_lock (&myevent_read_tdm6_lock);
		TDM6_dierction_read();
		read_unlock (&myevent_read_tdm6_lock); 
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm6_read, &wait_tdm_read6);
	
printk( "%s r_exit_find signal!\n", st( N ) );
return 0;
}

/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_seven(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_seven(void *data)
{
	printk( "%s r_start_is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read7, current);	
	add_wait_queue (&myevent_waitqueue_tdm7_read, &wait_tdm_read7);
	
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
	
		read_lock (&myevent_read_tdm7_lock);
		TDM7_dierction_read();
		read_unlock (&myevent_read_tdm7_lock); 
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm7_read, &wait_tdm_read7);
	
printk( "%s r_exit_find signal!\n", st( N ) );
return 0;
}

/**************************************************************************************************
Syntax:      	    static int tdm_read_thread_eight(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_read_thread_eight(void *data)
{
	printk( "r_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_read8, current);	
	add_wait_queue (&myevent_waitqueue_tdm8_read, &wait_tdm_read8);
	
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
	
		read_lock (&myevent_read_tdm8_lock);
		TDM8_dierction_read();
		read_unlock (&myevent_read_tdm8_lock); 
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm8_read, &wait_tdm_read8);
	
printk( "%s r_exit_find signal!\n", st( N ) );
return 0;
}

/////////////////////////////////Write Thread//////////////////////////////////
/////////////////////////////////Write Thread//////////////////////////////////
/////////////////////////////////Write Thread//////////////////////////////////
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_one(void *data)
{
 unsigned char  in_buf_dir1[1514];
 u16  in_size_dir1=0;
	
	
 printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
  
 
 DECLARE_WAITQUEUE (wait_tdm_write1, current);	
	add_wait_queue (&myevent_waitqueue_tdm1_write, &wait_tdm_write1);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
				
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir1 (&in_buf_dir1 ,&in_size_dir1)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir1_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF1_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF1_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM1_direction_write (in_buf_dir1 ,in_size_dir1);
		}
		
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm1_write, &wait_tdm_write1);

printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;			
}
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_two(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_two(void *data)
{
	 unsigned char  in_buf_dir2[1514];
	 u16  in_size_dir2=0;
	printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_write2, current);
	add_wait_queue (&myevent_waitqueue_tdm2_write, &wait_tdm_write2);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir2 (&in_buf_dir2 ,&in_size_dir2)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir2_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF2_Dir2_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF2_Dir2_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM2_direction_write (in_buf_dir2 ,in_size_dir2);
		}
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm2_write, &wait_tdm_write2);
printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;
}
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_three(void *data)
{
	 unsigned char  in_buf_dir3[1514];
	 u16  in_size_dir3=0;
	
	
	printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_write3, current);
	add_wait_queue (&myevent_waitqueue_tdm3_write, &wait_tdm_write3);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir3 (&in_buf_dir3 ,&in_size_dir3)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir3_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF1_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF1_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM3_direction_write (in_buf_dir3 ,in_size_dir3);
		}
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm3_write, &wait_tdm_write3);
printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;
}
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_four(void *data)
{
	 unsigned char  in_buf_dir4[1514];
	 u16  in_size_dir4=0;

	
	printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_write4, current);
	add_wait_queue (&myevent_waitqueue_tdm4_write, &wait_tdm_write4);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir4 (&in_buf_dir4 ,&in_size_dir4)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir1_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF1_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF1_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM4_direction_write (in_buf_dir4 ,in_size_dir4);
		}
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm4_write, &wait_tdm_write4);
printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;
}
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_five(void *data)
{

	 unsigned char  in_buf_dir5[1514];
	 u16  in_size_dir5=0;
	
	
	
	printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_write5, current);
	add_wait_queue (&myevent_waitqueue_tdm5_write, &wait_tdm_write5);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir5 (&in_buf_dir5 ,&in_size_dir5)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir5_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF1_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF1_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM5_direction_write (in_buf_dir5 ,in_size_dir5);
		}
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm5_write, &wait_tdm_write5);
printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;
}
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_six(void *data)
{
	 unsigned char  in_buf_dir6[1514];
	 u16  in_size_dir6=0;
	 
	
	printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_write6, current);
	add_wait_queue (&myevent_waitqueue_tdm6_write, &wait_tdm_write6);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir6 (&in_buf_dir6 ,&in_size_dir6)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir1_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF1_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF1_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM6_direction_write (in_buf_dir6 ,in_size_dir6);
		}
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm6_write, &wait_tdm_write6);
printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;
}
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_seven(void *data)
{
	 unsigned char  in_buf_dir7[1514];
	 u16  in_size_dir7=0;
	
	
	printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_write7, current);
	add_wait_queue (&myevent_waitqueue_tdm7_write, &wait_tdm_write7);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir7 (&in_buf_dir7 ,&in_size_dir7)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir1_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF1_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF1_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM7_direction_write (in_buf_dir7 ,in_size_dir7);
		}
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm7_write, &wait_tdm_write7);
printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;
}
/**************************************************************************************************
Syntax:      	    static int tdm_write_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			Testing thread for event low zagruzka cpu 
Return Value:	    
***************************************************************************************************/
static int tdm_write_thread_eight(void *data)
{
	unsigned char  in_buf_dir8[1514];
    u16  in_size_dir8=0;
	
	printk( "w_start_%s is parent [%05d]\n",st( N ), current->parent->pid );
	DECLARE_WAITQUEUE (wait_tdm_write8, current);
	add_wait_queue (&myevent_waitqueue_tdm8_write, &wait_tdm_write8);
	while(!kthread_should_stop()) 
	{
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
		//Есть пакет в буфере FIFO на отправку по направлению 0
		if(nbuf_get_datapacket_dir8 (&in_buf_dir8 ,&in_size_dir8)==1)
		{
			 //printk("-----------WRITE_to_tdm_dir8_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			 /*
			 printk("+FIF1_Dir1_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		     printk("+FIF1_Dir1_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
		     */	
		     TDM8_direction_write (in_buf_dir8 ,in_size_dir8);
		}
	
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue_tdm8_write, &wait_tdm_write8);
printk( "%s w_exit_find signal!\n", st( N ) );	
return 0;
}

/**************************************************************************************************
Syntax:      	    static int tdm_recieve_thread(void *data)
Parameters:     	void *ptr
Remarks:			 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static int tdm_recieve_thread(void *data)
{
   int out;
   printk( "%smain process [%d] is running\n",ktime_now(), current->pid );
   
     //Read_Thread_test
     test_thread_tdm_read_1  = kthread_run( tdm_read_thread_one,(void*)N,"tdm_read_1",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_read_2  = kthread_run( tdm_read_thread_two,(void*)N,"tdm_read_2",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_read_3  = kthread_run( tdm_read_thread_three,(void*)N,"tdm_read_3",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_read_4  = kthread_run( tdm_read_thread_four,(void*)N,"tdm_read_4",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_read_5  = kthread_run( tdm_read_thread_five,(void*)N,"tdm_read_5",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_read_6  = kthread_run( tdm_read_thread_six,(void*)N,"tdm_read_6",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_read_7  = kthread_run( tdm_read_thread_seven,(void*)N,"tdm_read_7",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_read_8  = kthread_run( tdm_read_thread_eight,(void*)N,"tdm_read_8",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     
     //Write_Thread_test
     test_thread_tdm_write_1 = kthread_run( tdm_write_thread_one,(void*)N,"tdm_write_1",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_write_2 = kthread_run( tdm_write_thread_two,(void*)N,"tdm_write_2",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_write_3 = kthread_run( tdm_write_thread_three,(void*)N,"tdm_write_3",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_write_4 = kthread_run( tdm_write_thread_four,(void*)N,"tdm_write_4",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_write_5 = kthread_run( tdm_write_thread_five,(void*)N,"tdm_write_5",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_write_6 = kthread_run( tdm_write_thread_six,(void*)N,"tdm_write_6",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_write_7 = kthread_run( tdm_write_thread_seven,(void*)N,"tdm_write_7",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     test_thread_tdm_write_8 = kthread_run( tdm_write_thread_eight,(void*)N,"tdm_write_8",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
     
   // r_t1 = kthread_run( tdm_recieve_thread_one, (void*)N, "tdm_recieve_%d", N );
   //Запускаем первый тред (приём данных с local bus)
  // r_t1 = kthread_run( tdm_recieve_thread_one, (void*)N, "tdm_recieve_%d", N );
   /*
   priority.sched_priority=0;
   sched_setscheduler(r_t1, SCHED_IDLE, &priority); 
   set_user_nice(r_t1, -10);	
   //out=task_nice(r_t1);
   */
   
   //Запускаем второй тред	
  // r_t2 = kthread_run( tdm_recieve_thread_two, (void*)N, "tdm_transmit_%d",N );
   /*
   priority1.sched_priority=0;
   sched_setscheduler(r_t2, SCHED_IDLE, &priority1);
   set_user_nice(r_t2, -5);
   */
   
   printk( "%smain process [%d] is completed\n",ktime_now(), current->pid );
return -1;
}






/**************************************************************************************************
Syntax:      	    static inline bool get_ethernet_packet(const u16 in_buf[757] ,const u16 in_size,const u16 priznak_packet)
Parameters:     	
Remarks:			Set hardware configuration for p2020 processor
Return Value:	    0  =>  Success  ,-EINVAL => Failure
***************************************************************************************************/
//static inline bool get_ethernet_packet(const u16 in_buf[757] ,const u16 in_size,const u16 priznak_packet)
static inline bool get_ethernet_packet(u16 *in_buf ,const u16 in_size,const u16 priznak_packet)
{
	UINT32  target_arp_ip_da_addr =0;
	UINT32  target_arp_nms_sa_addr=0;
	//u16 l_data_packet[759];
	
	//memset(&l_data_packet, 0x0000, sizeof(l_data_packet));
	//memcpy(l_data_packet,in_buf,in_size);
	
//#if 0	
	//printk("get_ethernet_packet=0x%x\n\r",priznak_packet);
	if(priznak_packet ==0x0800)
	{
		//printk("ip\n\r");
		//wake_up(&myevent_waitqueue);
	 memcpy(&target_arp_ip_da_addr,in_buf+15,4);
		//target_arp_ip_da_addr=in_buf[16];
	}	
	if(priznak_packet==0x0806)
	{
	  memcpy(&target_arp_nms_sa_addr,in_buf+14,4);
	  memcpy(&target_arp_ip_da_addr,in_buf+19,4);
	}	
	
	//printk("get_ethernet_packet=0x%x\n\r",target_arp_ip_da_addr);
	 ngraf_packet_for_matrica_kommutacii(in_buf,in_size,target_arp_ip_da_addr,target_arp_nms_sa_addr,0);
	
	//#endif	
	
	
	return 1;
 
 }	 



/**************************************************************************************************
Syntax:      	    unsigned int Hook_Func_ARp
Parameters:     	
Remarks:			Process recieve frame 

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
unsigned int Hook_Func_ARP(uint hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff *))

{
 u16 priznak_packet=0x0806;
 //Затычка для определения device	
 /*const char *virt_dev=0;
  *virt_dev=skb->dev->name; 
  * if (virt_dev && !strcasecmp(virt_dev ,"eth0"))
  * {
  * }
  */ 
    //if(g_my_kys_state==1)    //Information packet OK
    // {	
	    get_ethernet_packet(skb->mac_header,(uint)skb->mac_len+(uint)skb->len,priznak_packet);
        //return NF_ACCEPT;
     //} 
return NF_ACCEPT;	
}

/**************************************************************************************************
Syntax:      	    unsigned int Hook_Func
Parameters:     	
Remarks:			Process recieve frame 

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
unsigned int Hook_Func(uint hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff *))
{
	
	//__be32  my_kys_ipaddr    = MY_KYS_IPADDR;
    /* Указатель на структуру заголовка протокола eth в пакете */
	struct ethhdr *eth;
    /* Указатель на структуру заголовка протокола ip в пакете */
	struct iphdr *ip;
	/*Указатель на UDP заголовок*/
	struct udphdr *udph;
		/*указатель на icmp сообщение*/
	struct icmphdr *icmp;
		
	
	u16 priznak_packet=0x0800; //ip packet; 
	//
	UINT16  tcp_dest_port=0;
	UINT32  input_mac_da_addr[1];
	UINT16  input_mac_last_word;
	UINT8   input_mac_prelast_byte;
	UINT8   input_mac_last_byte;  //priznac commutacii po mac
	
	//Фильтрация 2 го уровня по ETH заголовку
	eth=(struct ethhdr *)skb_mac_header(skb);
	 //Фильтрация 3 го уровня по IP
	ip = (struct iphdr *)skb_network_header(skb);
    //ICMP пакет
	//icmp= (struct icmphdr*)skb_transport_header(skb);
	//UDP пакет 
	//udph = (struct udphdr *)skb_transport_header(skb);
	
	//printk("%x\n\r",eth->h_dest);
	
	
	//memcpy(input_mac_da_addr,eth->h_dest,6);
	memcpy(input_mac_da_addr,eth->h_dest,6);
	
	//printk("0x%x\n\r",input_mac_da_addr);
	
	input_mac_da_addr[1]=input_mac_da_addr[1]>>16;
	//Last four byte mac _address input_mac_last_word
	input_mac_last_word=input_mac_da_addr[1];
	//Last byte mac address for priznac_commutacii;
	//priznac coconst char *virt_dev=0;mmutacii po mac;
	//const char *virt_dev=0;
	  input_mac_prelast_byte=input_mac_last_word>>8;
	//priznac chto iformation channel packet;
	//input_mac_last_byte = input_mac_last_word;
	//virt_dev=skb->dev->name;
//#if 0  //Global comment	
	
	  //printk(">>>last16_word    =0x%04x<<|\n\r",input_mac_last_word);
	  
	  /*
	 printk(">>>last8_word    =0x%02x<<|\n\r",input_mac_last_byte);
	  printk(">>>last8_preword =0x%02x<<|\n\r",input_mac_prelast_byte);*/
    /*Принял от КУ-S Информационный пакет который сожержит
     *IP и MAC моего КУ-S не начинаю работат пока нет информационного пакета*/
	 //result_comparsion=strcmp(kys_information_packet_da_mac,buf_mac_dst);
	 //printk("+Drop_packet_size=%d|->>%d|data=%d\n\r",(uint)skb->mac_len,(uint)skb->len,(uint)skb->data_len);
	
	 /*Пустой UDP пакет отбрасываю*/
	 if((skb->mac_len+skb->len)==42) 
	 {
	 printk("bad input packet size =%d\n\r",skb->mac_len+skb->len);
	 return NF_DROP;
	 } 
	 
	
	 //Фильтрация по TCP  порт 7()
	 //Протокл TCP    
	 if(ip->protocol==TCP)
	 {	 
	    //Протокол TCP нужно фтльтровать порт 7
		 memcpy(&tcp_dest_port,skb->data+IPv4_HEADER_LENGTH+2,2);
		 //printk("+TCP _protocol=0x%x|port =0x%x\n\r+",ip->protocol,tcp_dest_port);
         if((tcp_dest_port==7)||(tcp_dest_port==59097))
         {
        	 printk("port 7 or 59097 _echo drop\n\r");
        	 return NF_DROP; 
         }
		
	   return NF_ACCEPT;	
	 }
	 
	 

	/*Принял от КУ-S Информационный пакет который сожержит
	*IP и MAC моего КУ-S не начинаю работат пока нет информационного пакета*/
	if(input_mac_last_word==kys_information_packet_mac_last_word)
	 {
		  /*Локальные переменные*/
		  //UINT32 g_my_kys_ip_addres=0x00000000;
		  bool   g_my_kys_state=0;
		  UINT8  g_my_kys_mac_addr[6];
		  
		
		//my_kys_ip_addr=(uint)ip->saddr;  	 	  
  	 	  //memcpy(my_kys_mac_addr,eth->h_source,6);	  
	      //#if 0	  //comment for FIFO buffer Testing 
	      //1.Беру отсюда IP и MAC адрес моего КУ-S(шлюзовой)
	      //2.отправляю назад пакет потдтверждения для КУ-S
	      //SA:01-ff-ff-ff-ff-00
	      //DA:00-25-01-00-11-2D (MAC KY-S) котроый получил    
		  //Копируем данные из информационного пакета в MAC и IP адрес    	  
	 	  //Берём IP адрес нашего КУ-S и MAC
		  //my_kys_ip_addr=(uint)ip->saddr;  	 	  
	 	 // memcpy(my_kys_mac_addr,eth->h_source,6);
	 	  /*заполняем структуру для нашего КY-S */	 	  
		  //CUR_KYS_IP_ADDR	= ip->saddr;  
	 	 // g_my_kys_ip_addres=(UINT8)ip->saddr;
		  g_my_kys_ip_addres=ip->saddr;
		  memcpy(g_my_kys_mac_addr,eth->h_source,6);
		  
		  /*
	 	  printk("\n\r");
	 	  printk("+KYS_Inform_PACKET|SA_MAC =|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|\n\r",g_my_kys_mac_addr[0],g_my_kys_mac_addr[1],g_my_kys_mac_addr[2],g_my_kys_mac_addr[3],g_my_kys_mac_addr[4],g_my_kys_mac_addr[5]);
	 	  printk("+KYS_Inform_PACKET|SA_IP  =0x%x\n\r",g_my_kys_ip_addres);
	 	  */
	      
	 	  //Заворачиваю назад с подменой MAC адреса
	 	 // p2020_revert_mac_header(eth->h_source,mac_addr1,&mac_header_for_kys);	 	  	    
	      //p2020_get_recieve_packet_and_setDA_MAC(skb->mac_header ,(uint)skb->mac_len+(uint)skb->len,mac_header_for_kys);    	 	 
	 	  
	      //Сообщаю состояние что принял инофрмационный пакета передаю в матрицу коммутации эту информацию
	      //чтобы дальше с ней работать.
	 	  g_my_kys_state=true;	  
	 	  ngraf_get_ip_mac_my_kys (g_my_kys_state,g_my_kys_ip_addres,g_my_kys_mac_addr);  	  
	      return NF_DROP;	//cбрасывю не пускаю дальше пакет в ОС  	  
	 }
	
	//Фильтр для пакетов от НМС3 и к НМС3  нужн подумать как ручками не прописывать может лучше сделать порт 18000;      
	if (((uint)ip->saddr==NMS3_IP_ADDR)||(uint)ip->daddr==NMS3_IP_ADDR)
	{	
	   
		//printk("packet_ok\n\r");
		//Тест TDM траффика	
    	#if 0	
		nbuf_set_datapacket_dir1  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		nbuf_set_datapacket_dir2  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		nbuf_set_datapacket_dir3  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		nbuf_set_datapacket_dir4  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);  
		nbuf_set_datapacket_dir5  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		nbuf_set_datapacket_dir6  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		nbuf_set_datapacket_dir7  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		nbuf_set_datapacket_dir8  (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		#endif
		//Более глубокий тест траффика
		
		#if 0
		if(TDM2_direction_WRITE_READY()==1)
		{	
		TDM2_direction_write (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		}
		//
		if(TDM3_direction_WRITE_READY()==1)
		{	
		TDM3_direction_write (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		}
        //
		if(TDM4_direction_WRITE_READY()==1)
		{	
		TDM4_direction_write (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		}
		//	    
		if(TDM5_direction_WRITE_READY()==1)
		{	
		TDM5_direction_write (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		}
		//
		if(TDM6_direction_WRITE_READY()==1)
		{	
		TDM6_direction_write (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		}
		//	
		if(TDM7_direction_WRITE_READY()==1)
		{	
		TDM7_direction_write (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		}
		//
		if(TDM8_direction_WRITE_READY()==1)
		{	
		TDM8_direction_write (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);
		}
		#endif
		
		//printk("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r");	
		//Нужна фтльтрация пакетов чтобы лишний раз не делать memcpy
	   get_ethernet_packet(skb->mac_header,(uint)skb->mac_len+(uint)skb->len, priznak_packet);
       return NF_DROP;
	}
	    	 
return NF_ACCEPT;	
}









/**************************************************************************************************
Syntax:      	    void Hardware_p2020_set_configuartion()
Parameters:     	
Remarks:			Set hardware configuration for p2020 processor

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
static void Hardware_p2020_set_configuartion()
{
 __be32 __iomem *pmjcr=0;
 __be32 __iomem *data=0;
/************Readme Registers Value**************
 * Default value for p2020MPC procssor GUTS_DEVDISR = 0x2608 0001
 * |31    28      24      20      16      12      8        4       0 
 * |0 0 1 0|0 1 1 0|0 0 0 0|1 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 1|
 * 
 *0        3       7       11      15     19       23     27        31
 * |0 0 1 0|0 1 1 0|0 0 0 0|1 0 0 0|0 0 0 0|0 0 0 0|0 0 0 0|0 0 0 1|
 	              1 1   1               1 1*/
		
UINT32 in_val_disable_alltsecs=0x27A03001;	
//UINT32 in_val_disable_alltsecs=0x260800E1;
UINT32 currnet_val_disable=0;
int reg_offset;	
	
	
 pmjcr = ioremap(0xffe00000+ PMJCR, 4);	
 printk("set_kernel:value to set=0x%x\n\r",pmjcr);
 //value to set in registers
 out_be32(pmjcr, in_val_disable_alltsecs);
 
 
 /*
 currnet_val_disable = in_be32(pmjcr);
 printk("get_kernel:,val=0x%x \n\r",currnet_val_disable);	
 */
 

}

/**************************************************************************************************
Syntax:      	    int init_module(void)
Parameters:     	none
Remarks:			load the kernel module driver - invoked by insmod. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
int mpc_init_module(void)
{

	/*
	** We use the miscfs to register our device.
	*/
         printk("init_module_tdm() called\n"); 
//#if 0 
      /* Заполняем структуру для регистрации hook функции */
      /* Указываем имя функции, которая будет обрабатывать пакеты */
         bundle.hook = Hook_Func;
      /* Устанавливаем указатель на модуль, создавший hook */
         bundle.owner = THIS_MODULE;
      /* Указываем семейство протоколов */
         bundle.pf = NFPROTO_IPV4;         
      /* Указываем, в каком месте будет срабатывать функция */
         bundle.hooknum = NF_INET_PRE_ROUTING;
      /* Выставляем самый высокий приоритет для функции */
         bundle.priority = NF_IP_PRI_FIRST;
      /* Регистрируем */
         nf_register_hook(&bundle);
//#endif
         
//#if 0         
         ////////////////////////////ARP_HOOK_FUNCTION/////////// 
         arp_bundle.hook =    Hook_Func_ARP;
         arp_bundle.owner=    THIS_MODULE;
         arp_bundle.pf   =    NFPROTO_ARP;
         arp_bundle.hooknum = NF_INET_PRE_ROUTING;
         arp_bundle.priority =NF_IP_PRI_FIRST;
         nf_register_hook(&arp_bundle);
         ////////////////////////////////////////////////
//#endif         
         //Future MAC Address Filtering enable and Disable
         //Future Temperature controlling and other options p2020 chips.
           Hardware_p2020_set_configuartion();         
	    // mpc_recieve_packet_hook_function = mpc_handle_frame;
           LocalBusCyc3_Init();   //__Initialization Local bus        
           InitIp_Ethernet() ;    //__Initialization P2020Ethernet devices
	       Init_FIFObuf();        //Initialization FIFO buffesrs
	       tdm_recieve_thread(NULL); //start thread functions
	     
	     //Timer1
	      init_timer(&timer1_read);
	      timer1_read.function = timer1_routine;
	      timer1_read.data = 1;
	      timer1_read.expires = jiffies + msecs_to_jiffies(50);//2000 ms 
	
	     //Timer2
	      init_timer(&timer2_write);
	      timer2_write.function = timer2_routine;
	      timer2_write.data = 1;
	      timer2_write.expires = jiffies + msecs_to_jiffies(50);//2000 ms 
	
	
	      add_timer(&timer1_read);  //Starting the timer1 
	      add_timer(&timer2_write);  //Starting the timer2
  

return 0;
}

/**************************************************************************************************
Syntax:      	    void cleanup_module(void)
Parameters:     	none
Remarks:			unload the kernel module driver - invoked by rmmod

Return Value:	    none

***************************************************************************************************/
void mpc_cleanup_module(void)
{	

	del_timer_sync(&timer1_read);              /* Deleting the timer */
	del_timer_sync(&timer2_write);             /* Deleting the timer */
	/* Регистрируем */
	nf_unregister_hook(&bundle);
	nf_unregister_hook(&arp_bundle);
	msleep(10);
	kthread_stop(test_thread_tdm_read_1); 
	kthread_stop(test_thread_tdm_read_2); 
	kthread_stop(test_thread_tdm_read_3); 
	kthread_stop(test_thread_tdm_read_4); 
	kthread_stop(test_thread_tdm_read_5); 
	kthread_stop(test_thread_tdm_read_6); 
	kthread_stop(test_thread_tdm_read_7); 
	kthread_stop(test_thread_tdm_read_8); 
	
	/////////////////////////////////////////
 	kthread_stop(test_thread_tdm_write_1); 
 	kthread_stop(test_thread_tdm_write_2);
 	kthread_stop(test_thread_tdm_write_3);
 	kthread_stop(test_thread_tdm_write_4);
 	kthread_stop(test_thread_tdm_write_5);
 	kthread_stop(test_thread_tdm_write_6);
 	kthread_stop(test_thread_tdm_write_7);
 	kthread_stop(test_thread_tdm_write_8);
 	
 	
	
	//kthread_stop(r_t1);
    //kthread_stop(r_t2);
    Clear_FIFObuf();

      
    printk("exit_module() called\n");
	//kthread_stop(tdm_transmit_task);      //Stop Thread func
	//kthread_stop(tdm_recieve_task); 
}

/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/




/*****************************************************************************/
/*	MODULE DESCRIPTION				     */
/*****************************************************************************/

module_init(mpc_init_module);
module_exit(mpc_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Konstantin Shiluaev <k_sjiluaev@supertel.spb.su>");
MODULE_DESCRIPTION("Test mpc local bus driver");























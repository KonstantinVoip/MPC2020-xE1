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

#define BROADCAST_FRAME 0xFFFF  //

#define IPv4_HEADER_LENGTH         20    //20 bait
#define UDP_HEADER_LENGTH          8     //8 bait
 



///////////////////////////////////////////DEFINE IP and MAC address//////////////////////////
/*IP Addrress*/
 //#define SEVA_NMS_IP_ADDR 0xC0A88261  //192.168.130.97
//#define NMS3_IP_ADDR     0xC0A8784C  //192.168.120.76
  #define NMS3_IP_ADDR     0xC0A9784C  //192.169.120.76
   UINT32 CUR_KYS_IP_ADDR =0x00000000; //

 
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
static inline bool get_ethernet_packet(const u16 *in_buf ,const u16 in_size,const u16 priznak_packet);


/*****************************************************************************/
/***********************	EXTERN FUNCTION DEFENITION************			*/
/*****************************************************************************/
/*функция для обработки пакета от Гришы содержащей граф сети для моего МПС*/
//extern bool ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size);
/*функция для передачи пакета в матрицу коммутации для определния куда его направить*/
extern void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii,u32 priznak_nms3_ot_arp_sa_addr);
/*устанавливаем MAC моего KY-S */
extern void ngraf_get_ip_mac_my_kys (UINT8 state,UINT32 ip_addres,UINT8 *mac_address);

//extern void nbuf_set_datapacket_dir0  (const u16 *in_buf ,const u16 in_size);
//extern void nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size);
extern void p2020_get_recieve_packet_and_setDA_MAC (const u16 *in_buf ,const u16 in_size,const u16 *mac_heder);
extern void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);

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
struct timer_list timer1,timer2;          //default timer   
static struct hrtimer hr_timer;           //high resolution timer 

///////////////////TASK _STRUCTURE///////////////
struct task_struct *r_t1,*r_t2,*test_thread_tdm;
static int N=2;

static DECLARE_WAIT_QUEUE_HEAD (myevent_waitqueue);
rwlock_t myevent_lock;


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
{struct timespec ts;ktime_get_ts(&ts);printk("%lld.%.9ld->>>", (long long)ts.tv_sec, ts.tv_nsec);return timespec_to_ktime(ts);}


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
 mod_timer(&timer1, jiffies + msecs_to_jiffies(5000)); // restarting timer
 //ktime_now();     
}

/**************************************************************************************************
Syntax:      	    void timer2_routine(unsigned long data)
Parameters:     	void data
Remarks:			timer functions 
***************************************************************************************************/
void timer2_routine(unsigned long data)
{
    //printk("++timer2_routin++\n\r");
    //mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); // restarting timer 2sec or 2000msec
	// mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); // restarting timer 2sec or 2000msec
	//ktime_now();
	//get ethernet packet and transmit to cyclone3 local bus //transmit success;
	//set transmission success;
    mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); // restarting timer 2sec or 2000msec
}

/**************************************************************************************************
Syntax:      	    static int tdm_recieve_thread_two(void *data)
Parameters:     	void *ptr
Remarks:			 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static int tdm_recieve_thread_two(void *data)
{
	printk( "%s is parent [%05d]\n",st( N ), current->parent->pid );	
	u16 status=0;
	unsigned char  in_buf_dir1[1514];
	u16  in_size_dir1=0;
	
	unsigned char  in_buf_dir2[1514];
	u16  in_size_dir2=0;
	
	unsigned char  in_buf_dir3[1514];
	u16  in_size_dir3=0;
	
	unsigned char  in_buf_dir4[1514];
	u16  in_size_dir4=0;
	
    
	
	while(!kthread_should_stop()) 
		{
		schedule();
		
	/*//////////////////////////////////Шина Local bus готова к записи по направадению 0//////////////////////*/
//#if 0
		if(TDM1_direction_WRITE_READY()==1)
			{			
		        
				//Есть пакет в буфере FIFO на отправку по направлению 0
			    if(nbuf_get_datapacket_dir1 (&in_buf_dir1 ,&in_size_dir1)==1)
		        {
			    	// printk("-----------WRITE_to_tdm_dir0_routine----->%s---------------\n\r",lbc_ready_towrite); 	
			    	 //printk("+FIFO_DIRO_insize_byte=%d\n\r+",in_size_dir0); 
		        	 
			    	 /*
			    	 printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		        	 printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf_dir0[(in_size_dir0/2)-6],in_buf_dir0[(in_size_dir0/2)-5],in_buf_dir0[(in_size_dir0/2)-4],in_buf_dir0[(in_size_dir0/2)-3],in_buf_dir0[(in_size_dir0/2)-2],in_buf_dir0[(in_size_dir0/2)-1]);
		        	 */
			    	 /*
			    	 printk("+FIFO_Dir0_rfirst   |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		        	 printk("+FIFO_Dir0_rlast    |0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|+\n\r",in_buf_dir0[in_size_dir0-6],in_buf_dir0[in_size_dir0-5],in_buf_dir0[in_size_dir0-4],in_buf_dir0[in_size_dir0-3],in_buf_dir0[in_size_dir0-2],in_buf_dir0[in_size_dir0-1]);
			    	 */
			    	 TDM1_direction_write (in_buf_dir1 ,in_size_dir1);
			    	 mdelay(32);
		        }
			
			}			
	/*///////////////////////////////Шина Local bus готова к записи по направадению 1//////////////////////////*/
//#if 0	
			if(TDM2_direction_WRITE_READY()==1)
			{
		       
//#if 0				
				if(nbuf_get_datapacket_dir2 (&in_buf_dir2 ,&in_size_dir2)==1)
				{
					
					// printk("-----------WRITELoopback_dir1_routine----->%s---------------\n\r",lbc_ready_towrite); 
					//printk("+FIFO_DIR1_insize_byte=%d\n\r+",in_size);
		        	//printk("+FIFO_Dir1_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
		        	//printk("+FIFO_Dir1_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[(in_size_dir0/2)-6],in_buf[(in_size_dir0/2)-5],in_buf[(in_size_dir0/2)-4],in_buf[(in_size_dir0/2)-3],in_buf[(in_size_dir0/2)-2],in_buf[(in_size_dir0/2)-1]);
					TDM2_direction_write (in_buf_dir2 ,in_size_dir2);	
					mdelay(32);
					//mdelay(150);
				}		
			
			}
    /*///////////////////////////////Шина Local bus готова к записи по направадению 2//////////////////////////*/
			if(TDM3_direction_WRITE_READY()==1)
			{
				if(nbuf_get_datapacket_dir3 (&in_buf_dir3 ,&in_size_dir3)==1)
				{
				     
					 ///p2020_get_recieve_virttsec_packet_buf(in_buf_dir2,in_size_dir2,2);
					 
					 
					 //printk("-----------WRITELoopback_dir2_routine----->%s---------------\n\r",lbc_ready_towrite);    
				     /*
					 printk("+FIFO_DIR2_insize_byte=%d\n\r+",in_size_dir2);
		        	 printk("+FIFO_Dir2_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf_dir2[0],in_buf_dir2[1],in_buf_dir2[2],in_buf_dir2[3],in_buf_dir2[4],in_buf_dir2[5]);
		        	 printk("+FIFO_Dir2_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf_dir2[(in_size_dir2/2)-6],in_buf_dir2[(in_size_dir2/2)-5],in_buf_dir2[(in_size_dir2/2)-4],in_buf_dir2[(in_size_dir2/2)-3],in_buf_dir2[(in_size_dir2/2)-2],in_buf_dir2[(in_size_dir2/2)-1]);
		        	 */
		        	 //mdelay(250); //250 миллисекунд задержки перед отправкой
		        	 TDM3_direction_write (in_buf_dir3  ,in_size_dir3);
		        	 mdelay(32);
		        	 //mdelay(250);
				}	 		
			}
   
		  /*///////////////////////////////Шина Local bus готова к записи по направадению 3//////////////////////////*/				
		    if(TDM4_direction_WRITE_READY()==1)
		    {
		    	if(nbuf_get_datapacket_dir4 (&in_buf_dir4 ,&in_size_dir4)==1)
		    	{
		    	  // printk("-----------WRITELoopback_dir3_routine----->%s---------------\n\r",lbc_ready_towrite);
		    	     //printk("+FIF3_DIRO_insize_byte=%d\n\r+",in_size); 
		        	 //printk("+FIF3_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
		        	 //printk("+FIF3_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[(in_size_dir0/2)-6],in_buf[(in_size_dir0/2)-5],in_buf[(in_size_dir0/2)-4],in_buf[(in_size_dir0/2)-3],in_buf[(in_size_dir0/2)-2],in_buf[(in_size_dir0/2)-1]);
		    	   TDM4_direction_write (in_buf_dir4 ,in_size_dir4); 
		    	   mdelay(32);
		    	}
            }	    
		   //mdelay(150);  
//#endif	
		    
		}
	printk( "%s find signal!\n", st( N ) );
	printk( "%s is completed\n", st( N ) );
return 0;	
}

/**************************************************************************************************
Syntax:      	    static int tdm_recieve_thread_one(void *data)
Parameters:     	void *ptr
Remarks:			 
Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static int tdm_recieve_thread_one(void *data)
{
printk( "%s is parent [%05d]\n",st( N ), current->parent->pid );

			while(!kthread_should_stop()) 
			{	
				//выполняемая работа потоковой функции
				// msleep( 100 );  	 
			     schedule();
//#if 0			     
			     if(TDM1_direction_READ_READY()==1){/*printk("------------READLoopback_TDM_DIR0------>%s---------------\n\r",lbc_ready_toread );*/TDM1_dierction_read();} 			 
			     if(TDM2_direction_READ_READY()==1){/*printk("------------READLoopback_TDM_DIR1------>%s---------------\n\r",lbc_ready_toread );*/TDM2_dierction_read();}
				 if(TDM3_direction_READ_READY()==1){/*printk("------------READLoopback_TDM_DIR2------>%s---------------\n\r",lbc_ready_toread );*/TDM3_dierction_read();}
				 if(TDM4_direction_READ_READY()==1){/*printk("------------READLoopback_TDM_DIR3------>%s---------------\n\r",lbc_ready_toread );*/TDM4_dierction_read();} 
//#endif
				 /*
				 if(TDM5_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR4------>%s---------------\n\r",lbc_ready_toread );TDM4_dierction_read();}
				 if(TDM6_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR5------>%s---------------\n\r",lbc_ready_toread );TDM5_dierction_read();}
				 if(TDM7_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR6------>%s---------------\n\r",lbc_ready_toread );TDM6_dierction_read();}
				 if(TDM8_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR7------>%s---------------\n\r",lbc_ready_toread );TDM7_dierction_read();}
			     if(TDM9_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR8------>%s---------------\n\r",lbc_ready_toread );TDM8_dierction_read();}
				 if(TDM10_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR9------>%s---------------\n\r",lbc_ready_toread );TDM9_dierction_read();}
			     
			     */
			
			}
		printk( "%s find signal!\n", st( N ) );
		printk( "%s is completed\n", st( N ) );
	return 0;	
}

/**************************************************************************************************
Syntax: static int tdm_test_thread_one(void *data)
***************************************************************************************************/
static int tdm_test_thread_one(void *data)
{
	unsigned int event_id = 0;
	printk( "%s is parent [%05d]\n",st( N ), current->parent->pid );
	
	
	DECLARE_WAITQUEUE (wait, current);	
	add_wait_queue (&myevent_waitqueue, &wait);

	while(!kthread_should_stop()) 
	{
	
	
		set_current_state (TASK_INTERRUPTIBLE);
		schedule ();
           
        
		read_lock (&myevent_lock);
	 	//printk("uup_1\n\r");
		read_unlock (&myevent_lock); 
		
	
		
	}
	set_current_state (TASK_RUNNING);
	remove_wait_queue (&myevent_waitqueue, &wait);
printk( "%s find signal!\n", st( N ) );	
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
   
   
   //test_thread_tdm = kthread_run( tdm_test_thread_one,NULL, CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
    // test_thread_tdm = kthread_run( tdm_test_thread_one,(void*)N,"tdm_test_%d",CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
   
   
    //r_t1 = kthread_run( tdm_recieve_thread_one, (void*)N, "tdm_recieve_%d", N );
   
   //Запускаем первый тред (приём данных с local bus)
   r_t1 = kthread_run( tdm_recieve_thread_one, (void*)N, "tdm_recieve_%d", N );
   /*
   priority.sched_priority=0;
   sched_setscheduler(r_t1, SCHED_IDLE, &priority); 
   set_user_nice(r_t1, -10);	
   //out=task_nice(r_t1);
   */
   
   //Запускаем второй тред	
   r_t2 = kthread_run( tdm_recieve_thread_two, (void*)N, "tdm_transmit_%d",N );
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
static inline bool get_ethernet_packet(const u16 in_buf[757] ,const u16 in_size,const u16 priznak_packet)
{
	UINT32  target_arp_ip_da_addr =0;
	UINT32  target_arp_nms_sa_addr=0;
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
	//ngraf_packet_for_matrica_kommutacii(in_buf,in_size,(u8)target_arp_ip_da_addr,(u8)target_arp_nms_sa_addr);	
	ngraf_packet_for_matrica_kommutacii(in_buf,in_size,target_arp_ip_da_addr,target_arp_nms_sa_addr);
	//return 1;
 
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
	
	
	memcpy(input_mac_da_addr,eth->h_dest,6);
	input_mac_da_addr[1]=input_mac_da_addr[1]>>16;
	//Last four byte mac _address input_mac_last_word
	input_mac_last_word=input_mac_da_addr[1];
	//Last byte mac address for priznac_commutacii;
	//priznac coconst char *virt_dev=0;mmutacii po mac;
	//const char *virt_dev=0;
	//input_mac_prelast_byte=input_mac_last_word>>8;
	//priznac chto iformation channel packet;
	//input_mac_last_byte = input_mac_last_word;
	//virt_dev=skb->dev->name;
//#if 0  //Global comment	
	
	//printk(">>>last8_word    =0x%02x<<|\n\r",input_mac_last_byte);
	//printk(">>>last8_preword =0x%02x<<|\n\r",input_mac_prelast_byte);
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
		  UINT32 g_my_kys_ip_addres=0x00000000;
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
		  CUR_KYS_IP_ADDR	= ip->saddr;  
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
	       //init_timer(&timer1);
	       //timer1.function = timer1_routine;
	       //timer1.data = 1;
	       //timer1.expires = jiffies + msecs_to_jiffies(2000);//2000 ms 
	
	     //Timer2
	    // init_timer(&timer2);
	    // timer2.function = timer2_routine;
	    // timer2.data = 1;
	    // timer2.expires = jiffies + msecs_to_jiffies(5000);//2000 ms 
	
	     //add_timer(&timer2);  //Starting the timer2
	     //add_timer(&timer1);  //Starting the timer1
	
  

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
	//del_timer_sync(&timer1);             /* Deleting the timer */
	//del_timer_sync(&timer2);             /* Deleting the timer */
	/* Регистрируем */
	nf_unregister_hook(&bundle);
	nf_unregister_hook(&arp_bundle);
	msleep(10);
	//kthread_stop(test_thread_tdm); 
	
	  kthread_stop(r_t1);
      kthread_stop(r_t2);
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



/**************************************************************************************************/
/*                                void loopback_write();                                       */
/**************************************************************************************************/
#ifdef TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER


void loopback_write()
{
UINT16 	loopbacklbcwrite_state=0;
	
	//if(recieve_tsec_packet.state==1)
	//{	
		
        //printk("+zdes1+\n\r");
        
        loopbacklbcwrite_state=TDM0_direction_WRITE_READY();
		if (loopbacklbcwrite_state==0)
		{
		printk("-----------WRITELoopback_routine----->%s---------------\n\r",lbc_notready_to_write);   
		}
			
		if(loopbacklbcwrite_state==1)
	    {
		printk("-----------WRITELoopback_routine----->%s------------------\n\r",lbc_ready_towrite); 
		
		TDM0_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
		   //TDM0_direction_write (softperfect_switch ,1514); 
		    // TDM0_direction_write (softperfect_switch_hex ,1514);
		     //TDM0_direction_write (test_full_packet_mas ,1514);
		
		//get();	
	    }

	//}
	
}
#endif

/**************************************************************************************************/
/*                                 void loopback_read();                                        */
/**************************************************************************************************/
#ifdef TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME

void loopback_read()
{
UINT16 loopbacklbcread_state=0;	
UINT16 loopbackout_buf[760];//1518 bait;
UINT16 loopbackout_size=0;


  //clear buffer
  memset(&loopbackout_buf, 0x0000, sizeof(loopbackout_buf));  
  
 
	 loopbacklbcread_state=TDM0_direction_READ_READY();
	 if(loopbacklbcread_state==0)
	 {
	 printk("------------READLoopback_routine----->%s---------------\n\r",lbc_notready_to_read );		
	 }
	    
	    
	 if(loopbacklbcread_state==1)
	 {
	 printk("------------READLoopback_routine------>%s---------------\n\r",lbc_ready_toread );	
	  TDM0_dierction_read  (loopbackout_buf,&loopbackout_size);
	 }

	
	 
	 
}
#endif



















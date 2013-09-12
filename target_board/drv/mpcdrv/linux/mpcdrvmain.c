/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvmain.c
*
* Description : P2020 mpc linux driver kernel module init 
*
* Author      : Konstantin Shiluaev
*
******************************************************************************
******************************************************************************
*
* Module's Description Record:
* ============================
*
* $ProjectRevision: 0.0.1 $
* $Source: 
* $State: Debug$
* $Revision: 0.0.1 $ $Name: $
* $Date: 2012/09/21 10:40:51 $
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



/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
/*needed by kernel 2.6.18*/
#ifndef CONFIG_HZ
#include <linux/autoconf.h>
#endif


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


#define DEFAULT_GATEWAY_IP_ADDRESS	0x0A000002  /* 10.0.0.2 */
#define BASE_IP_ADDRESS	            0x0A000001  /* 10.0.0.1   */
#define RECEIVER_IP_ADDRESS			0x0A000064	/* 10.0.0.100 */


#define P2020_IP_ADDRESS	0xACA8820A 			 /* 172.168.130.10 */
#define P2020_IP1_ADDRESS	0xACA88214 			 /* 172.168.130.20 */


char kos_mac_addr   [18]=					{"00:25:01:00:11:2D"};
char p2020_mac_addr [18]=					{"00:ff:ff:ff:11:0a"};
char information_packet_DAmacaddr [18] =	{"00:ff:ff:ff:11:0a"};
char mymps_ot_nms_packet_DAmacaddr [18]=	{"01:ff:ff:ff:11:0a"};
char service_channel_packet_DAmacaddr[18]=	{"01:ff:ff:ff:22:0a"};


/*****************************************************************************/
/***********************	EXTERN FUNCTION DEFENITION************			*/
/*****************************************************************************/
extern void ngraf_get_datapacket (const u16 *in_buf ,const u16 in_size);
extern void nbuf_get_datapacket  (const u16 *in_buf ,const u16 in_size);


#define DPRINT(fmt,args...) \
	           printk(KERN_INFO "%s,%i:" fmt "\n",__FUNCTION__,__LINE__,##args);

#define MAC_ADDR_LEN 6
#define PROMISC_MODE_ON  1   //флаг включения неразборчивый режим
#define PROMISC_MODE_OFF 0  //флаг выключения неразборчивого режима
/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
#define IN_PACKET_SIZE_DIRCTION0    256   //256 element = 512 bait
#define IN_NUM_OF_TDMCH_DIRECTION0   1    


/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
////NET FILTER STRUCTURE///////////////////////////////////////////////
//// Структура для регистрации функции перехватчика входящих ip пакетов
struct nf_hook_ops bundle;
struct ifparam *ifp;

/////////////////////////Timer structures//////////////////////////////////////
struct timer_list timer1,timer2;          //default timer   
static struct hrtimer hr_timer;           //high resolution timer 




/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/
static struct ethdata_packet
{ 
	u16 data[1514] ;
	u16 length;
    u16 state;
};





static struct ethdata_packet  recieve_tsec_packet;
/**************************************************************************************************
Syntax:      	    static inline u16* get_tsec_packet_data()
Parameters:     	
Remarks:			get data from input tsec packet 

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
static inline u16* get_tsec_packet_data()
{
	return recieve_tsec_packet.data;
}

/**************************************************************************************************
Syntax:      	    static inline get_tsec_packet_length()
Parameters:     	
Remarks:			get length from input packet

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
static inline u16 get_tsec_packet_length()
{
	return recieve_tsec_packet.length;
}

/**************************************************************************************************/
/*                                static inline put()                                            */
/**************************************************************************************************/
static inline put()
{
	
	printk("???put_input_packet_to_lbc=1???\n\r");
	recieve_tsec_packet.state=1;//TRUE;
}

/**************************************************************************************************/
/*                                static inline get()                                            */
/**************************************************************************************************/
static inline get()
{
	printk("???get_lbc_transmit_complete=0????????\n\r");
	recieve_tsec_packet.state=0;//FALSE;
	
}




/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
UINT16  tdm_transmit_state,tdm_recieve_state ;
static struct task_struct *tdm_transmit_task=NULL;
static struct task_struct *tdm_recieve_task=NULL;

/*****************************************************************************/
/*	EXTERNAL REFERENCES						     */
/*****************************************************************************/
/*
 * kernel module identification
 */
/*****************************************************************************/
/*	KERNEL MODULE HANDLING						                             */
/*****************************************************************************/
unsigned int Hook_Func(uint hooknum,
                  struct sk_buff *skb,
                  const struct net_device *in,
                  const struct net_device *out,
                  int (*okfn)(struct sk_buff *)  )
{
static u16 count=0;   
u16 i=0;
u16 result_comparsion=0; 
unsigned char buf[1514];
unsigned char  buf_mac_src[6];
unsigned char  buf_mac_dst[6];
__be32	curr_ipaddr = P2020_IP_ADDRESS ;




/* Указатель на структуру заголовка протокола eth в пакете */
struct ethhdr *eth;
/* Указатель на структуру заголовка протокола ip в пакете */
struct iphdr *ip;



   //Филтрацию 2 го уровня по MAC адресам постараюсь сделть потом аппаратно.!
    //Если пришёл пакет типа 0x800 (IPv4)    
    if (skb->protocol ==htons(ETH_P_IP))
    {
    	
    	 //Пока делаю программную фильтрацию 2 го уровня временно.
    	 eth=(struct ethhdr *)skb_mac_header(skb);
    	 print_mac(buf_mac_dst,eth->h_dest);
    	 printk("+Hook_Func+|in_DA_MAC =%s\n\r",buf_mac_dst);
        
    	 
    	 //Функция складирования пакета в буфер FIFO
    	 
    	 
    	 
    	 
    	 result_comparsion=strcmp(p2020_mac_addr,buf_mac_dst);
    	 if(result_comparsion==0)
    	 {
   		  //Здесь должна быть функции кладения в буфер FIFO
   		  //пока напрямую работаем	  
   		  //здесь будет собираю пакет для построение ГРАФА
   		  //Пакет предназначен моему МПС в DA мой mac адрес 
    	   //ngraf_get_datapacket (skb->data ,(uint)skb->len);  	  
    	 }
   
    	 
    	 
    	 
    	 
    	 
    	 ////////////Дальше доделаю эту фишку
    	 /* Сохраняем указатель на структуру заголовка IP */
	     ip = (struct iphdr *)skb_network_header(skb);
	    
	     if (curr_ipaddr== (uint)ip->daddr)
	     {

	    	 if(recieve_tsec_packet.state==0)	 
	    	   {	 
		    	 /*Фильтрация 3 го уровня по ip адресу нашего НМС.*/
		    	 printk("ipSA_addr=0x%x|ipDA_addr=0x%x\n\r",(uint)ip->saddr,(uint)ip->daddr);
		    	 printk("LEN =0x%x|LEN=%d\n\r",(uint)skb->mac_len+(uint)skb->len,(uint)skb->mac_len+(uint)skb->len);
		    	 printk("-----------------------------------------------------------------------\n\r");   
	    		   
	    		 memcpy(recieve_tsec_packet.data ,skb->mac_header,(uint)skb->mac_len+(uint)skb->len); 
	    	 	 recieve_tsec_packet.length =  (uint)skb->mac_len+(uint)skb->len;
	    	 	 put();
	    	        
	    	   }
	    	 
	    	 //return   NF_DROP;
	    	 return NF_ACCEPT;
	     }
     
    
    
    
    }
return NF_ACCEPT; 	     

//return NF_DROP;  //не пропускаем пакеты



}
/**************************************************************************************************
Syntax:      	    static inline ktime_t ktime_now(void)
Parameters:     	void 
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static inline ktime_t ktime_now(void)
{
	struct timespec ts;
	ktime_get_ts(&ts);
	printk("%lld.%.9ld\n\r", (long long)ts.tv_sec, ts.tv_nsec);
	return timespec_to_ktime(ts);
}


/**************************************************************************************************
Syntax:      	    void timer1_routine(unsigned long data)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
void timer1_routine(unsigned long data)
{
static u16 i=0;
	
UINT16 lbcread_state=0;
u16  out_buf[1518];//1518 bait;
u16  out_size;
u8  *out_num_of_tdm_ch=0;


	//printk(KERN_ALERT"Recieve lbc Timer1 Routine count-> %d data passed %ld\n\r",i++,data);
    //printk(KERN_ALERT"+timer1_routine+\n\r"); 
    mod_timer(&timer1, jiffies + msecs_to_jiffies(2000)); /* restarting timer */
	ktime_now();
    
	//#ifdef  P2020_RDBKIT
	//lbcread_state=1;
   // #endif
    //#ifdef P2020_MPC
	lbcread_state=TDM0_direction_READ_READY();
    printk("+RE_timer1_routine|lbcread_state=%d+\n\r",lbcread_state);
	// #endif
	
	if(lbcread_state==1)
	{
		TDM0_dierction_read  (out_buf,&out_size);	
	}

}

/**************************************************************************************************
Syntax:      	    void timer2_routine(unsigned long data)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
void timer2_routine(unsigned long data)
{
UINT16 lbcwrite_state=0;
//static u16 i=0;		
	

    //printk(KERN_ALERT"Inside Timer2 Routine count-> %d data passed %ld\n\r",i++,data);
    printk(KERN_ALERT"+WR_timer2_routine+\n\r");
    mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); /* restarting timer 2sec or 2000msec */
	ktime_now();
    
	
	//TDM0_direction_write (get_tsec_packet_data() ,get_tsec_packet_length());
	
	/*
	#ifdef  P2020_RDBKIT
	lbcwrite_state=1;
    #endif

    #ifdef P2020_MPC
	lbcwrite_state=TDM0_direction_WRITE_READY();
    #endif
	*/
	
	//if success packet to transmit ->>ready
	if(recieve_tsec_packet.state==1)
	 {
		 
	   lbcwrite_state=TDM0_direction_WRITE_READY();
	   printk("lbcwrite_state=%d\n\r",lbcwrite_state); 
	   if(lbcwrite_state==1)
	   {
		   /*
		   printk("++++++WRITE_PLIS_SUCCESS+++\n\r");
		   printk("packet_len=0x%x\n\r",get_tsec_packet_length());
		   printk("packet_data=0x%x\n\r",get_tsec_packet_data());
		   */   
		  //test function for default mas and size; 
		 //TDM0_direction_write (test_full_packet_mas ,IN_PACKET_SIZE_DIRCTION0);
	       TDM0_direction_write (get_tsec_packet_data() ,get_tsec_packet_length());    
	       get();
	   }
	  //get ethernet packet and transmit to cyclone3 local bus //transmit success;
	  //set transmission success;
	  
	}



}

/**************************************************************************************************
Syntax:      	    static int tdm_transmit_task(void *ptr)
Parameters:     	void *ptr
Remarks:			 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static int tdm_transmit(void *data)
{
u16  out_buf[759];//1518 bait;
u16  out_size;
u8  *out_num_of_tdm_ch=0;
    while(1)
	{							
    	  //Tdm_Direction0_write (test_full_packet_mas ,IN_PACKET_SIZE_DIRCTION0,IN_NUM_OF_TDMCH_DIRECTION0);
    	  mdelay(800);msleep(1);
		  //Tdm_Direction0_read  (out_buf,&out_size,&out_num_of_tdm_ch);
		  
		  printk("out_size=%d\n\r",out_size);
		  printk("0x%04x|0x%04x|0x%04x|0x%04x\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3]);
		if (kthread_should_stop()) return 0;
	}

return 0;
}


/**************************************************************************************************
Syntax:      	    static int tdm_recieve(void *data)
Parameters:     	void *ptr
Remarks:			 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static int tdm_recieve(void *data)
{
	
	
	while(1)
	{
		//DPRINT("tdm_thread_recieve");
		//mdelay(500);msleep(1);
		//if (kthread_should_stop()) return 0;
	}
	
	return 0;
}



/**************************************************************************************************
Syntax:      	    int init_module(void)
Parameters:     	none
Remarks:			load the kernel module driver - invoked by insmod. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
int mpc_init_module(void)
{
 UINT16 nazad;
	/*
	** We use the miscfs to register our device.
	*/
      DPRINT("init_module_tdm() called\n"); 
    
      /* Заполняем структуру для регистрации hook функции */
      /* Указываем имя функции, которая будет обрабатывать пакеты */
         bundle.hook = Hook_Func;
      /* Устанавливаем указатель на модуль, создавший hook */
         bundle.owner = THIS_MODULE;
      /* Указываем семейство протоколов */
         bundle.pf = PF_INET;
      /* Указываем, в каком месте будет срабатывать функция */
         bundle.hooknum = NF_INET_PRE_ROUTING;
      /* Выставляем самый высокий приоритет для функции */
         bundle.priority = NF_IP_PRI_FIRST;
      /* Регистрируем */
         nf_register_hook(&bundle);
      
      
         //Init Structure 
         recieve_tsec_packet.state=0;
         //LocalBusCyc3_Init();   //__Initialization Local bus 
	  //InitIp_Ethernet() ;    //__Initialization P2020Ethernet devices
	  
	
	//DPRINT("init_module_tdm() called\n");
	//nazad=mpc_recieve_packet(33,28);
	//printk("nazd= %d\n\r",nazad);
	//TIMER INITIALIZATION
	//mdelay(600);
	
	
	//Timer1
    init_timer(&timer1);
	timer1.function = timer1_routine;
	timer1.data = 1;
	timer1.expires = jiffies + msecs_to_jiffies(2000);//2000 ms 
	
    //Timer2
	init_timer(&timer2);
	timer2.function = timer2_routine;
	timer2.data = 1;
	timer2.expires = jiffies + msecs_to_jiffies(2000);//2000 ms 
	
	
	//add_timer(&timer2);  //Starting the timer2
	//add_timer(&timer1);  //Starting the timer1
	
    
	
	//Task module
	//tdm_transmit_task=kthread_run(tdm_transmit,NULL,"tdm_transmit");
	//tdm_recieve_task= kthread_run(tdm_recieve,NULL,"tdm_trecieve");

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

	DPRINT("exit_module() called\n");
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






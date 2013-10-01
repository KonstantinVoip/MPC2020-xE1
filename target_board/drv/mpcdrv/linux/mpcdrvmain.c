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


#define DEFAULT_GATEWAY_IP_ADDRESS	0x0A000002  // 10.0.0.2 
#define BASE_IP_ADDRESS	            0x0A000001  // 10.0.0.1   
#define RECEIVER_IP_ADDRESS			0x0A000064	// 10.0.0.100 


#define P2020_IP_ADDRESS	0xACA8820A 			 // 172.168.130.10 
#define P2020_IP1_ADDRESS	0xACA88214 			 // 172.168.130.20 


///MAC Addres for comparsions
///ReaL MAC Address
//MAC Address for KOS

char kys_information_packet_da_mac         [18]=  {"01:ff:ff:ff:ff:00"};
char kys_mymps_packet_da_mac               [18]=  {"01:ff:ff:ff:ff:11"};
char kys_service_channel_packet_da_mac     [18]=  {"01:ff:ff:ff:ff:22"};
 


 char kos_mac_addr   [18]=					{"00:25:01:00:11:2D"};
 char p2020_mac_addr [18]=					{"00:ff:ff:ff:11:0a"};
 char p2020_default_mac_addr [18]=			{"00:04:9f:ef:01:03"};
 char information_packet_DAmacaddr [18] =	{"00:ff:ff:ff:11:0a"};
 char mymps_ot_nms_packet_DAmacaddr [18]=	{"01:ff:ff:ff:11:0a"};
 //char service_channel_packet_DAmacaddr[18]=	{"01:ff:ff:ff:22:0a"};

 
/////////TEST LOOPBACK FUNCTION 
/*This test  Recieve packet  write to PLIS and theb Read packet from PLIS
 * no TIMER  for channel 0 and 1*/

//DIRECTION 0 Loopback Test
#define  TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER  1
//#define  TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME    1
//DIRECTION 0 Loopback Test

//DIRECTION 1 Loopback Test
//#define  TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER  1
//#define  TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME

//DIRECTION 2 Loopback Test
//#define  TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER  1
//#define  TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME
//DIRECTION 3 Loopback Test
//#define  TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER  1
//#define  TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME
//DIRECTION 4 Loopback Test
//#define  TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER  1
//#define  TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME
//DIRECTION 5 Loopback Test
//#define  TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER  1
//#define  TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME
//DIRECTION 6 Loopback Test

//DIRECTION 7 Loopback Test

//DIRECTION 8 Loopback Test

//DIRECTION 9 Loopback Test


///////////////////////TDM DIRECTION TEST//////////////
    #define TDM0_DIR_TEST  1
  //#define TDM1_DIR_TEST  1
  //#define TDM2_DIR_TEST  1
  //#define TDM3_DIR_TEST  1
  //#define TDM4_DIR_TEST  1       
  //#define TDM5_DIR_TEST  1
  //#define TDM6_DIR_TEST  1
  //#define TDM7_DIR_TEST  1
  //#define TDM8_DIR_TEST  1
  //#define TDM9_DIR_TEST  1
///////////////////////////////////////////////////////////
/////////DEFINE P2020 HARDWARE REGISTERS  Implementayions ////
//#define PORPLLSR	0x50D4
//#define LBCR        0x50D0  
//#define PMJCR		0x50D4
//#define PMJCR		0x50D0   //
//#define PMJCR     0xE00A0  //PVR—Processor version register 0x80211040
 #define PMJCR     0xE0070  //DEVICE Disable Registers  
 
 
 
 
static __be32 __iomem *pmjcr;
static __be32 __iomem *data;
 
static phys_addr_t  k_base = 0xffe00000;
static phys_addr_t  immrbase = -1;
 
 
/*****************************************************************************/
/***********************	PRIVATE STATIC FUNCTION DEFENITION****************/
/*****************************************************************************/

static void Hardware_p2020_set_configuartion();

static phys_addr_t l_get_immrbase(void);  //Get Register offset address of p2020



static void loopback_write();
static void loopback_read();



const char * lbc_ready_toread     =    "data_read_ready_OK";
const char * lbc_notready_to_read =    "data_read_ready_NOT";

const char * lbc_ready_towrite    =    "data_write_ready_OK";
const char * lbc_notready_to_write =   "data_write_ready_NOT";


/*****************************************************************************/
/***********************	EXTERN FUNCTION DEFENITION************			*/
/*****************************************************************************/
extern void ngraf_get_datapacket (const u16 *in_buf ,const u16 in_size);
//extern void nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size);
extern void p2020_get_recieve_packet_and_setDA_MAC (const u16 *in_buf ,const u16 in_size);



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
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
UINT16  tdm_transmit_state,tdm_recieve_state ;
static struct task_struct *tdm_transmit_task=NULL;
static struct task_struct *tdm_recieve_task=NULL;


/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/
static struct ethdata_packet
{ 
	u16 data[759] ;
	u16 length;
    u16 state;
};


static struct ethdata_packet  recieve_tsec_packet;





/**************************************************************************************************
Syntax:      	    void Hardware_p2020_set_configuartion()
Parameters:     	
Remarks:			Set hardware configuration for p2020 processor

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
void Hardware_p2020_set_configuartion()
{
 
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
	
	//printk("???put_input_packet_to_lbc=1???\n\r");
	recieve_tsec_packet.state=1;//TRUE;
}

/**************************************************************************************************/
/*                                static inline get()                                            */
/**************************************************************************************************/
static inline get()
{
	//printk("???get_lbc_transmit_complete=0????????\n\r");
	recieve_tsec_packet.state=0;//FALSE;
	
}


/**************************************************************************************************/
/*                                void loopback_write();                                       */
/**************************************************************************************************/
#ifdef TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER


void loopback_write()
{
UINT16 	loopbacklbcwrite_state=0;
	
	if(recieve_tsec_packet.state==1)
	{	
		loopbacklbcwrite_state=TDM0_direction_WRITE_READY();
		if (loopbacklbcwrite_state==0)
		{
		printk("-----------WRITELoopback_routine----->%s---------------\n\r",lbc_notready_to_write);   
		get();
		}
			
		if(loopbacklbcwrite_state==1)
	    {
		printk("-----------WRITELoopback_routine----->%s------------------\n\r",lbc_ready_towrite); 
		
		   TDM0_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
		   //TDM0_direction_write (softperfect_switch ,1514); 
		    // TDM0_direction_write (softperfect_switch_hex ,1514);
		     //TDM0_direction_write (test_full_packet_mas ,1514);
		
		get();	
	    }

	}
	
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
UINT16 ostatok_of_size_packet=0;



/* Указатель на структуру заголовка протокола eth в пакете */
struct ethhdr *eth;
/* Указатель на структуру заголовка протокола ip в пакете */
struct iphdr *ip;

    

    //Филтрацию 2 го уровня по MAC адресам постараюсь сделть потом аппаратно.!
    //Если пришёл пакет типа 0x800 (IPv4)    
    if (skb->protocol ==htons(ETH_P_IP))
    {
    	
      	//Не пропускаю пакеты (DROP) с длинной нечётным количеством байт
        //например 341 или что-то похожеею    	
    	
    	ostatok_of_size_packet =((uint)skb->mac_len+(uint)skb->len)%2;
    	if(ostatok_of_size_packet==1)
    	{
    	  printk("+Hook_Func+|DROP_PACKET_INOCORRECT_SIZE_bYTE =%d|size=%d\n\r",ostatok_of_size_packet,(uint)skb->mac_len+(uint)skb->len);
    	  return   NF_DROP;	//cбрасывю не пускаю дальше пакет в ОС c нечётной суммой
    	}
    
    	 //Пока делаю программную фильтрацию 2 го уровня временно.
    	 eth=(struct ethhdr *)skb_mac_header(skb);
    	 print_mac(buf_mac_dst,eth->h_dest);
    
    	 //Функция складирования пакета в буфер FIFO
    	 //result_comparsion=strcmp(p2020_default_mac_addr,buf_mac_dst);
    	   result_comparsion=strcmp(p2020_mac_addr,buf_mac_dst);
    	   if(result_comparsion==0)
    	   {
    	 	 printk("+Hook_Func+|in_DA_MAC =%s\n\r",buf_mac_dst);
    	     
    	 	 memcpy(recieve_tsec_packet.data ,skb->mac_header,(uint)skb->mac_len+(uint)skb->len);   
    	 	 //memcpy(recieve_tsec_packet.data ,softperfect_switch,(uint)skb->mac_len+(uint)skb->len); 
    	     recieve_tsec_packet.length =  (uint)skb->mac_len+(uint)skb->len;
    	    	     
    	     //p2020_get_recieve_packet_and_setDA_MAC(skb->mac_header,(uint)skb->mac_len+(uint)skb->len);
     		//Функция складирования в очередь FIFO
     		 //nbuf_get_datapacket_dir0 (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len); 
    	     put();
    		    		 
    		 
		  	  #ifdef	TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER
    		  //Test Function only recieve packet
    		  loopback_write();
			  #endif 
    		  
    		
    		  
              #ifdef	TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME
    		  //Test Function READ function
    		  loopback_read();
			  #endif
    		    
    	   //Функция складирования в очередь FIFO
    	   //nbuf_get_datapacket_dir0 (skb->mac_header ,(uint)skb->mac_len+(uint)skb->len);   		  
    		return NF_ACCEPT; 	  
    	 }
   
    	
    	 /*Информационный пакет при старте присылает КУ-S с совим IP и MAC адресом*/  
    	 result_comparsion=strcmp(kys_information_packet_da_mac,buf_mac_dst);
    	 if(result_comparsion==0)
    	 { 
    		 printk("+INPUT_INFORMATION_PACKET+\n\r");
    		 
    	  //1.Беру отсюда IP и MAC адрес моего КУ-S(шлюзовой)
    	  //2.отправляю назад пакет потдтверждения для КУ-S
    	  //SA:01-ff-ff-ff-ff-00
    	  //DA:00-25-01-00-11-2D (MAC KY-S) котроый получил
    	   
    		 return   NF_DROP;	//cбрасывю не пускаю дальше пакет в ОС c нечётной суммой
    	 }
    	 /*Пакет предназначенный только моему МПС структура графа который я строю*/
         result_comparsion=strcmp(kys_mymps_packet_da_mac,buf_mac_dst);
    	 if(result_comparsion==0)
    	 { 
           //1. Функция построения графа и поиска оптимального пути.
    		 printk("+INPUT_PACKET_ONLY_MY_MPC+\n\r");
    		  
    		 
    	    		 
    		 return   NF_DROP;	//cбрасывю не пускаю дальше пакет в ОС c нечётной суммой
    	 }
    	   
  
    	 /*Пакет предназначенный для отправки в служебный канал и обратно моему КУ-S*/
    	 result_comparsion=strcmp(kys_service_channel_packet_da_mac,buf_mac_dst);
    	 if(result_comparsion==0)
    	 {	
    		
    		 printk("+INPUT_PACKET_SERVICE_CHANNEL_and_NAZAD_KOS+\n\r");
    		 
    		 
    		 //1.отправляем пакет обратно моему КУ-S производим подмену MAC адреса
    		 //входной пакет SA :00-25-01-00-11-2D
    		 //              DA :01-ff-ff-ff-ff-22
    		 
    		//выходной пакет:SA :01-ff-ff-ff-ff-22
    		//               DA :00-25-01-00-11-2D
    		//p2020_get_recieve_packet_and_setDA_MAC(skb->mac_header,(uint)skb->mac_len+(uint)skb->len);
    		//////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!////////////////////////// 
    		//2.Отправляем пакет на анализ в граф по ip заголовку чтобы решить в какой служебный 
    	    //  канал нам отправить этот пакет.  1 <-> 10  
    	    //  ngraf_get_datapacket (skb->data ,(uint)skb->len);
    		 return   NF_DROP;	//cбрасывю не пускаю дальше пакет в ОС c нечётной суммой
    	 }
    	 
    	 ////////////Дальше доделаю эту фишку
    	 /* Сохраняем указатель на структуру заголовка IP */
	     /*ip = (struct iphdr *)skb_network_header(skb);
	 
	     if (curr_ipaddr== (uint)ip->daddr)
	     {

	    	 if(recieve_tsec_packet.state==0)	 
	    	   {	 
		    	 printk("---------------------------recieve_IP_PACKET-----------------------------------------\n\r");   
	    		 //Фильтрация 3 го уровня по ip адресу нашего НМС.
		    	 printk("ipSA_addr=0x%x|ipDA_addr=0x%x\n\r",(uint)ip->saddr,(uint)ip->daddr);
		    	 printk("LEN =0x%x|LEN=%d\n\r",(uint)skb->mac_len+(uint)skb->len,(uint)skb->mac_len+(uint)skb->len); 
	    		 memcpy(recieve_tsec_packet.data ,skb->mac_header,(uint)skb->mac_len+(uint)skb->len); 
	    	 	 recieve_tsec_packet.length =  (uint)skb->mac_len+(uint)skb->len;
	    	 	 
	    	 	 //put();
	    	        
	    	   }
	    	 
	    	 //return   NF_DROP;
	    	 return NF_ACCEPT;
	     }*/
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
 UINT16  lbcread_state=0;
 //UINT16  out_buf[1518];//1518 bait;
// UINT16  out_size=0;

 
 	 #ifdef TDM0_DIR_TEST
 	 lbcread_state=TDM0_direction_READ_READY();
 	 if(lbcread_state==1)
 	 {
 		 //TDM0_dierction_read  (out_buf,&out_size);
 		 TDM0_dierction_read();
 	 }
 	 #endif


 #ifdef TDM1_DIR_TEST
 lbcread_state=TDM1_direction_READ_READY();
 #endif

 #ifdef TDM2_DIR_TEST
 lbcread_state=TDM2_direction_READ_READY();
 #endif

 #ifdef TDM3_DIR_TEST
 lbcread_state=TDM3_direction_READ_READY();
 #endif

 #ifdef TDM4_DIR_TEST
 lbcread_state=TDM4_direction_READ_READY();
 #endif

 #ifdef TDM5_DIR_TEST
 lbcread_state=TDM5_direction_READ_READY();
 #endif
		
 #ifdef TDM6_DIR_TEST
 lbcread_state=TDM6_direction_READ_READY();
 #endif		
		
 #ifdef TDM7_DIR_TEST
 lbcread_state=TDM7_direction_READ_READY();
 #endif			
		
 #ifdef TDM8_DIR_TEST
 lbcread_state=TDM8_direction_READ_READY();
 #endif			
		
 #ifdef TDM9_DIR_TEST
 lbcread_state=TDM9_direction_READ_READY();
 #endif			
		

 

/*
	if(lbcread_state==0)
	{
	printk("------------READtimer1_routine----->%s---------------\n\r",lbc_notready_to_read );		
	}
    
    
	if(lbcread_state==1)
	{
	printk("------------READtimer1_routine------>%s---------------\n\r",lbc_ready_toread );		
	
    #ifdef TDM0_DIR_TEST
	TDM0_dierction_read  (out_buf,&out_size);
    #endif
*/	
	
	#ifdef TDM1_DIR_TEST
	TDM1_dierction_read  (out_buf,&out_size);
	#endif

	#ifdef TDM2_DIR_TEST
	TDM2_dierction_read  (out_buf,&out_size);
	#endif

	#ifdef TDM3_DIR_TEST
	TDM3_dierction_read  (out_buf,&out_size);
	#endif

	#ifdef TDM4_DIR_TEST
	TDM4_dierction_read  (out_buf,&out_size);
	#endif

	#ifdef TDM5_DIR_TEST
	TDM5_dierction_read  (out_buf,&out_size);
	#endif
		
	#ifdef TDM6_DIR_TEST
	TDM6_dierction_read  (out_buf,&out_size);
	#endif		
		
	#ifdef TDM7_DIR_TEST
	TDM7_dierction_read  (out_buf,&out_size);
	#endif			
		
	#ifdef TDM8_DIR_TEST
	TDM8_dierction_read  (out_buf,&out_size);
	#endif			
		
	#ifdef TDM9_DIR_TEST
	TDM9_dierction_read  (out_buf,&out_size);
	#endif		
	//}

    //printk(KERN_ALERT"+timer1_routine+\n\r"); 
    mod_timer(&timer1, jiffies + msecs_to_jiffies(5000)); // restarting timer
    //ktime_now();
      
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

    //printk("++timer2_routin++\n\r");
    //mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); // restarting timer 2sec or 2000msec
	//if success packet to transmit ->>ready
	if(recieve_tsec_packet.state==1)
	  {
	
	  	  #ifdef TDM0_DIR_TEST
		  lbcwrite_state=TDM0_direction_WRITE_READY();
 	  	  #endif	
		
	  	  #ifdef TDM1_DIR_TEST
		  lbcwrite_state=TDM1_direction_WRITE_READY();
		  #endif	 
	
	  	  #ifdef TDM2_DIR_TEST
	      lbcwrite_state=TDM2_direction_WRITE_READY();
 	      #endif
		
          #ifdef TDM3_DIR_TEST
	      lbcwrite_state=TDM3_direction_WRITE_READY();
		  #endif
	  
          #ifdef TDM4_DIR_TEST
	      lbcwrite_state=TDM4_direction_WRITE_READY();
 	      #endif
	  
          #ifdef TDM5_DIR_TEST
	      lbcwrite_state=TDM5_direction_WRITE_READY();
 	      #endif
	  
          #ifdef TDM6_DIR_TEST
	      lbcwrite_state=TDM6_direction_WRITE_READY();
 	      #endif
	  
          #ifdef TDM7_DIR_TEST
	      lbcwrite_state=TDM7_direction_WRITE_READY();
 	      #endif
	  
          #ifdef TDM8_DIR_TEST
	      lbcwrite_state=TDM8_direction_WRITE_READY();
	      #endif
	  
          #ifdef TDM9_DIR_TEST
	      lbcwrite_state=TDM9_direction_WRITE_READY();
	      #endif
	  

	   if (lbcwrite_state==0)
	   {
		   printk("-----------WRITEtimer2_routine----->%s---------------\n\r",lbc_notready_to_write);   
		   get();
	   }
	   
	  
	   if(lbcwrite_state==1)
	   {	       
		  printk("-----------WRITEtimer2_routine----->%s------------------\n\r",lbc_ready_towrite); 
		   
          #ifdef TDM0_DIR_TEST
		   TDM0_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
		  // TDM0_direction_write (test_full_packet_mas ,1514); 
		  
		  #endif
		   
  	  	  #ifdef TDM1_DIR_TEST
		   TDM1_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
		  #endif	 

  	  	  #ifdef TDM2_DIR_TEST
		   TDM2_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
 	 	  #endif

		  #ifdef TDM3_DIR_TEST
		   TDM3_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
		  #endif

		  #ifdef TDM4_DIR_TEST
		   TDM4_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
 	 	  #endif

		  #ifdef TDM5_DIR_TEST
		   TDM5_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
 	 	  #endif

		  #ifdef TDM6_DIR_TEST
		   TDM6_direction_write (get_tsec_packet_data() ,get_tsec_packet_length());
 	 	  #endif

		  #ifdef TDM7_DIR_TEST
		   TDM7_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
 	 	  #endif

		  #ifdef TDM8_DIR_TEST
		   TDM8_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
		  #endif

		  #ifdef TDM9_DIR_TEST
		   TDM9_direction_write (get_tsec_packet_data() ,get_tsec_packet_length()); 
		  #endif
		  
	   get();
	   }
	 
	  // mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); // restarting timer 2sec or 2000msec
	   //ktime_now();
	   //get ethernet packet and transmit to cyclone3 local bus //transmit success;
	  //set transmission success;
	  
	}
mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); // restarting timer 2sec or 2000msec


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

	/*
	** We use the miscfs to register our device.
	*/
        printk("init_module_tdm() called\n"); 
    
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
      
         
         //Initialization Functions Structure 
         recieve_tsec_packet.state=0;
        
         
         //Future MAC Address Filtering enable and Disable
         //Future Temperature controlling and other options p2020 chips.
         Hardware_p2020_set_configuartion();         
         LocalBusCyc3_Init();   //__Initialization Local bus 
	     InitIp_Ethernet() ;    //__Initialization P2020Ethernet devices
	     
	     Init_FIFObuf();        //Initialization FIFI buffesrs
	     
	     
	     
	     
	     
	     //Timer1
	     init_timer(&timer1);
	     timer1.function = timer1_routine;
	     timer1.data = 1;
	     timer1.expires = jiffies + msecs_to_jiffies(2000);//2000 ms 
	
	     //Timer2
	     init_timer(&timer2);
	     timer2.function = timer2_routine;
	     timer2.data = 1;
	     timer2.expires = jiffies + msecs_to_jiffies(5000);//2000 ms 
	
	
	     //add_timer(&timer2);  //Starting the timer2
	     add_timer(&timer1);  //Starting the timer1
	
    
	
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
	del_timer_sync(&timer1);             /* Deleting the timer */
	del_timer_sync(&timer2);             /* Deleting the timer */
	 /* Регистрируем */
	nf_unregister_hook(&bundle);

	printk("exit_module() called\n");
	//DPRINT("exit_module() called\n");
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






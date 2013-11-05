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

#define P2020_MPCRDB_KIT      1




#define DEFAULT_GATEWAY_IP_ADDRESS	0x0A000002  // 10.0.0.2 
#define BASE_IP_ADDRESS	            0x0A000001  // 10.0.0.1   
#define RECEIVER_IP_ADDRESS			0x0A000064	// 10.0.0.100 

/*
#define P2020_IP_ADDRESS	0xACA8820A 			 // 172.168.130.10 
#define P2020_IP1_ADDRESS	0xACA88214 			 // 172.168.130.20 
*/


#define P2020_IP_ADDRESS	0xC0A86F01 			 // 192.168.111.1
#define P2020_IP1_ADDRESS	0xC0A86F01			 // 192.168.111.2

/*
#define TEST_KYS_IPADDR_DA1  0xCOA86F01             //192.168.111.1              
#define TEST_KYS_IPADDR_DA2  0xCOA86F02             //192.168.111.2
*/

#define MY_KYS_IPADDR        0xC0A8829D             //192.168.130.157



///MAC Addres for comparsions
///ReaL MAC Address
//MAC Address for KOS


/*Первый стартовый информационный пакет от КУ-S содержит информация о IP и моего КУ-S
 *обратно отвечаю пакетом с поменённым MAC_oм 
 * */
char kys_information_packet_da_mac         [18]=  {"01:ff:ff:ff:ff:00"};



/*пакеты предназначенные моему МПС здесь содержиться  пакет информации о структуре
 * сети для Дейкстры  строю или изменяю граф*/
char kys_deicstra_mps_packet_da_mac               [18]=  {"01:ff:ff:ff:ff:11"};


/* с этим DA MAC адресом будут служебные данные от КУ-S ,замена 
   ppp который работает в данный момент на скорости 2400 бит/c у нас будет
   64 К/бит (1 тайм-слот)
   + здесь будет ледать информация о алгоритме декстры предназначенная другим МПС
   (сетевым элементам)
*/
/*анализирую только первый пять 5 байт MAC адреса */
//char kys_service_channel_packet_da_mac     [18]=  {"01:ff:ff:ff:22:00"};
  char kys_service_channel_packet_da_mac     [16]=  {"01:ff:ff:ff:22"}; 
  





 char p2020_rdbkit_mac_addr [18]=			{"00:04:9f:ef:01:01"};
 char kos_mac_addr   [18]=					{"00:25:01:00:11:2D"};
 char p2020_mac_addr [18]=					{"00:ff:ff:ff:11:0a"};
 char p2020_default_mac_addr [18]=			{"00:04:9f:ef:01:03"};
 char information_packet_DAmacaddr [18] =	{"00:ff:ff:ff:11:0a"};
 char mymps_ot_nms_packet_DAmacaddr [18]=	{"01:ff:ff:ff:11:0a"};
 //char service_channel_packet_DAmacaddr[18]=	{"01:ff:ff:ff:22:0a"};

 
 
 //
#define IPv4_HEADER_LENGTH         20    //20 bait
#define UDP_HEADER_LENGTH          8     //8 bait
 
 
 
 /*
 spinlock_t grisha_packet;
 unsigned long flags;
 */
 
 
 
 
 
 
/////////TEST LOOPBACK FUNCTION 
/*This test  Recieve packet  write to PLIS and theb Read packet from PLIS
 * no TIMER  for channel 0 and 1*/

//DIRECTION 0 Loopback Test
//#define  TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER  1
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
  //#define TDM0_DIR_TEST  1
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

/*функция для обработки пакета от Гришы содержащей граф сети для моего МПС*/
extern bool ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size);
/*функция для передачи пакета в матрицу коммутации для определния куда его направить*/
extern void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii);



extern void nbuf_set_datapacket_dir0  (const u16 *in_buf ,const u16 in_size);
//extern void nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size);
extern void p2020_get_recieve_packet_and_setDA_MAC (const u16 *in_buf ,const u16 in_size,const u16 *mac_heder);
extern void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);



UINT32 get_ipaddr_my_kys(); 






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

///////////////////TASK _STRUCTURE///////////////
struct task_struct *r_t1,*r_t2;
//static int tdm_recieve_thread(void *data);

//static int tdm_recieve_thread_one(void *data);



static int N=2;


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
static __be32  my_kys_ip_addr    =0;
static UINT16  my_kys_mac_addr[6] ;
UINT16 mac_addr1 [6]=   {0x01FF,0xFFFF,0xFF00};
UINT16 mac_header_for_kys[12];//={0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
char  socr_stroka[16];


static UINT16 information_packet_ok=0;


static inline ktime_t ktime_now(void);
module_param( N, int, 0 );
/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/


/*
static struct ethdata_packet
{ 
	u16 data[759] ;
	u16 length;
    u16 state;
};
static struct ethdata_packet  recieve_tsec_packet;
*/




UINT32 get_ipaddr_my_kys()
{
	return MY_KYS_IPADDR;	
}


/**************************************************************************************************
Syntax:      	    static inline u16* get_tsec_packet_data()
Parameters:     	
Remarks:			get data from input tsec packet 

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
/*
static inline u16* get_tsec_packet_data()
{
	return recieve_tsec_packet.data;
}
*/
/**************************************************************************************************
Syntax:      	    static inline get_tsec_packet_length()
Parameters:     	
Remarks:			get length from input packet

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
/*
static inline u16 get_tsec_packet_length()
{
	return recieve_tsec_packet.length;
}
*/
/**************************************************************************************************/
/*                                static inline put()                                            */
/**************************************************************************************************/
/*
static inline put()
{
	
	//printk("???put_input_packet_to_lbc=1???\n\r");
	recieve_tsec_packet.state=1;//TRUE;
}
*/
/**************************************************************************************************/
/*                                static inline get()                                            */
/**************************************************************************************************/
/*
static inline get()
{
	//printk("???get_lbc_transmit_complete=0????????\n\r");
	recieve_tsec_packet.state=0;//FALSE;
	
}
*/


///////////////////////THREAD TEST FUNCTIONS FOR MPC P2020///////////////
static char *sj( void ) 
{ // метка времени
	static char s[ 40 ];
	sprintf( s, "%08ld :", jiffies );
	return s;
}

static char *st( int lvl ) 
{ // метка потока
	static char s[ 40 ];
	sprintf( s, "%skthread [%05d:%d]",ktime_now() /*sj()*/, current->pid, lvl );
	return s;
}




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
                  int (*okfn)(struct sk_buff *))
{
static u16 count=0;   
//u16 i=0;
u16 result_comparsion=0;
UINT8 kys_last_mac[12];
unsigned char buf[1514];
unsigned char  buf_mac_src[6];
unsigned char  buf_mac_dst[6];


//__be32	curr_ipaddr = P2020_IP_ADDRESS ;
//__be32  test_kys1_ipaddr = P2020_IP_ADDRESS;        //192.168.111.1              
//__be32  test_kys2_ipaddr = P2020_IP1_ADDRESS ;      //192.168.111.2   
__be32  my_kys_ipaddr    = MY_KYS_IPADDR;

UINT16 ostatok_of_size_packet=0;

/* Указатель на структуру заголовка протокола eth в пакете */
struct ethhdr *eth;
/* Указатель на структуру заголовка протокола ip в пакете */
struct iphdr *ip;
/*Указатель на UDP заголовок*/
struct udphdr *udph;


//Фильтрация 2 го уровня по MAC
eth=(struct ethhdr *)skb_mac_header(skb);
print_mac(buf_mac_dst,eth->h_dest); 
//Фильтрация 3 го уровня по IP
ip = (struct iphdr *)skb_network_header(skb);





#if 0   //parse  Input packet dependebcie on ethernet device structure 
//TEST_MAC_Adddress
unsigned char  eth0_mac_dst[6];
unsigned char  eth1_mac_dst[6];
unsigned char  eth2_mac_dst[6];

unsigned char *eth0_mac= "c0ffc570";        //eth0
unsigned char *eth1_mac= "c0ffc834";        //eth1
unsigned char *eth2_mac= "c0ffc97c";        //eht2
unsigned char *virt_dev;
virt_dev=skb->dev->name;

	if (virt_dev && !strcasecmp(virt_dev ,"eth0"))   
	{
    /*Филтрацию 2 го уровня по MAC адресам постараюсь сделть потом аппаратно.!
     *Если пришёл пакет типа 0x800 (IPv4)  остальные отбрасываем*/    
     printk("--------------recieve_IP_PACKET_DEVICE=%s---------\n\r",virt_dev);   
	 //Фильтрация 3 го уровня по ip адресу нашего НМС.
	 //выода MAC flhtcjd
     printk("+macDA_addr=%s+\n\r",buf_mac_dst);
     printk("ipSA_addr=0x%x|ipDA_addr=0x%x\n\r",(uint)ip->saddr,(uint)ip->daddr);
	 printk("LEN =0x%x|LEN=%d\n\r",(uint)skb->mac_len+(uint)skb->len,(uint)skb->mac_len+(uint)skb->len);  
	 //if ping request for this ip send ping reply
	 //no transmit //dalee
    }

	if (virt_dev && !strcasecmp(virt_dev ,"eth1"))   
    {
    /*Филтрацию 2 го уровня по MAC адресам постараюсь сделть потом аппаратно.!
     *Если пришёл пакет типа 0x800 (IPv4)  остальные отбрасываем*/    
     printk("----------------recieve_IP_PACKET_DEVICE=%s---------------\n\r",virt_dev);   
	 //Фильтрация 3 го уровня по ip адресу нашего НМС.
	 //выода MAC flhtcjd
     printk("+macDA_addr=%s+\n\r",buf_mac_dst);
     printk("ipSA_addr=0x%x|ipDA_addr=0x%x\n\r",(uint)ip->saddr,(uint)ip->daddr);
	 printk("LEN =0x%x|LEN=%d\n\r",(uint)skb->mac_len+(uint)skb->len,(uint)skb->mac_len+(uint)skb->len);  
	 //if ping reguest(for this ip) send ping reply 
    }

   if (virt_dev && !strcasecmp(virt_dev ,"eth2"))   
   {
	/*Филтрацию 2 го уровня по MAC адресам постараюсь сделть потом аппаратно.!
     *Если пришёл пакет типа 0x800 (IPv4)  остальные отбрасываем*/    
     printk("---------------recieve_IP_PACKET_DEVICE=%s----------------------\n\r",virt_dev);   
	 //Фильтрация 3 го уровня по ip адресу нашего НМС.
	 //выода MAC flhtcjd
     printk("+macDA_addr=%s+\n\r",buf_mac_dst);
     printk("ipSA_addr=0x%x|ipDA_addr=0x%x\n\r",(uint)ip->saddr,(uint)ip->daddr);
	 printk("LEN =0x%x|LEN=%d\n\r",(uint)skb->mac_len+(uint)skb->len,(uint)skb->mac_len+(uint)skb->len);  
     //if ping request for this ip send ping reply  
   }
#endif

  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   	
   	
//Main basic code on p2020.
//#if 0
	//пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
    if (skb->protocol ==htons(ETH_P_IP))
    {

    	/*Не пропускаю пакеты (DROP) с длинной нечётным количеством байт
          *например 341 или что-то подобное 111*/    		
       	ostatok_of_size_packet =((uint)skb->mac_len+(uint)skb->len)%2;
       	if(ostatok_of_size_packet==1)
       	{
       	  //printk("+Hook_Func+|DROP_PACKET_INOCORRECT_SIZE_bYTE =%d|size=%d\n\r",ostatok_of_size_packet,(uint)skb->mac_len+(uint)skb->len);
       	  return   NF_ACCEPT;	//cбрасывю не пускаю в local bus
       	}
    	
    	
    	
    	 /*Принял от КУ-S Информационный пакет который сожержит
    	  *IP и MAC моего КУ-S не начинаю работат пока нет информационного пакета*/
    	 result_comparsion=strcmp(kys_information_packet_da_mac,buf_mac_dst);
    	 if(result_comparsion==0)
    	 {
     	 	  my_kys_ip_addr=(uint)ip->saddr;  	 	  
       	 	  memcpy(my_kys_mac_addr,eth->h_source,6);	  
		      //#if 0	  //comment for FIFO buffer Testing 
    	      //1.Беру отсюда IP и MAC адрес моего КУ-S(шлюзовой)
    	      //2.отправляю назад пакет потдтверждения для КУ-S
    	      //SA:01-ff-ff-ff-ff-00
    	      //DA:00-25-01-00-11-2D (MAC KY-S) котроый получил    
    		  //Копируем данные из информационного пакета в MAC и IP адрес    	  
    	 	  //Берём IP адрес нашего КУ-S и MAC
    	 	  my_kys_ip_addr=(uint)ip->saddr;  	 	  
    	 	  memcpy(my_kys_mac_addr,eth->h_source,6);	  
    		  printk("+KYS_Inform_PACKET|SA_MAC =|0x%04x|0x%04x|0x%04x\n\r",my_kys_mac_addr[0],my_kys_mac_addr[1],my_kys_mac_addr[2]);
    	      printk("+KYS_Inform_PACKET|SA_IP  =0x%x\n\r",my_kys_ip_addr);
    	 	
    	      p2020_revert_mac_header(eth->h_source,mac_addr1,&mac_header_for_kys);	 	  	    
    	 	  p2020_get_recieve_packet_and_setDA_MAC(skb->mac_header ,(uint)skb->mac_len+(uint)skb->len,mac_header_for_kys);
    	 
			//#endif	 
    	 	 
    	 return NF_DROP;	//cбрасывю не пускаю дальше пакет в ОС  	  
    	 
    	 }
    	 
    	 /*Пакет от Гришы со структурой графа расположен алогоритм дейкстры
    	  *строю граф для моего МПС на основании котрого будет дальнейшая маршрутизация 
    	  *анализирую ip заголовок если совпадает с адресом моего КУ-S то строю граф
    	  *кратчайших расстояний пакет заканчиваеться на*/
    	 result_comparsion=strcmp(kys_deicstra_mps_packet_da_mac,buf_mac_dst);
    	 if(result_comparsion==0)
    	 { 
    		 //1. Функция построения графа и поиска оптимального пути.
    		 
    		 if ((uint)ip->daddr==my_kys_ipaddr)
    		 {     
    			 //printk("+INPUT_PACKET_MY_MPC_STROI_GRAF-\n\r");
    			 /*Если пакет со структурой сети фрагментирован то здесь должна быть функция
    			  * предварительной сборки пакета а единой целое в один граф*/
    			 //section_unti_fragmentation_packet();
    			 /*этот ip DA address определяеться как вершина графа
    			  *от котрого будут строиться пути и рассчитываться стоимость
    			  *маршрута к другим сетевым элементам.*/    			 	 
    			 
    			// spin_lock_irqsave(grisha_packet,flags);
    			 
    			 ngraf_packet_for_my_mps(skb->data+IPv4_HEADER_LENGTH+UDP_HEADER_LENGTH  ,(uint)skb->len-(IPv4_HEADER_LENGTH+UDP_HEADER_LENGTH));
    			
    			 //spin_unlock_irqrestore(grisha_packet,flags);
    			 //ngraf_packet_for_my_mps(skb->data  ,(uint)skb->len);
    			  //ngraf_packet_for_my_mps(skb->transport_header  ,(uint)skb->len-20);
    		 }
    		 else
    		 {
             //в функции должен отправить пакет и признак коммутации по которому определю куда его пихнуть
    	     ngraf_packet_for_matrica_kommutacii(skb->mac_header ,(uint)skb->mac_len+(uint)skb->len,(uint)ip->daddr);
    		
    		 }
    	  return NF_DROP;	//cбрасывю не пускаю дальше пакет так как он нену жен больше
    	 }
    	  
    	/*Пакет предназначенный для отправки в служебный канал пакет с данными от Севы фишка в следующем
    	 *я его принимаю и анализирую последнюю часть мас пока работаю в одной подсеити потом может 
    	 *больше если недостаточно одной сети*/
    	result_comparsion=strcmp(kys_service_channel_packet_da_mac,strncpy (socr_stroka,buf_mac_dst,14)); 
    	if(result_comparsion==0)
    	{	
    	 printk("+INPUT_SERIVCE_CHANNEL_MAC=%s+\n\r",buf_mac_dst);
        
    	 //входной пакет SA :00-25-01-00-11-2D  (или MAC адрес нашего КY-S)
    	 //              DA :01-ff-ff-ff-22-xx  
         //в данном случае xx это номер ip подсети где находиться ставим в соответствие
    	                    /*157*/                       /*157*/
    	// DA :01-ff-ff-ff-22-9D     -> IP_DA = 192.168.130.157 
    	//выходной пакет:SA :01-ff-ff-ff-22-9D   (наша подсеть)   
        //               DA :00-25-01-00-11-2D   (или MAC адрес нашего КY-S)
    	/*Проверка и отброс пакета который пришёл сам себе на всякий случай сбрасываю его*/
    	// if(157 == 157){return NF_DROP;}
    	   memcpy(kys_last_mac,eth->h_dest,12);	
    	 
  
    	 //last_mac_value=eth->h_dest;                 //buf_mac_dst[6];
    	   //printk("last_mac_value=0x%x\n\r",kys_last_mac[5]);
         //добавляем подсеть типа коммутация по ip  
    	 
    	 
    	 //в функции должен отправить пакет и признак коммутации по которому определю куда его пихнуть
	      ngraf_packet_for_matrica_kommutacii(skb->mac_header ,(uint)skb->mac_len+(uint)skb->len,kys_last_mac[5]); 
	  	//  #if 0  //16.10.13	     
    	 //копирую в промежуточный буфер буфер пока не сделал  (FIFO) перед отправкой в шину localbus.
    	 //memcpy(recieve_tsec_packet.data ,skb->mac_header,(uint)skb->mac_len+(uint)skb->len); 
    	 //recieve_tsec_packet.length =  (uint)skb->mac_len+(uint)skb->len;	   
    	 //Здесь должен быть анализ и маршрутизация пакета в нужный DIRECTION
      	  #ifdef	TDM0_DIR_TEST_WRITE_LOCAL_LOOPBACK_NO_TIMER
    	 //Test Function only recieve packet
    	 loopback_write();
   	  	  #endif 
   		  
      	  #ifdef	TDM0_DIR_TEST_READ_LOCAL_LOOPBACK_NO_TIME
    	 //Test Function READ function
    	 loopback_read();
   	  	  #endif
      
	  	 // #endif //16.10.13     
      
      return NF_DROP;
      }
      else //ARP ZAPROS 
      {
      return	NF_ACCEPT;  
    	  
      }
    	
     //return	NF_DROP;	 
    //return NF_ACCEPT; //end if (skb->protocol ==htons(ETH_P_IP))
    }
//#endif
//return NF_ACCEPT; //end functions 	     
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
	printk("%lld.%.9ld->>>", (long long)ts.tv_sec, ts.tv_nsec);
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
Return Value:	    1  =>  Success  ,-1 => Failure
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
	u16  in_buf_dir0[757];
	u16  in_size_dir0=0;
	
	u16  in_buf_dir1[757];
	u16  in_size_dir1=0;
	
	u16  in_buf_dir2[757];
	u16  in_size_dir2=0;
	
	u16  in_buf_dir3[757];
	u16  in_size_dir3=0;
	
	
	
	
	while(!kthread_should_stop()) 
		{
	    
		schedule();
	/*//////////////////////////////////Шина Local bus готова к записи по направадению 0//////////////////////*/
			if(TDM0_direction_WRITE_READY()==1)
			{			
		        
				//Есть пакет в буфере FIFO на отправку по направлению 0
				if(nbuf_get_datapacket_dir0 (&in_buf_dir0 ,&in_size_dir0)==1)
		        {
		        	 //printk("-----------WRITE_to_tdm_dir0_routine----->%s---------------\n\r",lbc_ready_towrite); 
		        	   printk("+FIFO_DIRO_insize_byte=%d\n\r+",in_size_dir0); 
		        	   printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf_dir0[0],in_buf_dir0[1],in_buf_dir0[2],in_buf_dir0[3],in_buf_dir0[4],in_buf_dir0[5]);
		        	   printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf_dir0[(in_size_dir0/2)-6],in_buf_dir0[(in_size_dir0/2)-5],in_buf_dir0[(in_size_dir0/2)-4],in_buf_dir0[(in_size_dir0/2)-3],in_buf_dir0[(in_size_dir0/2)-2],in_buf_dir0[(in_size_dir0/2)-1]);
		        	   TDM0_direction_write (in_buf_dir0 ,in_size_dir0);
		        }
				
			}			
	/*///////////////////////////////Шина Local bus готова к записи по направадению 1//////////////////////////*/
#if 0	
			if(TDM1_direction_WRITE_READY()==1)
			{
		       
				
				if(nbuf_get_datapacket_dir1 (&in_buf_dir1 ,&in_size_dir1)==1)
				{
					
					//printk("-----------WRITELoopback_dir1_routine----->%s---------------\n\r",lbc_ready_towrite); 
					//printk("+FIFO_DIR1_insize_byte=%d\n\r+",in_size);
		        	//printk("+FIFO_Dir1_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
		        	//printk("+FIFO_Dir1_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[(in_size_dir0/2)-6],in_buf[(in_size_dir0/2)-5],in_buf[(in_size_dir0/2)-4],in_buf[(in_size_dir0/2)-3],in_buf[(in_size_dir0/2)-2],in_buf[(in_size_dir0/2)-1]);
					TDM1_direction_write (in_buf_dir1 ,in_size_dir1);	
				}		
			
			}
    /*///////////////////////////////Шина Local bus готова к записи по направадению 2//////////////////////////*/
			if(TDM2_direction_WRITE_READY()==1)
			{
				if(nbuf_get_datapacket_dir2 (&in_buf_dir2 ,&in_size_dir2)==1)
				{
				    //printk("-----------WRITELoopback_dir2_routine----->%s---------------\n\r",lbc_ready_towrite);    
				     //printk("+FIFO_DIR2_insize_byte=%d\n\r+",in_size);
		        	 //printk("+FIFO_Dir2_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
		        	 //printk("+FIFO_Dir2_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[(in_size_dir0/2)-6],in_buf[(in_size_dir0/2)-5],in_buf[(in_size_dir0/2)-4],in_buf[(in_size_dir0/2)-3],in_buf[(in_size_dir0/2)-2],in_buf[(in_size_dir0/2)-1]);
				    TDM2_direction_write (in_buf_dir2  ,in_size_dir2);
				}			
			}
    /*///////////////////////////////Шина Local bus готова к записи по направадению 3//////////////////////////*/				
		    if(TDM3_direction_WRITE_READY()==1)
		    {
		    	if(nbuf_get_datapacket_dir3 (&in_buf_dir3 ,&in_size_dir3)==1)
		    	{
		    	   //printk("-----------WRITELoopback_dir3_routine----->%s---------------\n\r",lbc_ready_towrite);
		    	     //printk("+FIF3_DIRO_insize_byte=%d\n\r+",in_size); 
		        	 //printk("+FIF3_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
		        	 //printk("+FIF3_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[(in_size_dir0/2)-6],in_buf[(in_size_dir0/2)-5],in_buf[(in_size_dir0/2)-4],in_buf[(in_size_dir0/2)-3],in_buf[(in_size_dir0/2)-2],in_buf[(in_size_dir0/2)-1]);
		    	   TDM3_direction_write (in_buf_dir3 ,in_size_dir3); 
		    	}
		    }
#endif	
		    
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
				//cpu_relax();
			     
			     if(TDM0_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR0------>%s---------------\n\r",lbc_ready_toread );TDM0_dierction_read();} 			 
				
#if 0
			     if(TDM1_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR1------>%s---------------\n\r",lbc_ready_toread );TDM1_dierction_read();}
				 if(TDM2_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR2------>%s---------------\n\r",lbc_ready_toread );TDM2_dierction_read();}
				 if(TDM3_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR3------>%s---------------\n\r",lbc_ready_toread );TDM3_dierction_read();} 
#endif
				 /*
				 if(TDM4_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR4------>%s---------------\n\r",lbc_ready_toread );TDM4_dierction_read();}
				 if(TDM5_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR5------>%s---------------\n\r",lbc_ready_toread );TDM5_dierction_read();}
				 if(TDM6_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR6------>%s---------------\n\r",lbc_ready_toread );TDM6_dierction_read();}
				 if(TDM7_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR7------>%s---------------\n\r",lbc_ready_toread );TDM7_dierction_read();}
			     if(TDM8_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR8------>%s---------------\n\r",lbc_ready_toread );TDM8_dierction_read();}
				 if(TDM9_direction_READ_READY()==1){printk("------------READLoopback_TDM_DIR9------>%s---------------\n\r",lbc_ready_toread );TDM9_dierction_read();}
			     
			     */
			
			}
		printk( "%s find signal!\n", st( N ) );
		printk( "%s is completed\n", st( N ) );
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
printk( "%smain process [%d] is running\n",ktime_now(), current->pid );
	r_t1 = kthread_run( tdm_recieve_thread_one, (void*)N, "tdm_recieve_%d", N );
	
	r_t2 = kthread_run( tdm_recieve_thread_two, (void*)N, "tdm_transmit_%d",N );
	
	//msleep(20000);
	//kthread_stop(r_t1);
	printk( "%smain process [%d] is completed\n",ktime_now(), current->pid );
return -1;
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
         //recieve_tsec_packet.state=0;
         
         
         //Future MAC Address Filtering enable and Disable
         //Future Temperature controlling and other options p2020 chips.
         Hardware_p2020_set_configuartion();         
         LocalBusCyc3_Init();   //__Initialization Local bus 
         InitIp_Ethernet() ;    //__Initialization P2020Ethernet devices
	     Init_FIFObuf();        //Initialization FIFI buffesrs
	     tdm_recieve_thread(NULL);
	  
	     
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
	msleep(10);
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






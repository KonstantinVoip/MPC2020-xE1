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


#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcP2020drvreg.x"
/*driver interface */

//#include "mpcdrvtest.h"
//#include "mpcdrvlbcnor.h"
#include "mpcdrvlbcCyclone.h"  //P2020 Local bus <->PLIS  API function
#include "mpcdrv_gianfar.h"    //P2020 Ethernet tsec <->  API function
 
//#include "mpcdrveth_ipv4.h"




/*External Header*/
////////////////////Extern function defeniton on another module mpcdrvgiafar.ko/////////////////
extern int mpc_recieve_packet(int a,int b);
//extern void p2020_get_recieve_virttsec_packet_buf(struct net_device *dev,u16 *buf,u16 len)

/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
#define DPRINT(fmt,args...) \
	           printk(KERN_INFO "%s,%i:" fmt "\n",__FUNCTION__,__LINE__,##args);



/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
#define IN_PACKET_SIZE_DIRCTION0    256   //256 element = 512 bait
#define IN_NUM_OF_TDMCH_DIRECTION0   1    


/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
/////////////////////////Timer structures//////////////////////////////////////
struct timer_list timer1,timer2;          //default timer   
static struct hrtimer hr_timer;    //high resolution timer 

/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/
UINT16 i;
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
/*	KERNEL MODULE HANDLING						     */
/*****************************************************************************/

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
UINT16 lbcread_state;
u16  out_buf[759];//1518 bait;
u16  out_size;
u8  *out_num_of_tdm_ch=0;


	printk(KERN_ALERT"Inside Timer1 Routine count-> %d data passed %ld\n\r",i++,data);
	mod_timer(&timer1, jiffies + msecs_to_jiffies(2000)); /* restarting timer */
	ktime_now();

	
	//#ifdef  P2020_RDBKIT
	 lbcread_state=1;
   // #endif
	
	
    //#ifdef P2020_MPC
	//lbcread_state=TDM0_direction_READ_READY();
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
UINT16 lbcwrite_state;
		
	printk(KERN_ALERT"Inside Timer2 Routine count-> %d data passed %ld\n\r",i++,data);
	mod_timer(&timer2, jiffies + msecs_to_jiffies(2000)); /* restarting timer 2sec or 2000msec */
	ktime_now();
	
	
	#ifdef  P2020_RDBKIT
	lbcwrite_state=1;
    #endif

    #ifdef P2020_MPC
	lbcwrite_state=TDM0_direction_WRITE_READY();
    #endif
	

	
	if(lbcwrite_state==1)
    {
    	 TDM0_direction_write (test_full_packet_mas ,IN_PACKET_SIZE_DIRCTION0);
  
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
	
	
	/*init_timer(&timer2);
	timer2.function = timer2_routine;
	timer2.data = 1;
	timer2.expires = jiffies + msecs_to_jiffies(2000);//2000 ms 
	*/
	
	  add_timer(&timer1);  //Starting the timer1
	//add_timer(&timer2);  //Starting the timer2
    
	
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
	//del_timer_sync(&timer2);             /* Deleting the timer */
	DPRINT("exit_module() called\n");
	//kthread_stop(tdm_transmit_task);   //Stop Thread func
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






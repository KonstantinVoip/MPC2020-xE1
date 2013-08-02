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
#include "mpcdrvlbcCyclone.h" 
//#include "mpcdrveth_ipv4.h"

/*External Header*/
////////////////////Extern Local BUS_CYCLONE3 Defenition/////////////////
extern void LocalBusCyc3_Init();
/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
#define DPRINT(fmt,args...) \
	           printk(KERN_INFO "%s,%i:" fmt "\n",__FUNCTION__,__LINE__,##args);



/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/

/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/

/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/

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
Syntax:      	    static int tdm_thread(void *ptr)
Parameters:     	void *ptr
Remarks:			 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static int tdm_thread(void *data)
{
	tdm_thread_state =1;
	
	
	
	
	while(1)
	{
		DPRINT("in kernel thread");
		mdelay(500);msleep(1);
		if (kthread_should_stop()) return 0;
	}
	
	
	
	
	tdm_thread_state =0;
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
	LocalBusCyc3_Init();
	
	tdm_thread_state =0;
	DPRINT("init_module() called\n");
	tdm_thread_task=kthread_run(tdm_thread,NULL,"tdm_thread");
	

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
	//if(tdm_thread_state)
	   
	DPRINT("exit_module() called\n");
	kthread_stop(tdm_thread_task);
	
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






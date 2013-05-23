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
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/kthread.h>
#include <linux/cpufreq.h>
#include <linux/of_platform.h>

#include <asm/prom.h>
#include <asm/time.h>
#include <asm/reg.h>
#include <asm/io.h>

#include <sysdev/fsl_soc.h>


#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcP2020drvreg.x"
/*driver interface */

#include "mpcdrvtest.h"
//#include "mpcdrvlbcnor.h"
#include "mpcdrvlbcCyclone.h" 
#include "mpcdrveth_ipv4.h"

/*External Header*/

/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
/*
 * debug
 */
/* debug print */
/*
#define PRINTE(fmt,arg...)	if(debug & 0x01) printk(KERN_ERR fmt, ## arg)
#define PRINTW(fmt,arg...)	if(debug & 0x02) printk(KERN_WARNING fmt, ## arg)
#define PRINTD(fmt,arg...)	if(debug & 0x04) printk(KERN_INFO fmt, ## arg)
#define PRINTDD(fmt,arg...)	if(debug & 0x08) printk(KERN_INFO fmt, ## arg)
#define PRINTB(txt,addr,len)	if(debug & 0x10) printb(txt,addr,len)
#define PRINTI(fmt,arg...)	if(debug & 0x80) printk(KERN_INFO fmt, ## arg)
#define printd(fmt,arg...)	printk(KERN_INFO fmt, ## arg)
*/

/* default debug level */
#define HIFDRV_DEBUG_DEF	0x03
//////////////////////////////////////////
static phys_addr_t  k_base = 0xffe00000;

static phys_addr_t l_get_immrbase(void);
static phys_addr_t immrbase = -1;

/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
//static __be32 __iomem *porpllsr;
#define PORPLLSR	0x50D4
#define LBCR        0x50D0  
/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
static __be32 __iomem *pmjcr;
static __be32 __iomem *data;

//Local bus static parameters
static  loff_t gstart_offset;
static  size_t glen;


//#define PMJCR		0x50D4
//#define PMJCR		0x50D0   //
//#define PMJCR     0xE00A0  //PVR Processor version register 0x80211040
//#define PMJCR     0xE0070 
  

/* KERNEL MODULE HANDLING */
static int mpcdrv_ioctl(struct inode*, struct file*, unsigned int, unsigned long);


u32 core_spd, mask, tmp,cpu;
	int shift;
	u32 busfreq, corefreq, val;

/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/
/* file operations - NOTE: only valid for kernel 2.4.x */
static struct file_operations mpcdrv_fops = {
	owner: THIS_MODULE,
	ioctl: mpcdrv_ioctl
};

static struct miscdevice mpcdrv_misc = {
	MISC_DYNAMIC_MINOR,
	"mpcdrv",
	&mpcdrv_fops
};

//static unsigned char sbuffer[DATA_MAX];
static unsigned char sbuffer[5]={"mmmmm"};
static unsigned char localbusbuffer[0x1000];



//static unsigned int sbuffer[5]={55,66,77,88,99};
//static unsigned int sbuffer[5]; 
  
  /*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/



/*****************************************************************************/
/*	EXTERNAL REFERENCES						     */
/*****************************************************************************/
/*
 * kernel module identification
 */


/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
phys_addr_t l_get_immrbase(void)
{
	struct device_node *soc;
	
	if (immrbase != -1)
			return immrbase;
	soc = of_find_node_by_type(NULL, "soc");
	if (soc) {
		printk ("SUCCESS: SOC device support !\n");	
		int size;
		u32 naddr;
		const u32 *prop = of_get_property(soc, "#address-cells", &size);
		
		if (prop && size == 4)
			naddr = *prop;
		else
			naddr = 2;
		
		prop = of_get_property(soc, "ranges", &size);
		if (prop)
			immrbase = of_translate_address(soc, prop + naddr);

		of_node_put(soc);
	}
	else
	{
    printk ("Error: SOC device not support !\n");
	}
	//printk("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!IMMRBASE=0x%x",immrbase);
	
	return immrbase;
}

/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/

/**************************************************************************************************
Syntax:      	    static int  mod_ioctl(struct inode* inodep, struct file* filep, unsigned int cmd,unsigned long arg)
Parameters:     	inodep,filep,cmd,arg
Remarks:			kernel module handler. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static int  mpcdrv_ioctl(struct inode* inodep, struct file* filep, unsigned int cmd,unsigned long arg)
{
	int pvr, msr;
	int lcrr; //Local bus clock ratio Register
    u32 pll;
	int reg_offset;
	//UINT32 in_val=0x40000000;
	UINT32 in_val_disable_alltsecs=0x260800E1;
	//reg_offset=arg;
	//in_reg=&arg;
	//printk("kernel_IN_nocopy:reg_offset=0x%x,cmd=0x%x\n\r",arg,cmd);
	//__copy_tofrom_user(sbuffer, &arg, sizeof(sbuffer));
	//__copy_tofrom_user(kb, cmd, sizeof(&kb));
	//PRINTK(KERN_ALERT "mpcdrv: mpcdrv_ioctl: set_data: %x\n", sbuffer);
	//printk("_1In_tokernel_from_user ka=0x%x,arg=0x%x\n\r",ka,arg);
	//printk("PMJCR=0x%x\n\r",PMJCR);
	//pmjcr = ioremap(0xffe00000+ PMJCR, 4);
	//pmjcr = ioremap(0xffe00000+PMJCR,4);
	                     /***to***//******from******/
	//__copy_tofrom_user(sbuffer, (unsigned int*)arg, DATA_MAX))
	//////////////////Input Registers Remapping///////////
	 /*
	 printk("PMJCR=0x%x\n\r",in_reg);
	 pmjcr = ioremap(0xffe00000+ in_reg, 4);
	 */
	//PRINTK(KERN_ALERT "mpcdrv: mpcdrv_ioctl\n");
    
	if (_IOC_TYPE(cmd) != MOD_MAGIC)
		return -ENOTTY;

	switch(cmd)
	{
		case IOC_INIT_P2020:
		//Initialize P2020 software and hardware module
		  //MPCInitNORflash(); //init structure
		break;
		
		case IOC_SET_DATA:

			if (!access_ok(VERIFY_WRITE, (unsigned int*)arg, DATA_MAX))
				return -EFAULT;

			if (__copy_tofrom_user(sbuffer, (unsigned int*)arg, DATA_MAX))
			{
				return -EFAULT;
			}

			PRINTK(KERN_ALERT "mpcdrv: mpcdrv_ioctl: set_data: %s\n", sbuffer);
			break;

		case IOC_GET_DATA:
		    printk("kernel get_data:0x%x\n\r",sbuffer[0]);
			//Return char Data
			if (!access_ok(VERIFY_READ, (unsigned int*)arg, DATA_MAX))
				return -EFAULT;	
			/*
			if (__copy_tofrom_user((unsigned int*)arg, &sbuffer[0], DATA_MAX))
			{
				return -EFAULT;
			}	
			*/	
			///////////////////////Return Char Buffer/////////////////////		
			if (__copy_tofrom_user((unsigned int*)arg, sbuffer, DATA_MAX))
			{
				return -EFAULT;
			}
			PRINTK(KERN_ALERT "mpcdrv: mpcdrv_ioctl: get_data\n");
			break;

			case IOC_MOD_IOREMAP:
				 printk("kernel:IOC_IOREMAP_P2020_registers,offset=0x%x\n\r",arg);
			     // mapping.
			     //arg -> p2020 reg offset 
			     data = ioremap(0xffe00000+ arg, 4);
		
			break;
			
		  case IOC_GET_PVR:
          	
			if (!access_ok(VERIFY_READ, (unsigned int*)arg, sizeof(&pvr)))
				return -EFAULT;

			__asm__ ("mfspr %0, 287" : "=r" (pvr) : );

			if (__copy_tofrom_user((unsigned int*)arg, &pvr, sizeof(&pvr)))
			{
				return -EFAULT;
			}
		
			break;
        
			
		case IOC_GET_MSR:

			__asm__ ("mfmsr %0" : "=r" (msr) : );

			if (!access_ok(VERIFY_READ, (unsigned int*)arg, sizeof(&msr)))
				return -EFAULT;
			
			if (__copy_tofrom_user((unsigned int*)arg, sbuffer, sizeof(&msr)))
			{
				return -EFAULT;
			}
			break;

		case IOC_GET_LCRR:
			lcrr= ioremap(k_base+ LBCR, 4);	
			//lcrr = ioremap(l_get_immrbase() + PORPLLSR, 4);	
			//printk("PORPLLSR=0x%x\n\r",lcrr);	
			pll=in_be32(lcrr);
           // printk("PLL=0x%x\n\r",pll); 				
			if (!access_ok(VERIFY_READ, (unsigned int*)arg, sizeof(&pll)))
				return -EFAULT;	
					
			if (__copy_tofrom_user((unsigned int*)arg, &pll, sizeof(&pll)))
			{
				return -EFAULT;
			}	
			break;	
			
			
		///////////////////////////////////////////////////	
		case IOC_GET_P2020REG: 
			 /*
			 sbuffer[0] = in_be32(data);
			 printk("kernel:0x%x\n\r",sbuffer[0]);
			 */
			 
			 //printk("get_kernel:pmjcr=0x%x\n\r",pmjcr);
			 
			 tmp = in_be32(data);
			 printk("get_kernel:,val=0x%x \n\r",tmp);	
             
             if (!access_ok(VERIFY_READ, (unsigned int*)arg, sizeof(&tmp)))
				return -EFAULT;	
					
			 if (__copy_tofrom_user((unsigned int*)arg, &tmp, sizeof(&tmp)))
			 {
			 return -EFAULT;
			 }	
             			 
			/* 
			 if (!access_ok(VERIFY_READ, (unsigned int*)arg, DATA_MAX))
				return -EFAULT;

			if (__copy_tofrom_user((unsigned int*)arg, &sbuffer[3], DATA_MAX))
			{
				return -EFAULT;
			}
			*/ 
		  break;
		   
        ///////////////////////////////////////////////////
        case IOC_SET_P2020REG:
			  printk("set_kernel:value to set=0x%x\n\r",arg);
			  //value to set in registers
			  out_be32(data, arg);
			  //out_be32(pmjcr, in_val_disable_alltsecs);
			  //printk("set_kernel:reg_offset=0x%x ,val=0x%x \n\r",PMJCR,in_val_disable_alltsecs);
			  //printk("SET_P2020REG=0x%x\n\r",arg);	
              //out_be32(pmjcr, 0x8000000F);		 
		 break; 	
		
		/////////////P2020 Loclal Bus data Functionality///////////////
		
		//set start offset on local bus curren flash
		case IOC_LBCOFFSET_P2020:
		gstart_offset=(loff_t)arg;
		printk("set_kernel:IOC_LBCOFFSET_P2020 val=0x%x \n\r",(int)gstart_offset);
		break;
		
        //set data buf size on Local bus operation		
		case IOC_LBCDATASIZE_P2020:
		glen=(size_t)arg; 
		break;
		 
		//set sam buffer for read and write 
		case IOC_LBCDATABUF_P2020: 
		//mpcNorReadata (gstart_offset,glen,localbusbuffer); 
        if (!access_ok(VERIFY_READ, (unsigned int*)arg, 0x1000))
				return -EFAULT;	
	
		///////////////////////Return Char Buffer/////////////////////		
		if (__copy_tofrom_user((unsigned int*)arg, localbusbuffer, 0x1000))
		{
			return -EFAULT;
		}
		break;
		
		default:
			PRINTK(KERN_ALERT "mpcdrv: mpcdrv_ioctl: unknown ioctl\n");
	}

	return 0;
}





/*****************************************************************************/
/*	KERNEL MODULE HANDLING						     */
/*****************************************************************************/

/**************************************************************************************************
Syntax:      	    int init_module(void)
Parameters:     	none
Remarks:			load the kernel module driver - invoked by insmod. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
int mpc_init_module(void)
{
u16 address=0;
u16 out_data=0;
u16 i;
u8  in_num_of_tdm_ch=0;
u16 in_size=1;
u16 *in_buf=0;
u16 *out_buf=0;
u16 *out_size=0;
u8  *out_num_of_tdm_ch=0;

//loff_t from=0x0000000000000000;
//size_t len=0x000000100;

//u16 lbc_buf[128];

    printk("SUPER_MODULE_SET_Cyclone3_Lbc_Kosta_Revision\n\r");    
	PRINTK(KERN_ALERT "mpcdrv: %s\n", "init_module");
	/*
	** We use the miscfs to register our device.
	*/
	//initialzate nor flash memory device.
	//p2020TestBus();
	test_Cyc3Init();
	Tdm_Direction0_write (in_buf , in_size,in_num_of_tdm_ch);
	Tdm_Direction0_read  (out_buf,out_size,out_num_of_tdm_ch);
	
	//plis_write16(PLIS_ADDRESS0,0xeeee);
	
	//out_data=plis_read16 (PLIS_ADDRESS0);
	//printk("iter%d ,address=0x%x,data= 0x%04x\n\r",i,address,out_data);
	
	/*
	for(i=0;i<16;i++)
    {	
    	
    out_data=plis_read16 (address+i);
	printk("iter%d ,address=0x%x,data= 0x%04x\n\r",i,address,out_data);
	
    }
    */
    
	//test_Cyc3Read();
	
	
	
	//test_Cyc3Write();
	//test_Cyc3Read();
	//MPCInitNORflash();
	//MPCInitlocalBus();
	
	//MPCDumplocalbusreg();
	

	
	
	//mpcNorReadata (from,len,lbc_buf);
	
	//InitEthipv4drv();
	/////////////////////////Initialize ethernet configurations/////////
	
	
	
	if (misc_register(&mpcdrv_misc))
	{
		PRINTK(KERN_ALERT "mpcdrv: cannot register misc device\n");
		return -EIO;
	}
	//pmjcr = ioremap(l_get_immrbase()+ PMJCR, 4);
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
	
	//kthread_stop(tdm_thread_task);
	
	PRINTK(KERN_ALERT "mpcdrv: %s\n", "cleanup_module");
	iounmap(pmjcr);
	misc_deregister(&mpcdrv_misc);
}

/*****************************************************************************/
/*	MODULE DESCRIPTION				     */
/*****************************************************************************/

module_init(mpc_init_module);
module_exit(mpc_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Konstantin Shiluaev <k_sjiluaev@supertel.spb.su>");
MODULE_DESCRIPTION("Test mpc local bus driver");






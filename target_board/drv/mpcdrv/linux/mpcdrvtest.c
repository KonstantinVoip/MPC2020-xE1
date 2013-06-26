/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvtest.c
*
* Description : Defenition P2020 mpc test kernel function
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
* $Date: 2012/10/23 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrvtest.c $
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
#include <linux/kernel.h>
#include <asm/fsl_lbc.h>
#include <linux/of.h>
#include <linux/of_platform.h>


#include "mpcdrvtest.h" 

/*External Header*/

/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
struct fsl_lbc_regs __iomem    *lbc;

/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/

/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/


/* KERNEL MODULE HANDLING */


/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/

  /*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/


/*****************************************************************************/
/*	EXTERNAL REFERENCES						     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
static irqreturn_t local_interrupt(int irq, void *dev_id);
static int my_dev_id,irq_counter=0;



/**************************************************************************************************
Syntax:      	  SINT32 p2020Init_irq()

Remarks:		  This Function test size of loff_t and size_t lenght. 

   
Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 p2020Init_irq()
{
	int err;
	
	if ((err = request_irq(81, local_interrupt, IRQF_SHARED,"test_ppc", &my_dev_id)) < 0) 
		{
		printk(KERN_ERR "Can't get IRQ ,err=%d\n",err);
		}
	
	enable_irq(81);
	
	
	
return 1;
}



/**************************************************************************************************
Syntax:      	  SINT32 p2020Exit_irq()

Remarks:		  This Function test size of loff_t and size_t lenght. 

   
Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 p2020Exit_irq()
{

	synchronize_irq(81);
	free_irq(81,&my_dev_id);
	
		    
return 1;
}





/* The interrupt handler for devices with one interrupt */
static irqreturn_t local_interrupt(int irq, void *grp_id)
{

	
	//printk("get_urq%d\n\r",irq);
	irq_counter++;
	
	printk("ISR =%d\n\r",irq_counter);

	return IRQ_NONE; 
	//return IRQ_HANDLED;
}































































/**************************************************************************************************
Syntax:      	  SINT32  p2020test_loff_t_data(loff_t from, size_t len)

Remarks:		  This Function test size of loff_t and size_t lenght. 

   
Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 p2020test_loff_t_data(loff_t from, size_t len)
{

	printk("sizeType_loff_t=%x,sizeType_size_t=%x\n\r",(int)from,(int)len);
    return STATUS_OK;
}

/**************************************************************************************************
Syntax:      	    SINT32 p2020testsizeof()

Remarks:			This Function wait for complete operations Read/Write. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 p2020testsizeof()
{
	unsigned char   a;
	unsigned short  b;
	unsigned long   c; 
	
	signed char     d;
	signed short    e;
	signed long     f;
	loff_t          t;
	size_t          len;
	
	printk(KERN_INFO "lott_f =  %d\n\r",sizeof t);
	printk(KERN_INFO "size_t =  %d\n\r",sizeof len);

	printk(KERN_INFO "unsigned char =  %d\n\r",sizeof a);
	printk(KERN_INFO "unsigned short= %d\n\r",sizeof b);
	printk(KERN_INFO "unsigned long= %d\n\r",sizeof c);
	printk(KERN_INFO "signed char  =  %d\n\r",sizeof d);
	printk(KERN_INFO "signed short= %d\n\r",sizeof e);
	printk(KERN_INFO "signed long =  %d\n\r",sizeof f);
	
 return STATUS_OK;	
}

/**************************************************************************************************
Syntax:      	    static SINT32 p2020TestBus(void);

Remarks:			This Function wait for complete operations Read/Write. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 p2020TestBus(void)
{
	
	struct device_node *parent,*branch;	
	struct of_bus *bus;
	//int onesize, i, na, ns;
	
	
	branch = of_find_node_by_name(NULL, "nor");
	if (!branch) 
	{
	printk(KERN_INFO "nor: can't find localbus nor node\n");
	of_node_put(branch);
	return STATUS_ERR;
	}
	
	parent=of_get_parent(branch);
	if (!parent) 
		{
		printk(KERN_INFO "parent: can't find parent node\n");
		of_node_put(parent);
		return STATUS_ERR;
		}
	
	//bus = of_match_bus(parent);
	printk(KERN_INFO "PARENT_NAME=%s\n",parent->name);
	
	
	
	
return 	STATUS_OK;
}


/**************************************************************************************************
Syntax:      	    SINT32 MPCInitlocalBus(void)

Remarks:			This Function wait for complete operations Read/Write. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 MPCInitlocalBus(void)
{
	 struct device_node             *dev_node;
	 struct of_device               *of_dev;
	 struct fsl_lbc_ctrl            *fsl_lbc_ctrl_dev;
	 
	    dev_node = of_find_node_by_name(NULL, "localbus");
		if (!dev_node) 
		{
			printk(KERN_INFO "fsl-lbc: can't find localbus node\n");
			of_node_put(dev_node);
			return STATUS_ERR;
		}
	 	
		of_dev=of_find_device_by_node(dev_node);
		if (!of_dev) 
			{
				printk(KERN_INFO "fsl-lbc: can't find localbus device\n");
				return STATUS_ERR;
			}
	
		fsl_lbc_ctrl_dev = kzalloc(sizeof(*fsl_lbc_ctrl_dev), GFP_KERNEL);
		if (!fsl_lbc_ctrl_dev)
			return -ENOMEM;	
		
		dev_set_drvdata(&of_dev->dev, fsl_lbc_ctrl_dev);
		
		
		fsl_lbc_ctrl_dev->regs = of_iomap(of_dev->node, 0);
		if (!fsl_lbc_ctrl_dev->regs) 
		{
			dev_err(&of_dev->dev, "failed to get memory region\n");
			return -ENODEV;
			//goto err;
		}
		
		lbc=fsl_lbc_ctrl_dev->regs;
		if(!lbc)
			{
				 printk(KERN_INFO "ERROR_LBC\n");	
				 return STATUS_ERR;
			}
	
return STATUS_OK;
}


/**************************************************************************************************
Syntax:      	    static SINT32  MPCDumplocalbusreg(void);

Remarks:			This Function wait for complete operations Read/Write. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32  MPCDumplocalbusreg(void)
{
	printk("MPC_P2020 Enchanged Local BUS DUMP Registers#\r\n"); 
	printk("--------------------------------------------------|#\r\n");
	printk("Base Register0   =0x%x  \n\r",lbc->bank[0].br);
	printk("Base Register1   =0x%x  \n\r",lbc->bank[1].br);
	printk("Base Register2   =0x%x  \n\r",lbc->bank[2].br);
	
	printk("Option Register0 =0x%x  \n\r",lbc->bank[0].or);
	printk("Option Register1 =0x%x  \n\r",lbc->bank[1].or);
	printk("Option Register2 =0x%x  \n\r",lbc->bank[2].or);
	
	
	printk("Transfer Error Attributes Register (LTEATR) =0x%x\n\r",lbc->lteatr);
	printk("Transfer Error Address Register (LTEAR) =0x%x\n\r",lbc->ltear);
	
	printk("Local Bus Configuration Register (LBCR) =0x%x\n\r",lbc->lbcr);
	printk("Local Bus Clock Ratio Register (LCRR) =0x%x\n\r",lbc->lcrr);
	
	
	printk("Flash Instruction Register (FIR) =0x%x\n\r",lbc->fir);
	printk("Flash Byte counter (FBCR) =0x%x\n\r",lbc->fbcr);
	
	
	printk("--------------------------------------------------|#\r\n");		
	
return STATUS_OK;

}


/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/














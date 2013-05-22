/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvlbcCyclone.c
*
* Description : Defenition function for P2020 <-> Cyclone 3  local bus p2020mps.
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
* --------- $Log: mpcdrvlbcCyclone.c $
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



#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/concat.h>
#include <linux/mtd/cfi.h>

#include <linux/mtd/map.h>

#include <linux/of.h>
#include <linux/of_platform.h>
/*External Header*/

//#include "mpcdrvlbcnor.h"

#include "mpcdrvlbcCyclone.h"

/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/


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


/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/
struct map_info    *map; 

/**************************************************************************************************
Syntax:      	    SINT32 Cyc3Init();

Remarks:			This Function wait for complete operations Read/Write. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 test_Cyc3Init()
{
	

	printk("+++++++++++++++++++++Init MAP_INFO_Structure++++++++++++++++++++++++++\n\r");	
	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (!map) {printk(KERN_WARNING "Failed to allocate memory for MAP_INFO_Structure\n");
	   		   return NULL;}
	
	map->name="Cyclone3";//dev_name(&dev->dev);
	map->phys=0xef000000;//res.start;
	map->size=0x01000000;//res_size; 16Mb
	map->bankwidth=2;//width;
	map->fldrv_priv= NULL;//cfi;
	map->virt=ioremap(map->phys,map->size);
	
	
	//
	if (!map->virt)
	{
	printk(KERN_INFO "Failed to ioremap() flash region\n");
	return STATUS_ERR;
	}
	
return 1;
}





/**************************************************************************************************
Syntax:      	    SINT32 Cyc3Read()

Remarks:			This Function wait for complete operations Read/Write. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 test_Cyc3Read()
{
int i;
loff_t start_offset=0x0000000000000000;
UINT16 y;



    printk("++++++++++mpcCyclone3Readata-+++++++++++\n\r") ;
   
    
  // y= __raw_readw(map->virt + start_offset);
    //Old_functions!!!!
    //map_copy_from(map, t_buf, start_offset, len);
	
	
	for(i=0;i<=16;i++)
	{
		y= __raw_readw(map->virt + start_offset+i);
		//printk("iter%d  0x%04x\n\r",i,t_buf[i]);
		printk("iter%d  0x%04x\n\r",i,y);  
	}

return 1;
}


/**************************************************************************************************
Syntax:      	    SINT32 Cyc3Write()

Remarks:			This Function wait for complete operations Read/Write. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 test_Cyc3Write()
{
UINT16  x=0x2222;	
loff_t ofs=0x0000000000000000;

printk("Start_write_to_PLIS\n\r");      
			 //__raw_writeb(x, map->virt + ofs);
			  __raw_writew(x, map->virt + ofs);
			//__raw_writel(x, map->virt + ofs);
	//map_write(map, CMD(0xB0), chip->in_progress_block_addr);
	//map_write(map, datum, adr);
	return 1;

}








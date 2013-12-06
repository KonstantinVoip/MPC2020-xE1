/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*     ..                                          All rights reserved.                                                  *
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



#include <linux/mtd/map.h>
#include <linux/of.h>
#include <linux/of_platform.h>
/*External Header*/
#include <linux/delay.h>  // mdelay ,msleep,
//#include <linux/if_ether.h>

#include "mpcdrvlbcCyclone.h"
#include "mpcdrv_gianfar.h"
//#include "mpcdrvngraf.h"
/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
//#define PLIS_DEBUG_1400  1


/**********************DEBUG READ PLIS OPERATION***********************/

//#define  TDM_DIR_0_READ_DEBUG   1
//#define  TDM_DIR_0_WRITE_DEBUG  1


//#define  TDM_DIR_1_READ_DEBUG   1
//#define  TDM_DIR_2_READ_DEBUG   1
//#define  TDM_DIR_3_READ_DEBUG   1
//#define  TDM_DIR_4_READ_DEBUG   1
 
//#define  TDM_DIR_1_WRITE_DEBUG  1 
//#define  TDM_DIR_2_WRITE_DEBUG  1
//#define  TDM_DIR_3_WRITE_DEBUG  1
//#define  TDM_DIR_4_WRITE_DEBUG  1
//#define  TDM_DIR_5_WRITE_DEBUG  1





///////////////////////TDM DIRECTION TEST DEBUG FUNCTION//////////////
 // #define TDM0_DIR_TEST_ETHERNET_SEND  1
 //#define TDM1_DIR_TEST_ETHERNET_SEND  1
 //#define TDM2_DIR_TEST_ETHERNET_SEND  1
 //#define TDM3_DIR_TEST_ETHERNET_SEND  1
 //#define TDM4_DIR_TEST_ETHERNET_SEND  1
 //#define TDM5_DIR_TEST_ETHERNET_SEND  1
 //#define TDM6_DIR_TEST_ETHERNET_SEND  1
 //#define TDM7_DIR_TEST_ETHERNET_SEND  1
 //#define TDM8_DIR_TEST_ETHERNET_SEND  1
 //#define TDM8_DIR_TEST_ETHERNET_SEND  1

///////////////////////////////////////////////////////////
/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
static inline UINT16  plis_read16 (const UINT16 addr);
static inline void    plis_write16(const UINT16 addr,const UINT16 value);
/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/


/*KERNEL MODULE HANDLING*/


/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/

  /*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#define ADD_ETHERNET_PREAMBULA 1


/*****************************************************************************/
/*	EXTERNAL REFERENCES						     */
/*****************************************************************************/





//extern void nbuf_set_datapacket_dir0 (const u16 *in_buf ,const u16 in_size);
/*функция для передачи пакета в матрицу коммутации для определния куда его направитъ*/
extern void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii,u8 otkuda_paket_tsec_tdm);


//extern void p2020_get_recieve_virttsec_packet_buf(UINT16 *buf,UINT16 len);
//extern void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);
/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
/*
static u8 eth_preambula_mas[8]=
{0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x5d};
*/

/*
static UINT16 eth_preambula_mas[4]=
{0x5555,0x5555,0x5555,0x555d};


/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/
struct map_info    *map; 
/**************************************************************************************************
Syntax:      	    LocalBusCyc3_Init();

Remarks:			This Function Init Cyclone3 device 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void LocalBusCyc3_Init()
{
	
	printk("++LocalBusCyc3_Init_func()_120++\n\r");
	//printk("+++++++++++++++++++++Init MAP_INFO_Structure++++++++++++++++++++++++++\n\r");	
	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (!map) 
	{
    printk("ERROR_Failed to allocate memory for MAP_INFO_Structure\n");	   
	return NULL;
	}
	
	map->name="Cyclone3";//dev_name(&dev->dev);
	map->phys=0xef000000;//res.start;
	map->size=0x01000000;//res_size; 16Mb
	map->bankwidth=2;//width;
	map->fldrv_priv= NULL;//cfi;
	//map->virt=0xe1180000;//ioremap(map->phys,map->size);
	map->virt=ioremap(map->phys,map->size);
	//printk("MAP_VIRT_OFFSET=0x%08x\n\r",map->virt);
	//
	if (!map->virt)
	{
	printk("ERROR_Failed to ioremap() flash region\n");
	return STATUS_ERR;
	}
	
   //return 1;
}
/**************************************************************************************************
Syntax:      	    UINT16 plis_read16 (const u16 addr)

Remarks:			This Function wait for complete operations Read/Write. 

Return Value:	    Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
inline UINT16 plis_read16 (const UINT16 addr)
{
  UINT16 out_data=0x0000;
 // UINT16 byte_offset=0x0000; 
  //UINT16 out_data=0x0000;
  //byte_offset = addr*2;
  //out_data= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+byte_offset);
  out_data= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));	
return out_data;
}
/**************************************************************************************************
Syntax:      	    void   plis_write16(const u16 addr,u16 value)
Remarks:			This Write to PLIS  address value. 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
inline void   plis_write16(const UINT16 addr,const  UINT16 value)
{
 //UINT16 byte_offset=0x0000; 	
 //byte_offset = addr*2;
 //__raw_writew(value, map->virt + PLIS_LINUX_START_DATA_OFFSET+byte_offset);
   __raw_writew(value, map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));  
}


/**************************************************************************************************
Syntax:      	    UINT16 TDM0_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM0_direction_READ_READY(void)
{
UINT16 dannie1000=0;
UINT16 dannie800=0;
UINT16 status =0; 


	dannie1000=plis_read16 (DIR0_PLIS_READOK_ADDR1000);
	if((dannie1000==0xabc1) || (dannie1000==0x1))
    {
	  status =1; 
  	} 
	else
	{
	  status=0;	
	  //printk("Status1000 =0x%x\n\r",dannie1000);
	  return (status);	
	}
	 
	// msleep(100);
  	//mdelay(50);
    dannie800=plis_read16 (DIR0_PLIS_READ_BUG_ADDR800);
	if((dannie800==0x1)||(dannie800==0xabc1))
	{
		status =1;
	}
	else
	{
		status =0;
	}
/*	printk("dannie1000_dir0=0x%x->>>>>",dannie1000);
	printk("dannie800_dir0=0x%x->>>>>>",dannie800);
	printk("Status_dir0 =%d\n\r",status);
*/	
return status;
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM1_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM1_direction_READ_READY(void)
{
UINT16 dannie1002=0;
UINT16 status =0; 
UINT16 dannie802=0;

	dannie1002=plis_read16 (DIR1_PLIS_READOK_ADDR1002);
	if((dannie1002==0xabc1) || (dannie1002==0x1))
	{
		status =1; 
	} 
	else
	{
	   status=0;	
	   //printk("Status1000 =0x%x\n\r",dannie1000);
	   return (status);	
	}
	//mdelay(400);
	dannie802=plis_read16 (DIR1_PLIS_READ_BUG_ADDR802);
	if((dannie802==0x1)||(dannie802==0xabc1))
	{
	status =1;	
	}
	else
	{
	status =0;
	}
	//printk("dannie1002_dir1=0x%x->>>>>",dannie1002);
	//printk("dannie802_dir1=0x%x->>>>>>",dannie802);
	//printk("Status_dir1 =%d\n\r",status);

return status;








		
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM2_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM2_direction_READ_READY(void)
{
 UINT16 dannie1004=0;
 UINT16 status =0;
 UINT16 dannie804=0;

	dannie1004=plis_read16 (DIR2_PLIS_READOK_ADDR1004);
	if((dannie1004==0xabc1) || (dannie1004==0x1))
	{
		status =1; 
	} 
	else
	{
	   status=0;	
	   //printk("Status1000 =0x%x\n\r",dannie1000);
	   return (status);	
	}
	//mdelay(400);
	dannie804=plis_read16 (DIR2_PLIS_READ_BUG_ADDR804);
	if((dannie804==0x1)||(dannie804==0xabc1))
	{
	status =1;	
	}
	else
	{
	status =0;
	}
	//printk("dannie1004_dir1=0x%x->>>>>",dannie1004);
	//printk("dannie804_dir1=0x%x->>>>>>",dannie804);
	//printk("Status_dir2 =%d\n\r",status);

return status;
	
	
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM3_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM3_direction_READ_READY(void)
{
	UINT16 dannie1006=0;
	UINT16 status =0;
	UINT16 dannie806=0;

	dannie1006=plis_read16 (DIR3_PLIS_READOK_ADDR1006);
	if((dannie1006==0xabc1) || (dannie1006==0x1))
	{
		status =1; 
	} 
	else
	{
	   status=0;	
	   //printk("Status1000 =0x%x\n\r",dannie1000);
	   return (status);	
	}
	//mdelay(400);
	dannie806=plis_read16 (DIR3_PLIS_READ_BUG_ADDR806);
	if((dannie806==0x1)||(dannie806==0xabc1))
	{
	status =1;	
	}
	else
	{
	status =0;
	//printk("dannie806_dir3=0x%x->>>>>>",dannie806);
	}
	//printk("dannie1006_dir1=0x%x->>>>>",dannie1006);
	//printk("dannie806_dir1=0x%x->>>>>>",dannie806);
	//printk("Status_dir3 =%d\n\r",status);

return status;
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM4_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM4_direction_READ_READY(void)
{
	
	UINT16 dannie1008=0;
	UINT16 tdmdir4_count_visim=0;
	UINT16 dannie808=0;

		 	 while(!dannie1008)
		 	 {	 
		 		 if(tdmdir4_count_visim==20)return 0;
			 
		 		dannie1008=plis_read16 (DIR4_PLIS_READOK_ADDR1008);
		 		 if(dannie1008==0xabc0)
		 		 {
		 			dannie1008=0; 
				 //printk("VISIM_READ_READY=0x%x\n\r",dannie1000);
		 		 }
		 		tdmdir4_count_visim++; 
		    }
	dannie808=plis_read16 (DIR4_PLIS_READ_BUG_ADDR808);
	return 1;
	
}
			
/**************************************************************************************************
Syntax:      	    UINT16 TDM5_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM5_direction_READ_READY(void)
{
	UINT16 dannie1010=0;
	UINT16 tdmdir5_count_visim=0;
	UINT16 dannie810=0;

		 	 while(!dannie1010)
		 	 {	 
		 		 if(tdmdir5_count_visim==20)return 0;
			 
		 		dannie1010=plis_read16 (DIR5_PLIS_READOK_ADDR1010);
		 		 if(dannie1010==0xabc0)
		 		 {
		 			dannie1010=0; 
				 //printk("VISIM_READ_READY=0x%x\n\r",dannie1000);
		 		 }
		 		tdmdir5_count_visim++; 
		    }
	dannie810=plis_read16 (DIR5_PLIS_READ_BUG_ADDR810);
	return 1;

}
								
/**************************************************************************************************
Syntax:      	    UINT16 TDM6_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM6_direction_READ_READY(void)
{
	UINT16 dannie1012=0;
	UINT16 tdmdir6_count_visim=0;
	UINT16 dannie812=0;

		 	 while(!dannie1012)
		 	 {	 
		 		 if(tdmdir6_count_visim==20)return 0;
			 
		 		dannie1012=plis_read16 (DIR6_PLIS_READOK_ADDR1012);
		 		 if(dannie1012==0xabc0)
		 		 {
		 			dannie1012=0; 
				 //printk("VISIM_READ_READY=0x%x\n\r",dannie1000);
		 		 }
		 		tdmdir6_count_visim++; 
		    }
	dannie812=plis_read16 (DIR6_PLIS_READ_BUG_ADDR812);
	return 1;
}								
/**************************************************************************************************
Syntax:      	    UINT16 TDM7_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM7_direction_READ_READY(void)
{
	UINT16 dannie1014=0;
	UINT16 tdmdir7_count_visim=0;
	UINT16 dannie814=0;

		 	 while(!dannie1014)
		 	 {	 
		 		 if(tdmdir7_count_visim==20)return 0;
			 
		 		dannie1014=plis_read16 (DIR7_PLIS_READOK_ADDR1014);
		 		 if(dannie1014==0xabc0)
		 		 {
		 			dannie1014=0; 
				 //printk("VISIM_READ_READY=0x%x\n\r",dannie1000);
		 		 }
		 		tdmdir7_count_visim++; 
		    }
	dannie814=plis_read16 (DIR7_PLIS_READ_BUG_ADDR814);
	return 1;
}
	
	
/**************************************************************************************************
Syntax:      	    UINT16 TDM8_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM8_direction_READ_READY(void)
{

	UINT16 dannie1016=0;
	UINT16 tdmdir8_count_visim=0;
	UINT16 dannie816=0;

		 	 while(!dannie1016)
		 	 {	 
		 		 if(tdmdir8_count_visim==20)return 0;
			 
		 		dannie1016=plis_read16 (DIR8_PLIS_READOK_ADDR1016);
		 		 if(dannie1016==0xabc0)
		 		 {
		 			dannie1016=0; 
				 //printk("VISIM_READ_READY=0x%x\n\r",dannie1000);
		 		 }
		 		tdmdir8_count_visim++; 
		    }
	dannie816=plis_read16 (DIR8_PLIS_READ_BUG_ADDR816);	
	return 1;
}
		
		
/**************************************************************************************************
Syntax:      	    UINT16 TDM9_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM9_direction_READ_READY(void)
{
	UINT16 dannie1018=0;
	UINT16 tdmdir9_count_visim=0;
	UINT16 dannie818=0;

		 	 while(!dannie1018)
		 	 {	 
		 		 if(tdmdir9_count_visim==20)return 0;
			 
		 		dannie1018=plis_read16 (DIR9_PLIS_READOK_ADDR1018);
		 		 if(dannie1018==0xabc0)
		 		 {
		 			dannie1018=0; 
				 //printk("VISIM_READ_READY=0x%x\n\r",dannie1000);
		 		 }
		 		tdmdir9_count_visim++; 
		    }
	dannie818=plis_read16 (DIR9_PLIS_READ_BUG_ADDR818);	
	return 1;

}


//*************************************WRITE_READY******************************//
//*****************************************************************************//
//*****************************************************************************//
/**************************************************************************************************
Syntax:      	    UINT16 TDM0_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM0_direction_WRITE_READY(void)
{
  UINT16  dannie30=1;

 //Next step Set delay to write succes operations !!!!!!!!!!!
 ////////////////////////////////////////////////////////////
 //mdelay(10);
 dannie30=plis_read16 (DIR0_PLIS_WRITEOK_ADDR30); 
 //printk("register 30 dannie= %d ready\n\r",dannie30);
 if(dannie30==0)
 {return 1; }
 else
 {return 0;}
 
 //old varioant 
 /*
 while(dannie30)
 {	 
  dannie30=plis_read16 (DIR0_PLIS_WRITEOK_ADDR30); 	 
  //printk("register 30 dannie= %d ready\n\r",dannie30);
 }
 */
 //printk("WRITE_READY_OK=%d\n\r",dannie30);
 //return 1; //WRITE READY SUCCESS
 // #ifdef TDM_DIRECTION0_WRITE_DEBUG	
 /*printk("WRITE_READY=%d\n\r",dannie30);
 //#endif
 if (dannie30 ==0) return 1; //WRITE READY SUCCESS
 if (dannie30 ==1) return 0; //NOT READY to write to PLIS
 */
 //else return 0;
}


/**************************************************************************************************
Syntax:      	    UINT16 TDM1_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM1_direction_WRITE_READY(void)
{
    UINT16  dannie32=1; 
	//Next step Set delay to write succes operations !!!!!!!!!!!
	////////////////////////////////////////////////////////////
	//mdelay(20);
	dannie32=plis_read16 (DIR1_PLIS_WRITEOK_ADDR32); 
	//printk("register 30 dannie= %d ready\n\r",dannie30);
	if(dannie32==0)
	{return 1; }
	else
	{return 0;}
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM2_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM2_direction_WRITE_READY(void)
{
	UINT16  dannie34=1; 
	//Next step Set delay to write succes operations !!!!!!!!!!!
	////////////////////////////////////////////////////////////
	//mdelay(20);
	dannie34=plis_read16 (DIR2_PLIS_WRITEOK_ADDR34); 
	//printk("register 34 dannie= %d ready\n\r",dannie3);
	if(dannie34==0)
	{return 1; }
	else
	{return 0;}
	
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM3_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM3_direction_WRITE_READY(void)
{
	UINT16  dannie36=1; 
	//Next step Set delay to write succes operations !!!!!!!!!!!
	////////////////////////////////////////////////////////////
	
	dannie36=plis_read16 (DIR3_PLIS_WRITEOK_ADDR36); 
	//printk("register 30 dannie= %d ready\n\r",dannie30);
	if(dannie36==0)
	{return 1; }
	else
	{return 0;}
	
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM4_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM4_direction_WRITE_READY(void)
{
	UINT16  dannie38=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
		 while(dannie38)
		 {
		  dannie38=plis_read16 (DIR4_PLIS_WRITEOK_ADDR38); 
		 }
		 //printk("WRITE_READY_OK=%d\n\r",dannie30);
	return 1;		
}			
/**************************************************************************************************
Syntax:      	    UINT16 TDM5_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM5_direction_WRITE_READY(void)
{
	UINT16  dannie40=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
		 while(dannie40)
		 {
		  dannie40=plis_read16 (DIR5_PLIS_WRITEOK_ADDR40); 
		 }
		 //printk("WRITE_READY_OK=%d\n\r",dannie30);
	return 1;			
				
}				
				
				
				
/**************************************************************************************************
Syntax:      	    UINT16 TDM6_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM6_direction_WRITE_READY(void)
{
	UINT16  dannie42=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
		 while(dannie42)
		 {
		  dannie42=plis_read16 (DIR6_PLIS_WRITEOK_ADDR42); 
		 }
		 //printk("WRITE_READY_OK=%d\n\r",dannie30);
	return 1;				
					
									
}					
					
					
/**************************************************************************************************
Syntax:      	    UINT16 TDM7_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM7_direction_WRITE_READY(void)
{
	UINT16  dannie44=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
		 while(dannie44)
		 {
		  dannie44=plis_read16 (DIR7_PLIS_WRITEOK_ADDR44); 
		 }
		 //printk("WRITE_READY_OK=%d\n\r",dannie30);
	return 1;					
																	
}					
						
						
						
/**************************************************************************************************
Syntax:      	    UINT16 TDM8_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM8_direction_WRITE_READY(void)
{
	UINT16  dannie46=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
		 while(dannie46)
		 {
		  dannie46=plis_read16 (DIR8_PLIS_WRITEOK_ADDR46); 
		 }
		 //printk("WRITE_READY_OK=%d\n\r",dannie30);
	return 1;
	
}	
	
	
	
	
/**************************************************************************************************
Syntax:      	    UINT16 TDM9_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM9_direction_WRITE_READY(void)
{
	UINT16  dannie48=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
		 while(dannie48)
		 {
		  dannie48=plis_read16 (DIR9_PLIS_WRITEOK_ADDR48); 
		 }
		 //printk("WRITE_READY_OK=%d\n\r",dannie30);
	return 1;

}




//*************************************WRITE_DATA_TO_PLIS******************************//
//*****************************************************************************//
//*****************************************************************************//


/**************************************************************************************************
Syntax:      	void TDM0_direction_write (const u16 *in_buf ,const u16 in_size)num_of_tdm_ch
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM0_direction_write (const u16 *in_buf ,const u16 in_size)
{
	
	u16 i=0;
    static UINT16 tdm0_write_iteration=0;
    u16 hex_element_size=0;
    u8  dir0_dop_nechet_packet=0;
    //u16 packet_size =511;
    
    #ifdef PLIS_DEBUG_1400 
    u16 dannie1400 =0; 
	#endif	   
    
    
     //printk("in_size=%d\n\r",in_size);
    
    
    //printk("ostatok_ot_delenia=%d|in_size=%d\n\r",(in_size)%2,in_size);
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
    	dir0_dop_nechet_packet=1;
    }
    
    
    hex_element_size=(in_size/2)+dir0_dop_nechet_packet;
    //Set size on PLIS in byte
   
    
    printk("+Tdm_Dir0_write->>!ITERATION=%d!|in_byte=%d|in_hex=%d+\n\r",tdm0_write_iteration,in_size,hex_element_size);
    
	#ifdef  TDM_DIR_0_WRITE_DEBUG	
	printk("+Tdm_Dir0_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir0_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
	#endif
    
	
	plis_write16(DIR0_PLIS_PACKSIZE_ADDR1600,in_size);
	
	for(i=0;i<hex_element_size/*+PATCHlbc_ONE_ITERATION_WRITE*/;i++)
	{
	    //mdelay(1);
		plis_write16( DIR0_PLIS_WRITE_ADDR200 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR0_PLIS_WRITEOK_ADDR30 ,PLIS_WRITE_SUCCESS);
	//printk("+Tdm_Dir0_write->>!ITERATION=%d!|in_byte=%d|in_hex=%d+\n\r",tdm0_write_iteration,in_size,hex_element_size);
	
#ifdef PLIS_DEBUG_1400
	dannie1400 =plis_read16 (PLIS_ADDR1400);
	printk("Read_Iter_dannie1400_After30_Success->Rdata=0x%x|\n\r",dannie1400);
#endif	
	
	tdm0_write_iteration++;	
}

/**************************************************************************************************
Syntax:      	void TDM1_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM1_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm1_write_iteration=0;
    u16 hex_element_size=0;
    u8  dir1_dop_nechet_packet=0;
    
     
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
    	dir1_dop_nechet_packet=1;
    }
    
	
	hex_element_size=(in_size/2)+dir1_dop_nechet_packet;   
    printk("+Tdm_Dir1_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm1_write_iteration,in_size,hex_element_size);
	

#ifdef  TDM_DIR_1_WRITE_DEBUG	    
    printk("+Tdm_Dir1_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir1_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
#endif    
	
	
	
	
	
	
	plis_write16(DIR1_PLIS_PACKSIZE_ADDR1602,in_size);
	
	
	for(i=0;i<hex_element_size/*+PATCHlbc_ONE_ITERATION_WRITE*/;i++)
	{
		plis_write16(DIR1_PLIS_WRITE_ADDR202 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR1_PLIS_WRITEOK_ADDR32 ,PLIS_WRITE_SUCCESS);
	tdm1_write_iteration++;

}


/**************************************************************************************************
Syntax:      	void TDM2_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM2_direction_write (const u16 *in_buf ,const u16 in_size)
{
	
	u16 i=0;
    static UINT16 tdm2_write_iteration=0;
    u16 hex_element_size=0;
    u8  dir2_dop_nechet_packet=0;
    
    
    
    
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
    	dir2_dop_nechet_packet=1;
    }
    
	
	hex_element_size=(in_size/2)+dir2_dop_nechet_packet; 
    
    
    //hex_element_size=in_size/2;
    
   // printk("+Tdm_Dir2_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm2_write_iteration,in_size,hex_element_size);
#ifdef  TDM_DIR_2_WRITE_DEBUG	   
    printk("+Tdm_Dir2_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir2_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
#endif    
	
	
	plis_write16(DIR2_PLIS_PACKSIZE_ADDR1604,in_size);
	
	for(i=0;i<hex_element_size/*+PATCHlbc_ONE_ITERATION_WRITE*/;i++)
	{
		plis_write16(DIR2_PLIS_WRITE_ADDR204  ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR2_PLIS_WRITEOK_ADDR34 ,PLIS_WRITE_SUCCESS);
	
	
	//printk("+Tdm_Dir2_write->>!ITERATION=%d!|in_byte=%d|in_hex=%d+\n\r",tdm2_write_iteration,in_size,hex_element_size);
	//mdelay(250);
	
	
	tdm2_write_iteration++;

}	
	
/**************************************************************************************************
Syntax:      	void TDM3_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM3_direction_write (const u16 *in_buf ,const u16 in_size)
{
	
	u16 i=0;
    static UINT16 tdm3_write_iteration=0;
    u16 hex_element_size=0;
    u8  dir3_dop_nechet_packet=0;
    
    
  
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
    	dir3_dop_nechet_packet=1;
    }
    
	hex_element_size=(in_size/2)+dir3_dop_nechet_packet; 
    //printk("+Tdm_Dir3_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm3_write_iteration,in_size,hex_element_size);
	
    
#ifdef  TDM_DIR_3_WRITE_DEBUG	   
    printk("+Tdm_Dir3_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir3_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
#endif    
	
	
	plis_write16(DIR3_PLIS_PACKSIZE_ADDR1606,in_size);
	
	for(i=0;i<hex_element_size+PATCHlbc_ONE_ITERATION_WRITE;i++)
	{
		plis_write16(DIR3_PLIS_WRITE_ADDR206  ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR3_PLIS_WRITEOK_ADDR36 ,PLIS_WRITE_SUCCESS);
	tdm3_write_iteration++;

}		
		
/**************************************************************************************************
Syntax:      	void TDM4_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM4_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm4_write_iteration=0;
    u16 hex_element_size=0;
    
    hex_element_size=in_size/2;
    printk("+Tdm_Dir4_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm4_write_iteration,in_size,hex_element_size);
	printk("+Tdm_Dir4_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir4_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
	for(i=0;i<hex_element_size+PATCHlbc_ONE_ITERATION_WRITE;i++)
	{
		plis_write16(DIR4_PLIS_WRITE_ADDR208 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR4_PLIS_WRITEOK_ADDR38 ,PLIS_WRITE_SUCCESS);
	tdm4_write_iteration++;	
			
}			
					
/**************************************************************************************************
Syntax:      	void TDM5_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM5_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm5_write_iteration=0;
    u16 hex_element_size=0;
    
    hex_element_size=in_size/2;
    printk("+Tdm_Dir5_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm5_write_iteration,in_size,hex_element_size);
	printk("+Tdm_Dir5_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir5_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
	for(i=0;i<hex_element_size+PATCHlbc_ONE_ITERATION_WRITE;i++)
	{
		plis_write16(DIR5_PLIS_WRITE_ADDR210 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR5_PLIS_WRITEOK_ADDR40 ,PLIS_WRITE_SUCCESS);
	tdm5_write_iteration++;			
				
}							
/*************************************************************************************************
Syntax:      	void TDM6_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM6_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm6_write_iteration=0;
    u16 hex_element_size=0;
    
    hex_element_size=in_size/2;
    printk("+Tdm_Dir6_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm6_write_iteration,in_size,hex_element_size);
	printk("+Tdm_Dir6_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir6_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
	for(i=0;i<hex_element_size+PATCHlbc_ONE_ITERATION_WRITE;i++)
	{
		plis_write16(DIR6_PLIS_WRITE_ADDR212 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR6_PLIS_WRITEOK_ADDR42 ,PLIS_WRITE_SUCCESS);
	tdm6_write_iteration++;				
}

/**************************************************************************************************
Syntax:      	void TDM7_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM7_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm7_write_iteration=0;
    u16 hex_element_size=0;
    
    hex_element_size=in_size/2;
    printk("+Tdm_Dir7_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm7_write_iteration,in_size,hex_element_size);
	printk("+Tdm_Dir7_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir7_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
	for(i=0;i<hex_element_size+PATCHlbc_ONE_ITERATION_WRITE;i++)
	{
		plis_write16(DIR7_PLIS_WRITE_ADDR214 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR7_PLIS_WRITEOK_ADDR44 ,PLIS_WRITE_SUCCESS);
	tdm7_write_iteration++;			
				
}

/**************************************************************************************************
Syntax:      	void TDM8_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM8_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm8_write_iteration=0;
    u16 hex_element_size=0;
    
    hex_element_size=in_size/2;
    printk("+Tdm_Dir8_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm8_write_iteration,in_size,hex_element_size);
	printk("+Tdm_Dir8_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir8_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
	for(i=0;i<hex_element_size+PATCHlbc_ONE_ITERATION_WRITE;i++)
	{
		plis_write16(DIR8_PLIS_WRITE_ADDR216 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR8_PLIS_WRITEOK_ADDR46,PLIS_WRITE_SUCCESS);
	tdm8_write_iteration++;					
}

/**************************************************************************************************
Syntax:      	void TDM9_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM9_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm9_write_iteration=0;
    u16 hex_element_size=0;
    
    hex_element_size=in_size/2;
    printk("+Tdm_Dir9_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm9_write_iteration,in_size,hex_element_size);
	printk("+Tdm_Dir9_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir9_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
	for(i=0;i<hex_element_size+PATCHlbc_ONE_ITERATION_WRITE;i++)
	{
		plis_write16(DIR9_PLIS_WRITE_ADDR218  ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR9_PLIS_WRITEOK_ADDR48 ,PLIS_WRITE_SUCCESS);
	tdm9_write_iteration++;					
}



//*************************************READ_DATA_FROM_PLIS******************************//
//*****************************************************************************//
//*****************************************************************************//


/**************************************************************************************************
Syntax:      	   void TDM0_dierction_read  (UINT16 *out_buf,UINT16  out_size_byte)
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM0_dierction_read ()
//void TDM0_dierction_read  (UINT16 *out_buf,UINT16  out_size_byte)
{
  UINT16 dannie1200=0; 
  UINT16 i=0;		  
  static UINT16 tdm0_read_iteration=0;
  UINT16 packet_size_hex=0;
  UINT16 ostatok_of_size_packet=0;
  UINT16  out_buf[760];//1518 bait;
  UINT8   dir0_dobavka_esli_packet_nechetnii=0;  

  //
  __be32  dir0_target_arp_ip_da_addr=0;
  //IP DA address read on direction 0
  __be32  dir0_ip_da_addr    = 0;
  //MAC DA address read on direction 0
  UINT8   dir0_mac_da_addr   = 0;
  //Priznak Sevi Packet 22-ff _predposlednii bait.
  UINT8   dir0_mac_priznak_kys = 0; 
  UINT16  dir0_priznak_arp_packet=0;
  
  
  memset(&out_buf, 0x0000, sizeof(out_buf));  
  dannie1200 = plis_read16 (DIR0_PLIS_PACKSIZE_ADDR1200 );
  
  //Если пакет нечётный читаем на еденицу больше из шины local bus;
  if((dannie1200)%2==1)
  {
	  dir0_dobavka_esli_packet_nechetnii=1; 
  }
  

  packet_size_hex=(dannie1200/2)+dir0_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex 
  //printk("+Tdm_Dir0_read->>ITERATION=%d|1200in_byte=%d|1200in_hex=%d|size=%d|+\n\r",tdm0_read_iteration,dannie1200,packet_size_hex,dannie1200+PATCH_READ_PACKET_SIZE_ADD_ONE); 
    	  
	  //16 bit  or 2 bait Local bus iteration
	  do
	  {
	  //mdelay(5);   
      out_buf[i]= plis_read16 (DIR0_PLIS_READ_ADDR400);
      i++;           
      }while(i<packet_size_hex);

	  //SET to FIFO buffer recieve TDM0 direction FIFO buffer
	  //nbuf_set_datapacket_dir0 (out_buf,dannie1200+PATCH_READ_PACKET_SIZE_ADD_ONE);
  
#ifdef  TDM_DIR_0_READ_DEBUG	  
	   
	    printk("+Tdm_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	    printk("+Tdm_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
#endif	  
	  
	  //Пока в одной подсети использую последниее цифры потом конечно нужно доделать будет и сделать.
	  //Пока сделано очень шрубо потом доработаю.
	  //Пока затычка в виде if коммутируем по MAC признак коммутации у на последняя цифра MAC DA являеться номером подсети
	  
	  //IP DA address read on direction 0
	 dir0_ip_da_addr    = out_buf[16];
	  //MAC DA address read on direction 0
	 dir0_mac_da_addr   = out_buf[2];
	 //определяю признак KY-S по mac адресам 
	 dir0_mac_priznak_kys = out_buf[2]>>8;
	 //priznak ARP 0x806
	 dir0_priznak_arp_packet =out_buf[6];
	 //
	 
	 //printk("++dir0_ip_da_addr=0x%x|dir0_mac_da_addr=0x%x+\n\r",dir0_ip_da_addr,dir0_mac_da_addr);
	 //printk("+dir0_mac_priznak_kys+=0x%x\n\r",dir0_mac_priznak_kys);
	  // rintk("+ARP_dir0_priznak_arp_packets=0x%x\n\r",dir0_priznak_arp_packet);
	 //broadcast frame arp request
	   //пока делаю затычку Broadcast APR отсылаю в ethernet tsec2 на выход пока петоя не замкнута	 
	   if(dir0_priznak_arp_packet==0x0806)
	   { 
		  //ARP packet for matrica  //2 bait massive 
		  //16+14+8= 
		   
		   //memcpy(&dir0_target_arp_ip_da_addr,out_buf+16+14+8,4);
		   memcpy(&dir0_target_arp_ip_da_addr,out_buf+19,4);
		   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
		   ngraf_packet_for_matrica_kommutacii(out_buf ,dannie1200,(u8)dir0_target_arp_ip_da_addr,0x11);   
	 	   
	 	  //send to tsec2
		  //p2020_get_recieve_virttsec_packet_buf(out_buf,dannie1200,2);
		  //p2020_get_recieve_virttsec_packet_buf(out_buf,dannie1200);//send to eternet tsec ARP broadcast
	   }
	   else
	   {
	      // printk("paket po ip hesder\n\r");
		     ngraf_packet_for_matrica_kommutacii(out_buf ,dannie1200,(u8)dir0_ip_da_addr,0x11); 
	   }
	   
	   //priznak packeta KY-S 
	   /*
	   if(dir0_mac_priznak_kys==0x22)
	   {
	      //packet kommutacii po mac address
		  ngraf_packet_for_matrica_kommutacii(out_buf ,dannie1200,dir0_mac_da_addr,0x11); 
	   }
	   */
	
	 #ifdef TDM0_DIR_TEST_ETHERNET_SEND
     p2020_get_recieve_virttsec_packet_buf(out_buf,dannie1200/*+PATCH_READ_PACKET_SIZE_ADD_ONE*/);//send to eternet
	 #endif  
  tdm0_read_iteration++; 
}


/**************************************************************************************************
Syntax:      	   void TDM1_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM1_dierction_read ()
{

	  UINT16 dannie1202=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm1_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  UINT16 ostatok_of_size_packet=0;
	  UINT16  out_buf1[760];//1518 bait;
	  
	  
	  __be32  dir1_target_arp_ip_da_addr=0;
	  
	  UINT8   dir1_dobavka_esli_packet_nechetnii=0; 
	  //IP DA address read on direction 1
	  __be32  dir1_ip_da_addr    = 0;
	  //MAC DA address read on direction 1
	  UINT8   dir1_mac_da_addr   = 0;
	  //Priznak Sevi Packet 22-ff _predposlednii bait.
	  UINT8   dir1_mac_priznak_kys = 0; 
	  
	  UINT16  dir1_priznak_arp_packet=0;
	  
	  
	  memset(&out_buf1, 0x0000, sizeof(out_buf1));
	  dannie1202 = plis_read16 (DIR1_PLIS_PACKSIZE_ADDR1202);
	 
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1202)%2==1)
	  {
		  dir1_dobavka_esli_packet_nechetnii=1; 
	  }
	  
	  
	  
	  packet_size_hex=(dannie1202/2)+dir1_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex 
	  printk("+Tdm_Dir1_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm1_read_iteration,dannie1202,packet_size_hex);  
	  //16 bit  or 2 bait Local bus iteration
	        do
	        {
	  	  //mdelay(5);   
	        out_buf1[i]= plis_read16 (DIR1_PLIS_READ_ADDR402);
	        i++;           
	        }while(i<packet_size_hex/*+PATCHlbc_ONE_ITERATION_READ+PATCH_READ_PACKET_SIZE_ADD_ONE*/);

	  	  //SET to FIFO buffer recieve TDM0 direction FIFO buffer
	  	  //nbuf_set_datapacket_dir0 (out_buf,dannie1200+PATCH_READ_PACKET_SIZE_ADD_ONE);
	    
	  #ifdef  TDM0_DIR_1_READ_DEBUG	  
	  	    printk("+Tdm_Dir1_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf1[0],out_buf1[1],out_buf1[2],out_buf1[3],out_buf1[4],out_buf1[5]);
	  	    printk("+Tdm_Dir1_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf1[packet_size_hex-5],out_buf1[packet_size_hex-4],out_buf1[packet_size_hex-3],out_buf1[packet_size_hex-2],out_buf1[packet_size_hex-1],out_buf1[packet_size_hex]);
	  #endif	  
	  	  
	  	  //Пока в одной подсети использую последниее цифры потом конечно нужно доделать будет и сделать.
	  	  //Пока сделано очень шрубо потом доработаю.
	  	  //Пока затычка в виде if коммутируем по MAC признак коммутации у на последняя цифра MAC DA являеться номером подсети
	  	  
	  	  //IP DA address read on direction 0
	  	 dir1_ip_da_addr    = out_buf1[16];
	  	  //MAC DA address read on direction 0
	  	 dir1_mac_da_addr   = out_buf1[2];
	  	 //определяю признак KY-S по mac адресам 
	  	 dir1_mac_priznak_kys = out_buf1[2]>>8;
	  	 
	     dir1_priznak_arp_packet =out_buf1[6];
	  	 
	  	 //printk("++dir1_ip_da_addr=0x%x|dir1_mac_da_addr=0x%x+\n\r",dir1_ip_da_addr,dir1_mac_da_addr);
	  	 //printk("+dir1_mac_priznak_kys+=0x%x\n\r",dir1_mac_priznak_kys);
		 //пока делаю затычку Broadcast APR отсылаю в ethernet tsec2  
		 if(dir1_priznak_arp_packet==0x0806)
		 {
		 		 
			   memcpy(&dir1_target_arp_ip_da_addr,out_buf1+19,4);
			   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
			   ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,(u8)dir1_target_arp_ip_da_addr,0x11);  
			     //ARP packet for matrica
		 		 //ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,0x0806); 		   
			     //p2020_get_recieve_virttsec_packet_buf(out_buf1,dannie1202,2);
		 }
	  	 //priznak ky-s
	  	 else
	  	 {
	  		ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,(u8)dir1_ip_da_addr,0x11); 	  	 
	  	 }
		 
		 
		 /*
		 if(dir1_mac_priznak_kys==0x22)
	  	 {
	  		 //packet kommutacii po mac address
	  		 ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,dir1_mac_da_addr,0x11); 
	  	 }
		 */
		 
		 
		 
		 
	  	 #ifdef TDM1_DIR_TEST_ETHERNET_SEND
	       //p2020_get_recieve_virttsec_packet_buf(out_buf1,dannie1202+PATCH_READ_PACKET_SIZE_ADD_ONE);//send to eternet
	  	 #endif  
	  tdm1_read_iteration++; 
}


/**************************************************************************************************
Syntax:      	   void TDM2_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM2_dierction_read ()
{
	  UINT16 dannie1204=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm2_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  UINT16 ostatok_of_size_packet=0;
	  UINT16  out_buf2[760];//1518 bait;
	 
	  //
	  __be32  dir2_target_arp_ip_da_addr=0;
	  UINT8   dir2_dobavka_esli_packet_nechetnii=0; 
	  
	  
	  
	  //IP DA address read on direction 1
	  __be32  dir2_ip_da_addr    = 0;
	  //MAC DA address read on direction 1
	  UINT8   dir2_mac_da_addr   = 0;
	  //Priznak Sevi Packet 22-ff _predposlednii bait.
	  UINT8   dir2_mac_priznak_kys = 0; 
	  //
	  UINT16  dir2_priznak_arp_packet=0;
	  
	  
	  memset(&out_buf2, 0x0000, sizeof(out_buf2));
	  	  
	  dannie1204 = plis_read16 (DIR2_PLIS_PACKSIZE_ADDR1204 );
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1204)%2==1)
	  {
		  dir2_dobavka_esli_packet_nechetnii=1; 
	  }
	  
	  packet_size_hex=(dannie1204/2)+dir2_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  	  
	  //printk("+Tdm_Dir2_read->>ITERATION=%d|1204in_byte=%d|1204in_hex=%d|size=%d|+\n\r",tdm2_read_iteration,dannie1204,packet_size_hex,dannie1204+PATCH_READ_PACKET_SIZE_ADD_ONE); 
	 	    

	  
	 	  //  else
	 	   // {	  
	 	  	  //16 bit  or 2 bait Local bus iteration
	 	  	  do
	 	  	  {
	 	  	  //mdelay(5);   
	 	        out_buf2[i]= plis_read16 (DIR2_PLIS_READ_ADDR404);
	 	        i++;           
	 	        }while(i<packet_size_hex/*+PATCHlbc_ONE_ITERATION_READ+PATCH_READ_PACKET_SIZE_ADD_ONE*/);

	 	  	  //SET to FIFO buffer recieve TDM0 direction FIFO buffer
	 	  	  //nbuf_set_datapacket_dir0 (out_buf,dannie1200+PATCH_READ_PACKET_SIZE_ADD_ONE);
	 	    
	 	  #ifdef  TDM0_DIR_2_READ_DEBUG	  
	 	  	    printk("+Tdm_Dir2_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf2[0],out_buf2[1],out_buf2[2],out_buf2[3],out_buf2[4],out_buf2[5]);
	 	  	    printk("+Tdm_Dir2_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf2[packet_size_hex-5],out_buf2[packet_size_hex-4],out_buf2[packet_size_hex-3],out_buf2[packet_size_hex-2],out_buf2[packet_size_hex-1],out_buf2[packet_size_hex]);
	 	  #endif	  
	 	  	  
	 	  	  //Пока в одной подсети использую последниее цифры потом конечно нужно доделать будет и сделать.
	 	  	  //Пока сделано очень шрубо потом доработаю.
	 	  	  //Пока затычка в виде if коммутируем по MAC признак коммутации у на последняя цифра MAC DA являеться номером подсети
	 	  	  
	 	  	  //IP DA address read on direction 0
	 	  	 dir2_ip_da_addr    = out_buf2[16];
	 	  	  //MAC DA address read on direction 0
	 	  	 dir2_mac_da_addr   = out_buf2[2];
	 	  	 //определяю признак KY-S по mac адресам 
	 	  	 dir2_mac_priznak_kys = out_buf2[2]>>8;
	 	  	 ///Arp priznak
	 	  	 dir2_priznak_arp_packet =out_buf2[6];
	 		 
  	 
	 	  	 if(dir2_priznak_arp_packet==0x0806)
	 	  	   {
				   memcpy(&dir2_target_arp_ip_da_addr,out_buf2+19,4);
				   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
				   ngraf_packet_for_matrica_kommutacii(out_buf2 ,dannie1204,(u8)dir2_target_arp_ip_da_addr,0x11);  
	 	  		 
	 	  	     //p2020_get_recieve_virttsec_packet_buf(out_buf2,dannie1204,2);//send to eternet tsec ARP broadcast   
	 	  	   } 
	 	  	   else
	 	  	   {   
	 	  	     ngraf_packet_for_matrica_kommutacii(out_buf2 ,dannie1204,(UINT8)dir2_ip_da_addr,0x11);  
	 	  	   }	  	   

	 	  	 
               	 	  	 
	 	  	   /*
	 	  	   if(dir2_mac_priznak_kys==0x22)
	 	  	   {
	 	  		 //packet kommutacii po mac address
	 	  		 ngraf_packet_for_matrica_kommutacii(out_buf2 ,dannie1204,dir2_mac_da_addr,0x11); 
	 	  	   }
	 	  	   */
//#endif	 	  	   
	    	 
	 	  	 #ifdef TDM2_DIR_TEST_ETHERNET_SEND
	 	       //p2020_get_recieve_virttsec_packet_buf(out_buf2,dannie1204+PATCH_READ_PACKET_SIZE_ADD_ONE);//send to eternet
	 	  	 #endif  
	 
	  tdm2_read_iteration++; 

}
	
	
	
/**************************************************************************************************
Syntax:      	   void TDM3_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM3_dierction_read  ()
{
	  UINT16 dannie1206=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm3_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  UINT16 ostatok_of_size_packet=0;
	  UINT16  out_buf3[760];//1518 bait;
	  	 	  
	  //
	  __be32  dir3_target_arp_ip_da_addr=0;
	  UINT8   dir3_dobavka_esli_packet_nechetnii=0; 
	  
	  //IP DA address read on direction 1
	  __be32  dir3_ip_da_addr    = 0;
	  //MAC DA address read on direction 1
	  UINT8   dir3_mac_da_addr   = 0;
	  //Priznak Sevi Packet 22-ff _predposlednii bait.
	  UINT8   dir3_mac_priznak_kys = 0; 
	  //	 	  
	  UINT16  dir3_priznak_arp_packet=0;
	  
	  
	  memset(&out_buf3 ,0x0000, sizeof(out_buf3));	 	  
	  dannie1206 = plis_read16 (DIR3_PLIS_PACKSIZE_ADDR1206 );
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1206)%2==1)
	  {
		  dir3_dobavka_esli_packet_nechetnii=1; 
	  }
	  packet_size_hex=(dannie1206/2)+dir3_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  

	 // printk("+Tdm_Dir3_read->>ITERATION=%d|1206in_byte=%d|1206in_hex=%d|size=%d|+\n\r",tdm3_read_iteration,dannie1206,packet_size_hex,dannie1206+PATCH_READ_PACKET_SIZE_ADD_ONE); 

	  	 	  	//16 bit  or 2 bait Local bus iteration
	            do
	  	 	  	{
	  	 	  	  //mdelay(5);   
	  	 	    out_buf3[i]= plis_read16 (DIR3_PLIS_READ_ADDR406);
	  	 	    i++;           
	  	 	    }while(i<packet_size_hex+PATCHlbc_ONE_ITERATION_READ+PATCH_READ_PACKET_SIZE_ADD_ONE);

	  	 	  	  //SET to FIFO buffer recieve TDM0 direction FIFO buffer
	  	 	  	  //nbuf_set_datapacket_dir0 (out_buf,dannie1200+PATCH_READ_PACKET_SIZE_ADD_ONE);
	  	 	    
	  	 	  #ifdef  TDM0_DIR_2_READ_DEBUG	  
	  	 	  	    printk("+Tdm_Dir2_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf2[0],out_buf2[1],out_buf2[2],out_buf2[3],out_buf2[4],out_buf2[5]);
	  	 	  	    printk("+Tdm_Dir2_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf2[packet_size_hex-5],out_buf2[packet_size_hex-4],out_buf2[packet_size_hex-3],out_buf2[packet_size_hex-2],out_buf2[packet_size_hex-1],out_buf2[packet_size_hex]);
	  	 	  #endif	  
	  	 	  	  
	  	 	  	  //Пока в одной подсети использую последниее цифры потом конечно нужно доделать будет и сделать.
	  	 	  	  //Пока сделано очень шрубо потом доработаю.
	  	 	  	  //Пока затычка в виде if коммутируем по MAC признак коммутации у на последняя цифра MAC DA являеться номером подсети
	  	 	  	  
	  	 	  	  //IP DA address read on direction 0
	  	 	  	 dir3_ip_da_addr    = out_buf3[16];
	  	 	  	  //MAC DA address read on direction 0
	  	 	  	 dir3_mac_da_addr   = out_buf3[2];
	  	 	  	 //определяю признак KY-S по mac адресам 
	  	 	  	 dir3_mac_priznak_kys = out_buf3[2]>>8;
	  	 	  	 //
	  	 	     dir3_priznak_arp_packet =out_buf3[6];
	  	 	  	 	 	  	 
	  	 	  	 //printk("++dir1_ip_da_addr=0x%x|dir1_mac_da_addr=0x%x+\n\r",dir1_ip_da_addr,dir1_mac_da_addr);
	  	 	  	 //printk("+dir1_mac_priznak_kys+=0x%x\n\r",dir1_mac_priznak_kys);
		 	  	 if(dir3_priznak_arp_packet==0x0806)
		 	  	   {
					   memcpy(&dir3_target_arp_ip_da_addr,out_buf3+19,4);
					   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
					   ngraf_packet_for_matrica_kommutacii(out_buf3 ,dannie1206,(u8)dir3_target_arp_ip_da_addr,0x11);  
		 	  		 
		 	  	     //p2020_get_recieve_virttsec_packet_buf(out_buf2,dannie1204,2);//send to eternet tsec ARP broadcast   
		 	  	   } 
		 	  	   else
		 	  	   {   
		 	  	     ngraf_packet_for_matrica_kommutacii(out_buf3 ,dannie1206,(UINT8)dir3_ip_da_addr,0x11);  
		 	  	   }	
	  	 	  	 
	  	 	  	 
	  	 	  	 
	  	 	  	 /*
	  	 	  	 if(dir3_mac_priznak_kys==0x22)
	  	 	  	 {
	  	 	  		 //packet kommutacii po mac address
	  	 	  		 ngraf_packet_for_matrica_kommutacii(out_buf3 ,dannie1206+PATCH_READ_PACKET_SIZE_ADD_ONE,dir3_mac_da_addr,0x11); 
	  	 	  	 }
	  	 	  	 */
	  	 	  	 
	  	 	  	 #ifdef TDM3_DIR_TEST_ETHERNET_SEND
	  	 	       //p2020_get_recieve_virttsec_packet_buf(out_buf2,dannie1204+PATCH_READ_PACKET_SIZE_ADD_ONE);//send to eternet
	  	 	  	 #endif  
	  	 	  
	  
tdm3_read_iteration++; 
		
		
}
		

/**************************************************************************************************
Syntax:      	   void TDM4_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM4_dierction_read  ()
{
	  UINT16 dannie1208=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm4_read_iteration=0;
	  UINT16 packet_size=0;
	  UINT16 packet_size_hex=0;
	  //out_size_byte=256;//512 bait;

#if 0	  
	  dannie1208 = plis_read16 (DIR4_PLIS_PACKSIZE_ADDR1208);
	  packet_size_hex=dannie1208/2; //convert byte to element of massive in hex 
	  printk("+Tdm_Dir4_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm4_read_iteration,dannie1208,packet_size_hex);   
	  
	  do
	   {

		 out_buf[i]=plis_read16 (DIR4_PLIS_READ_ADDR408);  
		 i++;
	   }while(i<packet_size_hex+1+PATCHlbc_ONE_ITERATION_READ);

	    
	  printk("+Tdm_Dir4_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir4_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	  
	  
	#ifdef TDM4_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif  

#endif	  
	  
	  tdm4_read_iteration++; 
				
}
			
			
			
			
/**************************************************************************************************
Syntax:      	   void TDM5_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM5_dierction_read  ()
{
	  UINT16 dannie1210=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm5_read_iteration=0;
	  UINT16 packet_size=0;
	  UINT16 packet_size_hex=0;
	  //out_size_byte=256;//512 bait;
	
#if 0	  
	  dannie1210 = plis_read16 (DIR5_PLIS_PACKSIZE_ADDR1210);
	  packet_size_hex=dannie1210/2; //convert byte to element of massive in hex 
	  printk("+Tdm_Dir5_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm5_read_iteration,dannie1210,packet_size_hex);   
	  

	  do
	   {
		 //out_buf[i]= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));
		 out_buf[i]=plis_read16 (DIR5_PLIS_READ_ADDR410);  
		 i++;
	   }while(i<packet_size_hex+1+PATCHlbc_ONE_ITERATION_READ);
	//#endif
	    
	  printk("+Tdm_Dir5_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir5_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	 
	#ifdef TDM5_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif  

#endif
	  
	  tdm5_read_iteration++; 
					
}
						
/**************************************************************************************************
Syntax:      	   void TDM6_dierction_read  (UINT16 *out_buf,UINT16  out_size_byte)
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM6_dierction_read  ()
{
	  UINT16 dannie1212=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm6_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  //out_size_byte=256;//512 bait;

#if 0	  
	  dannie1212 = plis_read16 (DIR6_PLIS_PACKSIZE_ADDR1212);
	  packet_size_hex=dannie1212/2; //convert byte to element of massive in hex 
	  printk("+Tdm_Dir6_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm6_read_iteration,dannie1212,packet_size_hex);   
	  

	  do
	   {
		 out_buf[i]=plis_read16 (DIR6_PLIS_READ_ADDR412);  
		 i++;
	   }while(i<packet_size_hex+1+PATCHlbc_ONE_ITERATION_READ);
	//#endif
	    
	  printk("+Tdm_Dir6_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir6_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	  
	  
	#ifdef TDM6_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif    
	  
#endif	  
	  tdm6_read_iteration++; 				
}
					
					
					
/***************************************************************************************************
Syntax:      	   void TDM7_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM7_dierction_read  ()
{
	  UINT16 dannie1214=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm7_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  
	  //out_size_byte=256;//512 bait;
#if 0	   
	  dannie1214 = plis_read16 (DIR7_PLIS_PACKSIZE_ADDR1214);
	  packet_size_hex=dannie1214/2; //convert byte to element of massive in hex 
	  printk("+Tdm_Dir7_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm7_read_iteration,dannie1214,packet_size_hex);   
	  

	  do
	   {
		 //out_buf[i]= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));
		 out_buf[i]=plis_read16 (DIR7_PLIS_READ_ADDR414);  
		 i++;
	   }while(i<packet_size_hex+1+PATCHlbc_ONE_ITERATION_READ);
	//#endif
	  printk("+Tdm_Dir7_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir7_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);

	#ifdef TDM7_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif    

#endif	  
	  
tdm7_read_iteration++; 			
}						
						
						
/**************************************************************************************************
Syntax:      	   void TDM8_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM8_dierction_read  ()
{
	
	  UINT16 dannie1216=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm8_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  
	  //out_size_byte=256;//512 bait;
#if 0	   
	  dannie1216 = plis_read16 (DIR8_PLIS_PACKSIZE_ADDR1216);
	  packet_size_hex=dannie1216/2; //convert byte to element of massive in hex 
	  printk("+Tdm_Dir8_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm8_read_iteration,dannie1216,packet_size_hex);   
	  

	  do
	   {
		 //out_buf[i]= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));
		 out_buf[i]=plis_read16 (DIR8_PLIS_READ_ADDR416);  
		 i++;
	   }while(i<packet_size_hex+1+PATCHlbc_ONE_ITERATION_READ);
	//#endif
	    
	  printk("+Tdm_Dir8_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir8_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	  
	  
	  
	  
	  //ngraf_packet_for_matrica_kommutacii(out_buf ,packet_size_hex,u32 priznak_kommutacii);
	  
	  
	#ifdef TDM8_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif   

#endif 	  
	  
	  tdm8_read_iteration++; 
	
}
	
	
	
	
/**************************************************************************************************
Syntax:      	   void TDM9_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM9_dierction_read  ()
{

	  UINT16 dannie1218=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm9_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  
	  //out_size_byte=256;//512 bait;

#if 0	  
	  dannie1218 = plis_read16 (DIR9_PLIS_PACKSIZE_ADDR1218);
	  packet_size_hex=dannie1218/2; //convert byte to element of massive in hex 
	  printk("+Tdm_Dir9_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm9_read_iteration,dannie1218,packet_size_hex);   
	  

	  do
	   {
		 //out_buf[i]= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));
		 out_buf[i]=plis_read16 (DIR9_PLIS_READ_ADDR418);  
		 i++;
	   }while(i<packet_size_hex+1+PATCHlbc_ONE_ITERATION_READ);
	//#endif
	    
	  printk("+Tdm_Dir9_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir9_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	  
	#ifdef TDM9_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif  
	
#endif
	  tdm9_read_iteration++; 
	  
}














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
#include "mpcdrv_gianfar.h"
/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
  #define TDM_DIRECTION0_WRITE_DEBUG 1
//#define TDM_DIRECTION0_READ_DEBUG  1

/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/

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
extern void p2020_get_recieve_virttsec_packet_buf(u16 *buf,u16 len);
/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
/*
static u8 eth_preambula_mas[8]=
{0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x5d};
*/

static u16 eth_preambula_mas[4]=
{0x5555,0x5555,0x5555,0x555d};



/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/
struct map_info    *map; 



void callback_functions(UINT16 *data)
{
UINT16 test_size=500;

*data=	test_size;	
}




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
		printk(KERN_WARNING "Failed to allocate memory for MAP_INFO_Structure\n");	   
	return NULL;}
	
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
	printk(KERN_INFO "Failed to ioremap() flash region\n");
	return STATUS_ERR;
	}
	
   //return 1;
}

/**************************************************************************************************
Syntax:      	    UINT16 TDM0_direction_READ_READY(void)

Remarks:			 

Return Value:	    Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM0_direction_READ_READY(void)
{
 u16 dannie1000=0;
 u16 count_visim=0;
 u16 dannie800=0;
 
 
 ////////////////////Future timing read data form plis///////////////
 dannie800=plis_read16 (PLIS_ADDRESS800);
 //printk("READ_800=0x%x\n\r",dannie800);
 
 
 //if(dannie800==1)
 //{
 
 	 while(!dannie1000)
 	 {	 
	 
 		 if(count_visim==20)return 0;
	 
 		 dannie1000=plis_read16 (PLIS_ADDRESS1000);
 		 if(dannie1000==0xabc0)
 		 {
		 dannie1000=0; 
		 printk("VISIM_READ_READY=0x%x\n\r",dannie1000);
 		 }
	 count_visim++; 
    }
 
 	//printk("OK_READ_READY=0x%x\n\r",dannie1000);
 	return 1;
 
 //}
 //else
 //{
	 
	//return 0 ;
 //}
 

 
 //#ifdef TDM_DIRECTION0_READ_DEBUG 
 //printk("dannie1000=0x%x\n\r",dannie1000);
 //#endif 
 //if(dannie1000==0xabc1) return 1;
 //else return 0;
 
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM0_direction_WRITE_READY(void)

Remarks:			 

Return Value:	    Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM0_direction_WRITE_READY(void)
{
 u16 dannie30=1;
 
 
 //Next step Set delay to write succes operations !!!!!!!!!!!
 ////////////////////////////////////////////////////////////
 
 while(dannie30)
 {
  dannie30=plis_read16 (PLIS_ADDRESS30); 	 

 
 }

 
 
 
 //printk("WRITE_READY_OK=%d\n\r",dannie30);
 return 1; //WRITE READY SUCCESS
 
 // #ifdef TDM_DIRECTION0_WRITE_DEBUG	
 /*printk("WRITE_READY=%d\n\r",dannie30);
 //#endif
 if (dannie30 ==0) return 1; //WRITE READY SUCCESS
 if (dannie30 ==1) return 0; //NOT READY to write to PLIS
 */
 //else return 0;
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
UINT16 plis_read16 (const u16 addr)
{
u16 byte_offset=0x0000; 
u16 out_data=0x0000;
 
byte_offset = addr*2;


 out_data= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+byte_offset);
//printk("READ_OFFSET=0x%08x\n\r",map->virt + PLIS_LINUX_START_DATA_OFFSET+byte_offset);
//printk("READ_OFFSET=0x%08x\n\r",byte_offset);
  return out_data;
}
/**************************************************************************************************
Syntax:      	    void   plis_write16(const u16 addr,u16 value)

Remarks:			This Write to PLIS  address value. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void   plis_write16(const u16 addr,const  u16 value)
{
  u16 byte_offset=0x0000; 	
  byte_offset = addr*2;
	
 __raw_writew(value, map->virt + PLIS_LINUX_START_DATA_OFFSET+byte_offset);
  // printk("WRITE_OFFSET=0x%08x\n\r",map->virt + PLIS_LINUX_START_DATA_OFFSET+byte_offset);
  //printk("WRITE_OFFSET=0x%08x\n\r",byte_offset);
}





/**************************************************************************************************
Syntax:      	   void Tdm_Direction0_write (const u16 *in_buf ,const u16 in_size,const u8 in_num_of_tdm_ch

Remarks:			This Write to PLIS  address value. 

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM0_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 dannie30 =1;
	u16 i=0;
    static u16 iteration=0;
    u16 hex_element_size=0;
    
    //Add preambula to input packet
    //eth_preambula_mas; 
 
//#if 0	
//printk("++++++++++++++++++++Tdm_Direction0_write= %d|in_size=%d+++++++++++++++++\n\r",iteration,in_size);
	
#ifdef TDM_DIRECTION0_WRITE_DEBUG
	   //printk("in_size=%d \n\r",in_size);  	    
#endif	
	   //printk("0x%04x|0x%04x|0x%04x|0x%04x\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3]); 
/*	   
#ifdef TDM_DIRECTION0_WRITE_DEBUG
	   for(i=0;i<in_size;i++)
	   {	   
	   printk("in_data = 0x%04x \n\r",in_buf[i]);  	    
	   }
#endif	
*/   
	   
/*	   
#ifdef TDM_DIRECTION0_WRITE_DEBUG
      printk("+++++++++++++++++++++++++++++++++++++++++++\n\r");  
#endif	   
*/	
	
#ifdef TDM_DIRECTION0_WRITE_DEBUG	
	//printk("dannie30=%x\n\r",dannie30);
#endif

	   //Read dannie   
	   //memcpy(out_buf,in_buf,in_size+1);   
	  // #ifdef  P2020_RDBKIT
      // printk("???????????write operation????????????????????????????????\n\r");
	  // #endif
    
    
    //Warning Add preambula to packet
    hex_element_size=in_size/2;
    
    
    
    //#ifdef P2020_MPC	   
	//Write dannie 
    printk("+Tdm_Direction0_write\iteration= %d+\n\r",iteration);  
    printk("+Tdm_Direction0_write\in_size_mas_byte= %d+\n\r",in_size);
    printk("+Tdm_Direction0_write\in_size_mas_element= %d+\n\r",in_size/2);
	printk("+Tdm_Dir0_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3]);
	printk("+Tdm_Dir0_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    
    
    
    
    
	for(i=0;i<hex_element_size+1;i++)
	{
		plis_write16(DIR0_ADDRESS_WRITE_DATA,in_buf[i]);
	}
	//Write dannie
	//WRITE to PLIS SUCCESS
	plis_write16(DIR0_ADDRESS_WRITE_SUCCESS ,PLIS_WRITE_SUCCESS );
//#endif  	

	
	iteration++;	
}

/**************************************************************************************************
Syntax:      	   void Tdm_Direction0_read  (u16 *out_buf,u16 *out_size,u8 *out_num_of_tdm_ch)

Remarks:		   This Read from PLIS0 tdm direction  

   
Return Value:	Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM0_dierction_read  (u16 *out_buf,UINT16  out_size_byte)
{
  //u16 dannie1000=0;
  u16 dannie800=0;
  u16 dannie1200=0; 
  u16 i=0;		  
  u16 iteration=0;
  static u16 riteration=0;
  u16 packet_size;
  UINT16 out_data=54;
  //out_size_byte=256;//512 bait;
 
  printk("+Tdm_Direction0_read\iteration= %d+\n\r",riteration);
  
   

#ifdef TDM_DIRECTION0_READ_DEBUG    
   dannie800=plis_read16 (PLIS_ADDRESS800);
   printk("dannie800=0x%x\n\r",dannie800);
#endif  
  

   

   
//Read dannie packet size on 1200 registers
//#ifdef P2020_MPC
   
  dannie1200 = plis_read16 (PLIS_ADDRESS1200);
  packet_size=(dannie1200+1)/2; //convert byte to element of massive in hex 
  
  printk("+Tdm_Direction0_read\in_size_mas_in2byte= %d+\n\r",packet_size);
   
 
       
   //
   //out_size_byte=512;//packet_size;
     
//#endif  
   
     //printk("packet_size->>>>>>>>>>%d\n\r",packet_size);
   
   
   //out_num_of_tdm_ch=1;
   
   //printk("out_size=%d\n\r",out_size_byte);
   #ifdef TDM_DIRECTION0_READ_DEBUG 
   //printk("dannie1200=0x%x\n\r",dannie1200);
   //printk("dannie1200=0x%x\n\r",out_size);
   #endif  
  

 //#ifdef  P2020_RDBKIT
  /* 
   do
     {
  	   out_buf[i]=test_full_packet_mas[i];	   
  	   i++;
     }while( i< out_size_byte);
  */
 //#endif
	
	
 //#ifdef P2020_MPC
  do
   {
	   out_buf[i]=plis_read16 (DIR0_ADDRESS_READ_DATA);  
	 //  #ifdef TDM_DIRECTION0_READ_DEBUG	   
	 //  printk("plis_read_data =0x%x\n\r",plis_read_data);
     //  #endif 	   
	   i++;
   }while( i< packet_size+1);
//#endif
   
  printk("+Tdm_Dir0_rfirst|0x%04x|0x%04x|0x%04x|0x%04x+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3]);
  printk("+Tdm_Dir0_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",out_buf[packet_size-4],out_buf[packet_size-3],out_buf[packet_size-2],out_buf[packet_size-1]);
  
  
  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size);

   riteration++; 
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
loff_t  ofs=0x0000000000000000;

printk("Start_write_to_PLIS\n\r");      
			 //__raw_writeb(x, map->virt + ofs);
			  __raw_writew(x, map->virt + ofs);
			//__raw_writel(x, map->virt + ofs);
	//map_write(map, CMD(0xB0), chip->in_progress_block_addr);
	//map_write(map, datum, adr);
	return 1;

}








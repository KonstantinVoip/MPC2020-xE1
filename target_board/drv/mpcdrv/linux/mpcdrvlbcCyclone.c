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
// static inline UINT16  plis_read16 (const UINT16 addr);
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
extern void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii,u32 priznak_nms3_ot_arp_sa_addr,u8 tdm_input_read_direction);


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
	map->phys=PLIS_PHYSICAL_RESOURCE_START;//0xef000000;//res.start;
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
***************************************************************************************************/
/*Zoya static inline */UINT16 plis_read16 (const UINT16 addr)
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
static inline void   plis_write16(const UINT16 addr,const  UINT16 value)
{
 //UINT16 byte_offset=0x0000; 	
 //byte_offset = addr*2;
 //__raw_writew(value, map->virt + PLIS_LINUX_START_DATA_OFFSET+byte_offset);
   __raw_writew(value, map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));  
}

//////////////////////////////////////////Тестовые функции работаем по таймерам/////////////////////////////////////////
/**************************************************************************************************
Syntax:      	    UINT16 TDM1_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM1_direction_READ_READY(void)
{
UINT16 dannie1000=0;
UINT16 dannie800=0;
   
	dannie1000=plis_read16 (DIR1_PLIS_READOK_ADDR1000);
	//printk("Status1000 =0x%x\n\r",dannie1000);
	if((dannie1000==0xabc1) || (dannie1000==0x1))
    {
	
		dannie800=plis_read16 (DIR1_PLIS_READ_BUG_ADDR800);
		if((dannie800==0x1)||(dannie800==0xabc1))
		{
			//printk("Status1000 =0x%x->>status800 =0x%x\n\r",dannie1000,dannie800);
			Event_TDM1_direction_READ_READY();
			
		}
		
  	} 

}
/**************************************************************************************************
Syntax:      	    UINT16 TDM2_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM2_direction_READ_READY(void)
{
UINT16 dannie1002=0;
UINT16 dannie802=0;

	dannie1002=plis_read16 (DIR2_PLIS_READOK_ADDR1002);
	if((dannie1002==0xabc1) || (dannie1002==0x1))
	{
	    dannie802=plis_read16 (DIR2_PLIS_READ_BUG_ADDR802);
		if((dannie802==0x1)||(dannie802==0xabc1))
		{
			Event_TDM2_direction_READ_READY();
		}
		
	} 
	//printk("dannie1002_dir1=0x%x->>>>>",dannie1002);
	//printk("dannie802_dir1=0x%x->>>>>>",dannie802);
	//printk("Status_dir1 =%d\n\r",status);
}

/**************************************************************************************************
Syntax:      	    UINT16 TDM3_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM3_direction_READ_READY(void)
{
 UINT16 dannie1004=0;
 UINT16 dannie804=0;

  
	dannie1004=plis_read16 (DIR3_PLIS_READOK_ADDR1004);
	if((dannie1004==0xabc1) || (dannie1004==0x1))
	{
		dannie804=plis_read16 (DIR3_PLIS_READ_BUG_ADDR804);
		if((dannie804==0x1)||(dannie804==0xabc1))
		{
			Event_TDM3_direction_READ_READY();
		}	
		
	}	
	
	//printk("dannie1004_dir1=0x%x->>>>>",dannie1004);
	//printk("dannie804_dir1=0x%x->>>>>>",dannie804);
	//printk("Status_dir2 =%d\n\r",status);
}
/**************************************************************************************************
Syntax:      	    UINT16 TDM4_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM4_direction_READ_READY(void)
{
	UINT16 dannie1006=0;
	UINT16 dannie806=0;

	dannie1006=plis_read16 (DIR4_PLIS_READOK_ADDR1006);
	if((dannie1006==0xabc1) || (dannie1006==0x1))
	{
		
		dannie806=plis_read16 (DIR4_PLIS_READ_BUG_ADDR806);
		if((dannie806==0x1)||(dannie806==0xabc1))
		{
			Event_TDM4_direction_READ_READY();
		}
			
	} 
	//printk("dannie1006_dir1=0x%x->>>>>",dannie1006);
	//printk("dannie806_dir1=0x%x->>>>>>",dannie806);
	//printk("Status_dir3 =%d\n\r",status);

}
/**************************************************************************************************
Syntax:      	    UINT16 TDM5_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM5_direction_READ_READY(void)
{
	UINT16 dannie1008=0;
	UINT16 dannie808=0;

	dannie1008=plis_read16 (DIR5_PLIS_READOK_ADDR1008);
	if((dannie1008==0xabc1) || (dannie1008==0x1))
	{

		dannie808=plis_read16 (DIR5_PLIS_READ_BUG_ADDR808);
		if((dannie808==0x1)||(dannie808==0xabc1))
		{
			Event_TDM5_direction_READ_READY();
		}
				
	} 
	
}

/**************************************************************************************************
Syntax:      	    UINT16 TDM6_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM6_direction_READ_READY(void)
{
	UINT16 dannie1010=0;
	UINT16 dannie810=0;

	dannie1010=plis_read16 (DIR6_PLIS_READOK_ADDR1010);
	if((dannie1010==0xabc1) || (dannie1010==0x1))
	{
		dannie810=plis_read16 (DIR6_PLIS_READ_BUG_ADDR810);
		if((dannie810==0x1)||(dannie810==0xabc1))
		{
			Event_TDM6_direction_READ_READY();
		}
	
	} 

}
/**************************************************************************************************
Syntax:      	    UINT16 TDM7_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM7_direction_READ_READY(void)
{
	UINT16 dannie1012=0;
	UINT16 dannie812=0;

	dannie1012=plis_read16 (DIR7_PLIS_READOK_ADDR1012);
	if((dannie1012==0xabc1) || (dannie1012==0x1))
	{
		
		dannie812=plis_read16 (DIR7_PLIS_READ_BUG_ADDR812);
		if((dannie812==0x1)||(dannie812==0xabc1))
		{ 
			Event_TDM7_direction_READ_READY();
		}
		
	} 

}

/**************************************************************************************************
Syntax:      	    UINT16 TDM8_direction_READ_READY_event_test(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
***************************************************************************************************/
void TDM8_direction_READ_READY(void)
{
	UINT16 dannie1014=0;
	UINT16 dannie814=0;

	dannie1014=plis_read16 (DIR8_PLIS_READOK_ADDR1014);
	if((dannie1014==0xabc1) || (dannie1014==0x1))
	{
		dannie814=plis_read16 (DIR8_PLIS_READ_BUG_ADDR814);
		if((dannie814==0x1)||(dannie814==0xabc1))
		{
			Event_TDM8_direction_READ_READY();
		}
			
	} 
	
}

#if 0
	
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
Syntax:      	    UINT16 TDM10_direction_READ_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
UINT16 TDM10_direction_READ_READY(void)
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
#endif

//*************************************WRITE_READY******************************//
//*****************************************************************************//
//*****************************************************************************//
//Тестовые функции работы по событиям.
/**************************************************************************************************
Syntax:      	    void TDM1_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM1_direction_WRITE_READY(void)
{
  UINT16  dannie30=1;
 //Next step Set delay to write succes operations !!!!!!!!!!!
 ////////////////////////////////////////////////////////////
 //mdelay(10);
 dannie30=plis_read16 (DIR1_PLIS_WRITEOK_ADDR30); 
 //printk("register 30 dannie= %d ready\n\r",dannie30);
 if(dannie30==0)
 {
	 Event_TDM1_direction_WRITE_READY();
 }

}
/**************************************************************************************************
Syntax:      	    void TDM2_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM2_direction_WRITE_READY(void)
{
    UINT16  dannie32=1; 
	//Next step Set delay to write succes operations !!!!!!!!!!!
	////////////////////////////////////////////////////////////
	//mdelay(20);
	dannie32=plis_read16 (DIR2_PLIS_WRITEOK_ADDR32); 
	//printk("register 30 dannie= %d ready\n\r",dannie30);
	if(dannie32==0)
	{
	Event_TDM2_direction_WRITE_READY();
	}

}

/**************************************************************************************************
Syntax:      	    void TDM3_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM3_direction_WRITE_READY(void)
{
	UINT16  dannie34=1; 
	//Next step Set delay to write succes operations !!!!!!!!!!!
	////////////////////////////////////////////////////////////
	//mdelay(20);
	dannie34=plis_read16 (DIR3_PLIS_WRITEOK_ADDR34); 
	//printk("register 34 dannie= %d ready\n\r",dannie3);
	if(dannie34==0)
	{
		Event_TDM3_direction_WRITE_READY();
	}

}

/**************************************************************************************************
Syntax:      	    void TDM4_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM4_direction_WRITE_READY(void)
{
	UINT16  dannie36=1; 
	//Next step Set delay to write succes operations !!!!!!!!!!!
	////////////////////////////////////////////////////////////
	dannie36=plis_read16 (DIR4_PLIS_WRITEOK_ADDR36); 
	//printk("register 30 dannie= %d ready\n\r",dannie30);
	if(dannie36==0)
	{
	Event_TDM4_direction_WRITE_READY();
	}
	
}
/**************************************************************************************************
Syntax:      	    void TDM5_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM5_direction_WRITE_READY(void)
{
	 UINT16  dannie38=1; 
	 dannie38=plis_read16 (DIR5_PLIS_WRITEOK_ADDR38); 
	 //printk("register 30 dannie= %d ready\n\r",dannie30);
	 if(dannie38==0)
	 {
	 Event_TDM5_direction_WRITE_READY();
	 }

}	
/**************************************************************************************************
Syntax:      	    void TDM6_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM6_direction_WRITE_READY(void)
{
	UINT16  dannie40=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
	 dannie40=plis_read16 (DIR6_PLIS_WRITEOK_ADDR40); 
	 //printk("register 30 dannie= %d ready\n\r",dannie30);
	 if(dannie40==0)
	 {
		 Event_TDM6_direction_WRITE_READY();
	 }

}


/**************************************************************************************************
Syntax:      	    void TDM7_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM7_direction_WRITE_READY(void)
{
	 UINT16  dannie42=1; 
	 dannie42=plis_read16 (DIR7_PLIS_WRITEOK_ADDR42); 
	 //printk("register 30 dannie= %d ready\n\r",dannie30);
	 if(dannie42==0)
	 {
		 Event_TDM7_direction_WRITE_READY();
	 }

									
}	
/**************************************************************************************************
Syntax:      	    void TDM8_direction_WRITE_READY(void)			 
Return Value:	    NONE.
Remarks     :
***************************************************************************************************/
void TDM8_direction_WRITE_READY(void)
{
	UINT16  dannie44=1; 
	     //Next step Set delay to write succes operations !!!!!!!!!!!
	 dannie44=plis_read16 (DIR8_PLIS_WRITEOK_ADDR44); 
	 //printk("register 30 dannie= %d ready\n\r",dannie30);
	 if(dannie44==0)
	 {
	 Event_TDM8_direction_WRITE_READY();
	 }															
}



						
#if 0						
/**************************************************************************************************
Syntax:      	    UINT16 TDM8_direction_WRITE_READY(void)			 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
UINT16 TDM9_direction_WRITE_READY(void)
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
UINT16 TDM10_direction_WRITE_READY(void)
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
#endif



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
void TDM1_direction_write (const u16 *in_buf ,const u16 in_size)
{
	
	u16 i=0;
    static UINT16 tdm1_write_iteration=0;
    u16 hex_element_size=0;
    u8  dir1_dop_nechet_packet=0;
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
    	dir1_dop_nechet_packet=1;
    }
    
    
    hex_element_size=(in_size/2)+dir1_dop_nechet_packet;
    //Set size on PLIS in byte
   
    
    //printk("+Tdm_Dir1_write->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm1_write_iteration,in_size,hex_element_size);
    /*
	printk("+Tdm_Dir1_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir1_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    */
    
	plis_write16(DIR1_PLIS_PACKSIZE_ADDR1600,in_size);
	for(i=0;i<hex_element_size;i++)
	{
	   
		plis_write16( DIR1_PLIS_WRITE_ADDR200 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR1_PLIS_WRITEOK_ADDR30 ,PLIS_WRITE_SUCCESS);
	
#ifdef PLIS_DEBUG_1400
	dannie1400 =plis_read16 (PLIS_ADDR1400);
	printk("Read_Iter_dannie1400_After30_Success->Rdata=0x%x|\n\r",dannie1400);
#endif	
	
	tdm1_write_iteration++;	
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
    
	//printk("+Tdm_Dir2_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm2_write_iteration,in_size,hex_element_size);
    /*
	printk("+Tdm_Dir2_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir2_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    */
	
	plis_write16(DIR2_PLIS_PACKSIZE_ADDR1602,in_size);
	
	for(i=0;i<hex_element_size;i++)
	{
		plis_write16(DIR2_PLIS_WRITE_ADDR202 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR2_PLIS_WRITEOK_ADDR32 ,PLIS_WRITE_SUCCESS);
	tdm2_write_iteration++;

}


/**************************************************************************************************
Syntax:      	void TDM3_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
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
   /*
	printk("+Tdm_Dir3_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir3_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
   */
	
	plis_write16(DIR3_PLIS_PACKSIZE_ADDR1604,in_size);
	
	for(i=0;i<hex_element_size;i++)
	{
		plis_write16(DIR3_PLIS_WRITE_ADDR204  ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR3_PLIS_WRITEOK_ADDR34 ,PLIS_WRITE_SUCCESS);
		
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
    u8  dir4_dop_nechet_packet=0;
    
    
  
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
    	dir4_dop_nechet_packet=1;
    }
    
	hex_element_size=(in_size/2)+dir4_dop_nechet_packet; 
    //printk("+Tdm_Dir4_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm4_write_iteration,in_size,hex_element_size);
    /*
	printk("+Tdm_Dir4_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir4_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    */
	
	
	plis_write16(DIR4_PLIS_PACKSIZE_ADDR1606,in_size);
	
	for(i=0;i<hex_element_size;i++)
	{
		plis_write16(DIR4_PLIS_WRITE_ADDR206  ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR4_PLIS_WRITEOK_ADDR36 ,PLIS_WRITE_SUCCESS);
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
    u8  dir5_dop_nechet_packet=0;
    
    
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
		dir5_dop_nechet_packet=1;
    }
    
    hex_element_size=(in_size/2)+dir5_dop_nechet_packet;
    printk("+Tdm_Dir5_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm5_write_iteration,in_size,hex_element_size);
    /*
	printk("+Tdm_Dir5_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir5_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    */
    
    plis_write16(DIR5_PLIS_PACKSIZE_ADDR1608,in_size);
    
	for(i=0;i<hex_element_size;i++)
	{
		plis_write16(DIR5_PLIS_WRITE_ADDR208 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR5_PLIS_WRITEOK_ADDR38 ,PLIS_WRITE_SUCCESS);
	tdm5_write_iteration++;	
			
}			
					
/**************************************************************************************************
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
    u8  dir6_dop_nechet_packet=0;
    
    
    
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
		dir6_dop_nechet_packet=1;
    }
    
    
    hex_element_size=(in_size/2)+dir6_dop_nechet_packet;
    //printk("+Tdm_Dir6_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm6_write_iteration,in_size,hex_element_size);
    /*
  	printk("+Tdm_Dir6_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
  	printk("+Tdm_Dir6_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    */
    
    
    plis_write16(DIR6_PLIS_PACKSIZE_ADDR1610,in_size);
  
	for(i=0;i<hex_element_size;i++)
	{
		plis_write16(DIR6_PLIS_WRITE_ADDR210 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR6_PLIS_WRITEOK_ADDR40 ,PLIS_WRITE_SUCCESS);
	tdm6_write_iteration++;			
				
}							
/*************************************************************************************************
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
    u8  dir7_dop_nechet_packet=0;
    
    
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
		dir7_dop_nechet_packet=1;
    }
    
    
  
    hex_element_size=(in_size/2)+dir7_dop_nechet_packet;
    //printk("+Tdm_Dir7_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm7_write_iteration,in_size,hex_element_size);
    /*
  	printk("+Tdm_Dir7_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
  	printk("+Tdm_Dir7_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    */
    
    
    plis_write16(DIR7_PLIS_PACKSIZE_ADDR1612,in_size);
    
	for(i=0;i<hex_element_size;i++)
	{
		plis_write16(DIR7_PLIS_WRITE_ADDR212 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR7_PLIS_WRITEOK_ADDR42 ,PLIS_WRITE_SUCCESS);
	tdm7_write_iteration++;				
}

/**************************************************************************************************
Syntax:      	void TDM8_direction_write (const u16 *in_buf ,const u16 in_size)
Remarks:		This Write to PLIS  address value. 
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
***************************************************************************************************/
void TDM8_direction_write (const u16 *in_buf ,const u16 in_size)
{
	u16 i=0;
    static UINT16 tdm8_write_iteration=0;
    u16 hex_element_size=0;
    u8  dir8_dop_nechet_packet=0;
   
    
	if((in_size)%2==1)
    { 
		//printk("+odd_packet=%d+\n\r",in_size); 
	    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
		dir8_dop_nechet_packet=1;
    }
    
    hex_element_size=(in_size/2)+dir8_dop_nechet_packet;
    //printk("+Tdm_Dir8_write->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm8_write_iteration,in_size,hex_element_size);
    /*
	printk("+Tdm_Dir8_wr_rfirst|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
	printk("+Tdm_Dir8_wr_rlast |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[hex_element_size-6],in_buf[hex_element_size-5],in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);
    */
    
    plis_write16(DIR8_PLIS_PACKSIZE_ADDR1614,in_size);
    
    
	for(i=0;i<hex_element_size;i++)
	{
		plis_write16(DIR8_PLIS_WRITE_ADDR214 ,in_buf[i]);
	}
	//WRITE to PLIS SUCCESS
	plis_write16(DIR8_PLIS_WRITEOK_ADDR44 ,PLIS_WRITE_SUCCESS);
	tdm8_write_iteration++;			
				
}

#if 0
/**************************************************************************************************
Syntax:      	void TDM8_direction_write (const u16 *in_buf ,const u16 in_size)
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
void TDM10_direction_write (const u16 *in_buf ,const u16 in_size)
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
#endif


//*************************************READ_DATA_FROM_PLIS******************************//
//*****************************************************************************//
//*****************************************************************************//


/**************************************************************************************************
Syntax:      	   void TDM1_dierction_read  (UINT16 *out_buf,UINT16  out_size_byte)
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
***************************************************************************************************/
void TDM1_dierction_read ()
//void TDM0_dierction_read  (UINT16 *out_buf,UINT16  out_size_byte)
{
  UINT16 dannie1200=0; 
  UINT16 i=0;		  
  static UINT16 tdm0_read_iteration=0;
  UINT16 packet_size_hex=0;
 // UINT16 ostatok_of_size_packet=0;
  UINT16  out_buf[760];//1518 bait;
  UINT8   dir0_dobavka_esli_packet_nechetnii=0;  

  //
  __be32  dir0_target_arp_ip_da_addr=0;
  __be32  dir0_target_arp_nms_sa_addr  =0;      
  //IP DA address read on direction 0
  __be32  dir0_ip_da_addr    = 0;
  //MAC DA address read on direction 0
  UINT8   dir0_mac_da_addr   = 0;
  //Priznak Sevi Packet 22-ff _predposlednii bait.
  UINT8   dir0_mac_priznak_kys = 0; 
  UINT16  dir0_priznak_arp_packet=0;
  
  
  memset(&out_buf, 0x0000, sizeof(out_buf));  
  dannie1200 = plis_read16 (DIR1_PLIS_PACKSIZE_ADDR1200 );
  
  /*Проверка что читаем пришёл нормальный пакет с направления 1*/
  if((dannie1200==65535)||(dannie1200==0)||(dannie1200>1518))
  {
	  printk("?bad_exeption_read_dir1_pack_size=%d?\n\r",dannie1200);
	  return;
  }
  
  
  //Если пакет нечётный читаем на еденицу больше из шины local bus;
  if((dannie1200)%2==1)
  {
	  dir0_dobavka_esli_packet_nechetnii=1; 
  }
  
  
  packet_size_hex=(dannie1200/2)+dir0_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex 
  //printk("+Tdm_Dir1_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm0_read_iteration,dannie1200,packet_size_hex);
  
  //printk("+Tdm_Dir1_read->>ITERATION=%d|1200in_byte=%d|1200in_hex=%d|size=%d|+\n\r",tdm0_read_iteration,dannie1200,packet_size_hex,dannie1200+PATCH_READ_PACKET_SIZE_ADD_ONE); 
  
  
	  //16 bit  or 2 bait Local bus iteration
	  do
	  {
	  //mdelay(5);   
      out_buf[i]= plis_read16 (DIR1_PLIS_READ_ADDR400);
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
	  //dir0_ip_da_addr    = out_buf[16];
	 memcpy(&dir0_ip_da_addr,out_buf+15,4);
	    
	 //MAC DA address read on direction 0
	 //dir0_mac_da_addr   = out_buf[2];
	 //определяю признак KY-S по mac адресам
	 //dir0_mac_priznak_kys = out_buf[2]>>8;
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
		   
		   memcpy(&dir0_target_arp_nms_sa_addr,out_buf+14,4);
		   memcpy(&dir0_target_arp_ip_da_addr,out_buf+19,4);
		   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
		   ngraf_packet_for_matrica_kommutacii(out_buf ,dannie1200,dir0_target_arp_ip_da_addr,dir0_target_arp_nms_sa_addr,1);   
	 	   
	 	  //send to tsec2
		  //p2020_get_recieve_virttsec_packet_buf(out_buf,dannie1200,2);
		  //p2020_get_recieve_virttsec_packet_buf(out_buf,dannie1200);//send to eternet tsec ARP broadcast
	   }
	   else
	   {
	      // printk("paket po ip hesder\n\r");
		     ngraf_packet_for_matrica_kommutacii(out_buf ,dannie1200,dir0_ip_da_addr,0x0000,0); 
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
Syntax:      	   void TDM2_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
***************************************************************************************************/
void TDM2_dierction_read ()
{

	  UINT16 dannie1202=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm2_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  //UINT16 ostatok_of_size_packet=0;
	  UINT16  out_buf1[760];//1518 bait;
	  
	  
	  __be32  dir1_target_arp_ip_da_addr=0;
	  __be32  dir1_target_arp_nms_sa_addr=0; 
	    UINT8 dir1_dobavka_esli_packet_nechetnii=0; 
	  //IP DA address read on direction 1
	  __be32  dir1_ip_da_addr    = 0;
	  //MAC DA address read on direction 1
	  UINT8   dir1_mac_da_addr   = 0;
	  //Priznak Sevi Packet 22-ff _predposlednii bait.
	  UINT8   dir1_mac_priznak_kys = 0; 
	  
	  UINT16  dir1_priznak_arp_packet=0;
	  
	  
	  memset(&out_buf1, 0x0000, sizeof(out_buf1));
	  dannie1202 = plis_read16 (DIR2_PLIS_PACKSIZE_ADDR1202);
	 
	  /*Проверка что читаем пришёл нормальный пакет с направления 2*/
	  if((dannie1202==65535)||(dannie1202==0)||(dannie1202>1518))
	  {
		  printk("?bad_exeption_read_dir1_pack_size=%d?\n\r",dannie1202);
		  return;
	  }
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1202)%2==1)
	  {
		  dir1_dobavka_esli_packet_nechetnii=1; 
	  }
	  
	  
	  
	  packet_size_hex=(dannie1202/2)+dir1_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex 
	  //printk("+Tdm_Dir2_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm2_read_iteration,dannie1202,packet_size_hex);  
	  //16 bit  or 2 bait Local bus iteration
	        do
	        {
	  	  //mdelay(5);   
	        out_buf1[i]= plis_read16 (DIR2_PLIS_READ_ADDR402);
	        i++;           
	        }while(i<packet_size_hex);

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
	  	  memcpy(&dir1_ip_da_addr,out_buf1+15,4);   
	  	  // dir1_ip_da_addr    = out_buf1[16];
	  	  //MAC DA address read on direction 0
	  	 //dir1_mac_da_addr   = out_buf1[2];
	  	 //определяю признак KY-S по mac адресам
	  	 //dir1_mac_priznak_kys = out_buf1[2]>>8;
	  	 
	     dir1_priznak_arp_packet =out_buf1[6];
	  	 
	  	 //printk("++dir1_ip_da_addr=0x%x|dir1_mac_da_addr=0x%x+\n\r",dir1_ip_da_addr,dir1_mac_da_addr);
	  	 //printk("+dir1_mac_priznak_kys+=0x%x\n\r",dir1_mac_priznak_kys);
		 //пока делаю затычку Broadcast APR отсылаю в ethernet tsec2
		 if(dir1_priznak_arp_packet==0x0806)
		 {
		 		 
			   memcpy(&dir1_target_arp_nms_sa_addr,out_buf1+14,4);
			   memcpy(&dir1_target_arp_ip_da_addr,out_buf1+19,4);
			   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
			   ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,dir1_target_arp_ip_da_addr,dir1_target_arp_nms_sa_addr,2);  
			     //ARP packet for matrica
		 		 //ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,0x0806); 		   
			     //p2020_get_recieve_virttsec_packet_buf(out_buf1,dannie1202,2);
		 }
	  	 //priznak ky-s
	  	 else
	  	 {
	  		ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,dir1_ip_da_addr,0x0000,0); 	  	 
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
	  tdm2_read_iteration++; 
}


/**************************************************************************************************
Syntax:      	   void TDM2_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description

***************************************************************************************************/
void TDM3_dierction_read ()
{
	  UINT16 dannie1204=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm2_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  //UINT16 ostatok_of_size_packet=0;
	  UINT16  out_buf2[760];//1518 bait;
	 
	  //
	  __be32  dir2_target_arp_ip_da_addr=0;
	  __be32  dir2_target_arp_nms_sa_addr=0;
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
	  	  
	  dannie1204 = plis_read16 (DIR3_PLIS_PACKSIZE_ADDR1204 );
	  /*Проверка что читаем пришёл нормальный пакет с направления 3*/
	  if((dannie1204==65535)||(dannie1204==0)||(dannie1204>1518))
	  {
		  printk("?bad_exeption_read_dir3_pack_size=%d?\n\r",dannie1204);
		  return;
	  }
	  
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1204)%2==1)
	  {
		  dir2_dobavka_esli_packet_nechetnii=1; 
	  }
	  
	  packet_size_hex=(dannie1204/2)+dir2_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  	  
	  //printk("+Tdm_Dir3_read->>ITERATION=%d|1204in_byte=%d|1204in_hex=%d|size=%d|+\n\r",tdm2_read_iteration,dannie1204,packet_size_hex,dannie1204+PATCH_READ_PACKET_SIZE_ADD_ONE); 
	  //printk("+Tdm_Dir3_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm2_read_iteration,dannie1204,packet_size_hex);  

	  
	 	  //  else
	 	   // {	  
	 	  	  //16 bit  or 2 bait Local bus iteration
	 	  	  do
	 	  	  {
	 	  	  //mdelay(5);   
	 	        out_buf2[i]= plis_read16 (DIR3_PLIS_READ_ADDR404);
	 	        i++;           
	 	        }while(i<packet_size_hex);

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
	 	  	 memcpy(&dir2_ip_da_addr,out_buf2+15,4);
	 	  	 // dir2_ip_da_addr    = out_buf2[16];
	 	  	 //MAC DA address read on direction 0
	 	  	 //dir2_mac_da_addr   = out_buf2[2];
	 	  	 //определяю признак KY-S по mac адресам
	 	  	 //dir2_mac_priznak_kys = out_buf2[2]>>8;
	 	  	 ///Arp priznak
	 	  	 dir2_priznak_arp_packet =out_buf2[6];
	 		 
  	 
	 	  	 if(dir2_priznak_arp_packet==0x0806)
	 	  	   {
				   
	 	  		   memcpy(&dir2_target_arp_nms_sa_addr,out_buf2+14,4);
	 	  		   memcpy(&dir2_target_arp_ip_da_addr,out_buf2+19,4);
				   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
				   ngraf_packet_for_matrica_kommutacii(out_buf2 ,dannie1204,dir2_target_arp_ip_da_addr,dir2_target_arp_nms_sa_addr,3);  
	 	  		 
	 	  	     //p2020_get_recieve_virttsec_packet_buf(out_buf2,dannie1204,2);//send to eternet tsec ARP broadcast   
	 	  	   } 
	 	  	   else
	 	  	   {   
	 	  	     ngraf_packet_for_matrica_kommutacii(out_buf2 ,dannie1204,dir2_ip_da_addr,0x0000,0);  
	 	  	   }	  	   

//#endif	 	  	   
	    	 
	 	  	 #ifdef TDM2_DIR_TEST_ETHERNET_SEND
	 	       //p2020_get_recieve_virttsec_packet_buf(out_buf2,dannie1204+PATCH_READ_PACKET_SIZE_ADD_ONE);//send to eternet
	 	  	 #endif  
	 
	  tdm2_read_iteration++; 

}
	
	
/**************************************************************************************************
Syntax:      	   void TDM4_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
***************************************************************************************************/
void TDM4_dierction_read  ()
{
	  UINT16 dannie1206=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm3_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  //UINT16 ostatok_of_size_packet=0;
	  UINT16  out_buf3[760];//1518 bait;
	  	 	  
	  //
	  __be32  dir3_target_arp_ip_da_addr=0;
	  __be32  dir3_target_arp_nms_sa_addr=0;
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
	  dannie1206 = plis_read16 (DIR4_PLIS_PACKSIZE_ADDR1206 );
	  
	  /*Проверка что читаем пришёл нормальный пакет с направления 4*/
	  if((dannie1206==65535)||(dannie1206==0)||(dannie1206>1518))
	  {
		  printk("?bad_exeption_read_dir4_pack_size=%d?\n\r",dannie1206);
		  return;
	  }
	  
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1206)%2==1)
	  {
		  dir3_dobavka_esli_packet_nechetnii=1; 
	  }
	  packet_size_hex=(dannie1206/2)+dir3_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  
      
	  //printk("+Tdm_Dir4_read->>ITERATION=%d|1206in_byte=%d|1206in_hex=%d|size=%d|+\n\r",tdm3_read_iteration,dannie1206,packet_size_hex,dannie1206+PATCH_READ_PACKET_SIZE_ADD_ONE); 
	  //printk("+Tdm_Dir4_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm3_read_iteration,dannie1206,packet_size_hex); 
	  	 	  	//16 bit  or 2 bait Local bus iteration
	            do
	  	 	  	{
	  	 	  	  //mdelay(5);   
	  	 	    out_buf3[i]= plis_read16 (DIR4_PLIS_READ_ADDR406);
	  	 	    i++;           
	  	 	    }while(i<packet_size_hex);

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
	  	 	  	memcpy(&dir3_ip_da_addr,out_buf3+15,4);
	  	 	  	    //dir3_ip_da_addr    = out_buf3[16];
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
		 	  		   memcpy(&dir3_target_arp_nms_sa_addr,out_buf3+14,4);
		 	  		   memcpy(&dir3_target_arp_ip_da_addr,out_buf3+19,4);
					   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
					   ngraf_packet_for_matrica_kommutacii(out_buf3 ,dannie1206,dir3_target_arp_ip_da_addr,dir3_target_arp_nms_sa_addr,4);  
		 	  		 
		 	  	     //p2020_get_recieve_virttsec_packet_buf(out_buf2,dannie1204,2);//send to eternet tsec ARP broadcast   
		 	  	   } 
		 	  	   else
		 	  	   {   
		 	  	     ngraf_packet_for_matrica_kommutacii(out_buf3 ,dannie1206,dir3_ip_da_addr,0x0000,0);  
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
Syntax:      	   void TDM5_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
***************************************************************************************************/
void TDM5_dierction_read  ()
{
	  UINT16 dannie1208=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm5_read_iteration=0;
	  UINT16 packet_size=0;
	  UINT16 packet_size_hex=0;
	  UINT16  out_buf5[760];//1518 bait;
	  
	  
	  UINT8 dir5_dobavka_esli_packet_nechetnii=0; 
	  __be32  dir5_target_arp_ip_da_addr=0;
	  __be32  dir5_target_arp_nms_sa_addr=0;
	  
	  
	  //IP DA address read on direction 1
	  __be32  dir5_ip_da_addr    = 0;
	  UINT16  dir5_priznak_arp_packet=0;
	  
	  
	  memset(&out_buf5 ,0x0000, sizeof(out_buf5));	 	  
	  dannie1208 = plis_read16 (DIR5_PLIS_PACKSIZE_ADDR1208 );
	  
	  /*Проверка что читаем пришёл нормальный пакет с направления 5*/
	  if((dannie1208==65535)||(dannie1208==0)||(dannie1208>1518))
	  {
		  printk("?bad_exeption_read_dir5_pack_size=%d?\n\r",dannie1208);
		  return;
	  }
	  
	  
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1208)%2==1)
	  {
		  dir5_dobavka_esli_packet_nechetnii=1; 
	  }
	  packet_size_hex=(dannie1208/2)+dir5_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  
	 //out_size_byte=256;//512 bait;
	   printk("+Tdm_Dir5_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm5_read_iteration,dannie1208,packet_size_hex);   
	 //printk("+Tdm_Dir5_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm5_read_iteration,dannie1208,packet_size_hex); 
	  
	  
	  do
	   {
		 out_buf5[i]=plis_read16 (DIR5_PLIS_READ_ADDR408);  
		 i++;
	   }while(i<packet_size_hex);

	  /*
	  printk("+Tdm_Dir4_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir4_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	  */
	  
  	  //IP DA address read on direction 0
  	  memcpy(&dir5_ip_da_addr,out_buf5+15,4);   
  	  // dir1_ip_da_addr    = out_buf1[16];
  	  //MAC DA address read on direction 0
  	 //dir1_mac_da_addr   = out_buf1[2];
  	 //определяю признак KY-S по mac адресам
  	 //dir1_mac_priznak_kys = out_buf1[2]>>8;
  	 
     dir5_priznak_arp_packet =out_buf5[6];
  	 
  	 //printk("++dir1_ip_da_addr=0x%x|dir1_mac_da_addr=0x%x+\n\r",dir1_ip_da_addr,dir1_mac_da_addr);
  	 //printk("+dir1_mac_priznak_kys+=0x%x\n\r",dir1_mac_priznak_kys);
	 //пока делаю затычку Broadcast APR отсылаю в ethernet tsec2
	 if(dir5_priznak_arp_packet==0x0806)
	 {
	 		 
		   memcpy(&dir5_target_arp_nms_sa_addr,out_buf5+14,4);
		   memcpy(&dir5_target_arp_ip_da_addr,out_buf5+19,4);
		   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
		   ngraf_packet_for_matrica_kommutacii(out_buf5 ,dannie1208,dir5_target_arp_ip_da_addr,dir5_target_arp_nms_sa_addr,5);  
		     //ARP packet for matrica
	 		 //ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,0x0806); 		   
		     //p2020_get_recieve_virttsec_packet_buf(out_buf1,dannie1202,2);
	 }
  	 //priznak ky-s
  	 else
  	 {
  		ngraf_packet_for_matrica_kommutacii(out_buf5 ,dannie1208,dir5_ip_da_addr,0x0000,0); 	  	 
  	 }
	  
	#ifdef TDM4_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif  
	 tdm5_read_iteration++; 
				
}
			
	
/**************************************************************************************************
Syntax:      	   void TDM6_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
***************************************************************************************************/
void TDM6_dierction_read  ()
{
	  UINT16 dannie1210=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm6_read_iteration=0;
	  UINT16 packet_size=0;
	  UINT16 packet_size_hex=0;
	  UINT16  out_buf6[760];//1518 bait;
	 
	  UINT8 dir6_dobavka_esli_packet_nechetnii=0;
	  __be32  dir6_target_arp_ip_da_addr=0;
	  __be32  dir6_target_arp_nms_sa_addr=0;
	  
	  __be32  dir6_ip_da_addr    = 0;
	  UINT16  dir6_priznak_arp_packet=0;
	  
	  
	  memset(&out_buf6 ,0x0000, sizeof(out_buf6));	 	  
	  dannie1210 = plis_read16 (DIR6_PLIS_PACKSIZE_ADDR1210 );
	  
	  /*Проверка что читаем пришёл нормальный пакет с направления 6*/
	  if((dannie1210==65535)||(dannie1210==0)||(dannie1210>1518))
	  {
		  printk("?bad_exeption_read_dir6_pack_size=%d?\n\r",dannie1210);
		  return;
	  }
	  
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1210)%2==1)
	  {
		  dir6_dobavka_esli_packet_nechetnii=1; 
	  }
	  packet_size_hex=(dannie1210/2)+dir6_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  //printk("+Tdm_Dir6_read->>ITERATION=%d|in_byte=%d|in_hex=%d+\n\r",tdm6_read_iteration,dannie1210,packet_size_hex);   
	  
      do
	   {
		 //out_buf[i]= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));
		 out_buf6[i]=plis_read16 (DIR6_PLIS_READ_ADDR410);  
		 i++;
	   }while(i<packet_size_hex);
	//#endif
	    
	  /*
	  printk("+Tdm_Dir5_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir5_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	  */
	#ifdef TDM5_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif  

  	  //IP DA address read on direction 0
  	  memcpy(&dir6_ip_da_addr,out_buf6+15,4);   
  	  // dir1_ip_da_addr    = out_buf1[16];
  	  //MAC DA address read on direction 0
  	 //dir1_mac_da_addr   = out_buf1[2];
  	 //определяю признак KY-S по mac адресам
  	 //dir1_mac_priznak_kys = out_buf1[2]>>8;
  	 
     dir6_priznak_arp_packet =out_buf6[6];
  	 
  	 //printk("++dir1_ip_da_addr=0x%x|dir1_mac_da_addr=0x%x+\n\r",dir1_ip_da_addr,dir1_mac_da_addr);
  	 //printk("+dir1_mac_priznak_kys+=0x%x\n\r",dir1_mac_priznak_kys);
	 //пока делаю затычку Broadcast APR отсылаю в ethernet tsec2
	 if(dir6_priznak_arp_packet==0x0806)
	 {
	 		 
		   memcpy(&dir6_target_arp_nms_sa_addr,out_buf6+14,4);
		   memcpy(&dir6_target_arp_ip_da_addr,out_buf6+19,4);
		   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
		   ngraf_packet_for_matrica_kommutacii(out_buf6 ,dannie1210,dir6_target_arp_ip_da_addr,dir6_target_arp_nms_sa_addr,6);  
		     //ARP packet for matrica
	 		 //ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,0x0806); 		   
		     //p2020_get_recieve_virttsec_packet_buf(out_buf1,dannie1202,2);
	 }
  	 //priznak ky-s
  	 else
  	 {
  		ngraf_packet_for_matrica_kommutacii(out_buf6 ,dannie1210,dir6_ip_da_addr,0x0000,0); 	  	 
  	 }
	  
	  tdm6_read_iteration++; 
					
}					
/**************************************************************************************************
Syntax:      	   void TDM7_dierction_read  (UINT16 *out_buf,UINT16  out_size_byte)
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
***************************************************************************************************/
void TDM7_dierction_read  ()
{
	  UINT16 dannie1212=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm7_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  UINT16  out_buf7[760];//1518 bait;
	  
	  UINT8   dir7_dobavka_esli_packet_nechetnii=0;
	  __be32  dir7_target_arp_ip_da_addr=0;
	  __be32  dir7_target_arp_nms_sa_addr=0;
	  
	  __be32  dir7_ip_da_addr    = 0;
	  UINT16  dir7_priznak_arp_packet=0;
	    
	  memset(&out_buf7 ,0x0000, sizeof(out_buf7));	 	  
	  dannie1212 = plis_read16 (DIR7_PLIS_PACKSIZE_ADDR1212 );
	  
	  /*Проверка что читаем пришёл нормальный пакет с направления 7*/
	  if((dannie1212==65535)||(dannie1212==0)||(dannie1212>1518))
	  {
		  printk("?bad_exeption_read_dir7_pack_size=%d?\n\r",dannie1212);
		  return;
	  }
	  
	  
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1212)%2==1)
	  {
		  dir7_dobavka_esli_packet_nechetnii=1; 
	  }
	  packet_size_hex=(dannie1212/2)+dir7_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  

	  //printk("+Tdm_Dir7_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm7_read_iteration,dannie1212,packet_size_hex);   
	 
	  do
	   {
		 out_buf7[i]=plis_read16 (DIR7_PLIS_READ_ADDR412);  
		 i++;
	   }while(i<packet_size_hex);

	  
	  /*  
	  printk("+Tdm_Dir6_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir6_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	  */
  	  //IP DA address read on direction 0
  	  memcpy(&dir7_ip_da_addr,out_buf7+15,4);   
  	  // dir1_ip_da_addr    = out_buf1[16];
  	  //MAC DA address read on direction 0
  	 //dir1_mac_da_addr   = out_buf1[2];
  	 //определяю признак KY-S по mac адресам
  	 //dir1_mac_priznak_kys = out_buf1[2]>>8;
  	 
     dir7_priznak_arp_packet =out_buf7[6];
  	 
  	 //printk("++dir1_ip_da_addr=0x%x|dir1_mac_da_addr=0x%x+\n\r",dir1_ip_da_addr,dir1_mac_da_addr);
  	 //printk("+dir1_mac_priznak_kys+=0x%x\n\r",dir1_mac_priznak_kys);
	 //пока делаю затычку Broadcast APR отсылаю в ethernet tsec2
	 if(dir7_priznak_arp_packet==0x0806)
	 {
	 		 
		   memcpy(&dir7_target_arp_nms_sa_addr,out_buf7+14,4);
		   memcpy(&dir7_target_arp_ip_da_addr,out_buf7+19,4);
		   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
		   ngraf_packet_for_matrica_kommutacii(out_buf7 ,dannie1212,dir7_target_arp_ip_da_addr,dir7_target_arp_nms_sa_addr,7);  
		     //ARP packet for matrica
	 		 //ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,0x0806); 		   
		     //p2020_get_recieve_virttsec_packet_buf(out_buf1,dannie1202,2);
	 }
  	 //priznak ky-s
  	 else
  	 {
  		ngraf_packet_for_matrica_kommutacii(out_buf7 ,dannie1212,dir7_ip_da_addr,0x0000,0); 	  	 
  	 }
	 
	#ifdef TDM6_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif    
	  
   tdm7_read_iteration++; 				
}
									
/***************************************************************************************************
Syntax:      	   void TDM8_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	   Returns 1 on success and negative value on failure.
 				   Value		 									Description
***************************************************************************************************/
void TDM8_dierction_read  ()
{
	  UINT16 dannie1214=0; 
	  UINT16 i=0;		  
	  static UINT16 tdm8_read_iteration=0;
	  UINT16 packet_size_hex=0;
	  UINT16  out_buf8[760];//1518 bait;
	  
	  
	  UINT8 dir8_dobavka_esli_packet_nechetnii=0;
	  __be32  dir8_target_arp_ip_da_addr =0;
	  __be32  dir8_target_arp_nms_sa_addr=0;
	  
	  __be32  dir8_ip_da_addr    = 0;
	  UINT16  dir8_priznak_arp_packet=0;
	  
	 
	  memset(&out_buf8 ,0x0000, sizeof(out_buf8));	 	  
	  dannie1214 = plis_read16 (DIR8_PLIS_PACKSIZE_ADDR1214);
	  
	  /*Проверка что читаем пришёл нормальный пакет с направления 8*/
	  if((dannie1214==65535)||(dannie1214==0)||(dannie1214>1518))
	  {
		  printk("?bad_exeption_read_dir8_pack_size=%d?\n\r",dannie1214);
		  return;
	  }
	  
	  
	  
	  
	  //Если пакет нечётный читаем на еденицу больше из шины local bus;
	  if((dannie1214)%2==1)
	  {
		  dir8_dobavka_esli_packet_nechetnii=1; 
	  }
	  packet_size_hex=(dannie1214/2)+dir8_dobavka_esli_packet_nechetnii; //convert byte to element of massive in hex
	  
	  //printk("+Tdm_Dir8_read->>iteration=%d|in_byte=%d|in_hex=%d+\n\r",tdm8_read_iteration,dannie1214,packet_size_hex);   
	  
	  do
	   {
		 //out_buf[i]= __raw_readw(map->virt + PLIS_LINUX_START_DATA_OFFSET+(addr*2));
		 out_buf8[i]=plis_read16 (DIR8_PLIS_READ_ADDR414);  
		 i++;
	   }while(i<packet_size_hex);
	//#endif
	  /*
	  printk("+Tdm_Dir7_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	  printk("+Tdm_Dir7_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
      */
	#ifdef TDM7_DIR_TEST_ETHERNET_SEND
	  p2020_get_recieve_virttsec_packet_buf(out_buf,packet_size_hex);//send to eternet
	#endif    
  	  //IP DA address read on direction 0
  	  memcpy(&dir8_ip_da_addr,out_buf8+15,4);   
  	  // dir1_ip_da_addr    = out_buf1[16];
  	  //MAC DA address read on direction 0
  	 //dir1_mac_da_addr   = out_buf1[2];
  	 //определяю признак KY-S по mac адресам
  	 //dir1_mac_priznak_kys = out_buf1[2]>>8;
  	 
     dir8_priznak_arp_packet =out_buf8[6];
  	 
  	 //printk("++dir1_ip_da_addr=0x%x|dir1_mac_da_addr=0x%x+\n\r",dir1_ip_da_addr,dir1_mac_da_addr);
  	 //printk("+dir1_mac_priznak_kys+=0x%x\n\r",dir1_mac_priznak_kys);
	 //пока делаю затычку Broadcast APR отсылаю в ethernet tsec2
	 if(dir8_priznak_arp_packet==0x0806)
	 {
	 		 
		   memcpy(&dir8_target_arp_nms_sa_addr,out_buf8+14,4);
		   memcpy(&dir8_target_arp_ip_da_addr,out_buf8+19,4);
		   //printk("ARP zaprosi on tdm dir0 0x%x\n\r",dir0_target_arp_ip_da_addr);
		   ngraf_packet_for_matrica_kommutacii(out_buf8 ,dannie1214,dir8_target_arp_ip_da_addr,dir8_target_arp_nms_sa_addr,8);  
		     //ARP packet for matrica
	 		 //ngraf_packet_for_matrica_kommutacii(out_buf1 ,dannie1202,0x0806); 		   
		     //p2020_get_recieve_virttsec_packet_buf(out_buf1,dannie1202,2);
	 }
  	 //priznak ky-s
  	 else
  	 {
  		ngraf_packet_for_matrica_kommutacii(out_buf8 ,dannie1214,dir8_ip_da_addr,0x0000,0); 	  	 
  	 }

tdm8_read_iteration++; 			
}						
						


#if 0
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
Syntax:      	   void TDM10_dierction_read  ()
Remarks:		   This Read from PLIS0 tdm direction  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
void TDM10_dierction_read  ()
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

#endif













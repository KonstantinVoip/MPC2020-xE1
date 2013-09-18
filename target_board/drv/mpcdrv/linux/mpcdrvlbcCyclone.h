/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*        ..                                       All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvlbcCyclone.h
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
* --------- $Log: mpcdrvlbcCyclone.h $
* --------- Initial revision
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
-----------------------------------------------------------------------------*/
#ifndef MPCDRVLBCCYCLONE_H
#define MPCDRVLBCCYCLONE_H

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/


/*****************************************************************************/
/*	INCLUDES							     */

#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/elbcTransnmitBuf.h"
/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/

/*  
 * 
 * Cyclone 3 PLIS memory Addresation for Linux  ofs=0x0000000000000000;
 * this start memory windows for CS0  ->and physical 0xef000000
 * 
 * this logical memory for read and write 
 */

    //#define P2020_RDBKIT  1
//  #define P2020_MPC     1




///////////DEFINE for GLOBAL PLIS CONFIGURATIONs
#define PLIS_LINUX_START_DATA_OFFSET  0x0000000000000000
#define PLIS_PHYSICAL_RESOURCE_START  0xef000000
#define PLIS_PHYSICAL_RESOURCE_SIZE   0x01000000


/*  Registers offset on Cyclone 3 ->lbc MAP */


typedef enum{
   //PLIS_ADDRESS0     		= 0,
  
   
   //TDM DIR 0 REGISTER
     DIR0_PLIS_ADDRESS30 			= 30,  //0x1E  plis write success address
     DIR0_PLIS_ADDRESS800 			= 800,
     DIR0_PLIS_ADDRESS1000  		= 1000,
     DIR0_PLIS_ADDRESS1200          = 1200,//       plis read  packet size 
     DIR0_PLIS_ADDRESS200           = 200, //0xC8   plis read data
     DIR0_PLIS_ADDRESS400           = 400 //0x190   plis write data
     
     
   //TDM DIR 1 REGISTER
   
     
     
   //TDM DIR 2 REGISTER
   
   
   //TDM DIR 3 REGISTER
   
   
   //TDM DIR 4 REGISTER
   
   
   //TDM DIR 5 REGISTER
   
   
   //TDM DIR 6 REGISTER
   
  
   //TDM DIR 7 REGISTER
   
   
   //TDM DIR 8 REGISTER
   
   
   //TDM DIR 9 REGISTER
 
}Plis_register_address;




typedef enum
{
	
	PLIS_VALUE =54
	
	//TDM DIR 0 REGISTER
	//TDM DIR 1 REGISTER
	//TDM DIR 2 REGISTER
	//TDM DIR 3 REGISTER
	//TDM DIR 4 REGISTER
	//TDM DIR 5 REGISTER
	//TDM DIR 6 REGISTER
	//TDM DIR 7 REGISTER
	//TDM DIR 8 REGISTER
	//TDM DIR 9 REGISTER
	
	
}Plis_register_value;











////////////PLIS_Direction0_DEFENITION////////////////////

#define  PLIS_WRITE_SUCCESS               0x0003



/*****************************************************************************/
/********************	TESTING FUNCTIONS ********						     */
/*****************************************************************************/

/*****************************************************************************/
/*	PUBLIC DATA YPES						     */
/*****************************************************************************/
//Initialization function of Cyclon3 <-> P2020
void LocalBusCyc3_Init();

SINT32 test_Cyc3Read();
SINT32 test_Cyc3Write();


/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/

//
void TDM0_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM1_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM2_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM3_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM4_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM5_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM6_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM7_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM8_dierction_read  (u16 *out_buf,u16 out_size_byte);
void TDM9_dierction_read  (u16 *out_buf,u16 out_size_byte);




//Write Buffer to PLIS on 10 Direction
void TDM0_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM1_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM2_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM3_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM4_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM5_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM6_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM7_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM8_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM9_direction_write (const u16 *in_buf ,const u16 in_size);




//10 READ READY functions DIRECTION ON MK8 DEVICE
UINT16 TDM0_direction_READ_READY(void);
UINT16 TDM1_direction_READ_READY(void);
UINT16 TDM2_direction_READ_READY(void);
UINT16 TDM3_direction_READ_READY(void);
UINT16 TDM4_direction_READ_READY(void);
UINT16 TDM5_direction_READ_READY(void);
UINT16 TDM6_direction_READ_READY(void);
UINT16 TDM7_direction_READ_READY(void);
UINT16 TDM8_direction_READ_READY(void);
UINT16 TDM9_direction_READ_READY(void);



//10 WRITE READY functions DIRECTION ON MK8 DEVICE
UINT16 TDM0_direction_WRITE_READY(void);
UINT16 TDM1_direction_WRITE_READY(void);
UINT16 TDM2_direction_WRITE_READY(void);
UINT16 TDM3_direction_WRITE_READY(void);
UINT16 TDM4_direction_WRITE_READY(void);
UINT16 TDM5_direction_WRITE_READY(void);
UINT16 TDM6_direction_WRITE_READY(void);
UINT16 TDM7_direction_WRITE_READY(void);
UINT16 TDM8_direction_WRITE_READY(void);
UINT16 TDM9_direction_WRITE_READY(void);



#endif /* MPCDRVLBCCYCLONE_H */


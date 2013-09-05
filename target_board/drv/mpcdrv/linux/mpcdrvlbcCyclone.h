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

////////////PLIS_Direction0_DEFENITION////////////////////

#define  PLIS_WRITE_SUCCESS               0x0003
#define  DIR0_ADDRESS_WRITE_SUCCESS       30  //0x1E
#define  DIR0_ADDRESS_WRITE_DATA          200 //0xC8
#define  DIR0_ADDRESS_READ_DATA           400 //0x190



///////////DEFINE for GLOBAL PLIS CONFIGURATIONs
#define PLIS_LINUX_START_DATA_OFFSET  0x0000000000000000
#define PLIS_PHYSICAL_RESOURCE_START  0xef000000
#define PLIS_PHYSICAL_RESOURCE_SIZE   0x01000000



 
#define  PLIS_ADDRESS0    0
#define  PLIS_ADDRESS30   30       

#define  PLIS_ADDRESS800  800
#define  PLIS_ADDRESS1000 1000

#define  PLIS_ADDRESS1200 1200    //this output size of ip packet

/*****************************************************************************/
/********************	TESTING FUNCTIONS ********						     */
/*****************************************************************************/
void callback_functions(UINT16 *data);



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
UINT16 plis_read16 (const u16 addr);
void   plis_write16(const u16 addr,const u16 value);



//Write Buffer to PLIS
void TDM0_direction_write (const u16 *in_buf ,const u16 in_size);
//Read Buffer from PLIS 
void TDM0_dierction_read  (u16 *out_buf,u16 out_size_byte);



//Functions read_data_on Local_bus _ready
UINT16 TDM0_direction_READ_READY(void);
UINT16 TDM1_direction_READ_READY(void);
UINT16 TDM2_direction_READ_READY(void);

//Functions write_data_on_Local_bus _ready
UINT16 TDM0_direction_WRITE_READY(void);
UINT16 TDM1_direction_WRITE_READY(void);
UINT16 TDM2_direction_WRITE_READY(void);



/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#endif /* MPCDRVLBCCYCLONE_H */


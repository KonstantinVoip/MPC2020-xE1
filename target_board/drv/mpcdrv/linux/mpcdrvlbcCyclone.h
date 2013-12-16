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

//debug plis address 1400
#define PLIS_LINK_TDM_DIR20           20
#define PLIS_ADDR1400           	1400
#define PATCHlbc_ONE_ITERATION_READ    1
#define PATCHlbc_ONE_ITERATION_WRITE   1
#define PATCH_READ_PACKET_SIZE_ADD_ONE 1



/*PLIS Defenition Constant */
typedef enum
{
	 PLIS_LINUX_START_DATA_OFFSET = 0x0000000000000000,
	 PLIS_PHYSICAL_RESOURCE_START = 0xef000000,
	 PLIS_PHYSICAL_RESOURCE_SIZE  = 0x01000000,
	 PLIS_WRITE_SUCCESS = 0x0003
     	

}Plis_register_value;



/*  Registers offset on Cyclone 3 ->lbc MAP */
typedef enum{
  
   //TDM DIR 0 REGISTER
	 DIR0_PLIS_WRITEOK_ADDR30 			    = 30,  //0x1E  plis write success address
     DIR0_PLIS_READ_BUG_ADDR800 			= 800, //plis patch read register
     DIR0_PLIS_READOK_ADDR1000  		    = 1000,//read ok start
     DIR0_PLIS_PACKSIZE_ADDR1200            = 1200,//plis read in  packet size 
     DIR0_PLIS_PACKSIZE_ADDR1600            = 1600,//send plis packet size
     DIR0_PLIS_READ_ADDR400                 = 400, //0xC8   plis write data
     DIR0_PLIS_WRITE_ADDR200                = 200, //0x190   plis read data
     
    //TDM DIR 1 REGISTER
     DIR1_PLIS_WRITEOK_ADDR32 			   = 32,  //0x1E  plis write success address
     DIR1_PLIS_READ_BUG_ADDR802 		   = 802,
     DIR1_PLIS_READOK_ADDR1002  		   = 1002,
     DIR1_PLIS_PACKSIZE_ADDR1202           = 1202,//       plis read  packet size 
     DIR1_PLIS_PACKSIZE_ADDR1602           = 1602,//send plis packet size
     DIR1_PLIS_READ_ADDR402                = 402, //0xC8   plis read data
     DIR1_PLIS_WRITE_ADDR202               = 202, //0x190   plis write data
     
 
    //TDM DIR 2 REGISTER
     DIR2_PLIS_WRITEOK_ADDR34 			   = 34,  //0x1E  plis write success address
     DIR2_PLIS_READ_BUG_ADDR804 		   = 804,
     DIR2_PLIS_READOK_ADDR1004  		   = 1004,
     DIR2_PLIS_PACKSIZE_ADDR1204           = 1204,//       plis read  packet size 
     DIR2_PLIS_PACKSIZE_ADDR1604           = 1604,
     DIR2_PLIS_READ_ADDR404                = 404, //0xC8   plis read data
     DIR2_PLIS_WRITE_ADDR204               = 204, //0x190   plis write data 
   
    //TDM DIR 3 REGISTER
     DIR3_PLIS_WRITEOK_ADDR36 			   = 36,  //0x1E  plis write success address
     DIR3_PLIS_READ_BUG_ADDR806 		   = 806,
     DIR3_PLIS_READOK_ADDR1006  		   = 1006,
     DIR3_PLIS_PACKSIZE_ADDR1206           = 1206,//       plis read  packet size 
     DIR3_PLIS_PACKSIZE_ADDR1606           = 1606,
     DIR3_PLIS_READ_ADDR406                = 406, //0xC8   plis read data
     DIR3_PLIS_WRITE_ADDR206               = 206, //0x190   plis write data
   
    //TDM DIR 4 REGISTER
     DIR4_PLIS_WRITEOK_ADDR38 			   = 38,  //0x1E  plis write success address
     DIR4_PLIS_READ_BUG_ADDR808 		   = 808,
     DIR4_PLIS_READOK_ADDR1008  		   = 1008,
     DIR4_PLIS_PACKSIZE_ADDR1208           = 1208,//       plis read  packet size 
     DIR4_PLIS_PACKSIZE_ADDR1608           = 1608,
     DIR4_PLIS_READ_ADDR408                = 408, //0xC8   plis read data
     DIR4_PLIS_WRITE_ADDR208               = 208, //0x190   plis write data
   
    //TDM DIR 5 REGISTER
     DIR5_PLIS_WRITEOK_ADDR40 			   = 40,  //0x1E  plis write success address
     DIR5_PLIS_READ_BUG_ADDR810 		   = 810,
     DIR5_PLIS_READOK_ADDR1010  		   = 1010,
     DIR5_PLIS_PACKSIZE_ADDR1210           = 1210,//       plis read  packet size 
     DIR5_PLIS_PACKSIZE_ADDR1610           = 1610,//send packet size in byte
     DIR5_PLIS_READ_ADDR410                = 410, //0xC8   plis read data
     DIR5_PLIS_WRITE_ADDR210               = 210, //0x190   plis write data
   
    //TDM DIR 6 REGISTER
     DIR6_PLIS_WRITEOK_ADDR42 			   = 42,  //0x1E  plis write success address
     DIR6_PLIS_READ_BUG_ADDR812 		   = 812,
     DIR6_PLIS_READOK_ADDR1012  		   = 1012,
     DIR6_PLIS_PACKSIZE_ADDR1212           = 1212,//       plis read  packet size 
     DIR6_PLIS_PACKSIZE_ADDR1612           = 1612,//send packet size in byte
     DIR6_PLIS_READ_ADDR412                = 412, //0xC8   plis read data
     DIR6_PLIS_WRITE_ADDR212               = 212, //0x190   plis write data
  
   //TDM DIR 7 REGISTER
     DIR7_PLIS_WRITEOK_ADDR44 			   = 44,  //0x1E  plis write success address
     DIR7_PLIS_READ_BUG_ADDR814 		   = 814,
     DIR7_PLIS_READOK_ADDR1014  		   = 1014,
     DIR7_PLIS_PACKSIZE_ADDR1214           = 1214,//       plis read  packet size 
     DIR7_PLIS_PACKSIZE_ADDR1614           = 1614,//send packet size in byte
     DIR7_PLIS_READ_ADDR414                = 414, //0xC8   plis read data
     DIR7_PLIS_WRITE_ADDR214               = 214, //0x190   plis write data
   
     //TDM DIR 8 REGISTER
     DIR8_PLIS_WRITEOK_ADDR46 			   = 46,  //0x1E  plis write success address
     DIR8_PLIS_READ_BUG_ADDR816 		   = 816,
     DIR8_PLIS_READOK_ADDR1016  		   = 1016,
     DIR8_PLIS_PACKSIZE_ADDR1216           = 1216,//       plis read  packet size 
     DIR8_PLIS_PACKSIZE_ADDR1616           = 1616,//send packet size in byte
     DIR8_PLIS_READ_ADDR416                = 416, //0xC8   plis read data
     DIR8_PLIS_WRITE_ADDR216               = 216, //0x190   plis write dataa
   
   //TDM DIR 9 REGISTER
     DIR9_PLIS_WRITEOK_ADDR48 			   = 48,  //0x1E  plis write success address
     DIR9_PLIS_READ_BUG_ADDR818 		   = 818,
     DIR9_PLIS_READOK_ADDR1018  		   = 1018,
     DIR9_PLIS_PACKSIZE_ADDR1218           = 1218,//       plis read  packet size 
     DIR9_PLIS_READ_ADDR418                = 418, //0xC8   plis read data
     DIR9_PLIS_WRITE_ADDR218               = 218, //0x190   plis write data
     
}Plis_register_address;

/*****************************************************************************/
/********************	TESTING FUNCTIONS ********						     */
/*****************************************************************************/

/*****************************************************************************/
/*	PUBLIC DATA YPES						     */
/*****************************************************************************/
//Initialization function of Cyclon3 <-> P2020
void LocalBusCyc3_Init();

/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/

/* Read Data from Direction */
//void TDM0_dierction_read  (u16 *out_buf,u16 out_size_byte);


void TDM1_dierction_read  ();
void TDM2_dierction_read  ();
void TDM3_dierction_read  ();
void TDM4_dierction_read  ();
void TDM5_dierction_read  ();
void TDM6_dierction_read  ();
void TDM7_dierction_read  ();
void TDM8_dierction_read  ();
void TDM9_dierction_read  ();
void TDM10_dierction_read ();


/* Write Data to direction */
void TDM1_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM2_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM3_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM4_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM5_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM6_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM7_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM8_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM9_direction_write (const u16 *in_buf ,const u16 in_size);
void TDM10_direction_write (const u16 *in_buf ,const u16 in_size);


/*Ready Read DATA */
UINT16 TDM1_direction_READ_READY(void);
UINT16 TDM2_direction_READ_READY(void);
UINT16 TDM3_direction_READ_READY(void);
UINT16 TDM4_direction_READ_READY(void);
UINT16 TDM5_direction_READ_READY(void);
UINT16 TDM6_direction_READ_READY(void);
UINT16 TDM7_direction_READ_READY(void);
UINT16 TDM8_direction_READ_READY(void);
UINT16 TDM9_direction_READ_READY(void);
UINT16 TDM10_direction_READ_READY(void);


/*Ready Write Data*/
UINT16 TDM1_direction_WRITE_READY(void);
UINT16 TDM2_direction_WRITE_READY(void);
UINT16 TDM3_direction_WRITE_READY(void);
UINT16 TDM4_direction_WRITE_READY(void);
UINT16 TDM5_direction_WRITE_READY(void);
UINT16 TDM6_direction_WRITE_READY(void);
UINT16 TDM7_direction_WRITE_READY(void);
UINT16 TDM8_direction_WRITE_READY(void);
UINT16 TDM9_direction_WRITE_READY(void);
UINT16 TDM10_direction_WRITE_READY(void);



#endif /* MPCDRVLBCCYCLONE_H */


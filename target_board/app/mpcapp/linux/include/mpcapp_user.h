/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcapp_user.h
*
* Description : User support  defenitions and functions declarations for MPC p2020
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
* $Date: 2012/09/25 16:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcapp_user.h $
* --------- Initial revision
******************************************************************************/
/*-----------------------------------------------------------------------------
GENERAL NOTES


-----------------------------------------------------------------------------*/
#ifndef MPCAPP_USER_H
#define MPCAPP_USER_H

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/

/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/
/*
 * debug related
 */

/////////////Enable and Disable Menu on me board P2020
#define DEBUGGING

/* 
#define PRINTE(fmt,arg...)	{ if(CCApp_Dbg(MIF_DBG_GET) & MIF_DBG_ERR) printd("ERR - " fmt, ## arg); }
#define PRINTW(fmt,arg...)	{ if(CCApp_Dbg(MIF_DBG_GET) & MIF_DBG_WARN) printd("WARN - " fmt, ## arg); }
#define PRINTD(fmt,arg...)	{ if(CCApp_Dbg(MIF_DBG_GET) & MIF_DBG_INFO) printd(fmt, ## arg); }
#define PRINTDD(fmt,arg...)	{ if(CCApp_Dbg(MIF_DBG_GET) & MIF_DBG_DETAIL) printd(fmt, ## arg); }
#define PRINTP(fmt,arg...)	{ if(CCApp_Dbg(MIF_DBG_GET) & MIF_DBG_PRINT1) printd(fmt, ## arg); }
#define PRINTL3(msg,len)	{ if(CCApp_Dbg(MIF_DBG_GET) & MIF_DBG_PRINT2) Printl3ccuif(msg, len); }
#define PRINTM(fmt,arg...)	{ if(CCApp_Dbg(MIF_DBG_GET) & MIF_DBG_PRINT3) printd(fmt, ## arg); }
*/

  
int fd;//File descriptor for Character Device 
  

SINT32 mpcapp_Start(void *ccuif_par);
SINT32 mpcapp_open_character_device();


/*Temporary Testing Application Enable  */
SINT32 mpcapp_StartTestApllication();

/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/
/* support functions */
int user_par(char text[], int def);

/*
unsigned long get_ip_address(char *prompt, unsigned long default_ip);
void get_ieee_address(char *prompt, unsigned char *ieee);
unsigned long printb(char *txt, void *vpb, unsigned long lb);
char *itoa(int ival, char *buf, int radix);
unsigned long lchend(unsigned long val);
unsigned long CCApp_Dbg(unsigned long dbg);
*/


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/



#endif	/* MPCAPP_USER_H */



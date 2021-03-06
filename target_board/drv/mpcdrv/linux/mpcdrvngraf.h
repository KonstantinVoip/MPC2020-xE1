/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvnbuf.h
*
* Description : Declaration Buffer's function for transmit <->recieve data on localbus P2020. 
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
* --------- $Log: mpcdrvnbuf.h $
* --------- Initial revision
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
-----------------------------------------------------------------------------*/
#ifndef MPCDRVGRAF_H
#define MPCDRVGRAF_H

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/


/*****************************************************************************/
/*	INCLUDES							     */
#include <linux/kernel.h>
#include <linux/module.h> // Needed by all modules
//#include <linux/init.h>   // Needed for the macros
//#include <linux/delay.h>  // mdelay ,msleep,
//#include <linux/timer.h>  // timer 
//#include <linux/ktime.h>  // hardware timer (ktime)
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"

//#include <linux/types.h>    //Data types for linux

/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/

UINT16 marsrutiazation_enable;
/*****************************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!EXTERN FUNCTIONS AND DATA TYPES						 */
/******************************************************************************/
//void ngraf_get_datapacket (const u16 *in_buf ,const u16 in_size);
bool ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size);
void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii,u32 priznak_nms3_ot_arp_sa_addr,u8 tdm_input_read_direction);
void ngraf_get_ip_mac_my_kys (UINT8 state,UINT32 ip_addres,UINT8 *mac_address);
//UINT32 ngraf_get_ipaddr_my_kys();  

  
/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#endif //MPCDRVGRAF_H


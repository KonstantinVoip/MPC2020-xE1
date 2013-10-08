/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrv_gianfar.h
*
* Description : Low Level MPC P2020 Ethernet Driver. 
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
* $Date: 2013/06/11 22:12:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrv_gianfar.h $
* --------- Initial revision
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
-----------------------------------------------------------------------------*/
#ifndef MPCDRVGIANFAR_H
#define MPCDRVGIANFAR_H

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/


/*****************************************************************************/
/*	INCLUDES							     */
/*
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/mii.h>
#include <linux/phy.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/crc32.h>
#include <linux/workqueue.h>
#include <linux/ethtool.h>
*/

#ifdef CONFIG_GIANFAR_L2SRAM
#include <asm/fsl_85xx_cache_sram.h>
#define ALIGNMENT 0x20
#endif


/*	INCLUDES							     */
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"

/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/

/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/








/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#endif /* MPCDRVETHIPV4_H */


//Start Ethernet Initialization
void InitIp_Ethernet();
/*struct net_device * get_tsec0();
struct net_device * get_tsec1();
struct net_device * get_tsec2();
struct net_device * get_virt_tsec3();
struct net_device * get_virt_tsec4();
*/

void p2020_get_recieve_packet_and_setDA_MAC(const u16 *in_buf,const u16 in_size,const u16 *mac_header);
void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len);
void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);
//Recieve and transmit packet on ethernet








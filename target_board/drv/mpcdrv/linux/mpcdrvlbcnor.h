/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvlbcnor.h
*
* Description : Declaration function for NOr flash on local bus p2020rdb . 
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
* --------- $Log: mpcdrvlbcnor.h $
* --------- Initial revision
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
-----------------------------------------------------------------------------*/
#ifndef MPCDRVLBCNOR_H
#define MPCDRVLBCNOR_H

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/


/*****************************************************************************/
/*	INCLUDES							     */
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"

/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/
#define CFI_MODE_CFI	1
#define CFI_MODE_JEDEC	0


#define xip_disable(map, chip, adr)
#define xip_enable(map, chip, adr)
#define XIP_INVAL_CACHED_RANGE(x...)


#define UDELAY(map, chip, adr, usec)  \
do {  \
	spin_unlock(chip->mutex);  \
	cfi_udelay(usec);  \
	spin_lock(chip->mutex);  \
} while (0)

#define INVALIDATE_CACHE_UDELAY(map, chip, adr, len, usec)  \
do {  \
	spin_unlock(chip->mutex);  \
	INVALIDATE_CACHED_RANGE(map, adr, len);  \
	cfi_udelay(usec);  \
	spin_lock(chip->mutex);  \
} while (0)


/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/
SINT32  MPCInitNORflash();
SINT32  mpcNorReadata (loff_t start_offset,size_t len,u_char *buf);
SINT32  mpcNorWritedata(loff_t start_offset,size_t len,const u_char *buf);



/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#endif /* MPCDRVLBCNOR_H */


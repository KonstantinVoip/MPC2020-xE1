/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcUniDef.h
*
* Description : This module contains system unified defintions
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
* $Date: 2012/10/08 16:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcUniDef.h $
* --------- Initial revision
******************************************************************************/
/*-----------------------------------------------------------------------------
GENERAL NOTES


-----------------------------------------------------------------------------*/
#ifndef MPC_UNIDEFS_H
#define MPC_UNIDEFS_H

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/

/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/

/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/

/*
 * debug related
 */
 
 
/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/
#define ON   1
#define OFF  0






/*Enable and disable Debugging P2020 processor*/
//#define DEBUGGING ON
//#define DEBUGGING OFF








/* For function parameters mark-out: */
#define IN
#define OUT
#define INOUT


/* ##### Constant Definitions ##### */
#ifndef NULL
#define NULL            ((void *) 0x0)			                    /* Null Void Pointer */
#endif  /* NULL */

#define UDFALSE			 		0x0									/* Logical FALSE of the Boolean Type */
#define UDTRUE			 		0x1									/* Logical TRUE  of the Boolean Type */

#define UDOFF	 			    0x0									/* Logical FALSE of the Integer Type */
#define UDON	 				0x1									/* Logical TRUE  of the Integer Type */

/* ##### */
typedef enum
{
	STATUS_ERR = -1,
	STATUS_OK  =  1
} UDSTATUS;



/* ##### Macro Definitions ##### */
#define printd printf
#define PRINTK printk
#define PRINTF printf

#define PROMPT_TXT		"MPC_APP> "

/*
 * operating system dependecies
 */
/* standard input */
#define GET_STDIN(cmdline,cmdlen)	fgets(cmdline, cmdlen, stdin)
/* character input recognition */
#define CHAR_IDENT	    0x01000000
#define CHAR_IDENT_MASK	0xffffff00
/* return key definition */
#define RETURN_KEY	'\n'




/* ##### Type Definitions ##### */
typedef unsigned char	UINT8;							/* Unsigned Integer of 8 Bits */
typedef unsigned short	UINT16;							/* Unsigned Integer of 16 Bits */
typedef unsigned long 	UINT32;							/* Unsigned Integer of 32 Bits */


typedef signed   char 	SINT8;							/* Signed Integer of 8 Bits */
typedef signed   short	SINT16;							/* Signed Integer of 16 Bits */
typedef signed   long 	SINT32;							/* Signed Integer of 32 Bits */


typedef UINT16	        UDBOOL;					 		/* Boolean Type */



/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/



#endif	/* MPC_UNIDEFS_H */



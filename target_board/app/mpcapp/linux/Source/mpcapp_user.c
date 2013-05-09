
/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcapp_user.c
*
* Description : Release support functions declarations for MPC Router
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
* --------- $Log: mpcapp_user.c $
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
#include <stdio.h>						 /* snprintf */
#include <stdlib.h>					     /* malloc, free */
#include <string.h>					     /* memset, strncmp */
#include <fcntl.h>


/*****************************************************************************/
/*	External Header						     */
/*****************************************************************************/
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
#include  <mpcapp_user.h>					     /* own header */




/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/



/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/



/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/



/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/

/* debug flag */
//static unsigned long ccapp_dbg = MIF_DBG_ERR | MIF_DBG_WARN | MIF_DBG_PRINT3;



/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/



/*****************************************************************************/
/*	EXTERNAL REFERENCES						     */
/*****************************************************************************/

/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/
	
	
//Open character device
SINT32 mpcapp_open_character_device()
{	
	
	fd = open("./mpcdrv", O_RDWR);
	if (fd < 0)
	{
		perror("user");
		printf("The device ./mpcdrv does not exist. It means either the module is not loaded \n or the character device is not created. Do in order: \ninsmod -f mpcdrv.o, mknod ./mpcdrv c 10 minor_number. The minor is taken from /proc/misc.\n");
		return (STATUS_ERR);
	}
    
	
return (STATUS_OK);	
}

/*---------------------------------------------------------------------------*
Function:	get user integer input parameter
Parameters:	text: text to be printed as prompt
		def: default value
Return:		scanned value
*----------------------------------------------------------------------------*/

int user_par(char text[], int def)
{
	char in_line[60];		       /* input string from keyboard */
	int sc;						    /* scanned value */

	if((def & CHAR_IDENT_MASK) == CHAR_IDENT) {
		printd("%s (RET=%c): ", text ? text : "",
			def & ~CHAR_IDENT_MASK);
	}
	else {
		//printd("%s (RET=%d): ", text ? text : "", def);
	      printd("%s  ", text ? text : "", def);
	}

	GET_STDIN(in_line, 59);

	/* return key */
	if(in_line[0] == RETURN_KEY)
		sc = def;

	/* non digit key - use character identifier */
	else if((in_line[0] < '0') || (in_line[0] > '9'))
		sc = CHAR_IDENT | (int)in_line[0];

	/* hex digit */
	else if((in_line[0] == '0') && ((in_line[1] == 'x') || (in_line[1] == 'X')))
		sscanf(&in_line[2], "%x", &sc);

	/* decimal digit */
	else
		sc = atoi(in_line);

	return(sc);
}

/*---------------------------------------------------------------------------*
Function:	set debug level
Parameters:	dbg: debig level
Return:		current debig level setting
*----------------------------------------------------------------------------*/

#if 0

unsigned long CCApp_Dbg(unsigned long dbg)
{
	/* get current debug state */
	if(dbg & MIF_DBG_GET) {
		return(ccapp_dbg);
	}

	/* set current debug state if not only 'SHOW' requested */
	else if(dbg != MIF_DBG_SHOW) {
		ccapp_dbg = (dbg & ~MIF_DBG_SHOW);
	}

	/* show debug settings requested */
	if(dbg & MIF_DBG_SHOW) {
		printd("\nCCAPP debugs level codes:\n");
		printd("  ERR debugs      : 0x%08x (%s)\n", MIF_DBG_ERR, (ccapp_dbg & MIF_DBG_ERR) ? "enabled" : "disabled");
		printd("  WARN debugs     : 0x%08x (%s)\n", MIF_DBG_WARN, (ccapp_dbg & MIF_DBG_WARN) ? "enabled" : "disabled");
		printd("  INFO debugs     : 0x%08x (%s)\n", MIF_DBG_INFO, (ccapp_dbg & MIF_DBG_INFO) ? "enabled" : "disabled");
		printd("  DETAIL debugs   : 0x%08x (%s)\n", MIF_DBG_DETAIL, (ccapp_dbg & MIF_DBG_DETAIL) ? "enabled" : "disabled");
		printd("  DATA debugs     : 0x%08x (%s)\n", MIF_DBG_DATA, (ccapp_dbg & MIF_DBG_DATA) ? "enabled" : "disabled");
		printd("  PROMPT debugs   : 0x%08x (%s)\n", MIF_DBG_PRINT1, (ccapp_dbg & MIF_DBG_PRINT1) ? "enabled" : "disabled");
		printd("  L3 debugs       : 0x%08x (%s)\n", MIF_DBG_PRINT2, (ccapp_dbg & MIF_DBG_PRINT2) ? "enabled" : "disabled");
		printd("  Msg Type debugs : 0x%08x (%s)\n", MIF_DBG_PRINT3, (ccapp_dbg & MIF_DBG_PRINT3) ? "enabled" : "disabled");
		printd("  current setting : 0x%08lx\n", ccapp_dbg);
	}

	return(ccapp_dbg);
}
#endif


/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/





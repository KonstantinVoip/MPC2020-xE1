
/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcapp_menu_localbus.c
*
* Description : This module implements P2020 API debug menu for localbus functions.
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
* $Date: 2012/10/11 16:51:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcapp_menu_localbus.c $
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


/*****************************************************************************/
/*	External Header						     */
/*****************************************************************************/
					       /* own header */

#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcP2020drvreg.x"

#include <mpcapp_menu.h>


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


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/


/*****************************************************************************/
/*	EXTERNAL REFERENCES						     */
/*****************************************************************************/

/*****************************************************************************/
/*	PUBLIC FUNCTION DEFINITIONS					     */
/*****************************************************************************/

////////////////////////////////////Local BUS Menu Header//////////////	
struct p2020menu localbusmenu[]=
{
  {
  /*int item*/              0,
  /*char *command*/    "show HW configuration",
  /*char *description*/"  Local Bus",
  /*int  func_parameters*/" [-]",
  /*SINT32 (*func_pointer)(int ,int )*/elbcshow,
  /*type_in */            NO_INPUT_PARAMETR , 
  /*parent_menu*/         NO_TYPE
  },
  ///////////////////////////////End Items of current Menu//////////////////////
  {0xffff,"NULL","NULL","NULL",0,NO_INPUT_PARAMETR ,NO_TYPE }
};	

	
/**************************************************************************************************
Syntax:      	    int elbc_menu(void)
Remarks:			create elbc_menu function. 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
int elbc_menu(void)
{
char*  localbusmenuHeader="Enhanced Local Bus menu";     
int exit_to_parent_menu=0;	 
    //printf ("++++++++++elbc_menu++++++++\n"); 
	 
     while(!exit_to_parent_menu) 
	 {mpc_build_menu(localbusmenu,localbusmenuHeader,&exit_to_parent_menu);}

return 1;
}	
	

/*---------------------------------------------------------------------------*
Function:	set debug level
Parameters:	dbg: debig level
Return:		current debig level setting
*----------------------------------------------------------------------------*/



/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/





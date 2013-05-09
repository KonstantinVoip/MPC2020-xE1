
/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcapp_menu_test.c
*
* Description : This module implements p2020 test_menu_API debug interface.
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
* --------- $Log: mpcapp_menu_test.c $
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
struct p2020menu mpcDebugMenuItems[]=
{
   {
  /*int item*/              0,
  /*char *command*/    "pvr(SPR 287)",
  /*char *description*/"   read e500v2 reg",
  /*int  func_parameters*/" [-]",
  /*SINT32 (*func_pointer)(int ,int )*/e500v2core_getpvr,
  /*type_in */            NO_TYPE , 
  /*parent_menu*/         NO_INPUT_PARAMETR
   },
  
  {
  /*int item*/              1,
  /*char *command*/    "reg read",
  /*char *description*/"       regoffset",
  /*int  func_parameters*/" [0xhex]",
  /*SINT32 (*func_pointer)(int ,int )*/regP2020read,
  /*type_in */            HEX_TYPE , 
  /*parent_menu*/         ONE_INPUT_PARAMETR
  },
 
  {
  /*int item*/              2,
  /*char *command*/        "reg write",
  /*char *description*/    "regoffset|data",
  /*int  func_parameters*/"[0xhex][0xhex]",
  /*SINT32 (*func_pointer)(int ,int )*/regP2020write,
  /*type_in */            HEX_TYPE , 
  /*parent_menu*/         TWO_INPUT_PARAMETR
  },
 
  {
  /*int item*/              3,
  /*char *command*/    "rfk",
  /*char *description*/"     read buf from kernel",
  /*int  func_parameters*/"      [0xhex]",
  /*SINT32 (*func_pointer)(int ,int )*/get_data_from_kerenl,
  /*type_in */            HEX_TYPE , 
  /*parent_menu*/         ONE_INPUT_PARAMETR
  },


  {
  /*int item*/              4,
  /*char *command*/    "wfk",
  /*char *description*/"    write buf to kernel",
  /*int  func_parameters*/"      [0xhex]",
  /*SINT32 (*func_pointer)(int ,int )*/set_data_to_kernel,
  /*type_in */            HEX_TYPE , 
  /*parent_menu*/         ONE_INPUT_PARAMETR
  }, 

 ////////////////////////End Current MENU Item////////////////////////
  {0xffff,"NULL","NULL","NULL",0,NO_INPUT_PARAMETR ,NO_TYPE }

};	

/**************************************************************************************************
Syntax:      	    int p2020_test_menu(void)

Remarks:			create p2020_test_menu function. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
int p2020_test_menu(void)
{
int exit_to_parent_menu=0;
//printf ("++++++++++p2020_test_menu++++++++\n");
char*  mpcmenuHeader="P2020 Test Menu";
/////////////////////Menu Value///////////////
     while(!exit_to_parent_menu) 
	 {mpc_build_menu(mpcDebugMenuItems,mpcmenuHeader,&exit_to_parent_menu);}

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





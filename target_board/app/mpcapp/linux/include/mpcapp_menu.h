/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcapp_menu.h
*
* Description : This module implements P2020 router debuging menu definition.
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
* $Date: 2012/10/11 16:45:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcapp_menu.h $
* --------- Initial revision
******************************************************************************/
/*-----------------------------------------------------------------------------
GENERAL NOTES


-----------------------------------------------------------------------------*/
#ifndef MPCAPP_MENU_H
#define MPCAPP_MENU_H

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


//
#define   NO_INPUT_PARAMETR      0
#define   ONE_INPUT_PARAMETR     1
#define   TWO_INPUT_PARAMETR     2
#define   THREE_INPUT_PARAMETR   3
#define   FOUR_INPUT_PARAMETR    4
//
#define   NO_TYPE                0
#define   HEX_TYPE               1 
#define   CHAR_TYPE              2
#define   DECIMAL_TYPE           3




/*
 * debug related
 */

/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/
//int debug_menu;
struct p2020menu
{
 int  item;
 char *command;
 char *description;
 char *func_parameters;
 SINT32 (*func_pointer)(int,... );    //+
 int  type_in;//type of input fuction parameters
 int  parent_menu;  //Pointer to Parent Menu
}mpcDebugMenuItems[],localbusmenu[];


/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/
//Create BASE MAIN MENU for P2020MPC router
SINT32 mpcapp_create_MainMenu(void);

//build menu function
SINT32 mpc_build_menu(struct p2020menu mpcDebugMenuItems[],char *menuname,int *exit_to_parent);

//Create Current Menu finction 
int p2020_test_menu(void);
int elbc_menu(void);


//support Menu function :stop string output 
SINT32 stop_output_screen();





/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#endif	/* MPCAPP_MENU_H */


























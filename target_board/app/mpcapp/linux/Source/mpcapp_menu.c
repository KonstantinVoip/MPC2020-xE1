
/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcapp_menu.c
*
* Description : This module implements P2020 router debuging menu procedures releasiations.
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
* --------- $Log: mpcapp_menu.c $
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

/*****************************************************************************/
/*	External Header						     */
/*****************************************************************************/
					       /* own header */

#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcUniDef.h"
#include "/mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/INC/mpcP2020drvreg.x"
#include  <mpcapp_user.h>					     /* own header */
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
/**************************************************************************************************
Syntax:      	SINT32 stop_output_screen()
Remarks:	    Stop output screen string
  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 stop_output_screen()
{
char ch;
    printf("next[.]");
	do{
	  ch=getchar();
	  }while (ch!= '.');
return 1;
}

/**************************************************************************************************
Syntax:      	SINT32 mpc_build_menu(struct p2020menu mpcDebugMenuItems[],char *menuname,int *exit_to_parent)
Remarks:	    This Function create menu p2020 router. 
  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 mpc_build_menu(struct p2020menu mpcDebugMenuItems[],char *menuname,int *exit_to_parent)
{
int par1,par2,req; 
int litem=0; 
int a=1;
static int max_menu_items=50;
req =0;

printd("--------------------------[%s]------------------------\n",menuname);
printd(". -next output . q - parent menu   ? - Refresh                       \n");
printd("Item Command           Parameters                       Description       \n");            
printd("--------------------------------------------------------------------\n"); 

while(a<=max_menu_items)
{
      
   if(mpcDebugMenuItems[litem].item==0xffff)
   {a=max_menu_items;
   printf ("--------------------------------End Menu-----------------------------\n\r");
   }
   else{printf(" %d. %s        %s                     %s\n\r",mpcDebugMenuItems[litem].item,mpcDebugMenuItems[litem].command,mpcDebugMenuItems[litem].func_parameters,mpcDebugMenuItems[litem].description);
   litem++;}
a++;
}

//If Debugging is on =1 
#ifdef DEBUGGING 
//req = user_par("------>", 0);
#endif


printf("req= %d\n\r",req);

if(req==16777329)
{
//
printf("exit_to_parent\n\r");
*exit_to_parent=1;
return 1;
}
else
{
  if(req>litem){return 1;}
   //
	switch(mpcDebugMenuItems[req].parent_menu)
	 {
	 case 0:
	 par1=0;
	 printf("no input func parameters\n\r");
	 mpcDebugMenuItems[req].func_pointer(par1);
	 stop_output_screen();
	 break;
	 
	 case 1: 
	 printf("one input func parametrs\n\r");
	 printf("par1=0x");
	 scanf("%x",&par1); 
	 mpcDebugMenuItems[req].func_pointer(par1); 
	 stop_output_screen();
	 break;
	 
	 case 2:
	 printf("two input func parameters\n\r");
	 printf("par1=0x");
	 scanf("%x",&par1);
 	 
     printf("\n");
	 
	 printf("par2=0x");
	 scanf("%x",&par2);
 
	 
	 mpcDebugMenuItems[req].func_pointer(par1,par2); 
  	 stop_output_screen();
	 break;
 	 
	 default:
	 printf("!!!!!!!!!!!!!!!Warning no parameters\n\r!!!!!!!!!!!!");
	 break;
	 }

}
//req = user_par("------>", 0);

return 1;
}
/**************************************************************************************************
Syntax:      	SINT32 mpcapp_create_MainMenu(void)
Remarks:	    This Function create Main menu for p2020 router. 
  
Return Value:	Returns 1 on success and negative value on failure.
 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 mpcapp_create_MainMenu(void)
{
	int status = 1;
	int done = 0;
	int req=1;
    
	
    //bulid menu    
	while(!done) {
        printd("---------------------------------[Main P2020 Debug Menu]-----------------------\n");
        printd("^ - Main Menu  q - Exit application  ? - Refresh                               \n");
        printd("-------------------------------------------------------------------------------\n"); 
    	printd("  1.  P2020Test Menu...                        \n");
		printd("  2.  Enhanced Local bus...               \n");
		printd("  3.  eTSEC...                            \n");
		printd("  4.  DMA...                              \n");
		printd("  5.  Test Application...                 \n");
		printd("  6.  Test Kernel module...               \n");
        printd("  7.  Settings...                         \n"); 
	   
	    
	    
#ifdef DEBUGGING	   
 //req = user_par("->", 0);
#endif        
		
		switch(req) {
			case 1:
                //P2020 application core base application select				
				p2020_test_menu();
				break;

			case 2:
                //P2020 Enhanced Local bus application select Menu
				elbc_menu();
				break;

			case 3:
                //P2020 eTsec application select 
				//CCApp_IsdnPrintCfg();
				break;

			case 4:
                //P2020 DMA application select
				//CCApp_IsdnPrintReq();
				break;

			case 5:
                 //End use Application
				//CCApp_IsdnPrintInd();
				break;
			
			case 6:
                 //Test Kernel Application 
				//CCApp_IsdnPrintInd();
				break;
			
			case 7:
			     //CCApp_IsdnPrintInd();
			     break;
			
			case (CHAR_IDENT | 'q'):
				done = 1;
				break;

			default:
				printd("ccapp_main_help: unknown request %d\n", req);
				status=-1;
				break;
		}
	}//end while loop

	return(status);
}
/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/





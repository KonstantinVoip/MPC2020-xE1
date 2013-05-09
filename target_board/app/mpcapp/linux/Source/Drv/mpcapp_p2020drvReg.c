/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************


******************************************************************************
*
* Module      : mpcapp_p2020drvReg.c
*
* Description : This Low Level driver function for Read and write P2020 Registers.
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
* $Date: 2012/10/12 14:05:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcapp_p2020drvReg.c $
* --------- Initial revision
******************************************************************************/
/*-----------------------------------------------------------------------------
GENERAL NOTES


-----------------------------------------------------------------------------*/
/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/
#include <stdio.h>						 /* snprintf */
#include <stdlib.h>					     /* malloc, free */
#include <string.h>					     /* memset, strncmp */
#include <sys/ioctl.h>

/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include <mpcapp_user.h>		      /*user support functions*/
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
Syntax:      	    SINT32 regP2020read (int addr,int data)

Remarks:			read p2020 register value. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32 regP2020read (int addr,int data)
{
//printf("regP2020read_function=%d,%d\n\r",addr,data);  
int p2020reg_offset=addr;			   
unsigned int kernel_to_user[5];
int ret;

    printf("p2020reg_offset =0x%x\n\r",p2020reg_offset);
	//Select P2020 cur Register offset
	ret = ioctl(fd, IOC_MOD_IOREMAP,p2020reg_offset);
	if (ret < 0){perror("user: IOC_GET_DATA");return ret;}
	//printf("user_IN: cmd =0x%x,arg =0x%x\n\r", IOC_GET_P2020REG,in_p2020_reg);
								
	ret = ioctl(fd, IOC_GET_P2020REG,kernel_to_user);
	if (ret < 0){perror("user: IOC_GET_P2020REG");return ret;}
	            
	printf("user_space: get_data:0x%x\n", kernel_to_user[0]);


return 1;
}
   
/**************************************************************************************************
Syntax:      	    SINT32 regP2020write(UINT32 addr,unsigned char *data)
                    
Remarks:			write value to p2020 registers. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32 regP2020write(UINT32 addr,UINT32 data)
{
int ret;
UINT32 p2020reg_offset=addr;
UINT32 p2020reg_setvalue=data;

		//Select P2020 cur Register offset
		ret = ioctl(fd, IOC_MOD_IOREMAP,p2020reg_offset);
		if (ret < 0){perror("user: IOC_GET_DATA");return ret;}
               
	    ret = ioctl(fd, IOC_SET_P2020REG, p2020reg_setvalue);
	    if (ret < 0){perror("user: IOC_SET_P2020REG");return ret;}
	    //printf("user: set_reg: 0x%x\n", pvr);	
	    printf("set_user:set_val_P2020=0x%x \n\r",p2020reg_setvalue);


return 1;  
}


/**************************************************************************************************
Syntax:      	    SINT32  e500v2core_getpvr(void)
                    
Remarks:			get value processor version register PVR 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32  e500v2core_getpvr(void)
{
int ret;
int pvr;
        ret = ioctl(fd, IOC_GET_PVR,&pvr);
		if (ret < 0){perror("user:get_pvr");return ret;}
        
		printf("user :get_pvr: 0x%x\n",pvr);

return 1;
}
/**************************************************************************************************
Syntax:      	    SINT32  get_data_from_kerenl(UINT32 data_from_kernel)
                    
Remarks:			get data from kernel space. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/ 
SINT32  get_data_from_kerenl(UINT32 data_from_kernel)
{
int ret;
unsigned char kernel_to_user[5];


         ret = ioctl(fd, IOC_GET_DATA,kernel_to_user);
		 if (ret < 0){perror("user:IOC_GET_DATA");return ret;}  

         printf("user_space: get_data: %s\n",kernel_to_user);

		 
return 1;
}


/**************************************************************************************************
Syntax:      	    SINT32  set_data_to_kernel(UINT32 data_to_kernel)
                    
Remarks:			set data to kernel space. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32  set_data_to_kernel(UINT32 data_to_kernel)
{
int ret;
unsigned char s[5]={"eeeee"};

         ret = ioctl(fd, IOC_SET_DATA,s);
		 if (ret < 0){perror("user:IOC_SET_DATA");return ret;}  

//printf("set_data_to_kernel\n");
return 1;
} 


/**************************************************************************************************
Syntax:      	    SINT32 mpcp2020Init_kernel()
                    
Remarks:			Initialization Kernel software and hardware p2020 . 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32 mpcp2020Init_kernel()
{

int ret;
int data=0;

	        ret = ioctl(fd, IOC_INIT_P2020,&data);
			if (ret < 0){perror("user:mpcp2020Init_kernel");return ret;}
	        
return 1;	
}






/**************************************************************************************************
Syntax:      	   SINT32 mpcp2020startlbcoffset (UINT32 offset) 
                    
Remarks:		   Set start offset on Local bus memory 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32 mpcp2020startlbcoffset (UINT32 offset) 
{
int ret;
	   ret = ioctl(fd, IOC_LBCOFFSET_P2020,offset);
	   if (ret < 0){perror("user:IOC_LBCOFFSET_P2020");return ret;}  
       
			
	//printf("set_data_to_kernel\n");	
return 1;	
}




/**************************************************************************************************
Syntax:      	   SINT32 mpcp2020readlbc (UINT32 len,unsigned char *out
                    
Remarks:		   Read data on local bus on current size

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32 mpcp2020readlbc (UINT32 len,unsigned char *outbuf)
{
	int ret;
    ret = ioctl(fd, IOC_LBCDATASIZE_P2020,len);
	if (ret < 0){perror("user:IOC_LBCDATASIZE_P2020");return ret;}  
	
	
    
    ret = ioctl(fd, IOC_LBCDATABUF_P2020,outbuf);
	if (ret < 0){perror("user:IOC_LBCDATASIZE_P2020");return ret;}
	
	
	
return 1;	
}




/**************************************************************************************************
Syntax:      	   SINT32 mpcp2020writelbc   (UINT32 len,const unsigned char *inbuf)
                    
Remarks:		   Write data to local bus on current size. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
SINT32 mpcp2020writelbc   (UINT32 len,const unsigned char *inbuf)
{
	
	

	
	
return 1;	
}


/**************************************************************************************************
Syntax:      	   SINT32  elbcshow(void)
                    
Remarks:		   Show local bus configurations. 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/

SINT32  elbcshow(void)
{
printf ("Show Local Bus Menu  OK!\n\r");
return 1;
}








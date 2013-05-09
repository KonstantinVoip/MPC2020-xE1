/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcP2020drvreg.x
*
* Description : External Defenition read/write Register for Freescale P2020 processor.
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
* $Date: 2012/10/08 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcP2020drvreg.x $
* --------- Initial revision
******************************************************************************/

#ifndef _MPCP2020drvreg_X_
#define _MPCP2020drvreg_X_
//Ioctl kernel space <-> User Space

#define DATA_MAX 5
#define MOD_MAGIC 'M'
#define MOD_SET_DATA 1
#define MOD_GET_DATA 2
#define MOD_GET_PVR 3
#define MOD_GET_MSR 4
#define MOD_GET_LCRR 5

#define MOD_SET_P2020REG  10
#define MOD_GET_P2020REG  20 
#define MOD_IOREMAP       30


//P2020 Kernel Initialization function add 26.10.2012
#define MOD_INIT_P2020    100
#define IOC_INIT_P2020 _IOW(MOD_MAGIC, MOD_INIT_P2020, unsigned int) 
/////////////////////////////////////////////////////////////////////

//P2020 LocalBus Managment function add 26.10.2012

//Set start offset on Local Bus
#define MOD_LBCOFFSET_P2020   200
#define IOC_LBCOFFSET_P2020 _IOW(MOD_MAGIC, MOD_LBCOFFSET_P2020, UINT32)

//Set Size of data read/write on local bus
#define MOD_LBCDATASIZE_P2020 300
#define IOC_LBCDATASIZE_P2020 _IOW(MOD_MAGIC, MOD_LBCDATASIZE_P2020, UINT32)

//Get/Set Local bus buffer 
#define MOD_LBCDATABUF_P2020  400
#define IOC_LBCDATABUF_P2020 _IOW(MOD_MAGIC, MOD_LBCDATABUF_P2020, unsigned char)

//Add function Ioremap 
#define IOC_MOD_IOREMAP  _IOW(MOD_MAGIC, MOD_IOREMAP, unsigned int) 
#define IOC_SET_P2020REG _IOW(MOD_MAGIC, MOD_SET_P2020REG, unsigned int) 
#define IOC_GET_P2020REG _IOW(MOD_MAGIC, MOD_GET_P2020REG, unsigned int) 
#define IOC_SET_DATA _IOW(MOD_MAGIC, MOD_SET_DATA, unsigned int) 
#define IOC_GET_DATA _IOW(MOD_MAGIC, MOD_GET_DATA, unsigned int) 
#define IOC_GET_PVR _IOW(MOD_MAGIC, MOD_GET_PVR, unsigned int) 
#define IOC_GET_MSR _IOW(MOD_MAGIC, MOD_GET_MSR, unsigned int) 
#define IOC_GET_LCRR _IOW(MOD_MAGIC, MOD_GET_LCRR, unsigned int)




/////////////////////////Initialization P2020 fuction//////////////////
extern SINT32 mpcp2020Init_kernel(); 

////////////////////////P2020 LocalBusManagment function
extern SINT32 mpcp2020startlbcoffset (UINT32 offset); 
extern SINT32 mpcp2020readlbc        (UINT32 len,unsigned char *outbuf);
extern SINT32 mpcp2020writelbc       (UINT32 len,const unsigned char *inbuf);

///////////////////////P2020 register managment//////////////
extern SINT32  regP2020read (int addr,int data); 
extern SINT32  regP2020write(UINT32 addr,UINT32 data); 
 


 
/////////////////////////////Miscelanuos fuction/////////////// 
extern SINT32  e500v2core_getpvr(void); 
extern SINT32  get_data_from_kerenl(UINT32 data_from_kernel);
extern SINT32  set_data_to_kernel(UINT32 data_to_kernel); 
 
///Enhanced Local bus show configuration
extern SINT32  elbcshow(void);


						 
#endif /* _MPCP2020drvreg_X_*/
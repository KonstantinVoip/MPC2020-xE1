/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrv_gianfar.c
*             
* Description : Defenition of low level fubction.
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
* $Date: 2013/06/10 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrv_gianfar.c $
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

/*
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/inetdevice.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/crc32.h>
#include <linux/mii.h>
#include <linux/phy.h>
#include <linux/phy_fixed.h>
#include <linux/of.h>
#include <net/xfrm.h>
#include <sysdev/fsl_soc.h>
#ifdef CONFIG_GFAR_SW_PKT_STEERING
#include <asm/fsl_msg.h>
#endif

#ifdef CONFIG_NET_GIANFAR_FP
#include <linux/if_arp.h>
#include <linux/netdevice.h>
#include <net/route.h>
#include <net/ip.h>
#include <linux/jhash.h>
#endif
*/

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>





//#include "gianfar.h"
//#include "fsl_pq_mdio.h"
#include "mpcdrv_gianfar.h"




static struct net_device *tsec_get_device_by_name(const char *ifname);
static int tsec_get_device(const char *ifname);








/**************************************************************************************************
Syntax:      	    void InitIp_Ethernet()
Parameters:     	
Remarks:			Initialize ethernet tsec1,tsec2,tsec3 function

Return Value:	    

***************************************************************************************************/
void InitIp_Ethernet()
{
 UINT16 status;
//Имена наших device "eth0"<->Tsec 1 ,"eth1"<->Tsec 2,"eth2"<->Tsec 3
  const char *ifname="eth0";
//const char *ifname="eth1"; 
//const char *ifname="eth2"; 
   status=tsec_get_device(ifname);	
   	
 //Get and open ethernet device
 
 
	
}


/**************************************************************************************************
Syntax:      	  static struct net_device *pktgen_dev_get_by_name(const char *ifname)

Remarks:		  Важная функция позволяет нам получить структуру struct net_device по имени устройства,"eth0","eth1","eth2"

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static struct net_device *tsec_get_device_by_name(const char *ifname)
{
	char b[IFNAMSIZ+5];
	int i = 0;	
	for (i = 0; ifname[i] != '@'; i++) 
	{
		if (i == IFNAMSIZ)
			break;

		b[i] = ifname[i];
	}
	b[i] = 0;

	return dev_get_by_name(&init_net, b);
}


/**************************************************************************************************
Syntax:      	    static int tsec_get_device(const char *ifname)
Parameters:     	
Remarks:			Get p2020 tsec ethernt device 

Return Value:	    

***************************************************************************************************/
static int tsec_get_device(const char *ifname)
{
	struct net_device *odev;
    UINT16 status;
	
	odev = tsec_get_device_by_name(ifname);
	if (!odev)
	{
		printk(KERN_ERR "mpcdv: no such netdevice: \"%s\"\n", ifname);
		return STATUS_ERR;
	}


	if (odev->type != ARPHRD_ETHER)
	{
		printk(KERN_ERR "mpcdrv: not an ethernet device: \"%s\"\n", ifname);
		status = STATUS_ERR;
	} 
	
	else if (!netif_running(odev)) 
	{
		printk(KERN_ERR "mpcdrv: device is down: \"%s\"\n", ifname);
		status = STATUS_ERR;
	} 
	else 
	{
		
		printk("mpcdrv:Get the Tsec device is name %s,alias %s\n\r",odev->name,odev->ifalias);
		
		return STATUS_OK;
	}

	dev_put(odev);
	return status;


}




















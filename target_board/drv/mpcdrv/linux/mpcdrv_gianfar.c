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


#include "gianfar.h"
//#include "fsl_pq_mdio.h"
#include "mpcdrv_gianfar.h"



static void p2020_get_from_tdmdir_and_put_to_ethernet(struct net_device *dev);





/*
static u8 eth_preambula_mas[8]=
{0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x5d};
*/


static struct ethdata_packet1
{ 
	//u16 data[759] ; //packet test buffer massive;
	u16 *data;
	u16 length;
    u16 state;
};


static struct ethdata_packet1 transmit_tsec_packet;


/**************************************************************************************************
Syntax:      	    void InitIp_Ethernet()
Parameters:     	
Remarks:			Initialize ethernet tsec1,tsec2,tsec3 and virtual tsec function

Return Value:	    

***************************************************************************************************/
void InitIp_Ethernet()
{printk("+++++++InitIp_Ethernet()+++++++\n\r");}


/**************************************************************************************************
Syntax:      	    static inline u16* get_tdmdir_packet_data()
Parameters:     	
Remarks:			get data from input local bus tdm direction 

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
static inline u16* get_tdmdir_packet_data()
{
	return transmit_tsec_packet.data;
}

/**************************************************************************************************
Syntax:      	    static inline u16 get_tdmdir_packet_length()
Parameters:     	
Remarks:			get length from input local bus tdm direction

Return Value:	    0  =>  Success  ,-EINVAL => Failure

***************************************************************************************************/
static inline u16 get_tdmdir_packet_length()
{
	return transmit_tsec_packet.length;
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

////////////////////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************/
/*                        Test1                                       */
/**********************************************************************/

//#if 0
void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len)
{
	//Real and Virtual Ethernet devices
	//eth0,eth1,eth2,eth3,eth4,eth5,eth6.,eth7,eth8;
	const char *ifname3="eth2";
	//printk("++++++p2020_get_recieve_virttsec_packet_buf+++\n\r");
	
	transmit_tsec_packet.data  = buf;
	transmit_tsec_packet.length= len;
	
    /*	
    memcpy(recieve_tsec_packet.data ,skb->mac_header,(uint)skb->mac_len+(uint)skb->len);
    recieve_tsec_packet.length =  (uint)skb->mac_len+(uint)skb->len;
	*/
	//printk("p2020_get:Get the Tsec device is name %s,alias %s\n\r",dev->name,dev->ifalias);
	
	//buf ++ ok;
	//printk("virt_TSEC_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",buf[0],buf[1],buf[2],buf[3]);
	  //printk("virt_TSEC_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",l_data[0],l_data[1],l_data[2],l_data[3]);
	p2020_get_from_tdmdir_and_put_to_ethernet(tsec_get_device_by_name(ifname3));
}

//#endif


/**********************************************************************/
/*                    Test2                                            */
/**********************************************************************/
void p2020_get_from_tdmdir_and_put_to_ethernet(struct net_device *dev)
{
	struct gfar_private *priv = netdev_priv(dev);
	struct gfar_priv_rx_q *rx_queue = priv->rx_queue[priv->num_rx_queues-1];
	struct rxbd8 *bdp = rx_queue->cur_rx;
	struct sk_buff *skb=NULL;
	__u8 *eth;
	__be16 protocol = htons(ETH_P_IP);
	netdev_tx_t (*xmit)(struct sk_buff *, struct net_device *)= dev->netdev_ops->ndo_start_xmit;
	unsigned char *data;
    u16 len;
	u16 ret;
	
	
	len =  get_tdmdir_packet_length();      
	len=len*2;
	//printk("+p2020_get_from_tdmdir_and_put_to_ethernet|len=%d \n\r",len);
	
	
	//printk("???????????????????virt_device_len =%d\n\r",len);
	data = get_tdmdir_packet_data();      
	//Char Buffer only
	   
	 // printk("+put_to_eth_rfirst|0x%02x|0x%02x|0x%02x|0x%02x+\n\r",data[0],data[1],data[2],data[3]);
	 // printk("+put_to_eth_rlast |0x%02x|0x%02x|0x%02x|0x%02x+\n\r",data[len-3],data[len-2],data[len-1],data[len]);
	
	
	//printk("virt_TSEC_|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x\n\r",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
	  
	
	skb = netdev_alloc_skb(dev, len);
	  if (!skb) 
	  {
		dev->stats.rx_dropped++;
		priv->extra_stats.rx_skbmissing++;
		printk("no cannot allocate Virtual Ethernt SKB buffer \n\r");
		//return;
	  }
	 

	  
	    //printk("+++++++++++++++++SKB _OK+++++++++++++++\n\r"); 
	   //copy received packet to skb buffer 
	    memcpy(skb->data, data, len);
	    //  Reserve for ethernet and IP header 
		eth = (__u8 *) skb_push(skb, 14);
		memcpy(eth, data, 12);
	     *(__be16 *) & eth[12] = protocol;		
	    //printk("+++++++++++++++++MEMCPY_OK+++++++++++++++\n\r");
		skb_put(skb, len);
		//printk("+++++++++++++++++SKB_PUT_OK+++++++++++++++\n\r");
	    // Tell the skb what kind of packet this is 
		skb->protocol = eth_type_trans(skb, dev);
		//printk("+++++++++++++++++skb->protocol_OK+++++++++++++++\n\r");
		//////////////////Create and Fill packet to Send///////////////
	    ret = (*xmit)(skb, dev);
		
	    switch (ret) 
		{
		 case NETDEV_TX_OK:
		 printk("mpcdrv:->>>>>>>>>>>>>>>>>>>>NETDEV_Transmit_OK\n");
		 break;
	
	
		 case NETDEV_TX_BUSY:
		 printk("++++++mpcdrv:NETDEV_Transmit_bUSY+++\n");
		 break;
	
		 default:
		 printk("+++++++++++mpcdrv:Unriable +++++++++\n");
		 break;
	     }
		
		if (NET_RX_DROP == ret) 
		{priv->extra_stats.kernel_dropped++;} 
		else {
		// Increment the number of packets 
		dev->stats.rx_packets++;dev->stats.rx_bytes += len;}
	   
}
















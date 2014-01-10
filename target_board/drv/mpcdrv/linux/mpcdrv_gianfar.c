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



//KY-S Destination MAC addrress
//static UINT16 my_kus_mac_addr [6]={0x0025,0x0100,0x112D};
static UINT16 my_kys_mac1_addr[12]={0x0025,0x0100,0x1f05,0x01ff,0xffff,0xff00};


struct net_device *tsec0_dev,*tsec1_dev,*tsec2_dev;

static void p2020_get_from_tdmdir_and_put_to_ethernet(struct net_device *dev);
static struct net_device *tsec_get_device_by_name(const char *ifname);
static void p2020_tsec_set_hardware_reg_configuration(struct net_device *dev);


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
{
const char *ifname0="eth0";
const char *ifname1="eth1";
const char *ifname2="eth2";
int  init_status_tsec2 =0;
int  init_status_tsec1 =0;


//tsec 0 ,tsec1 ,tsec2 ethernet controller
printk("+++++++InitIp_Ethernet()_and_get_ethernet_device_structure+++++++\n\r");
//Clear Hlam on structure for reinitializate
memset(&tsec0_dev, 0x0000, sizeof(tsec0_dev));  
memset(&tsec1_dev, 0x0000, sizeof(tsec1_dev));  
memset(&tsec2_dev, 0x0000, sizeof(tsec2_dev));  

tsec0_dev=tsec_get_device_by_name(ifname0);
if(!tsec0_dev){printk("No Device Found %s\n\r",ifname0);}

tsec1_dev=tsec_get_device_by_name(ifname1);
if(!tsec1_dev){printk("No Device Found %s\n\r",ifname1);}

tsec2_dev=tsec_get_device_by_name(ifname2);
if(!tsec2_dev){printk("No Device Found %s\n\r",ifname2);}


//Set multicast configuration for  1 and 2 device
p2020_tsec_set_hardware_reg_configuration(tsec0_dev);
p2020_tsec_set_hardware_reg_configuration(tsec1_dev);
p2020_tsec_set_hardware_reg_configuration(tsec2_dev);


//Start GIANFAR DRIVER eth1.
//int	(*ndo_init_tsec1)(struct net_device *dev) = tsec1_dev->netdev_ops ->ndo_init;
//init_status_tsec1=(*ndo_init_tsec1)(tsec1_dev);
//printk("+!Start_ETH1_status= %d!+\n\r",init_status_tsec1);
//Start GIANFAR DRIVER eth2.
/*
int	(*ndo_init)(struct net_device *dev) = tsec2_dev->netdev_ops ->ndo_init;
init_status_tsec2=(*ndo_init)(tsec2_dev);
printk("+!Start_ETH2_status= %d!+\n\r",init_status_tsec2);
*/
//p2020_get_from_tdmdir_and_put_to_ethernet(tsec2_dev);



}

/**************************************************************************************************
Syntax:      	    static void p2020_tsec_set_hardware_reg_configuration
Parameters:     	
Remarks:	        set p2020 etsec registers configurations for hardware	
Return Value:	    
****************************************
***************************************************************************************************/
void p2020_tsec_set_hardware_reg_configuration(struct net_device *dev)
{
	struct dev_mc_list *mc_ptr;
	struct gfar_private *priv = netdev_priv(dev);
	struct gfar  *regs = priv->gfargrp[0].regs;
	u32 tempval;
	

	
	// Set RCTRL to PROMISC mode

	printk("++Enable Promis mode for eth2+\n\r");
	tempval = gfar_read(&regs->rctrl);
	tempval |= RCTRL_PROM;
	gfar_write(&regs->rctrl, tempval);
	
	
	
/*
	printk("++Enable Multicast Frame for eth2\n\r++");
	//Set multicast frames incoming packet
	gfar_write(&regs->igaddr0, 0xffffffff);
	gfar_write(&regs->igaddr1, 0xffffffff);
	gfar_write(&regs->igaddr2, 0xffffffff);
	gfar_write(&regs->igaddr3, 0xffffffff);
	gfar_write(&regs->igaddr4, 0xffffffff);
	gfar_write(&regs->igaddr5, 0xffffffff);
	gfar_write(&regs->igaddr6, 0xffffffff);
	gfar_write(&regs->igaddr7, 0xffffffff);
	gfar_write(&regs->gaddr0, 0xffffffff);
	gfar_write(&regs->gaddr1, 0xffffffff);
	gfar_write(&regs->gaddr2, 0xffffffff);
	gfar_write(&regs->gaddr3, 0xffffffff);
	gfar_write(&regs->gaddr4, 0xffffffff);
	gfar_write(&regs->gaddr5, 0xffffffff);
	gfar_write(&regs->gaddr6, 0xffffffff);
	gfar_write(&regs->gaddr7, 0xffffffff);
 */
	
}

/**************************************************************************************************
Syntax:      	    void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12])
Parameters:     	
Remarks:			Set DA address my_kos only Input Packet
Return Value:	    

***************************************************************************************************/
void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12])
{
	// printk("podmena_mac_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",out_mac[0],out_mac[1],out_mac[2],out_mac[3]);
	   printk("+++Revert_MAC+++\n\r");
	  //Подмена MAC заголовков для отправки обратно КY-S;
	  memcpy(out_mac,dst,6);
	  memcpy(out_mac+3,src,6);    
	  printk("podmena_mac_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",out_mac[0],out_mac[1],out_mac[2],out_mac[3]);
	  
	  
}










/**************************************************************************************************
Syntax:      	    p2020_get_recieve_packet_and_setDA_MAC (const u16 *in_buf ,const u16 in_size
Parameters:     	
Remarks:			Set DA address my_kos only Input Packet
Return Value:	    

***************************************************************************************************/
void p2020_get_recieve_packet_and_setDA_MAC (const u16 *in_buf ,const u16 in_size,const u16 *mac_header)
{
	 //printk("+p2020_get_recieve_packet_and_setDA_MAC+\n\r");
	 //printk("virt_TSEC_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",mac_header[0],mac_header[1],mac_header[2],mac_header[3]);
	 
	 //memcpy(in_buf, my_kys_mac1_addr, 6);
	  memcpy(in_buf, mac_header, 12);
	 
	 
     //memcpy(in_buf, mac_header, 36); 
	 
	 //podmena MAC adressa
	 printk("revert_MAC_DA_|0x%04x|0x%04x|0x%04x\n\r",in_buf[0],in_buf[1],in_buf[2]);
	 printk("revert_MAC_SA_|0x%04x|0x%04x|0x%04x\n\r",in_buf[3],in_buf[4],in_buf[5]);
	 
	 
	  //put to buffer 
     transmit_tsec_packet.data  = in_buf;
	 transmit_tsec_packet.length= in_size;
	       
	 //Send Packet to ethernet eTSEC1
	 p2020_get_from_tdmdir_and_put_to_ethernet(tsec1_dev);
	 //Send Packet to ethernet eTSEC2
	 p2020_get_from_tdmdir_and_put_to_ethernet(tsec2_dev);
	//printk("OK\n\r");
	
	
	
}



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




////////////////////////////////////////////////////////////////////////////////////////////////////
/**********************************************************************/
/*                        Test1                                       */
/**********************************************************************/

//#if 0
void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len,u8 tsec_id)
{
	//Real and Virtual Ethernet devices
	//eth0,eth1,eth2,eth3,eth4,eth5,eth6.,eth7,eth8;
	//const char *ifname3="eth2";
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
	//p2020_get_from_tdmdir_and_put_to_ethernet(tsec_get_device_by_name(ifname3));
	
	/*
	if(tsec_id==0)
	{
		p2020_get_from_tdmdir_and_put_to_ethernet(tsec0_dev);
	}*/
	
	
	
	if(tsec_id==1)
	{
	p2020_get_from_tdmdir_and_put_to_ethernet(tsec1_dev);
	}
	
	if(tsec_id==2)
	{	
	p2020_get_from_tdmdir_and_put_to_ethernet(tsec2_dev);
	}
	
    
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

	
//#if 0	
	

	len =  get_tdmdir_packet_length();      
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
		 //printk("mpcdrv:->>>>>>>>>>>>>>>>>>>>NETDEV_Transmit_OK\n");
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
	   
//#endif

}



/**************************************************************************************************
Syntax:      	    void StopIp_Ethernet()
Parameters:     	
Remarks:		    Stop ethernet tsec1,tsec2,tsec3 and virtual tsec function

Return Value:	    
***************************************************************************************************/
void StopIp_Ethernet()
{
	int  stop_status_tsec2 =0;
	int  stop_status_tsec1 =0;

	//Stop GIANFAR DRIVER eth1.
	//int	(*ndo_stop_tsec1)(struct net_device *dev) = tsec1_dev->netdev_ops ->ndo_stop;
	//stop_status_tsec1=(*ndo_stop_tsec1)(tsec1_dev);
	//printk("+!Stop_ETH1_status= %d!+\n\r",stop_status_tsec1);
	//Stop GIANFAR DRIVER eth2.
	int	(*ndo_stop_tsec2)(struct net_device *dev) = tsec2_dev->netdev_ops ->ndo_stop;
	stop_status_tsec2=(*ndo_stop_tsec2)(tsec2_dev);
	printk("+!Stop_ETH2_status= %d!+\n\r",stop_status_tsec2);
	
}










/**************************************************************************************************
Syntax:      	    static int tsec_get_device(const char *ifname)
Parameters:     	
Remarks:			Get p2020 tsec ethernt device 
Return Value:	    Potom Razbrus
***************************************************************************************************/
#if 0

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
#endif



















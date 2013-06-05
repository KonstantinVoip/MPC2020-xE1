/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrveth_ipv4.c
*             
* Description : Defenition ethernet driver for  transmit <->recieve ipv4 packet data on P2020.
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
* $Date: 2012/10/23 10:40:51 $
* $Locker: $
*
* Module's revision history:
* ==========================
* --------- $Log: mpcdrveth_ipv4.c $
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
#include <linux/sys.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <linux/ptrace.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/capability.h>
#include <linux/hrtimer.h>
#include <linux/freezer.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/rtnetlink.h>
#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/udp.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/wait.h>
#include <linux/etherdevice.h>
#include <linux/kthread.h>
#include <net/net_namespace.h>
#include <net/checksum.h>
#include <net/ipv6.h>
#include <net/addrconf.h>



// Add kosta Include for DMA and routing table 


//#include <net/route.h>
//#include <linux/dma-mapping.h>

#ifdef CONFIG_XFRM
#include <net/xfrm.h>
#endif
#include <asm/byteorder.h>
#include <linux/rcupdate.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/timex.h>
#include <linux/uaccess.h>
#include <asm/dma.h>
#include <asm/div64.h>		/* do_div */


#include "mpcdrveth_ipv4.h"
/*External Header*/
/* If lock -- can be removed after some work */
#define   if_lock(t)           spin_lock(&(t->if_lock));
#define   if_unlock(t)           spin_unlock(&(t->if_lock));

#define IP_NAME_SZ 32
#define MAX_MPLS_LABELS 16 /* This is the max label stack depth */
#define MPLS_STACK_BOTTOM htonl(0x00000100)

#define MAX_CFLOWS  65536

#define REMOVE 1
#define FIND   0



/* Thread control flag bits */
#define T_STOP        (1<<0)	/* Stop run */
#define T_RUN         (1<<1)	/* Start run */
#define T_REMDEVALL   (1<<2)	/* Remove all devs */
#define T_REMDEV      (1<<3)	/* Remove one dev */



/* Device flag bits */
#define F_IPSRC_RND   (1<<0)	/* IP-Src Random  */
#define F_IPDST_RND   (1<<1)	/* IP-Dst Random  */
#define F_UDPSRC_RND  (1<<2)	/* UDP-Src Random */
#define F_UDPDST_RND  (1<<3)	/* UDP-Dst Random */
#define F_MACSRC_RND  (1<<4)	/* MAC-Src Random */
#define F_MACDST_RND  (1<<5)	/* MAC-Dst Random */
#define F_TXSIZE_RND  (1<<6)	/* Transmit size is random */
#define F_IPV6        (1<<7)	/* Interface in IPV6 Mode */
#define F_MPLS_RND    (1<<8)	/* Random MPLS labels */
#define F_VID_RND     (1<<9)	/* Random VLAN ID */
#define F_SVID_RND    (1<<10)	/* Random SVLAN ID */
#define F_FLOW_SEQ    (1<<11)	/* Sequential flows */
#define F_IPSEC_ON    (1<<12)	/* ipsec on for flows */
#define F_QUEUE_MAP_RND (1<<13)	/* queue map Random */
#define F_QUEUE_MAP_CPU (1<<14)	/* queue map mirrors smp_processor_id() */



/* Used to help with determining the pkts on receive */
#define PKTGEN_MAGIC 0xbe9be955
#define PG_PROC_DIR "pktgen"
#define PGCTRL	    "pgctrl"




static LIST_HEAD(pktgen_threads);


/* Module parameters, defaults. */
static int pg_count_d __read_mostly = 1000;
static int pg_delay_d __read_mostly;
static int pg_clone_skb_d  __read_mostly;
static int debug  __read_mostly;

static struct proc_dir_entry *pg_proc_dir;


struct pktgen_hdr {
	__be32 pgh_magic;
	__be32 seq_num;
	__be32 tv_sec;
	__be32 tv_usec;
};



//static struct task_struct *tdm_thread_task = NULL;
struct pktgen_thread {
	spinlock_t if_lock;		/* for list of devices */
	struct list_head if_list;	/* All device here */
	struct list_head th_list;
	struct task_struct *tsk;
	char result[512];

	/* Field for thread to receive "posted" events terminate,
	   stop ifs etc. */

	u32 control;
	int cpu;

	wait_queue_head_t queue;
	struct completion start_done;
};
struct pktgen_thread *t;



struct pktgen_dev {
	/*
	 * Try to keep frequent/infrequent used vars. separated.
	 */
	struct proc_dir_entry *entry;	/* proc file */
	struct pktgen_thread *pg_thread;/* the owner */
	struct list_head list;		/* chaining in the thread's run-queue */

	int running;		/* if false, the test will stop */

	/* If min != max, then we will either do a linear iteration, or
	 * we will do a random selection from within the range.
	 */
	__u32 flags;
	int removal_mark;	/* non-zero => the device is marked for
				 * removal by worker thread */

	int min_pkt_size;	/* = ETH_ZLEN; */
	int max_pkt_size;	/* = ETH_ZLEN; */
	int pkt_overhead;	/* overhead for MPLS, VLANs, IPSEC etc */
	int nfrags;
	u64 delay;		/* nano-seconds */

	__u64 count;		/* Default No packets to send */
	__u64 sofar;		/* How many pkts we've sent so far */
	__u64 tx_bytes;		/* How many bytes we've transmitted */
	__u64 errors;		/* Errors when trying to transmit,
				   pkts will be re-sent */

	/* runtime counters relating to clone_skb */

	__u64 allocated_skbs;
	__u32 clone_count;
	int last_ok;		/* Was last skb sent?
				 * Or a failed transmit of some sort?
				 * This will keep sequence numbers in order
				 */
	ktime_t next_tx;
	ktime_t started_at;
	ktime_t stopped_at;
	u64	idle_acc;	/* nano-seconds */

	__u32 seq_num;

	int clone_skb;		/*
				 * Use multiple SKBs during packet gen.
				 * If this number is greater than 1, then
				 * that many copies of the same packet will be
				 * sent before a new packet is allocated.
				 * If you want to send 1024 identical packets
				 * before creating a new packet,
				 * set clone_skb to 1024.
				 */

	char dst_min[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */
	char dst_max[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */
	char src_min[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */
	char src_max[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */

	struct in6_addr in6_saddr;
	struct in6_addr in6_daddr;
	struct in6_addr cur_in6_daddr;
	struct in6_addr cur_in6_saddr;
	/* For ranges */
	struct in6_addr min_in6_daddr;
	struct in6_addr max_in6_daddr;
	struct in6_addr min_in6_saddr;
	struct in6_addr max_in6_saddr;

	/* If we're doing ranges, random or incremental, then this
	 * defines the min/max for those ranges.
	 */
	__be32 saddr_min;	/* inclusive, source IP address */
	__be32 saddr_max;	/* exclusive, source IP address */
	__be32 daddr_min;	/* inclusive, dest IP address */
	__be32 daddr_max;	/* exclusive, dest IP address */

	__u16 udp_src_min;	/* inclusive, source UDP port */
	__u16 udp_src_max;	/* exclusive, source UDP port */
	__u16 udp_dst_min;	/* inclusive, dest UDP port */
	__u16 udp_dst_max;	/* exclusive, dest UDP port */

	/* DSCP + ECN */
	__u8 tos;            /* six MSB of (former) IPv4 TOS
				are for dscp codepoint */
	__u8 traffic_class;  /* ditto for the (former) Traffic Class in IPv6
				(see RFC 3260, sec. 4) */

	/* MPLS */
	unsigned nr_labels;	/* Depth of stack, 0 = no MPLS */
	__be32 labels[MAX_MPLS_LABELS];

	/* VLAN/SVLAN (802.1Q/Q-in-Q) */
	__u8  vlan_p;
	__u8  vlan_cfi;
	__u16 vlan_id;  /* 0xffff means no vlan tag */

	__u8  svlan_p;
	__u8  svlan_cfi;
	__u16 svlan_id; /* 0xffff means no svlan tag */

	__u32 src_mac_count;	/* How many MACs to iterate through */
	__u32 dst_mac_count;	/* How many MACs to iterate through */

	unsigned char dst_mac[ETH_ALEN];
	unsigned char src_mac[ETH_ALEN];

	__u32 cur_dst_mac_offset;
	__u32 cur_src_mac_offset;
	__be32 cur_saddr;
	__be32 cur_daddr;
	__u16 ip_id;
	__u16 cur_udp_dst;
	__u16 cur_udp_src;
	__u16 cur_queue_map;
	__u32 cur_pkt_size;

	__u8 hh[14];
	/* = {
	   0x00, 0x80, 0xC8, 0x79, 0xB3, 0xCB,

	   We fill in SRC address later
	   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	   0x08, 0x00
	   };
	 */
	__u16 pad;		/* pad out the hh struct to an even 16 bytes */

	struct sk_buff *skb;	/* skb we are to transmit next, used for when we
				 * are transmitting the same one multiple times
				 */
	struct net_device *odev; /* The out-going device.
				  * Note that the device should have it's
				  * pg_info pointer pointing back to this
				  * device.
				  * Set when the user specifies the out-going
				  * device name (not when the inject is
				  * started as it used to do.)
				  */
	char odevname[32];
	struct flow_state *flows;
	unsigned cflows;	/* Concurrent flows (config) */
	unsigned lflow;		/* Flow length  (config) */
	unsigned nflows;	/* accumulated flows (stats) */
	unsigned curfl;		/* current sequenced flow (state)*/

	u16 queue_map_min;
	u16 queue_map_max;

#ifdef CONFIG_XFRM
	__u8	ipsmode;		/* IPSEC mode (config) */
	__u8	ipsproto;		/* IPSEC type (config) */
#endif
	char result[512];
};
struct pktgen_dev *pkt_dev = NULL;



#define VLAN_TAG_SIZE(x) ((x)->vlan_id == 0xffff ? 0 : 4)
#define SVLAN_TAG_SIZE(x) ((x)->svlan_id == 0xffff ? 0 : 4)

struct flow_state {
	__be32 cur_daddr;
	int count;
#ifdef CONFIG_XFRM
	struct xfrm_state *x;
#endif
	__u32 flags;
};
/* flow flag bits */
#define F_INIT   (1<<0)		/* flow has been initialized */

static inline ktime_t ktime_now(void)
{
	struct timespec ts;
	ktime_get_ts(&ts);

	return timespec_to_ktime(ts);
}


static int pktgen_if_show()
//static int pktgen_if_show(struct seq_file *seq, void *v)
{
	//const struct pktgen_dev *pkt_dev = seq->private;
	
	struct seq_file *seq=NULL;
	ktime_t stopped;
	u64 idle;

	printk(
		   "Params: count %llu  min_pkt_size: %u  max_pkt_size: %u\n",
		   (unsigned long long)pkt_dev->count, pkt_dev->min_pkt_size,
		   pkt_dev->max_pkt_size);

	printk(
		   "     frags: %d  delay: %llu  clone_skb: %d  ifname: %s\n",
		   pkt_dev->nfrags, (unsigned long long) pkt_dev->delay,
		   pkt_dev->clone_skb, pkt_dev->odevname);

	printk( "     flows: %u flowlen: %u\n", pkt_dev->cflows,
		   pkt_dev->lflow);

	printk(
		   "     queue_map_min: %u  queue_map_max: %u\n",
		   pkt_dev->queue_map_min,
		   pkt_dev->queue_map_max);

	if (pkt_dev->flags & F_IPV6) 
	{
	  //NO ipv6
	  #if 0
		char b1[128], b2[128], b3[128];
		fmt_ip6(b1, pkt_dev->in6_saddr.s6_addr);
		fmt_ip6(b2, pkt_dev->min_in6_saddr.s6_addr);
		fmt_ip6(b3, pkt_dev->max_in6_saddr.s6_addr);
		printk(seq,
			   "     saddr: %s  min_saddr: %s  max_saddr: %s\n", b1,
			   b2, b3);

		fmt_ip6(b1, pkt_dev->in6_daddr.s6_addr);
		fmt_ip6(b2, pkt_dev->min_in6_daddr.s6_addr);
		fmt_ip6(b3, pkt_dev->max_in6_daddr.s6_addr);
		printk(seq,
			   "     daddr: %s  min_daddr: %s  max_daddr: %s\n", b1,
			   b2, b3);
      #endif
	} 
	else 
	{
		printk(
			   "     dst_min: %s  dst_max: %s\n",
			   pkt_dev->dst_min, pkt_dev->dst_max);
		printk(
			   "        src_min: %s  src_max: %s\n",
			   pkt_dev->src_min, pkt_dev->src_max);
	}

	printk( "     src_mac: ");

	printk( "%pM ",
		   is_zero_ether_addr(pkt_dev->src_mac) ?
			     pkt_dev->odev->dev_addr : pkt_dev->src_mac);

	printk( "dst_mac: ");
	printk( "%pM\n", pkt_dev->dst_mac);

	printk(
		   "     udp_src_min: %d  udp_src_max: %d"
		   "  udp_dst_min: %d  udp_dst_max: %d\n",
		   pkt_dev->udp_src_min, pkt_dev->udp_src_max,
		   pkt_dev->udp_dst_min, pkt_dev->udp_dst_max);

	printk(
		   "     src_mac_count: %d  dst_mac_count: %d\n",
		   pkt_dev->src_mac_count, pkt_dev->dst_mac_count);

	if (pkt_dev->nr_labels) {
		unsigned i;
		printk( "     mpls: ");
		for (i = 0; i < pkt_dev->nr_labels; i++)
			printk( "%08x%s", ntohl(pkt_dev->labels[i]),
				   i == pkt_dev->nr_labels-1 ? "\n" : ", ");
	}

	if (pkt_dev->vlan_id != 0xffff)
		printk( "     vlan_id: %u  vlan_p: %u  vlan_cfi: %u\n",
			   pkt_dev->vlan_id, pkt_dev->vlan_p,
			   pkt_dev->vlan_cfi);

	if (pkt_dev->svlan_id != 0xffff)
		printk( "     svlan_id: %u  vlan_p: %u  vlan_cfi: %u\n",
			   pkt_dev->svlan_id, pkt_dev->svlan_p,
			   pkt_dev->svlan_cfi);

	if (pkt_dev->tos)
		printk( "     tos: 0x%02x\n", pkt_dev->tos);

	if (pkt_dev->traffic_class)
		printk( "     traffic_class: 0x%02x\n", pkt_dev->traffic_class);

	printk( "     Flags: ");

	if (pkt_dev->flags & F_IPV6)
		printk( "IPV6  ");

	if (pkt_dev->flags & F_IPSRC_RND)
		printk( "IPSRC_RND  ");

	if (pkt_dev->flags & F_IPDST_RND)
		printk(seq, "IPDST_RND  ");

	if (pkt_dev->flags & F_TXSIZE_RND)
		printk( "TXSIZE_RND  ");

	if (pkt_dev->flags & F_UDPSRC_RND)
		printk( "UDPSRC_RND  ");

	if (pkt_dev->flags & F_UDPDST_RND)
		printk( "UDPDST_RND  ");

	if (pkt_dev->flags & F_MPLS_RND)
		printk(  "MPLS_RND  ");

	if (pkt_dev->flags & F_QUEUE_MAP_RND)
		printk(  "QUEUE_MAP_RND  ");

	if (pkt_dev->flags & F_QUEUE_MAP_CPU)
		printk(  "QUEUE_MAP_CPU  ");

	if (pkt_dev->cflows) {
		if (pkt_dev->flags & F_FLOW_SEQ)
			printk(  "FLOW_SEQ  "); //in sequence flows
		else
			printk(  "FLOW_RND  ");
	}

#ifdef CONFIG_XFRM
	if (pkt_dev->flags & F_IPSEC_ON)
		printk(  "IPSEC  ");
#endif

	if (pkt_dev->flags & F_MACSRC_RND)
		printk( "MACSRC_RND  ");

	if (pkt_dev->flags & F_MACDST_RND)
		printk( "MACDST_RND  ");

	if (pkt_dev->flags & F_VID_RND)
		printk( "VID_RND  ");

	if (pkt_dev->flags & F_SVID_RND)
		printk( "SVID_RND  ");

	printk( "\n");

	// not really stopped, more like last-running-at 
	stopped = pkt_dev->running ? ktime_now() : pkt_dev->stopped_at;
	idle = pkt_dev->idle_acc;
	do_div(idle, NSEC_PER_USEC);

	printk(
		   "Current:\n     pkts-sofar: %llu  errors: %llu\n",
		   (unsigned long long)pkt_dev->sofar,
		   (unsigned long long)pkt_dev->errors);

	printk(
		   "     started: %lluus  stopped: %lluus idle: %lluus\n",
		   (unsigned long long) ktime_to_us(pkt_dev->started_at),
		   (unsigned long long) ktime_to_us(stopped),
		   (unsigned long long) idle);

	printk(
		   "     seq_num: %d  cur_dst_mac_offset: %d  cur_src_mac_offset: %d\n",
		   pkt_dev->seq_num, pkt_dev->cur_dst_mac_offset,
		   pkt_dev->cur_src_mac_offset);

	if (pkt_dev->flags & F_IPV6) 
	{
		//NO IPv6
		#if 0
		char b1[128], b2[128];
		fmt_ip6(b1, pkt_dev->cur_in6_daddr.s6_addr);
		fmt_ip6(b2, pkt_dev->cur_in6_saddr.s6_addr);
		printk(seq, "     cur_saddr: %s  cur_daddr: %s\n", b2, b1);
	    #endif
	}
	else
		printk( "     cur_saddr: 0x%x  cur_daddr: 0x%x\n",
			   pkt_dev->cur_saddr, pkt_dev->cur_daddr);

	printk( "     cur_udp_dst: %d  cur_udp_src: %d\n",
		   pkt_dev->cur_udp_dst, pkt_dev->cur_udp_src);

	printk( "     cur_queue_map: %u\n", pkt_dev->cur_queue_map);

	printk( "     flows: %u\n", pkt_dev->nflows);

	if (pkt_dev->result[0])
		printk( "Result: %s\n", pkt_dev->result);
	else
		printk( "Result: Idle\n");

	return 0;
}


#ifdef CONFIG_XFRM
static int pktgen_output_ipsec(struct sk_buff *skb, struct pktgen_dev *pkt_dev)
{
	struct xfrm_state *x = pkt_dev->flows[pkt_dev->curfl].x;
	int err = 0;
	struct iphdr *iph;

	if (!x)
		return 0;
	/* XXX: we dont support tunnel mode for now until
	 * we resolve the dst issue */
	if (x->props.mode != XFRM_MODE_TRANSPORT)
		return 0;

	spin_lock(&x->lock);
	iph = ip_hdr(skb);

	err = x->outer_mode->output(x, skb);
	if (err)
		goto error;
	err = x->type->output(x, skb);
	if (err)
		goto error;

	x->curlft.bytes += skb->len;
	x->curlft.packets++;
error:
	spin_unlock(&x->lock);
	return err;
}

static void free_SAs(struct pktgen_dev *pkt_dev)
{
	if (pkt_dev->cflows) {
		/* let go of the SAs if we have them */
		int i = 0;
		for (;  i < pkt_dev->cflows; i++) {
			struct xfrm_state *x = pkt_dev->flows[i].x;
			if (x) {
				xfrm_state_put(x);
				pkt_dev->flows[i].x = NULL;
			}
		}
	}
}

static int process_ipsec(struct pktgen_dev *pkt_dev,
			      struct sk_buff *skb, __be16 protocol)
{
	if (pkt_dev->flags & F_IPSEC_ON) {
		struct xfrm_state *x = pkt_dev->flows[pkt_dev->curfl].x;
		int nhead = 0;
		if (x) {
			int ret;
			__u8 *eth;
			nhead = x->props.header_len - skb_headroom(skb);
			if (nhead > 0) {
				ret = pskb_expand_head(skb, nhead, 0, GFP_ATOMIC);
				if (ret < 0) {
					printk(KERN_ERR "Error expanding "
					       "ipsec packet %d\n", ret);
					goto err;
				}
			}

			/* ipsec is not expecting ll header */
			skb_pull(skb, ETH_HLEN);
			ret = pktgen_output_ipsec(skb, pkt_dev);
			if (ret) {
				printk(KERN_ERR "Error creating ipsec "
				       "packet %d\n", ret);
				goto err;
			}
			/* restore ll */
			eth = (__u8 *) skb_push(skb, ETH_HLEN);
			memcpy(eth, pkt_dev->hh, 12);
			*(u16 *) &eth[12] = protocol;
		}
	}
	return 1;
err:
	kfree_skb(skb);
	return 0;
}
#endif




/**************************************************************************************************
Syntax:      	  static struct net_device *pktgen_dev_get_by_name(const char *ifname)

Remarks:		  Важная функция позволяет нам получить структуру struct net_device по имени устройства,"eth0","eth1","eth2"

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static struct net_device *pktgen_dev_get_by_name(struct pktgen_dev *pkt_dev,const char *ifname)
{
	char b[IFNAMSIZ+5];
	int i = 0;
    printk("+++++++++pktgen_dev_get_by_name++++++++\n");	
	for (i = 0; ifname[i] != '@'; i++) 
	{
		if (i == IFNAMSIZ)
			break;

		b[i] = ifname[i];
	}
	b[i] = 0;

	return dev_get_by_name(&init_net, b);
}

/*
 * Adds a dev at front of if_list.
 */
static int add_dev_to_thread(struct pktgen_thread *t,struct pktgen_dev *pkt_dev)
{
	int rv = 0;

	if_lock(t);

	if (pkt_dev->pg_thread) {
		printk(KERN_ERR "pktgen: ERROR: already assigned "
		       "to a thread.\n");
		rv = -EBUSY;
		goto out;
	}

	list_add(&pkt_dev->list, &t->if_list);
	pkt_dev->pg_thread = t;
	pkt_dev->running = 0;

out:
	if_unlock(t);
	return rv;
}



/* Associate pktgen_dev with a device. */
static int pktgen_setup_dev(struct pktgen_dev *pkt_dev, const char *ifname)
{
	struct net_device *odev;
	int err;
    printk("+++++++++pktgen_setup_dev++++++++\n");	
	/* Clean old setups */
	if (pkt_dev->odev)
	{
		dev_put(pkt_dev->odev);
		pkt_dev->odev = NULL;
	}
	
	odev = pktgen_dev_get_by_name(pkt_dev, ifname);
	if (!odev)
	{
		printk(KERN_ERR "pktgen: no such netdevice: \"%s\"\n", ifname);
		return -ENODEV;
	}

	if (odev->type != ARPHRD_ETHER)
	{
		printk(KERN_ERR "pktgen: not an ethernet device: \"%s\"\n", ifname);
		err = -EINVAL;
	} else if (!netif_running(odev)) 
	{
		printk(KERN_ERR "pktgen: device is down: \"%s\"\n", ifname);
		err = -ENETDOWN;
	} else 
	{
		pkt_dev->odev = odev;
		return 0;
	}

	dev_put(odev);
	return err;
}


/* Called under thread lock */
static int pktgen_add_device(struct pktgen_thread *t, const char *ifname)
{
	//struct pktgen_dev *pkt_dev;
	int err;

	/* We don't allow a device to be on several threads */
    /*
	pkt_dev = __pktgen_NN_threads(ifname, FIND);
	if (pkt_dev) {
		printk(KERN_ERR "pktgen: ERROR: interface already used.\n");
		return -EBUSY;
	}
    */
	printk("+++++++++pktgen_add_device++++++++\n");	
	
	pkt_dev = kzalloc(sizeof(struct pktgen_dev), GFP_KERNEL);
	if (!pkt_dev)
	{
		return -ENOMEM;
    }
	strcpy(pkt_dev->odevname, ifname);
	pkt_dev->flows = vmalloc(MAX_CFLOWS * sizeof(struct flow_state));
	if (pkt_dev->flows == NULL)
	{
		kfree(pkt_dev);
		return -ENOMEM;
	}
	memset(pkt_dev->flows, 0, MAX_CFLOWS * sizeof(struct flow_state));

	
	//Kosta add
	//printk("pkt_dev->cur_pkt_size=%x,pkt_dev->pkt_overhead=%x\n\r",pkt_dev->cur_pkt_size,pkt_dev->pkt_overhead);
	pkt_dev->cur_pkt_size =0x200;
	
	pkt_dev->dst_mac[5]=0xe9;
	pkt_dev->dst_mac[4]=0x8b;
	pkt_dev->dst_mac[3]=0x75;
	pkt_dev->dst_mac[2]=0x27;
	pkt_dev->dst_mac[1]=0x00;
	pkt_dev->dst_mac[0]=0x08;
	
	
	//////End Kosta add
	pkt_dev->removal_mark = 0;
	pkt_dev->min_pkt_size = ETH_ZLEN;
	pkt_dev->max_pkt_size = ETH_ZLEN;
	pkt_dev->nfrags = 0;
	pkt_dev->clone_skb = pg_clone_skb_d;
	pkt_dev->delay = pg_delay_d;
	pkt_dev->count = pg_count_d;
	pkt_dev->sofar = 0;
	pkt_dev->udp_src_min = 9;	/* sink port */
	pkt_dev->udp_src_max = 9;
	pkt_dev->udp_dst_min = 9;
	pkt_dev->udp_dst_max = 9;

	pkt_dev->vlan_p = 0;
	pkt_dev->vlan_cfi = 0;
	pkt_dev->vlan_id = 0xffff;
	pkt_dev->svlan_p = 0;
	pkt_dev->svlan_cfi = 0;
	pkt_dev->svlan_id = 0xffff;

	err = pktgen_setup_dev(pkt_dev, ifname);
	if (err)
	{
		goto out1;
    }

	
    /*
	pkt_dev->entry = proc_create_data(ifname, 0600, pg_proc_dir,&pktgen_if_fops, pkt_dev);
	if (!pkt_dev->entry)
	{
		printk(KERN_ERR "pktgen: cannot create %s/%s procfs entry.\n",PG_PROC_DIR, ifname);
		err = -EINVAL;
		goto out2;
	}
    */
#ifdef CONFIG_XFRM
	pkt_dev->ipsmode = XFRM_MODE_TRANSPORT;
	pkt_dev->ipsproto = IPPROTO_ESP;
#endif


	return add_dev_to_thread(t, pkt_dev);
out2:
	dev_put(pkt_dev->odev);
out1:
#ifdef CONFIG_XFRM
	//free_SAs(pkt_dev);
#endif
	vfree(pkt_dev->flows);
	kfree(pkt_dev);
	return err;
}

///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////Send Packets Specific P2020 Code//////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static void spin(struct pktgen_dev *pkt_dev, ktime_t spin_until)
{
	ktime_t start_time, end_time;
	s64 remaining;
	struct hrtimer_sleeper t;

	hrtimer_init_on_stack(&t.timer, CLOCK_MONOTONIC, HRTIMER_MODE_ABS);
	hrtimer_set_expires(&t.timer, spin_until);

	remaining = ktime_to_us(hrtimer_expires_remaining(&t.timer));
	if (remaining <= 0) {
		pkt_dev->next_tx = ktime_add_ns(spin_until, pkt_dev->delay);
		return;
	}

	start_time = ktime_now();
	if (remaining < 100)
		udelay(remaining); 	/* really small just spin */
	else {
		/* see do_nanosleep */
		hrtimer_init_sleeper(&t, current);
		do {
			set_current_state(TASK_INTERRUPTIBLE);
			hrtimer_start_expires(&t.timer, HRTIMER_MODE_ABS);
			if (!hrtimer_active(&t.timer))
				t.task = NULL;

			if (likely(t.task))
				schedule();

			hrtimer_cancel(&t.timer);
		} while (t.task && pkt_dev->running && !signal_pending(current));
		__set_current_state(TASK_RUNNING);
	}
	end_time = ktime_now();

	pkt_dev->idle_acc += ktime_to_ns(ktime_sub(end_time, start_time));
	pkt_dev->next_tx = ktime_add_ns(end_time, pkt_dev->delay);
}





///////////////////////////////////////////////////////////////////////////////////

static inline void set_pkt_overhead(struct pktgen_dev *pkt_dev)
{
	pkt_dev->pkt_overhead = 0;
	pkt_dev->pkt_overhead += pkt_dev->nr_labels*sizeof(u32);
	pkt_dev->pkt_overhead += VLAN_TAG_SIZE(pkt_dev);
	pkt_dev->pkt_overhead += SVLAN_TAG_SIZE(pkt_dev);
}
///////////////////////////////////////////////////////////////////////////////////

static void set_cur_queue_map(struct pktgen_dev *pkt_dev)
{

	if (pkt_dev->flags & F_QUEUE_MAP_CPU)
		pkt_dev->cur_queue_map = smp_processor_id();

	else if (pkt_dev->queue_map_min <= pkt_dev->queue_map_max) {
		__u16 t;
		if (pkt_dev->flags & F_QUEUE_MAP_RND) {
			t = random32() %
				(pkt_dev->queue_map_max -
				 pkt_dev->queue_map_min + 1)
				+ pkt_dev->queue_map_min;
		} else {
			t = pkt_dev->cur_queue_map + 1;
			if (t > pkt_dev->queue_map_max)
				t = pkt_dev->queue_map_min;
		}
		pkt_dev->cur_queue_map = t;
	}
	pkt_dev->cur_queue_map  = pkt_dev->cur_queue_map % pkt_dev->odev->real_num_tx_queues;
}
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////SUPPORT FUNCTION/////////////////////
#ifdef CONFIG_XFRM
/* If there was already an IPSEC SA, we keep it as is, else
 * we go look for it ...
*/
static void get_ipsec_sa(struct pktgen_dev *pkt_dev, int flow)
{
	struct xfrm_state *x = pkt_dev->flows[flow].x;
	if (!x) {
		/*slow path: we dont already have xfrm_state*/
		x = xfrm_stateonly_find(&init_net,
					(xfrm_address_t *)&pkt_dev->cur_daddr,
					(xfrm_address_t *)&pkt_dev->cur_saddr,
					AF_INET,
					pkt_dev->ipsmode,
					pkt_dev->ipsproto, 0);
		if (x) {
			pkt_dev->flows[flow].x = x;
			set_pkt_overhead(pkt_dev);
			pkt_dev->pkt_overhead += x->props.header_len;
		}

	}
}
#endif


///////////////////////////////////////////////////////////////////////////////////
static inline int f_seen(const struct pktgen_dev *pkt_dev, int flow)
{
	return !!(pkt_dev->flows[flow].flags & F_INIT);
}

///////////////////////////////////////////////////////////////////////////////////

static inline int f_pick(struct pktgen_dev *pkt_dev)
{
	int flow = pkt_dev->curfl;

	if (pkt_dev->flags & F_FLOW_SEQ) {
		if (pkt_dev->flows[flow].count >= pkt_dev->lflow) {
			/* reset time */
			pkt_dev->flows[flow].count = 0;
			pkt_dev->flows[flow].flags = 0;
			pkt_dev->curfl += 1;
			if (pkt_dev->curfl >= pkt_dev->cflows)
				pkt_dev->curfl = 0; /*reset */
		}
	} else {
		flow = random32() % pkt_dev->cflows;
		pkt_dev->curfl = flow;

		if (pkt_dev->flows[flow].count > pkt_dev->lflow) {
			pkt_dev->flows[flow].count = 0;
			pkt_dev->flows[flow].flags = 0;
		}
	}

	return pkt_dev->curfl;
}



static inline __be16 build_tci(unsigned int id, unsigned int cfi,
			       unsigned int prio)
{
	return htons(id | (cfi << 12) | (prio << 13));
}
static void mpls_push(__be32 *mpls, struct pktgen_dev *pkt_dev)
{
	unsigned i;
	for (i = 0; i < pkt_dev->nr_labels; i++)
		*mpls++ = pkt_dev->labels[i] & ~MPLS_STACK_BOTTOM;

	mpls--;
	*mpls |= MPLS_STACK_BOTTOM;
}




/**************************************************************************************************
Syntax:      	 static void mod_cur_headers(struct pktgen_dev *pkt_dev)

Remarks:		 Increment/randomize headers according to flags and current values
                 for IP src/dest, UDP src/dst port, MAC-Addr src/dst
				 
Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static void mod_cur_headers(struct pktgen_dev *pkt_dev)
{
	__u32 imn;
	__u32 imx;
	int flow = 0;

	if (pkt_dev->cflows)
		flow = f_pick(pkt_dev);

	/*  Deal with source MAC */
	if (pkt_dev->src_mac_count > 1) {
		__u32 mc;
		__u32 tmp;

		if (pkt_dev->flags & F_MACSRC_RND)
			mc = random32() % pkt_dev->src_mac_count;
		else {
			mc = pkt_dev->cur_src_mac_offset++;
			if (pkt_dev->cur_src_mac_offset >=
			    pkt_dev->src_mac_count)
				pkt_dev->cur_src_mac_offset = 0;
		}

		tmp = pkt_dev->src_mac[5] + (mc & 0xFF);
		pkt_dev->hh[11] = tmp;
		tmp = (pkt_dev->src_mac[4] + ((mc >> 8) & 0xFF) + (tmp >> 8));
		pkt_dev->hh[10] = tmp;
		tmp = (pkt_dev->src_mac[3] + ((mc >> 16) & 0xFF) + (tmp >> 8));
		pkt_dev->hh[9] = tmp;
		tmp = (pkt_dev->src_mac[2] + ((mc >> 24) & 0xFF) + (tmp >> 8));
		pkt_dev->hh[8] = tmp;
		tmp = (pkt_dev->src_mac[1] + (tmp >> 8));
		pkt_dev->hh[7] = tmp;
	}

	/*  Deal with Destination MAC */
	if (pkt_dev->dst_mac_count > 1) {
		__u32 mc;
		__u32 tmp;

		if (pkt_dev->flags & F_MACDST_RND)
			mc = random32() % pkt_dev->dst_mac_count;

		else {
			mc = pkt_dev->cur_dst_mac_offset++;
			if (pkt_dev->cur_dst_mac_offset >=
			    pkt_dev->dst_mac_count) {
				pkt_dev->cur_dst_mac_offset = 0;
			}
		}

		tmp = pkt_dev->dst_mac[5] + (mc & 0xFF);
		pkt_dev->hh[5] = tmp;
		tmp = (pkt_dev->dst_mac[4] + ((mc >> 8) & 0xFF) + (tmp >> 8));
		pkt_dev->hh[4] = tmp;
		tmp = (pkt_dev->dst_mac[3] + ((mc >> 16) & 0xFF) + (tmp >> 8));
		pkt_dev->hh[3] = tmp;
		tmp = (pkt_dev->dst_mac[2] + ((mc >> 24) & 0xFF) + (tmp >> 8));
		pkt_dev->hh[2] = tmp;
		tmp = (pkt_dev->dst_mac[1] + (tmp >> 8));
		pkt_dev->hh[1] = tmp;
	}

	if (pkt_dev->flags & F_MPLS_RND) {
		unsigned i;
		for (i = 0; i < pkt_dev->nr_labels; i++)
			if (pkt_dev->labels[i] & MPLS_STACK_BOTTOM)
				pkt_dev->labels[i] = MPLS_STACK_BOTTOM |
					     ((__force __be32)random32() &
						      htonl(0x000fffff));
	}

	if ((pkt_dev->flags & F_VID_RND) && (pkt_dev->vlan_id != 0xffff)) {
		pkt_dev->vlan_id = random32() & (4096-1);
	}

	if ((pkt_dev->flags & F_SVID_RND) && (pkt_dev->svlan_id != 0xffff)) {
		pkt_dev->svlan_id = random32() & (4096 - 1);
	}

	if (pkt_dev->udp_src_min < pkt_dev->udp_src_max) {
		if (pkt_dev->flags & F_UDPSRC_RND)
			pkt_dev->cur_udp_src = random32() %
				(pkt_dev->udp_src_max - pkt_dev->udp_src_min)
				+ pkt_dev->udp_src_min;

		else {
			pkt_dev->cur_udp_src++;
			if (pkt_dev->cur_udp_src >= pkt_dev->udp_src_max)
				pkt_dev->cur_udp_src = pkt_dev->udp_src_min;
		}
	}

	if (pkt_dev->udp_dst_min < pkt_dev->udp_dst_max) {
		if (pkt_dev->flags & F_UDPDST_RND) {
			pkt_dev->cur_udp_dst = random32() %
				(pkt_dev->udp_dst_max - pkt_dev->udp_dst_min)
				+ pkt_dev->udp_dst_min;
		} else {
			pkt_dev->cur_udp_dst++;
			if (pkt_dev->cur_udp_dst >= pkt_dev->udp_dst_max)
				pkt_dev->cur_udp_dst = pkt_dev->udp_dst_min;
		}
	}

	if (!(pkt_dev->flags & F_IPV6)) {

		imn = ntohl(pkt_dev->saddr_min);
		imx = ntohl(pkt_dev->saddr_max);
		if (imn < imx) {
			__u32 t;
			if (pkt_dev->flags & F_IPSRC_RND)
				t = random32() % (imx - imn) + imn;
			else {
				t = ntohl(pkt_dev->cur_saddr);
				t++;
				if (t > imx)
					t = imn;

			}
			pkt_dev->cur_saddr = htonl(t);
		}

		if (pkt_dev->cflows && f_seen(pkt_dev, flow)) {
			pkt_dev->cur_daddr = pkt_dev->flows[flow].cur_daddr;
		} else {
			imn = ntohl(pkt_dev->daddr_min);
			imx = ntohl(pkt_dev->daddr_max);
			if (imn < imx) {
				__u32 t;
				__be32 s;
				if (pkt_dev->flags & F_IPDST_RND) {

					t = random32() % (imx - imn) + imn;
					s = htonl(t);

					while (ipv4_is_loopback(s) ||
					       ipv4_is_multicast(s) ||
					       ipv4_is_lbcast(s) ||
					       ipv4_is_zeronet(s) ||
					       ipv4_is_local_multicast(s)) {
						t = random32() % (imx - imn) + imn;
						s = htonl(t);
					}
					pkt_dev->cur_daddr = s;
				} else {
					t = ntohl(pkt_dev->cur_daddr);
					t++;
					if (t > imx) {
						t = imn;
					}
					pkt_dev->cur_daddr = htonl(t);
				}
			}
			if (pkt_dev->cflows) {
				pkt_dev->flows[flow].flags |= F_INIT;
				pkt_dev->flows[flow].cur_daddr =
				    pkt_dev->cur_daddr;
#ifdef CONFIG_XFRM
				if (pkt_dev->flags & F_IPSEC_ON)
					get_ipsec_sa(pkt_dev, flow);
#endif
				pkt_dev->nflows++;
			}
		}
	} else {		/* IPV6 * */

		if (pkt_dev->min_in6_daddr.s6_addr32[0] == 0 &&
		    pkt_dev->min_in6_daddr.s6_addr32[1] == 0 &&
		    pkt_dev->min_in6_daddr.s6_addr32[2] == 0 &&
		    pkt_dev->min_in6_daddr.s6_addr32[3] == 0) ;
		else {
			int i;

			/* Only random destinations yet */

			for (i = 0; i < 4; i++) {
				pkt_dev->cur_in6_daddr.s6_addr32[i] =
				    (((__force __be32)random32() |
				      pkt_dev->min_in6_daddr.s6_addr32[i]) &
				     pkt_dev->max_in6_daddr.s6_addr32[i]);
			}
		}
	}

	if (pkt_dev->min_pkt_size < pkt_dev->max_pkt_size) {
		__u32 t;
		if (pkt_dev->flags & F_TXSIZE_RND) {
			t = random32() %
				(pkt_dev->max_pkt_size - pkt_dev->min_pkt_size)
				+ pkt_dev->min_pkt_size;
		} else {
			t = pkt_dev->cur_pkt_size + 1;
			if (t > pkt_dev->max_pkt_size)
				t = pkt_dev->min_pkt_size;
		}
		pkt_dev->cur_pkt_size = t;
	}

	set_cur_queue_map(pkt_dev);

	pkt_dev->flows[flow].count++;
}

/**************************************************************************************************
Syntax:      	 static struct sk_buff *fill_packet_ipv4(struct net_device *odev,struct pktgen_dev *pkt_dev)

Remarks:		 Fill ipv4 Packets

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static struct sk_buff *fill_packet_ipv4(struct net_device *odev,struct pktgen_dev *pkt_dev)
{
	struct sk_buff *skb = NULL;
	__u8 *eth;
	struct udphdr *udph;
	int datalen, iplen;
	struct iphdr *iph;
	struct pktgen_hdr *pgh = NULL;
	__be16 protocol = htons(ETH_P_IP);
	__be32 *mpls;
	__be16 *vlan_tci = NULL;                 /* Encapsulates priority and VLAN ID */
	__be16 *vlan_encapsulated_proto = NULL;  /* packet type ID field (or len) for VLAN tag */
	__be16 *svlan_tci = NULL;                /* Encapsulates priority and SVLAN ID */
	__be16 *svlan_encapsulated_proto = NULL; /* packet type ID field (or len) for SVLAN tag */
	u16 queue_map;
   
	if (pkt_dev->nr_labels)
		protocol = htons(ETH_P_MPLS_UC);

	if (pkt_dev->vlan_id != 0xffff)
		protocol = htons(ETH_P_8021Q);

	//Update any of the values, used when we're incrementing various
	// fields.
	
	queue_map = pkt_dev->cur_queue_map;
	//mod_cur_headers(pkt_dev);

	datalen = (odev->hard_header_len + 16) & ~0xf;
	printk("datalen =%x\n\r",datalen);
	
	skb = __netdev_alloc_skb(odev,pkt_dev->cur_pkt_size + 64 + datalen + pkt_dev->pkt_overhead, GFP_NOWAIT);
	if (!skb)
	{
		printk("No memory =%x\n\r",pkt_dev->result);
		//sprintf(pkt_dev->result, "No memory");
		return NULL;
	}

	
	
	skb_reserve(skb, datalen);

	//  Reserve for ethernet and IP header 
	eth = (__u8 *) skb_push(skb, 14);
	mpls = (__be32 *)skb_put(skb, pkt_dev->nr_labels*sizeof(__u32));
	
	printk("eth =%p,mpls=%p \n\r",eth,mpls);
	
	if (pkt_dev->nr_labels)
	{
		mpls_push(mpls, pkt_dev);
    }
		
		
	if (pkt_dev->vlan_id != 0xffff) 
	{
		if (pkt_dev->svlan_id != 0xffff) 
		{
			svlan_tci = (__be16 *)skb_put(skb, sizeof(__be16));
			*svlan_tci = build_tci(pkt_dev->svlan_id,
					       pkt_dev->svlan_cfi,
					       pkt_dev->svlan_p);
			svlan_encapsulated_proto = (__be16 *)skb_put(skb, sizeof(__be16));
			*svlan_encapsulated_proto = htons(ETH_P_8021Q);
		}
		vlan_tci = (__be16 *)skb_put(skb, sizeof(__be16));
		*vlan_tci = build_tci(pkt_dev->vlan_id,
				      pkt_dev->vlan_cfi,
				      pkt_dev->vlan_p);
		vlan_encapsulated_proto = (__be16 *)skb_put(skb, sizeof(__be16));
		*vlan_encapsulated_proto = htons(ETH_P_IP);
	}

	

	skb->network_header = skb->tail;
	skb->transport_header = skb->network_header + sizeof(struct iphdr);
	skb_put(skb, sizeof(struct iphdr) + sizeof(struct udphdr));
	skb_set_queue_mapping(skb, queue_map);
	iph = ip_hdr(skb);
	udph = udp_hdr(skb);


	memcpy(eth, pkt_dev->hh, 12);
	*(__be16 *) & eth[12] = protocol;

	// Eth + IPh + UDPh + mpls 
	datalen = pkt_dev->cur_pkt_size - 14 - 20 - 8 - pkt_dev->pkt_overhead;
	printk("pkt_dev->cur_pkt_size=%x,pkt_dev->pkt_overhead=%x\n\r",pkt_dev->cur_pkt_size,pkt_dev->pkt_overhead);
	
	if (datalen < sizeof(struct pktgen_hdr))
		datalen = sizeof(struct pktgen_hdr);
    printk("datalen Eth + IPh + UDPh + mpls  =%x\n\r",datalen);
		
		
	udph->source = htons(pkt_dev->cur_udp_src);
	udph->dest = htons(pkt_dev->cur_udp_dst);
	udph->len = htons(datalen + 8);	// DATA + udphdr 
	udph->check = 0;	// No checksum 

	iph->ihl = 5;
	iph->version = 4;
	iph->ttl = 128;
	iph->tos = pkt_dev->tos;
	iph->protocol = IPPROTO_UDP;	// UDP 
	iph->saddr = 0xC0A8827C;//pkt_dev->cur_saddr;
	iph->daddr = 0x0A000001;//pkt_dev->cur_daddr;
	iph->id = htons(pkt_dev->ip_id);
	pkt_dev->ip_id++;
	iph->frag_off = 0;
	iplen = 20 + 8 + datalen;
	iph->tot_len = htons(iplen);
	iph->check = 0;
	iph->check = ip_fast_csum((void *)iph, iph->ihl);
	skb->protocol = protocol;
	skb->mac_header = (skb->network_header - ETH_HLEN - pkt_dev->pkt_overhead);
	skb->dev = odev;
	skb->pkt_type = PACKET_HOST;

	
	
	printk("iph->saddr=%x,iph->daddr=%x,skb->protocol\n\r",iph->saddr,iph->daddr,skb->protocol);
	if (pkt_dev->nfrags <= 0) 
	{
		
		//printk("+++++pkt_dev->nfrags <= 0\n\r");
		pgh = (struct pktgen_hdr *)skb_put(skb, datalen);
		//printk("pgh =%p,datalen=%x \n\r",pgh,datalen);
		//printk("summary=%x\n\r",datalen - sizeof(struct pktgen_hdr));
		
		memset(pgh + 1, 0, datalen - sizeof(struct pktgen_hdr));
	
	
	} 
	else 
	{
		int frags = pkt_dev->nfrags;
		int i, len;

		pgh = (struct pktgen_hdr *)(((char *)(udph)) + 8);

		if (frags > MAX_SKB_FRAGS)
			frags = MAX_SKB_FRAGS;
		if (datalen > frags * PAGE_SIZE) 
		{
			len = datalen - frags * PAGE_SIZE;
			memset(skb_put(skb, len), 0, len);
			datalen = frags * PAGE_SIZE;
		}

		i = 0;
		while (datalen > 0) 
		{
			struct page *page = alloc_pages(GFP_KERNEL | __GFP_ZERO, 0);
			skb_shinfo(skb)->frags[i].page = page;
			skb_shinfo(skb)->frags[i].page_offset = 0;
			skb_shinfo(skb)->frags[i].size = (datalen < PAGE_SIZE ? datalen : PAGE_SIZE);
			datalen -= skb_shinfo(skb)->frags[i].size;
			skb->len += skb_shinfo(skb)->frags[i].size;
			skb->data_len += skb_shinfo(skb)->frags[i].size;
			i++;
			skb_shinfo(skb)->nr_frags = i;
		}

		while (i < frags) 
		{
			int rem;

			if (i == 0)
				break;

			rem = skb_shinfo(skb)->frags[i - 1].size / 2;
			if (rem == 0)
				break;

			skb_shinfo(skb)->frags[i - 1].size -= rem;

			skb_shinfo(skb)->frags[i] =
			    skb_shinfo(skb)->frags[i - 1];
			get_page(skb_shinfo(skb)->frags[i].page);
			skb_shinfo(skb)->frags[i].page =
			    skb_shinfo(skb)->frags[i - 1].page;
			skb_shinfo(skb)->frags[i].page_offset +=
			    skb_shinfo(skb)->frags[i - 1].size;
			skb_shinfo(skb)->frags[i].size = rem;
			i++;
			skb_shinfo(skb)->nr_frags = i;
		}
	 
	}

	// Stamp the time, and sequence number,
	// convert them to network byte order
 
	if (pgh) 
	{
		struct timeval timestamp;

		pgh->pgh_magic = htonl(PKTGEN_MAGIC);
		pgh->seq_num = htonl(pkt_dev->seq_num);

		do_gettimeofday(&timestamp);
		pgh->tv_sec = htonl(timestamp.tv_sec);
		pgh->tv_usec = htonl(timestamp.tv_usec);
	}

	
	
#ifdef CONFIG_XFRM
	if (!process_ipsec(pkt_dev, skb, protocol))
		return NULL;
#endif

	return skb;
}

/*
static void pktgen_wait_for_skb(struct pktgen_dev *pkt_dev)
{
	ktime_t idle_start = ktime_now();

	while (atomic_read(&(pkt_dev->skb->users)) != 1) {
		if (signal_pending(current))
			break;

		if (need_resched())
			pktgen_resched(pkt_dev);
		else
			cpu_relax();
	}
	pkt_dev->idle_acc += ktime_to_ns(ktime_sub(ktime_now(), idle_start));
}
*/



static void pktgen_clear_counters(struct pktgen_dev *pkt_dev)
{
	pkt_dev->seq_num = 1;
	pkt_dev->idle_acc = 0;
	pkt_dev->sofar = 0;
	pkt_dev->tx_bytes = 0;
	pkt_dev->errors = 0;
}

/**************************************************************************************************
Syntax:      	  static void pktgen_xmit(struct pktgen_dev *pkt_dev)

Remarks:		  Read pkt_dev from the interface and set up internal pktgen_dev
                  structure to have the right information to create/send packets 
Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static void pktgen_setup_inject(struct pktgen_dev *pkt_dev)
{   
	int ntxq;

	
	printk("++++++++pktgen_setup_inject+++\n\r") ;
	if (!pkt_dev->odev) 
	{
		printk(KERN_ERR "pktgen: ERROR: pkt_dev->odev == NULL in "
		       "setup_inject.\n");
		sprintf(pkt_dev->result,
			"ERROR: pkt_dev->odev == NULL in setup_inject.\n");
		return;
	}

	/* make sure that we don't pick a non-existing transmit queue */
	ntxq = pkt_dev->odev->real_num_tx_queues;

	if (ntxq <= pkt_dev->queue_map_min) 
	{
		printk(KERN_WARNING "pktgen: WARNING: Requested "
		       "queue_map_min (zero-based) (%d) exceeds valid range "
		       "[0 - %d] for (%d) queues on %s, resetting\n",
		       pkt_dev->queue_map_min, (ntxq ?: 1) - 1, ntxq,
		       pkt_dev->odevname);
		pkt_dev->queue_map_min = ntxq - 1;
	}
	if (pkt_dev->queue_map_max >= ntxq)
	{
		printk(KERN_WARNING "pktgen: WARNING: Requested "
		       "queue_map_max (zero-based) (%d) exceeds valid range "
		       "[0 - %d] for (%d) queues on %s, resetting\n",
		       pkt_dev->queue_map_max, (ntxq ?: 1) - 1, ntxq,
		       pkt_dev->odevname);
		pkt_dev->queue_map_max = ntxq - 1;
	}

	/* Default to the interface's mac if not explicitly set. */

	if (is_zero_ether_addr(pkt_dev->src_mac))
		memcpy(&(pkt_dev->hh[6]), pkt_dev->odev->dev_addr, ETH_ALEN);

	/* Set up Dest MAC */
	memcpy(&(pkt_dev->hh[0]), pkt_dev->dst_mac, ETH_ALEN);

	/* Set up pkt size */
	pkt_dev->cur_pkt_size = pkt_dev->min_pkt_size;

	if (pkt_dev->flags & F_IPV6) 
	{
		/*
		 * Skip this automatic address setting until locks or functions
		 * gets exported
		 */

#ifdef NOTNOW
		int i, set = 0, err = 1;
		struct inet6_dev *idev;

		for (i = 0; i < IN6_ADDR_HSIZE; i++)
			if (pkt_dev->cur_in6_saddr.s6_addr[i]) {
				set = 1;
				break;
			}

		if (!set) {

			/*
			 * Use linklevel address if unconfigured.
			 *
			 * use ipv6_get_lladdr if/when it's get exported
			 */

			rcu_read_lock();
			idev = __in6_dev_get(pkt_dev->odev);
			if (idev) {
				struct inet6_ifaddr *ifp;

				read_lock_bh(&idev->lock);
				for (ifp = idev->addr_list; ifp;
				     ifp = ifp->if_next) {
					if (ifp->scope == IFA_LINK
					    && !(ifp->
						 flags & IFA_F_TENTATIVE)) {
						ipv6_addr_copy(&pkt_dev->
							       cur_in6_saddr,
							       &ifp->addr);
						err = 0;
						break;
					}
				}
				read_unlock_bh(&idev->lock);
			}
			rcu_read_unlock();
			if (err)
				printk(KERN_ERR "pktgen: ERROR: IPv6 link "
				       "address not availble.\n");
		}
#endif
	} 
	else 
	{
		pkt_dev->saddr_min = 0;
		pkt_dev->saddr_max = 0;
		if (strlen(pkt_dev->src_min) == 0) {

			struct in_device *in_dev;

			rcu_read_lock();
			in_dev = __in_dev_get_rcu(pkt_dev->odev);
			if (in_dev) {
				if (in_dev->ifa_list) {
					pkt_dev->saddr_min =
					    in_dev->ifa_list->ifa_address;
					pkt_dev->saddr_max = pkt_dev->saddr_min;
				}
			}
			rcu_read_unlock();
		} else {
			pkt_dev->saddr_min = in_aton(pkt_dev->src_min);
			pkt_dev->saddr_max = in_aton(pkt_dev->src_max);
		}

		pkt_dev->daddr_min = in_aton(pkt_dev->dst_min);
		pkt_dev->daddr_max = in_aton(pkt_dev->dst_max);
	}
	/* Initialize current values. */
	pkt_dev->cur_dst_mac_offset = 0;
	pkt_dev->cur_src_mac_offset = 0;
	pkt_dev->cur_saddr = pkt_dev->saddr_min;
	pkt_dev->cur_daddr = pkt_dev->daddr_min;
	pkt_dev->cur_udp_dst = pkt_dev->udp_dst_min;
	pkt_dev->cur_udp_src = pkt_dev->udp_src_min;
	pkt_dev->nflows = 0;
}

/**************************************************************************************************
Syntax:      	  pktgen_run(struct pktgen_thread *t)

Remarks:		  Set up structure for sending pkts, clear counters 
                  
Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static void pktgen_run(struct pktgen_thread *t)
{
	struct pktgen_dev *pkt_dev;
	int started = 0;

	printk("pktgen: entering pktgen_run. %p\n", t);

	if_lock(t);
	list_for_each_entry(pkt_dev, &t->if_list, list) {

		/*
		 * setup odev and create initial packet.
		 */
		pktgen_setup_inject(pkt_dev);

		if (pkt_dev->odev) {
			pktgen_clear_counters(pkt_dev);
			pkt_dev->running = 1;	/* Cranke yeself! */
			pkt_dev->skb = NULL;
			pkt_dev->started_at = pkt_dev->next_tx = ktime_now();

			set_pkt_overhead(pkt_dev);

			strcpy(pkt_dev->result, "Starting");
			//printk("pkt_dev->result =%s\n\r",pkt_dev->result);
			
			started++;
		} else
			strcpy(pkt_dev->result, "Error starting");
	}
	if_unlock(t);
	if (started)
		t->control &= ~(T_STOP);
}



/**************************************************************************************************
Syntax:      	  static void pktgen_xmit(struct pktgen_dev *pkt_dev)

Remarks:		  Send Packet's

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
//Создание и заполнение полей пакета ipv4 and ipv6
static struct sk_buff *fill_packet(struct net_device *odev,struct pktgen_dev *pkt_dev)
{
  printk("net_device->odev =%p,pktgen_dev->pkt_dev=%p\n\r",odev, pkt_dev);  
  return fill_packet_ipv4(odev, pkt_dev);
}



/**************************************************************************************************
Syntax:      	  static void pktgen_xmit(struct pktgen_dev *pkt_dev)

Remarks:		  Send Packet's

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static void pktgen_xmit(struct pktgen_dev *pkt_dev)
{
	struct net_device *odev = pkt_dev->odev;
	netdev_tx_t (*xmit)(struct sk_buff *, struct net_device *)
		= odev->netdev_ops->ndo_start_xmit;
	struct netdev_queue *txq;
	u16 queue_map;
	int ret;
    
	
	printk("pkt_dev->odev =%p\n\r",pkt_dev->odev);
 
    /////////////////////////////////////////////
	//printk("pkt_dev->odev =%p\n\r",pkt_dev->odev);
    printk("returning %s rx_packet=%x \n\r",odev->name,odev->stats.rx_packets);
	
	
	//
	//////////////////Create and Fill packet to Send///////////////
	pkt_dev->skb = fill_packet(odev, pkt_dev);
	if (pkt_dev->skb == NULL) 
	{
	  printk(KERN_ERR "pktgen: ERROR: couldn't allocate skb in fill_packet.\n");
	  schedule();
	  pkt_dev->clone_count--;	// back out increment, OOM 
	  return;
	}
	//pktgen_setup_inject(pkt_dev);
	//Send PAcket
	//pktgen_if_show();

 while(1)
 { 
	ret = (*xmit)(pkt_dev->skb, odev);
	
	switch (ret) 
	{
	case NETDEV_TX_OK:
	printk("++++++pktgen:NETDEV_Transmit_OK+++++++++++++++++++\n");
	break;
	
	
	case NETDEV_TX_BUSY:
	printk("++++++pktgen:NETDEV_Transmit_bUSY+++++++++++++++++++\n");
	break;
	
	default:
	printk("++++++pktgen:Unriable +++++++++++++++++++\n");
	break;
	}
 }	
	
	
#if 0	
	/*
	// If device is offline, then don't send 
	if (unlikely(!netif_running(odev) || !netif_carrier_ok(odev))) 
	{
		pktgen_stop_device(pkt_dev);
		return;
	}
    */
	
	

	
	// This is max DELAY, this has special meaning of
	// "never transmit"
	if (unlikely(pkt_dev->delay == ULLONG_MAX)) 
	{
		pkt_dev->next_tx = ktime_add_ns(ktime_now(), ULONG_MAX);
		printk("warning: pkt_dev->delay ==%d\n\r",pkt_dev->delay);
		return;
	}


	
	// If no skb or clone count exhausted then get new one 
	
	if (!pkt_dev->skb || (pkt_dev->last_ok &&
			      ++pkt_dev->clone_count >= pkt_dev->clone_skb)) 
	{
		
		
		//printk("enter: pkt_dev->skb\n\r");
		
		// build a new pkt 
		kfree_skb(pkt_dev->skb);
		//////////////////Create and Fill packet to Send///////////////
		pkt_dev->skb = fill_packet(odev, pkt_dev);
		
		
		if (pkt_dev->skb == NULL) 
		{
			printk(KERN_ERR "pktgen: ERROR: couldn't allocate skb in fill_packet.\n");
			schedule();
			pkt_dev->clone_count--;	// back out increment, OOM 
			return;
		}

		pkt_dev->allocated_skbs++;
		pkt_dev->clone_count = 0;	// reset counter 
	}

	
		
	
	if (pkt_dev->delay && pkt_dev->last_ok)
		spin(pkt_dev, pkt_dev->next_tx);

	queue_map = skb_get_queue_mapping(pkt_dev->skb);
	txq = netdev_get_tx_queue(odev, queue_map);

	__netif_tx_lock_bh(txq);
	atomic_inc(&(pkt_dev->skb->users));

	if (unlikely(netif_tx_queue_stopped(txq) || netif_tx_queue_frozen(txq)))
		ret = NETDEV_TX_BUSY;
	else
		ret = (*xmit)(pkt_dev->skb, odev);
    
	switch (ret) 
	{
	case NETDEV_TX_OK:
		txq_trans_update(txq);
		pkt_dev->last_ok = 1;
		pkt_dev->sofar++;
		pkt_dev->seq_num++;
		pkt_dev->tx_bytes += pkt_dev->cur_pkt_size;
		printk("++++++pktgen:NETDEV_TX_OK+++++++++++++++++++\n");
		
		break;
	default: // Drivers are not supposed to return other values! 
		if (net_ratelimit())
			printk("pktgen: %s xmit error: %d\n",pkt_dev->odevname, ret);
			pkt_dev->errors++;
		// fallthru 
	case NETDEV_TX_LOCKED:
	case NETDEV_TX_BUSY:
		// Retry it next time 
		printk("++++++pktgen:NETDEV_TX_BUSY+++++++++++++++++++\n");
		atomic_dec(&(pkt_dev->skb->users));
		pkt_dev->last_ok = 0;
	    
	}
	__netif_tx_unlock_bh(txq);

	// If pkt_dev->count is zero, then run forever
	if ((pkt_dev->count != 0) && (pkt_dev->sofar >= pkt_dev->count))
	{
		//pktgen_wait_for_skb(pkt_dev);
          printk("++++++++++++++++++++++++++++++++++++++++\n\r");
		
		// Done with this 
		//pktgen_stop_device(pkt_dev);
	}
#endif  

}



/**************************************************************************************************
Syntax:      	  static int pktgen_thread_worker(void *arg)

Remarks:		  Main Thread functions

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static int pktgen_thread_worker(void *arg)
{
	 // struct net_device *net;
	  //Имена наших device "eth0"<->Tsec 1 ,"eth1"<->Tsec 2,"eth2"<->Tsec 3
	  //const char *ifname="eth0";
	    const char *ifname="eth2";  
	
	pktgen_add_device(t,ifname);
      //mod_cur_headers(pkt_dev);
	  //pktgen_setup_inject(pkt_dev);
	 // pktgen_if_show();	 
	 //mod_cur_headers(pkt_dev);
	  pktgen_xmit(pkt_dev);
	//printk("pktgen: starting pktgen/%d:  pid=%d\n",cpu, task_pid_nr(current));
	//pktgen_xmit(pkt_dev);
	
	//while (!kthread_should_stop()) 
	//{
      //pktgen_run(t);      
	 // printk("Thread run++++++++\n");		
	  // pktgen_xmit(pkt_dev);
	//}
	//net=pktgen_dev_get_by_name(pkt_dev,ifname);
	//printk("returning %s dev_id=%x \n\r",net->name,net->stats.rx_packets);
	//printk("pkt_dev=%p\n",pkt_dev);
	
	return 0;
}
/**************************************************************************************************
Syntax:      	  static int __init pktgen_create_thread(int cpu)

Remarks:		  Initialization and create thread functions for send IPv4 packet

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
static int __init pktgen_create_thread(int cpu)
{
    
	t = kzalloc(sizeof(struct pktgen_thread), GFP_KERNEL);
	if (!t) {
		printk(KERN_ERR "pktgen: ERROR: out of memory, can't create new thread.\n");
		return -ENOMEM;
	}
	
	spin_lock_init(&t->if_lock);
	t->cpu = cpu;
	INIT_LIST_HEAD(&t->if_list);
	list_add_tail(&t->th_list, &pktgen_threads);
	init_completion(&t->start_done);
	

	tdm_thread_task = kthread_run(pktgen_thread_worker, NULL, "tdm_thread");
    
	if (IS_ERR(tdm_thread_task))
	{
		printk(KERN_ERR "pktgen: kernel_thread() failed for cpu\n");
		return PTR_ERR(tdm_thread_task);
	}
    
	//kthread_bind(tdm_thread_task, cpu);
	//t->tsk = tdm_thread_task;

	//wake_up_process(tdm_thread_task);
	//wait_for_completion(&t->start_done);
	
	return 0;
}

/**************************************************************************************************
Syntax:      	  SINT32 InitEthipv4drv()

Remarks:		  Init function for module send Ipv4 Packet

Return Value:	  Returns 1 on success and negative value on failure.

 				Value		 									Description
				-------------------------------------------------------------------------------------
				= 1												Success
				=-1												Failure
***************************************************************************************************/
SINT32 InitEthipv4drv()
{
int cpu;
    
    for_each_online_cpu(cpu) 
	{
		int err;
		printk("cpu_num_of_core=%d\n\r",cpu);
		err = pktgen_create_thread(cpu);
		if (err)
		{
			printk(KERN_WARNING "pktgen: WARNING: Cannot create thread for cpu %d (%d)\n", cpu, err);
	    }
	}

return 1;
}













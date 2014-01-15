/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvnbuf.h
*
* Description : Declaration Buffer's function for transmit <->recieve data on localbus P2020. 
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
* --------- $Log: mpcdrvnbuf.h $
* --------- Initial revision
******************************************************************************/

/*-----------------------------------------------------------------------------
 GENERAL NOTES
-----------------------------------------------------------------------------*/
#ifndef MPCDRVNBUF_H
#define MPCDRVNBUF_H

/*****************************************************************************/
/*	COMMON CONFIGURATION						     */
/*****************************************************************************/


/*****************************************************************************/
/*	INCLUDES							     */
//#include <linux/kfifo.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/log2.h>
/*****************************************************************************/
/*	PUBLIC MACROS							     */
/*****************************************************************************/
/*
 * Function Get DATA form 10 direction on Local BUS 
 * 
 */
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool nbuf_get_datapacket_dir1  ( unsigned char *in_buf , u16 *in_size);
bool nbuf_get_datapacket_dir2  ( unsigned char *in_buf , u16 *in_size);
bool nbuf_get_datapacket_dir3  ( unsigned char *in_buf , u16 *in_size);
bool nbuf_get_datapacket_dir4  ( unsigned char *in_buf , u16 *in_size);
bool nbuf_get_datapacket_dir5  ( unsigned char *in_buf , u16 *in_size);
bool nbuf_get_datapacket_dir6  ( unsigned char *in_buf , u16 *in_size);
bool nbuf_get_datapacket_dir7  ( unsigned char *in_buf , u16 *in_size);
bool nbuf_get_datapacket_dir8  ( unsigned char *in_buf , u16 *in_size);
//bool nbuf_get_datapacket_dir9  ( unsigned char *in_buf , u16 *in_size);
//bool nbuf_get_datapacket_dir10  (unsigned char *in_buf , u16 *in_size);
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void nbuf_set_datapacket_dir1  (const unsigned char *in_buf ,const u16 in_size);
void nbuf_set_datapacket_dir2  (const unsigned char *in_buf ,const u16 in_size);
void nbuf_set_datapacket_dir3  (const unsigned char *in_buf ,const u16 in_size);
void nbuf_set_datapacket_dir4  (const unsigned char *in_buf ,const u16 in_size);
void nbuf_set_datapacket_dir5  (const unsigned char *in_buf ,const u16 in_size);
void nbuf_set_datapacket_dir6  (const unsigned char *in_buf ,const u16 in_size);
void nbuf_set_datapacket_dir7  (const unsigned char *in_buf ,const u16 in_size);
void nbuf_set_datapacket_dir8  (const unsigned char *in_buf ,const u16 in_size);
//void nbuf_set_datapacket_dir9  (const unsigned char *in_buf ,const u16 in_size);
//void nbuf_set_datapacket_dir10 (const unsigned char *in_buf ,const u16 in_size);
/////////////////////////////////////////////////////////////////////////////////////


/*Function Ready to get Data onto the buffer */
//bool mpcfifo_get_buf_ready(struct mpcfifo *rbd_p);


/*****************************************************************************/
/*	                 EXTERN FUNCTIONS AND DATA TYPES						     */
/******************************************************************************/
void Init_FIFObuf();
void Clear_FIFObuf();

/*****************************************************************************/
/*	PUBLIC DATA TYPES						     */
/*****************************************************************************/

/*
struct mpcfifo {

	unsigned int  in;	    	//(put) data is added at offset (in % size) 
	unsigned int  out;	     	//(get) data is extracted from off. (out % size)
	unsigned int  num;       	//actual number of stored infos
	unsigned int  obj_num;   	//maximum number of objects to store
    unsigned int  obj_size[10];	 	//the size of the allocated buffer
    unsigned int  obj_put_curr_size; //size of current packet put to FIFO
    unsigned int  obj_get_curr_size; //size of current packet get from FIFO
    unsigned int  rbufd_size;   //obj_num * obj_size = size of information buffer
    unsigned char *buffer;	    //the buffer holding the data 
	spinlock_t    *lock;	    //protects concurrent modifications 
};
*/


typedef struct description_packet
{
 u16 size;	
 
 //u16 data[757];
 unsigned char data[1514];
 
}DATA_lbc;


struct mpcfifo {
	DATA_lbc      q[8];
	unsigned int  head;
	unsigned int  tail;
    u16           N;
    u16           cur_put_packet_size;
    u16           cur_get_packet_size;
    u16           fifo_pusto;
    u16           fifo_zapolneno;
    spinlock_t    *lock;
};









#if 0

extern struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size,gfp_t gfp_mask, spinlock_t *lock);
extern struct kfifo *kfifo_alloc(unsigned int size, gfp_t gfp_mask,spinlock_t *lock);
extern void   kfifo_free(struct kfifo *fifo);
extern unsigned int __kfifo_put(struct kfifo *fifo,const unsigned char *buffer, unsigned int len);
extern unsigned int __kfifo_get(struct kfifo *fifo,unsigned char *buffer, unsigned int len);

/**
 * __kfifo_reset - removes the entire FIFO contents, no locking version
 * @fifo: the fifo to be emptied.
 */
static inline void __kfifo_reset(struct kfifo *fifo)
{
	fifo->in = fifo->out = 0;
}

/**
 * kfifo_reset - removes the entire FIFO contents
 * @fifo: the fifo to be emptied.
 */
static inline void kfifo_reset(struct kfifo *fifo)
{
	unsigned long flags;

	spin_lock_irqsave(fifo->lock, flags);

	__kfifo_reset(fifo);

	spin_unlock_irqrestore(fifo->lock, flags);
}

/**
 * kfifo_put - puts some data into the FIFO
 * @fifo: the fifo to be used.
 * @buffer: the data to be added.
 * @len: the length of the data to be added.
 *
 * This function copies at most @len bytes from the @buffer into
 * the FIFO depending on the free space, and returns the number of
 * bytes copied.
 */
static inline unsigned int kfifo_put(struct kfifo *fifo,const unsigned char *buffer, unsigned int len)
{
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(fifo->lock, flags);

	ret = __kfifo_put(fifo, buffer, len);

	spin_unlock_irqrestore(fifo->lock, flags);

	return ret;
}

/**
 * kfifo_get - gets some data from the FIFO
 * @fifo: the fifo to be used.
 * @buffer: where the data must be copied.
 * @len: the size of the destination buffer.
 *
 * This function copies at most @len bytes from the FIFO into the
 * @buffer and returns the number of copied bytes.
 */
static inline unsigned int kfifo_get(struct kfifo *fifo,unsigned char *buffer, unsigned int len)
{
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(fifo->lock, flags);

	ret = __kfifo_get(fifo, buffer, len);

	/*
	 * optimization: if the FIFO is empty, set the indices to 0
	 * so we don't wrap the next time
	 */
	if (fifo->in == fifo->out)
		fifo->in = fifo->out = 0;

	spin_unlock_irqrestore(fifo->lock, flags);

	return ret;
}

/**
 * __kfifo_len - returns the number of bytes available in the FIFO, no locking version
 * @fifo: the fifo to be used.
 */
static inline unsigned int __kfifo_len(struct kfifo *fifo)
{
	return fifo->in - fifo->out;
}

/**
 * kfifo_len - returns the number of bytes available in the FIFO
 * @fifo: the fifo to be used.
 */
static inline unsigned int kfifo_len(struct kfifo *fifo)
{
	unsigned long flags;
	unsigned int ret;

	spin_lock_irqsave(fifo->lock, flags);

	ret = __kfifo_len(fifo);

	spin_unlock_irqrestore(fifo->lock, flags);

	return ret;
}

#endif




















/*****************************************************************************/
/*	PUBLIC FUNCTION PROTOTYPES					     */
/*****************************************************************************/


/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
#endif /* MPCDRVNBUF_H */


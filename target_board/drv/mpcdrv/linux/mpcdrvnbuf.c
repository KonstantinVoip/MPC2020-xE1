/**********************************************************************************************************************
*                                        (c) COPYRIGHT by OAO SUPERTEL.                                             *
*                                               All rights reserved.                                                  *
***********************************************************************************************************************
* Module      : mpcdrvnbuf.c
*
* Description : Defenition Buffer's function for transmit <->recieve data on localbus P2020.
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
* --------- $Log: mpcdrvnbuf.c $
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
#include "mpcdrvnbuf.h"
/*External Header*/

/*****************************************************************************/
/*	DEBUG MPC FIFO                          							     */
/****************************************************************************/
#define DEBUG_MPC_FIFO_BUFFER    1







/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
#define FIFO_PACKET_SIZE_BYTE  1514          
#define FIFO_PACKET_NUM        10 
#define GFP_MASK               GFP_KERNEL


///FIFO QUEUE
//10 Recieve TDM DIRECTION READ FIFO strtucrure
struct mpcfifo *fifo_tdm0_dir_read,fifo_tdm1_dir_read,fifo_tdm2_dir_read,fifo_tdm3_dir_read;

/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
struct mpcfifo *mpcfifo_init(unsigned int obj_size,unsigned int obj_num , gfp_t gfp_mask, spinlock_t *lock);

/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
//FIFO Function 
static unsigned int mpcfifo_term(struct mpcfifo *rbd_p);
static unsigned int mpcfifo_put(struct mpcfifo *rbd_p, void *obj);
static unsigned int mpcfifo_get(struct mpcfifo *rbd_p, void *obj);
static unsigned int mpcfifo_flush(struct mpcfifo *rbd_p);
static unsigned int mpcfifo_num(struct mpcfifo *rbd_p);
static unsigned int mpcfifo_print(struct mpcfifo *rbd_p, int mode);








/* KERNEL MODULE HANDLING */


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

/*****************************************************************************
Syntax:      	    void Init_FIFObuf()
Remarks:			This Function Init FIFO buffers 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				--------------------------------------------------------------
				= 1												Success
				=-1												Failure
*******************************************************************************/
void Init_FIFObuf()
{
	 spinlock_t my_lock_fifo_0;
	 memset(&fifo_tdm0_dir_read, 0x0000, sizeof(fifo_tdm0_dir_read)); 
	
	 //INIT FIFO DEIRECTION0 READ
	 fifo_tdm0_dir_read=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_0);
     if(!fifo_tdm0_dir_read) 
     {
    	printk("No FIFO _0 ENABLE\n\r");
  
     }	
	//INIT FIFO DEIRECTION1 READ
	//fifo_tdm1_dir_read=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_1);
	//INIT FIFO DEIRECTION2 READ
	//fifo_tdm2_dir_read= mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_2);
	//INIT FIFO DEIRECTION3 READ
	//fifo_tdm3_dir_read=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_3);
	//INIT FIFO DEIRECTION4 READ
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
	//INIT FIFO DEIRECTION5 READ
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
	//INIT FIFO DEIRECTION6 READ
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
	//INIT FIFO DEIRECTION7 READ
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
	//INIT FIFO DEIRECTION8 READ
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
	//INIT FIFO DEIRECTION9 READ
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
	
	printk("++++++++INITIALIZE FIFO_OK+++++++++++++\n\r");
	
	
}
/*****************************************************************************
Syntax:      	    void Init_FIFObuf()
Remarks:			This Function Init FIFO buffers 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				--------------------------------------------------------------
				= 1												Success
				=-1												Failure
*******************************************************************************/
struct mpcfifo *mpcfifo_init(unsigned int obj_size,unsigned int obj_num , gfp_t gfp_mask, spinlock_t *lock)
{
	
	struct mpcfifo *ret;
	unsigned char  *buffer;
	unsigned int    size;
	int cur;
	
	if((obj_num < 1) || (obj_size < 1)) 
	{
		printk("nrbuf_init: invalid obj_num %d or obj_size %d\n",obj_num, obj_size);
		return(NULL);
	}
	
	
	size = obj_num * obj_size;
	printk("size =%d byte\n\r",size);
	
	buffer = kmalloc(size, GFP_MASK);
	if (!buffer)
	{
		return(NULL);
	}
	
	printk("buffer kmalloc _OK\n\r");
	
	ret = kmalloc(sizeof(struct mpcfifo), GFP_MASK);
	if (!ret)
	{	
		return(NULL);
	}
	printk("mpc info kmalloc _OK\n\r");
	
	ret->in =  0;
	ret->out=  0;
	ret->num = 0;
	ret->obj_num = obj_num;
	ret->obj_size = obj_size;
	ret->rbufd_size = obj_num * obj_size;
	ret->buffer=buffer;
	ret->lock=lock;
	
	for(cur = 0; cur < ret->rbufd_size; cur++)
	{
		ret->buffer[cur] = 0;
	}
	
	
	
	return (ret);

}


/////////////////////////////////////////FIFO QUEUQ FUNCTION//////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************
Syntax:      	    static unsigned int mpcfifo_put(struct mpcfifo *rbd_p, void *obj)		 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static unsigned int mpcfifo_put(struct mpcfifo *rbd_p, void *obj)
{
	int i;
	unsigned long flags;
	
	
	spin_lock_irqsave(rbd_p->lock, flags);

	
	if(!rbd_p)
	{
	  return 0;
	}
	
	printk("current_interation  =%d\n\r",rbd_p->num );
	
	if(rbd_p->num >=rbd_p->obj_num)
	{
		printk("FIFO BUFFER is FULL STOP TOKEN\n\r");
	    return 0;
	}
	
	if(rbd_p->num == rbd_p->obj_num)
	{
	printk("[RB OV]\n");
	return 0;
	}

	smp_mb();
	
	
	for(i = 0; i < rbd_p->obj_size; i++)
	{
		rbd_p->buffer[rbd_p->in + i] = *(((unsigned char *)obj) + i);
    }
	
	rbd_p->in += rbd_p->obj_size;
	
	if(rbd_p->in == rbd_p->rbufd_size)
	{	
			rbd_p->in = 0;
	}

	smp_wmb();
	
	rbd_p->num++;	
	printk("++++mpcfifo_put_SUCCESS++++\n\r");
	
	spin_unlock_irqrestore(rbd_p->lock, flags);
	
	
	
	return 1;
	
}
/**************************************************************************************************
Syntax:      	    static unsigned int mpcfifo_get(struct mpcfifo *rbd_p, void *obj		 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static unsigned int mpcfifo_get(struct mpcfifo *rbd_p, void *obj)
{
	int i;
	unsigned long flags;
	spin_lock_irqsave(rbd_p->lock, flags);
	
	
	if(!rbd_p)
	{		
	return 0;
	}
	
	smp_rmb();
	
	if(rbd_p->num == 0)
	{
	 return 0;
	}

	for(i = 0; i < rbd_p->obj_size; i++)
	{	
		*(((unsigned char *)obj) + i) = rbd_p->buffer[rbd_p->out + i];

	}	
		
	rbd_p->out += rbd_p->obj_size;

	if(rbd_p->out == rbd_p->rbufd_size)
	{	
		rbd_p->out = 0;
	}
		
	rbd_p->num--;
	
    smp_mb();
    printk("++++mpcfifo_get_SUCCESS+++\n\r");	
   
   spin_unlock_irqrestore(rbd_p->lock, flags);
   
   return 1;
}


/**************************************************************************************************
Syntax:      	    static unsigned int mpcfifo_term(struct mpcfifo *rbd_p)		 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static unsigned int mpcfifo_term(struct mpcfifo *rbd_p)
{
	
	
	
}


/**************************************************************************************************
Syntax:      	    static unsigned int mpcfifo_flush(struct mpcfifo *rbd_p)		 
Return Value:	    Returns 1 on success and negative value on failure.
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static unsigned int mpcfifo_flush(struct mpcfifo *rbd_p)
{
	
	
}


/**************************************************************************************************
Syntax:      	    static unsigned int mpcfifo_num(struct mpcfifo *rbd_p)			 
Return Value:	    get number of actually stored info elements
 				    Value		 									Description
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static unsigned int mpcfifo_num(struct mpcfifo *rbd_p)
{
	
	
	
}


/**************************************************************************************************
Syntax:      	    mpcfifo_print(struct mpcfifo *rbd_p, int mode)		 
Return Value:	    Print FIFO buffer content if debug ENABLED
 				    Value		 									Description
				    mode: 0 => do not print buffer contents
		      	  		  1 => print infos stored in buffer
		      	  		  2 => print whole information buffer	   
				   -------------------------------------------------------------------------------------
				   = 1												Success
				   =-1												Failure
***************************************************************************************************/
static unsigned int mpcfifo_print(struct mpcfifo *rbd_p, int mode)
{
	
	int i, j;
	int cur;

	if(!rbd_p) 
	{
		printk("mpcfifo_print: no ring buffer\n");
		return 0 ;
	}
	
	/* print NRBUF's contents */
	printk("  nrbuf_print: rbd_p at 0x%lx of size %d mode %d\n",(unsigned long)rbd_p, rbd_p->rbufd_size, mode);
	printk("  put position     : %d\n", rbd_p->in);
	printk("  get position     : %d\n", rbd_p->out);
	printk("  number of objects: %d (max %d of size %d)\n",rbd_p->num, rbd_p->obj_num, rbd_p->obj_size);

	switch(mode) 
	{
		case 0:				 /* no buffer print - return */
			break;

		case 1:			  /* print elements stored in buffer */
			if(rbd_p->num == 0) 
			{
				printk("  no elements in buffer\n");
				break;
			}

			cur = rbd_p->out;

			for(i = 0; i < rbd_p->num; i++) 
			{
				printk("  [%d]:", cur / rbd_p->obj_size);

				for(j = 0; j < rbd_p->obj_size; j++) {
					printk(" %02x", rbd_p->buffer[cur]);
					cur++;
				}
				printk("\n");

				if(cur == rbd_p->rbufd_size)
					cur = 0;
			}
			break;

		case 2:			      /* print whole buffer contents */
			cur = 0;

			for(i = 0; i < rbd_p->obj_num; i++) {
				printk("  [%d]:", i);

				for(j = 0; j < rbd_p->obj_size; j++) {
					printk(" %02x", rbd_p->buffer[cur]);
					cur++;
				}
				printk("\n");
			}
			break;

		default:
			printk("nrbuf_print: invalid mode %d\n", mode);
			break;
	}
	
 return 1;
}


/////////////////////External FUNCTION for GET DATA only FIFO's////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
Syntax:      	    nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size)
{
	u16 out_buf[757];
	u16  packet_size_hex=756;
	u16 status;
	
	printk("+++SET TO FIFO BUFFER DIRECTION0++++\n\r");
	
	//FILL struct FIFO 
	fifo_tdm0_dir_read ->obj_size=in_size;
	//Set to the FIFO buffer
	status=mpcfifo_put(fifo_tdm0_dir_read, in_buf);
    
	//mpcfifo_print(fifo_tdm0_dir_read, 0);
	
	
	mpcfifo_print(fifo_tdm0_dir_read, 0);
	
	 mpcfifo_get(fifo_tdm0_dir_read, out_buf);
	//mpcfifo_print(fifo_tdm0_dir_read, 0);

	
	printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	
	
	
	
	
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir1 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir1 (const u16 *in_buf ,const u16 in_size)
{
		
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir2 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir2 (const u16 *in_buf ,const u16 in_size)
{
		
}
/*****************************************************************************
Syntax:      	    nbuf_get_datapacket_dir3 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir3 (const u16 *in_buf ,const u16 in_size)
{
		
}

/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir4 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir4 (const u16 *in_buf ,const u16 in_size)
{
		
}
/*****************************************************************************
Syntax:      	   void nbuf_get_datapacket_dir5 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir5 (const u16 *in_buf ,const u16 in_size)
{
		
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir6 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir6 (const u16 *in_buf ,const u16 in_size)
{
		
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir7 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir7 (const u16 *in_buf ,const u16 in_size)
{
		
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir8 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir8 (const u16 *in_buf ,const u16 in_size)
{
		
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir9 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir9 (const u16 *in_buf ,const u16 in_size)
{
		
}

















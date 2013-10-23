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
//10 Transmit to TDM DIRECTION FIFO strtucrure.
struct mpcfifo *fifo_put_to_tdm0_dir,*fifo_put_to_tdm1_dir,*fifo_put_to_tdm2_dir,*fifo_put_to_tdm3_dir;


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
extern void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len);

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
	 spinlock_t my_lock_fifo_0,my_lock_fifo_1,my_lock_fifo_2,my_lock_fifo_3;
	 memset(&fifo_put_to_tdm0_dir, 0x0000, sizeof(fifo_put_to_tdm0_dir)); 
	
	 /*
	 memset(&fifo_put_to_tdm1_dir, 0x0000, sizeof(fifo_put_to_tdm1_dir)); 
	 memset(&fifo_put_to_tdm2_dir, 0x0000, sizeof(fifo_put_to_tdm2_dir)); 
	 memset(&fifo_put_to_tdm3_dir, 0x0000, sizeof(fifo_put_to_tdm3_dir)); 
	 */
	 //Add to 10 directions only futures
	 /*
	 memset(&fifo_put_to_tdm4_dir, 0x0000, sizeof(fifo_put_to_tdm0_dir)); 
	 memset(&fifo_put_to_tdm5_dir, 0x0000, sizeof(fifo_put_to_tdm0_dir)); 
	 memset(&fifo_put_to_tdm6_dir, 0x0000, sizeof(fifo_put_to_tdm0_dir)); 
	 memset(&fifo_put_to_tdm7_dir, 0x0000, sizeof(fifo_put_to_tdm0_dir)); 
	 memset(&fifo_put_to_tdm8_dir, 0x0000, sizeof(fifo_put_to_tdm0_dir)); 
	 */
	 
	 
	 
	 
	 //INIT FIFO DEIRECTION0 PUT to direction 0
	 fifo_put_to_tdm0_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_0);
     if(!fifo_put_to_tdm0_dir) {printk("???Transmit FIFO DIRECTION_0 Initialization Failed???\n\r");}	
     printk("FIFO_0_TRANSMIT_INIT_OK\n\r");
 
/*
     //INIT FIFO DEIRECTION1 PUT to direction 1
	 fifo_put_to_tdm1_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_1);
     if(!fifo_put_to_tdm1_dir) {printk("???Transmit FIFO DIRECTION_1 Initialization Failed??? \n\r");}	
     printk("FIFO_1_TRANSMIT_INIT_OK\n\r");
     
     //INIT FIFO DEIRECTION2 PUT to direction 2
	 fifo_put_to_tdm2_dir= mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_2);
     if(!fifo_put_to_tdm2_dir) {printk("???Transmit FIFO DIRECTION_2 Initialization Failed??? \n\r");}
     printk("FIFO_2_TRANSMIT_INIT_OK\n\r");  

     //INIT FIFO DEIRECTION3 PUT to direction 3
	 fifo_put_to_tdm3_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_3);
     if(!fifo_put_to_tdm3_dir) {printk("???Transmit FIFO DIRECTION_3 Initialization Failed??? \n\r");}   
     printk("FIFO_3_TRANSMIT_INIT_OK\n\r"); 
*/    
     
     //Only Future 10 directions
     /*
     //INIT FIFO DEIRECTION4 PUT to direction 4
	  fifo_put_to_tdm3_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
     //INIT FIFO DEIRECTION5 PUT to direction 5
	 //mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
     //INIT FIFO DEIRECTION6 PUT to direction 6
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
     //INIT FIFO DEIRECTION7 PUT to direction 7
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
     //INIT FIFO DEIRECTION8 PUT to direction 8
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
     //INIT FIFO DEIRECTION9 PUT to direction 9
	//mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock);
	*/
	
	
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
	
	if(rbd_p->num==10)
	{
		
		printk("+++mpcfifo_put_full_fifo++\n\r");
		return 0;
	}
	
	if(!rbd_p)
	{
	  return 0;
	}
	
	printk("++mpcfifo_put_iteration=%d++\n\r",rbd_p->num );
	
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
Remark              
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
    printk("++mpcfifo_get_sucess++\n\r");	
   
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
Remarks:			get data from FIFO buffer
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size)
{
	 u16 out_buf[757];
	 u16  packet_size_hex=0;
	 
	 
	 //u16 status;
	 mpcfifo_get(fifo_put_to_tdm0_dir, out_buf);
	 printk("+nbuf_get_datapacket_dir0_sizein_byte=%d+\n\r",fifo_put_to_tdm0_dir->obj_size);
	 packet_size_hex=(fifo_put_to_tdm0_dir->obj_size)/2;
	 
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
 
	 printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
 	 printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-6],out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1]);
	 
	//printk("+++SET TO FIFO BUFFER DIRECTION0++++\n\r");
	
     
 	 //отправляю в ethernet
 	  p2020_get_recieve_virttsec_packet_buf(out_buf,fifo_put_to_tdm0_dir->obj_size);//send to eternet
	
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
	
	 //mpcfifo_get(fifo_tdm0_dir_read, out_buf);
	//mpcfifo_print(fifo_tdm0_dir_read, 0);

	
	//printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	//printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1],out_buf[packet_size_hex]);
	
	
	
	
	
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



////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////SET DATA PACKET TO FIFO BUFFER//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////



/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir0  (const u16 *in_buf ,const u16 in_siz
Remarks:			set data to fifo buffer
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir0  (const u16 *in_buf ,const u16 in_size)
{
u16 status;
	
	//FILL struct FIFO 
     fifo_put_to_tdm0_dir ->obj_size=in_size;
	//Set to the FIFO buffer
	 status=mpcfifo_put(fifo_put_to_tdm0_dir, in_buf);  
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir1  (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir1  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir2  (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir2  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir3  (const u16 *in_buf ,const u16 in_size
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir3  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:             void nbuf_set_datapacket_dir4  (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir4  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir5  (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir5  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir9 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir6  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir7  (const u16 *in_buf ,const u16 in_size))
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir7  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir8  (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir8  (const u16 *in_buf ,const u16 in_size)
{
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir9  (const u16 *in_buf ,const u16 in_size
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir9  (const u16 *in_buf ,const u16 in_size)
{
	
}














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
#include <linux/delay.h>  
/*****************************************************************************/
/*	INCLUDES							     */
/*****************************************************************************/
#include "mpcdrvnbuf.h"
//#include <linux/spinlock.h>
/*External Header*/

/*****************************************************************************/
/*	DEBUG MPC FIFO                          							     */
/****************************************************************************/
#define DEBUG_MPC_FIFO_BUFFER    1
//#define DEBUG_GET_FIFO_SEND_TO_ETHERNET 1


/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
#define FIFO_PACKET_SIZE_BYTE  1514          
#define FIFO_PACKET_NUM        8
#define GFP_MASK               GFP_KERNEL


///FIFO QUEUE
//10 Transmit to TDM DIRECTION FIFO strtucrure.
struct mpcfifo *fifo_put_to_tdm1_dir,*fifo_put_to_tdm2_dir,*fifo_put_to_tdm3_dir,*fifo_put_to_tdm4_dir,*fifo_put_to_tdm5_dir,*fifo_put_to_tdm6_dir,*fifo_put_to_tdm7_dir,*fifo_put_to_tdm8_dir;


/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
struct mpcfifo *mpcfifo_init(unsigned int obj_size,unsigned int obj_num , gfp_t gfp_mask, spinlock_t *lock);

/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
//FIFO Function 
static unsigned int mpcfifo_term(struct mpcfifo *rbd_p);
//static unsigned int mpcfifo_put(struct mpcfifo *rbd_p, void *obj);
//static unsigned int mpcfifo_put(struct mpcfifo *rbd_p,const u16 *buf);
   
static unsigned int mpcfifo_put(struct mpcfifo *rbd_p,const unsigned char *buf);
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
extern void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len,u8 tsec_id);

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
	 spinlock_t my_lock_fifo_1,my_lock_fifo_2,my_lock_fifo_3,my_lock_fifo_4,my_lock_fifo_5,my_lock_fifo_6,my_lock_fifo_7,my_lock_fifo_8;		 
	 memset(&fifo_put_to_tdm1_dir, 0x0000, sizeof(fifo_put_to_tdm1_dir)); 
	 memset(&fifo_put_to_tdm2_dir, 0x0000, sizeof(fifo_put_to_tdm2_dir)); 
	 memset(&fifo_put_to_tdm3_dir, 0x0000, sizeof(fifo_put_to_tdm3_dir)); 
	 memset(&fifo_put_to_tdm4_dir, 0x0000, sizeof(fifo_put_to_tdm4_dir)); 
	 memset(&fifo_put_to_tdm5_dir, 0x0000, sizeof(fifo_put_to_tdm5_dir)); 
	 memset(&fifo_put_to_tdm6_dir, 0x0000, sizeof(fifo_put_to_tdm6_dir)); 
	 memset(&fifo_put_to_tdm7_dir, 0x0000, sizeof(fifo_put_to_tdm7_dir)); 
	 memset(&fifo_put_to_tdm8_dir, 0x0000, sizeof(fifo_put_to_tdm8_dir)); 
	 
	 //INIT FIFO DEIRECTION0 PUT to direction 0
	 fifo_put_to_tdm1_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_1);
     if(!fifo_put_to_tdm1_dir) {printk("???Transmit FIFO DIRECTION_0 Initialization Failed???\n\r");}	
     printk("+FIFO_1_TRANSMIT_INIT_OK+\n\r");
 
     //INIT FIFO DEIRECTION1 PUT to direction 1
	 fifo_put_to_tdm2_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_2);
     if(!fifo_put_to_tdm2_dir) {printk("???Transmit FIFO DIRECTION_1 Initialization Failed??? \n\r");}	
     printk("+FIFO_2_TRANSMIT_INIT_OK+\n\r");
     
     //INIT FIFO DEIRECTION2 PUT to direction 2
	 fifo_put_to_tdm3_dir= mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_3);
     if(!fifo_put_to_tdm3_dir) {printk("???Transmit FIFO DIRECTION_2 Initialization Failed??? \n\r");}
     printk("+FIFO_3_TRANSMIT_INIT_OK+\n\r");  

     //INIT FIFO DEIRECTION3 PUT to direction 3
	 fifo_put_to_tdm4_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_4);
     if(!fifo_put_to_tdm4_dir) {printk("???Transmit FIFO DIRECTION_4 Initialization Failed??? \n\r");}   
     printk("+FIFO_4_TRANSMIT_INIT_OK+\n\r"); 
    
	 fifo_put_to_tdm5_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_5);
     if(!fifo_put_to_tdm5_dir) {printk("???Transmit FIFO DIRECTION_5 Initialization Failed??? \n\r");}   
     printk("+FIFO_5_TRANSMIT_INIT_OK+\n\r");
     
	 fifo_put_to_tdm6_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_6);
     if(!fifo_put_to_tdm6_dir) {printk("???Transmit FIFO DIRECTION_6 Initialization Failed??? \n\r");}   
     printk("+FIFO_6_TRANSMIT_INIT_OK+\n\r");
     
	 fifo_put_to_tdm7_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_7);
     if(!fifo_put_to_tdm7_dir) {printk("???Transmit FIFO DIRECTION_7 Initialization Failed??? \n\r");}   
     printk("+FIFO_7_TRANSMIT_INIT_OK+\n\r");
     
	 fifo_put_to_tdm8_dir=mpcfifo_init(FIFO_PACKET_SIZE_BYTE,FIFO_PACKET_NUM ,GFP_MASK,&my_lock_fifo_8);
     if(!fifo_put_to_tdm8_dir) {printk("???Transmit FIFO DIRECTION_8 Initialization Failed??? \n\r");}   
     printk("+FIFO_8_TRANSMIT_INIT_OK+\n\r");
     //Only Future 10 directions
}

/*****************************************************************************
Syntax:      	    void Clear_FIFObuf()
Remarks:			This Function Clear FIFO buffer for fifo_put_to_tdm0_dir 
Return Value:	    Returns 1 on success and negative value on failure.
 				Value		 									Description
				--------------------------------------------------------------
				= 1												Success
				=-1												Failure
*******************************************************************************/
void Clear_FIFObuf()
{
	
	kfree(fifo_put_to_tdm1_dir);
	kfree(fifo_put_to_tdm2_dir);
	kfree(fifo_put_to_tdm3_dir);
	kfree(fifo_put_to_tdm4_dir);
	kfree(fifo_put_to_tdm5_dir);
	kfree(fifo_put_to_tdm6_dir);
	kfree(fifo_put_to_tdm7_dir);
	kfree(fifo_put_to_tdm8_dir);
	printk("+free_fifo_buffer_OK+\n\r");
	
}





/*****************************************************************************
Syntax:      	    struct mpcfifo *mpcfifo_init(unsigned int obj_size,unsigned int obj_num , gfp_t gfp_mask, spinlock_t *loc
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

    /////////////////Initialize structure mpcfifo buffer	
	ret = kmalloc(sizeof(struct mpcfifo), GFP_MASK);
	if (!ret)
	{	
		return(NULL);
	}
	
	ret->tail=0;
	ret->N=obj_num/*+1*/; //потом разберусь
	ret->head=ret->N;
	
	ret->cur_put_packet_size=0;
	ret->cur_get_packet_size=0;

	
	ret->lock = lock;
	
	ret->fifo_pusto=0;
	ret->fifo_zapolneno=0;
	
	
	printk("+mpcfifo_init_alocate_memory_for_structure+\n\r");
	
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
//static unsigned int mpcfifo_put(struct mpcfifo *rbd_p,const u16 *buf)
static unsigned int mpcfifo_put(struct mpcfifo *rbd_p,const unsigned char *buf)
{
	u16 i;
	DATA_lbc  ps;
	
	//Нет указателя на FIFO выходим из очереди
	if(!rbd_p){return 0;}
	
	//Естественно нужна защита от одновременного доступа к даннымю потом сделаю! пока так.
		
	ps.size=rbd_p->cur_put_packet_size;
	
	for(i=0;i<ps.size;i++)
	{
		//ps.data[i]=(u16)buf[i];
		ps.data[i]=buf[i];   
	}
	
	rbd_p->q[rbd_p->tail++]=ps;
	
	
	//rbd_p->tail=rbd_p->N -rbd_p->head;
	
	rbd_p->tail=rbd_p->tail %rbd_p->N; //глубина очереди 10 элементов потом обнуляем хвост в 0 на начало.
	
	//printk("++mpcfifo_put_OK!++\n\r");
	
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
	u16 i;
	signed int schetchic1=0;
	signed int schetchic=0;
	DATA_lbc local;
	unsigned long flags;
	signed int l_tail=0;
	
	
	//проверяем условие что нормальный указатель в памяти не хлам
	if(!rbd_p)
	{return 0;}
	
	
	l_tail=rbd_p->tail;
    //Буфер пуст взять нечего назад ноль видимо 

	
	
	
	//printk("l_tail=%d>>rbd_p->head=%d\n\r",l_tail,rbd_p->head);
	
	if (l_tail -rbd_p->head ==0)
	{
		//mdelay(500);
		return 0;
	}
	
	
	
	//schetchic1=rbd_p->head;//-rbd_p->tail;
	if(rbd_p->head-l_tail==FIFO_PACKET_NUM)
	{
		//mdelay(500);
		//printk(">>>>>>?????????<<<<<\n\r");
		return 0;
	}
	

	rbd_p->head =rbd_p->head %rbd_p->N;
	
	local.size = rbd_p->q[rbd_p->head].size;
	
	rbd_p->cur_get_packet_size=rbd_p->q[rbd_p->head].size;
	
	for(i=0;i<local.size;i++)
	{
		//*(((u16 *)obj) + i) = rbd_p->q[rbd_p->head].data[i];
		  *(((u8 *)obj) + i) = rbd_p->q[rbd_p->head].data[i];
	
	}
	rbd_p->head++;
	
	
	//mdelay(500);
	//printk("++mpc_fifo_get_ok++\n\r");
	
   /*	
   printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r", local.data[0], local.data[1], local.data[2], local.data[3], local.data[4], local.data[5]);
   printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r", local.data[rbd_p->cur_get_packet_size-6], local.data[rbd_p->cur_get_packet_size-5], local.data[rbd_p->cur_get_packet_size-4], local.data[rbd_p->cur_get_packet_size-3], local.data[rbd_p->cur_get_packet_size-2], local.data[rbd_p->cur_get_packet_size-1]);
   */	
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

#if 0	
	
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
	
#endif 
	
 return 1;
}


/////////////////////External FUNCTION for GET DATA only FIFO's////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
Syntax:      	    nbuf_get_datapacket_dir1 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data from FIFO buffer
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
//bool nbuf_get_datapacket_dir0 (u16 *in_buf ,u16 *in_size)
bool nbuf_get_datapacket_dir1 (unsigned char *in_buf ,u16 *in_size)
{
	 //u16 out_buf_dir0[757];
	 //u16  packet_size_hex=0;
	// u16  packet_size_in_byte=0;
	 u16  static get_iteration_dir1=0;
	 unsigned long flags;
	 u16  status=0;
	 
	// printk(">>>>>>>>>>>>++++++++++++++++++++<<<<<<<<<<<<<<<<\n\r");
	 spin_lock_irqsave(fifo_put_to_tdm1_dir->lock,flags);
//#if 0	 
	// printk(">>>>>>>>>>>>nbuf_get_datapacket_dir0|Iter=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir0);
	 status=mpcfifo_get(fifo_put_to_tdm1_dir, in_buf);
	 //printk(">>>>>>>>>>>>+++++++status=%d++++++++++++<<<<<<<<<<<<<<<<\n\r",status);
    
	  
	 if(status==0)
	   {
			 memset(&in_buf,0x0000,sizeof(in_buf));
			 *in_size=0x0000;
			 spin_unlock_irqrestore(fifo_put_to_tdm1_dir->lock,flags);
			 return 0;	 
	   }
	 *in_size=fifo_put_to_tdm1_dir->cur_get_packet_size;
	 spin_unlock_irqrestore(fifo_put_to_tdm1_dir->lock,flags);
	//printk(">>>>>>>>>>>>nbuf_get_datapacket_dir0|iter=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir0);
	 /*
	 packet_size_in_byte=(fifo_put_to_tdm0_dir->cur_get_packet_size)*2;
	 *in_size=packet_size_in_byte;
	 */
	 
	 //*in_size=fifo_put_to_tdm0_dir->cur_get_packet_size;
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir0|iter=%d|size=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir0,fifo_put_to_tdm0_dir->cur_get_packet_size);
	 
	 

	
#ifdef DEBUG_GET_FIFO_SEND_TO_ETHERNET 
	 p2020_get_recieve_virttsec_packet_buf(out_buf_dir0,packet_size_in_byte);//send to eternet	 
#endif	 
 	 

//#endif	 
 	 get_iteration_dir1++;
     return status;
 	 
	 /* 
	 printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
 	 printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-6],out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1]);
	 */
	
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir2 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir2 (unsigned char *in_buf ,u16 *in_size)
{
	 //u16 out_buf_dir1[757];
	 //u16  packet_size_hex=0;
	 //u16  packet_size_in_byte;
	 u16  status=0;
	 u16  static get_iteration_dir2=0;
	 unsigned long flags;
	 
	 spin_lock_irqsave(fifo_put_to_tdm2_dir->lock,flags);
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir1 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir1);
	 //u16 status;
	 status=mpcfifo_get(fifo_put_to_tdm2_dir, in_buf);
	 //printk(">>>>>>>>>>>>+++++++status=%d++++++++++++<<<<<<<<<<<<<<<<\n\r",status);
	 if(status==0)
	   {
			 memset(&in_buf,0x0000,sizeof(in_buf));
			 *in_size=0x0000;
			 spin_unlock_irqrestore(fifo_put_to_tdm2_dir->lock,flags);
			 return 0;
			 
	   }
	 *in_size=fifo_put_to_tdm2_dir->cur_get_packet_size; 
	 spin_unlock_irqrestore(fifo_put_to_tdm2_dir->lock,flags);
	 
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir1|iter=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir1);
	//packet_size_in_byte=(fifo_put_to_tdm1_dir->cur_get_packet_size)*2;
	//*in_size=packet_size_in_byte;
	
	 
	 
	 //отправляю в ethernet
	 
#ifdef DEBUG_GET_FIFO_SEND_TO_ETHERNET 
	   p2020_get_recieve_virttsec_packet_buf(out_buf_dir1,packet_size_in_byte);//send to eternet	 
#endif	 
	 
	   
	 get_iteration_dir2++;
     return status;
	 /* 
	 printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	 printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-6],out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1]);
	 */
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir3 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir3 (unsigned char *in_buf , u16 *in_size)
{
	 //u16  out_buf_dir2[757];
	// u16  packet_size_hex=0;
	 //u16  packet_size_in_byte;
	 u16  status=0;
	 u16  static get_iteration_dir3=0;
	 unsigned long flags;
	 
	 
	 spin_lock_irqsave(fifo_put_to_tdm3_dir->lock,flags);
	 status=mpcfifo_get(fifo_put_to_tdm3_dir, in_buf);
	 //printk(">>>>>>>>>>>>+++++++status=%d++++++++++++<<<<<<<<<<<<<<<<\n\r",status);
	 if(status==0)
	   {
			 memset(&in_buf,0x0000,sizeof(in_buf));
			 *in_size=0x0000;
			 spin_unlock_irqrestore(fifo_put_to_tdm3_dir->lock,flags);
			 return 0;
			 
	   }
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir2 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir2);
	 //packet_size_in_byte=(fifo_put_to_tdm2_dir->cur_get_packet_size)*2;
	 //*in_size=packet_size_in_byte;   
	 *in_size=fifo_put_to_tdm3_dir->cur_get_packet_size;
	 spin_unlock_irqrestore(fifo_put_to_tdm3_dir->lock,flags);
	 
	 
	  //mdelay(250);
	 //отправляю в ethernet
#ifdef DEBUG_GET_FIFO_SEND_TO_ETHERNET  
	 p2020_get_recieve_virttsec_packet_buf(out_buf_dir2,packet_size_in_byte);//send to eternet	 
#endif	 
	 
	 get_iteration_dir3++;
     return status;
	 /* 
	 printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	 printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-6],out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1]);
	 */
		
}
/*****************************************************************************
Syntax:      	    nbuf_get_datapacket_dir4 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir4 (unsigned char *in_buf , u16 *in_size)
{
	 //u16 out_buf_dir3[757];
	// u16  packet_size_hex=0;
	 //u16  packet_size_in_byte;
	 u16  static get_iteration_dir4=0;
	 u16 status=0;
	 unsigned long flags;
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir3 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir3);
	 
	 
	 spin_lock_irqsave(fifo_put_to_tdm4_dir->lock,flags);
	 status=mpcfifo_get(fifo_put_to_tdm4_dir, in_buf);
	 if(status==0)
	 {
		 memset(&in_buf,0x0000,sizeof(in_buf));
		 *in_size=0x0000;
		 spin_unlock_irqrestore(fifo_put_to_tdm4_dir->lock,flags);
		 return 0; 
	 }
	 
	 *in_size=fifo_put_to_tdm4_dir->cur_get_packet_size; 
	 spin_unlock_irqrestore(fifo_put_to_tdm4_dir->lock,flags);
	// printk(">>>>>>>>>>>>nbuf_get_datapacket_dir3 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir3);
	// packet_size_in_byte=(fifo_put_to_tdm3_dir->cur_get_packet_size)*2;
	// *in_size=packet_size_in_byte;
		 
	 //Copy Massive to out functions
	 //*in_size=packet_size_in_byte;
	 //*in_buf=out_buf_dir3;
	 //отправляю в ethernet	 
#ifdef DEBUG_GET_FIFO_SEND_TO_ETHERNET
	   p2020_get_recieve_virttsec_packet_buf(out_buf_dir3,packet_size_in_byte);//send to eternet	 
#endif
	 
	 get_iteration_dir4++;
     return status;	 
	 /* 
	 printk("+FIFO_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[0],out_buf[1],out_buf[2],out_buf[3],out_buf[4],out_buf[5]);
	 printk("+FIFO_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",out_buf[packet_size_hex-6],out_buf[packet_size_hex-5],out_buf[packet_size_hex-4],out_buf[packet_size_hex-3],out_buf[packet_size_hex-2],out_buf[packet_size_hex-1]);
	 */
	
}

/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir5 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir5 (unsigned char *in_buf , u16 *in_size)
{
	 u16  static get_iteration_dir5=0;
	 u16 status=0;
	 unsigned long flags;
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir3 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir3);
	
	 spin_lock_irqsave(fifo_put_to_tdm5_dir->lock,flags);
	 status=mpcfifo_get(fifo_put_to_tdm5_dir, in_buf);
	 if(status==0)
	 {
		 memset(&in_buf,0x0000,sizeof(in_buf));
		 *in_size=0x0000;
		 spin_unlock_irqrestore(fifo_put_to_tdm5_dir->lock,flags);
		 return 0; 
	 }
	 
	 *in_size=fifo_put_to_tdm5_dir->cur_get_packet_size; 
	 spin_unlock_irqrestore(fifo_put_to_tdm5_dir->lock,flags);
	 get_iteration_dir5++;
  return status; 
	 
}
/*****************************************************************************
Syntax:      	   void nbuf_get_datapacket_dir6 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir6 (unsigned char *in_buf , u16 *in_size)
{
	 
	 u16  static get_iteration_dir6=0;
	 u16 status=0;
	 unsigned long flags;
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir3 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir3);
	 
	 
	 spin_lock_irqsave(fifo_put_to_tdm6_dir->lock,flags);
	 status=mpcfifo_get(fifo_put_to_tdm6_dir, in_buf);
	 if(status==0)
	 {
		 memset(&in_buf,0x0000,sizeof(in_buf));
		 *in_size=0x0000;
		 spin_unlock_irqrestore(fifo_put_to_tdm6_dir->lock,flags);
		 return 0; 
	 }
	 
	 *in_size=fifo_put_to_tdm6_dir->cur_get_packet_size; 
	 spin_unlock_irqrestore(fifo_put_to_tdm6_dir->lock,flags);
	 get_iteration_dir6++;
   return status;

}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir7 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir7 ( unsigned char *in_buf , u16 *in_size)
{
	 u16  static get_iteration_dir7=0;
	 u16 status=0;
	 unsigned long flags;
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir3 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir3);
	 
	 
	 spin_lock_irqsave(fifo_put_to_tdm7_dir->lock,flags);
	 status=mpcfifo_get(fifo_put_to_tdm7_dir, in_buf);
	 if(status==0)
	 {
		 memset(&in_buf,0x0000,sizeof(in_buf));
		 *in_size=0x0000;
		 spin_unlock_irqrestore(fifo_put_to_tdm7_dir->lock,flags);
		 return 0; 
	 }
	 
	 *in_size=fifo_put_to_tdm7_dir->cur_get_packet_size; 
	 spin_unlock_irqrestore(fifo_put_to_tdm7_dir->lock,flags);

	 get_iteration_dir7++; 
  return status;
	 
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir8 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir8 ( unsigned char *in_buf , u16 *in_size)
{
	 u16  static get_iteration_dir8=0;
	 u16 status=0;
	 unsigned long flags;
	 //printk(">>>>>>>>>>>>nbuf_get_datapacket_dir3 Iteration=%d<<<<<<<<<<<<<<<<\n\r",get_iteration_dir3);
	 
	 
	 spin_lock_irqsave(fifo_put_to_tdm8_dir->lock,flags);
	 status=mpcfifo_get(fifo_put_to_tdm8_dir, in_buf);
	 if(status==0)
	 {
		 memset(&in_buf,0x0000,sizeof(in_buf));
		 *in_size=0x0000;
		 spin_unlock_irqrestore(fifo_put_to_tdm8_dir->lock,flags);
		 return 0; 
	 }
	 
	 *in_size=fifo_put_to_tdm8_dir->cur_get_packet_size; 
	 spin_unlock_irqrestore(fifo_put_to_tdm8_dir->lock,flags);
	 
	 get_iteration_dir8++;
  return status;

}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir8 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir9 ( unsigned char *in_buf , u16 *in_size)
{
		return 0;
}
/*****************************************************************************
Syntax:      	    void nbuf_get_datapacket_dir9 (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
bool nbuf_get_datapacket_dir10 ( unsigned char *in_buf , u16 *in_size)
{
	    return 0;
	
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
void nbuf_set_datapacket_dir1  (const unsigned char *in_buf ,const u16 in_size)
{
u16 status=0;
u16 static set_iteration_dir1=0;  
unsigned long flags;
//u8 dop_nechet_packet =0;    
     //printk(">>>>>>>>>>>>>>nbuf_set_datapacket_dir0|iter=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir0);	 
     //set packet size to fifo buffer

/*
if((in_size)%2==1)
{ 
	printk("+odd_packet=%d+\n\r",in_size); 
    //пропускаю только пакеты в заголовке ethernet type =0x0800 ARP имеет 0x0806 ETH_P_ARP
	dop_nechet_packet=1;
}
in_size = in_size+dop_nechet_packet;
*/ 
     
     spin_lock_irqsave(fifo_put_to_tdm1_dir->lock,flags);
     
     //comment kosta for hex 01.12.13
     //fifo_put_to_tdm0_dir->cur_put_packet_size=in_size/2;
     
     // printk(">>>>>>>>>>>>>>nbuf_set_datapacket_dir0|iter=%d|size=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir0,in_size);	
      fifo_put_to_tdm1_dir->cur_put_packet_size=in_size;
     
     //Set to the FIFO buffer
	  status=mpcfifo_put(fifo_put_to_tdm1_dir, in_buf);  
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
	  spin_unlock_irqrestore(fifo_put_to_tdm1_dir->lock,flags);
	  
	  
	  set_iteration_dir1++;

}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir1  (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo direction1
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir2  (const unsigned char *in_buf ,const u16 in_size)
{
	u16 status=0;
	u16 static set_iteration_dir2=0;  
	unsigned long flags;
	    // printk(">>.>>>>>>>>>>>>nbuf_set_datapacket_dir1|iter=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir1);	 
	     //set packet size to fifo buffer
	     spin_lock_irqsave(fifo_put_to_tdm2_dir->lock,flags);
	     //fifo_put_to_tdm1_dir->cur_put_packet_size=in_size/2;
	       fifo_put_to_tdm2_dir->cur_put_packet_size=in_size;
	     //Set to the FIFO buffer
		  status=mpcfifo_put(fifo_put_to_tdm2_dir, in_buf);  
		  spin_unlock_irqrestore(fifo_put_to_tdm2_dir->lock,flags); 
		  //mpcfifo_print(fifo_tdm0_dir_read, 0);
		  set_iteration_dir2++;
  
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir3  (const u16 *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo direction2
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir3  (const unsigned char *in_buf ,const u16 in_size)
{
	u16 status=0;
	u16 static set_iteration_dir3=0;  
	unsigned long flags;
	    
	
	    //printk(">>.>>>>>>>>>>>>nbuf_set_datapacket_dir2|iter=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir2);	 
	     
	     
	     spin_lock_irqsave(fifo_put_to_tdm3_dir->lock,flags);
	     //set packet size to fifo buffer
	     //fifo_put_to_tdm2_dir->cur_put_packet_size=in_size/2;
	       fifo_put_to_tdm3_dir->cur_put_packet_size=in_size;
	       
	     
	     //Set to the FIFO buffer
		  status=mpcfifo_put(fifo_put_to_tdm3_dir, in_buf);  
		 
		  
		  spin_unlock_irqrestore(fifo_put_to_tdm3_dir->lock,flags);
		  
		  //mpcfifo_print(fifo_tdm0_dir_read, 0);
		 set_iteration_dir3++;
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir3  (const u16 *in_buf ,const u16 in_size
Remarks:			get data packet and set to fifo direction3
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir4  (const unsigned char *in_buf ,const u16 in_size)
{
	u16 status=0;
	u16 static set_iteration_dir4=0;  
	unsigned long flags;
	
	     //printk(">>.>>>>>>>>>>>>nbuf_set_datapacket_dir3|iter=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir3);	 
	     //set packet size to fifo buffer
	     spin_lock_irqsave(fifo_put_to_tdm4_dir->lock,flags);
	     fifo_put_to_tdm4_dir->cur_put_packet_size=in_size;
	     //fifo_put_to_tdm3_dir->cur_put_packet_size=in_size/2;
	     //Set to the FIFO buffer
		  status=mpcfifo_put(fifo_put_to_tdm4_dir, in_buf);  
		  spin_unlock_irqrestore(fifo_put_to_tdm4_dir->lock,flags);
		  //mpcfifo_print(fifo_tdm0_dir_read, 0);
     set_iteration_dir4++;
}
/*****************************************************************************
Syntax:             void nbuf_set_datapacket_dir5  (const unsigned char *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir5  (const unsigned char *in_buf ,const u16 in_size)
{
	u16 status=0;
	u16 static set_iteration_dir5=0;  
	unsigned long flags;
	
    spin_lock_irqsave(fifo_put_to_tdm5_dir->lock,flags);
    
    //comment kosta for hex 01.12.13
    //fifo_put_to_tdm0_dir->cur_put_packet_size=in_size/2;
    
    // printk(">>>>>>>>>>>>>>nbuf_set_datapacket_dir0|iter=%d|size=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir0,in_size);	
     fifo_put_to_tdm5_dir->cur_put_packet_size=in_size;
     
    //Set to the FIFO buffer
	  status=mpcfifo_put(fifo_put_to_tdm5_dir, in_buf);  
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
	  spin_unlock_irqrestore(fifo_put_to_tdm5_dir->lock,flags);

	  set_iteration_dir5++;
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir6  (const unsigned char *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir6  (const unsigned char *in_buf ,const u16 in_size)
{
  u16 status=0;
  u16 static set_iteration_dir6=0;  
  unsigned long flags;

  spin_lock_irqsave(fifo_put_to_tdm6_dir->lock,flags);
  //comment kosta for hex 01.12.13
  //fifo_put_to_tdm0_dir->cur_put_packet_size=in_size/2;
  // printk(">>>>>>>>>>>>>>nbuf_set_datapacket_dir0|iter=%d|size=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir0,in_size);	
   fifo_put_to_tdm6_dir->cur_put_packet_size=in_size;
  //Set to the FIFO buffer
   status=mpcfifo_put(fifo_put_to_tdm6_dir, in_buf);  
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
   spin_unlock_irqrestore(fifo_put_to_tdm6_dir->lock,flags);
	 	  
   set_iteration_dir6++;
 
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir7  (const unsigned char *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir7  (const unsigned char *in_buf ,const u16 in_size)
{
	 u16 status=0;
	 u16 static set_iteration_dir7=0;  
	 unsigned long flags;
     spin_lock_irqsave(fifo_put_to_tdm7_dir->lock,flags);
     
     //comment kosta for hex 01.12.13
     //fifo_put_to_tdm0_dir->cur_put_packet_size=in_size/2;
     // printk(">>>>>>>>>>>>>>nbuf_set_datapacket_dir0|iter=%d|size=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir0,in_size);	
      fifo_put_to_tdm7_dir->cur_put_packet_size=in_size;
     //Set to the FIFO buffer
	  status=mpcfifo_put(fifo_put_to_tdm7_dir, in_buf);  
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
	  spin_unlock_irqrestore(fifo_put_to_tdm7_dir->lock,flags);
	 
	  set_iteration_dir7++;
	
	
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir8  (const unsigned char *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir8  (const unsigned char *in_buf ,const u16 in_size)
{
	 u16 status=0;
	 u16 static set_iteration_dir8=0;  
	 unsigned long flags;
     
	  spin_lock_irqsave(fifo_put_to_tdm8_dir->lock,flags);
     // printk(">>>>>>>>>>>>>>nbuf_set_datapacket_dir0|iter=%d|size=%d<<<<<<<<<<<<<<<<\n\r",set_iteration_dir0,in_size);	
      fifo_put_to_tdm8_dir->cur_put_packet_size=in_size;
     //Set to the FIFO buffer
	  status=mpcfifo_put(fifo_put_to_tdm8_dir, in_buf);  
	 //mpcfifo_print(fifo_tdm0_dir_read, 0);
	  spin_unlock_irqrestore(fifo_put_to_tdm8_dir->lock,flags);
	 
	  set_iteration_dir8++;
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir9  (const unsigned char *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir9  (const unsigned char *in_buf ,const u16 in_size)
{
	//in_buf=0;
	//in_size=0;
	
}
/*****************************************************************************
Syntax:      	    void nbuf_set_datapacket_dir10  (const unsigned char *in_buf ,const u16 in_size)
Remarks:			get data packet and set to fifo
Return Value:	    Returns 1 on success and negative value on failure.
*******************************************************************************/
void nbuf_set_datapacket_dir10  (const unsigned char *in_buf ,const u16 in_size)
{
	//in_buf=0;
	//in_size=0;
}














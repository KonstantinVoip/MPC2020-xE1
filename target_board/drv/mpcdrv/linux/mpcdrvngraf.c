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
#include "mpcdrvngraf.h"
/*External Header*/

/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
struct ngarf_setevoi_element
{
/////point begining
//IP address
//num landing
//port number
/////point end
//IP address
//num landing
//port number	
};
/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
UINT16 number_peak=6;                          ///Количество вершин графа (сетевых элементов)
UINT16 cost_matrica_puti[6][6];                //матрица стоимостей путей  //Здесь С
UINT16 MAX;                                  //вместо бесконечностей

/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					     */
/*****************************************************************************/
static void set_max_element_cost_matrica();
static bool in_arr(UINT16 j ,UINT16 *arr);
static void algoritm_djeicstra();



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


/*****************************************************************************/
/*	PRIVATE FUNCTION DEFINITIONS					     */
/*****************************************************************************/
/**************************************************************************************************
Syntax:      	    void ngraf_packet_for_my_mps(skb->data ,(uint)skb->len)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
void ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size)
{
	u16 hex_element_size;	
	u16 ngaf_ip_array[757];

	hex_element_size=in_size/2;
	memcpy(ngaf_ip_array,in_buf,hex_element_size);
	printk("+ngraf_get_datapacket\in_size_mas_byte= %d+\n\r",in_size);
	printk("+ngraf_get_datapacket\in_size_mas_element= %d+\n\r",in_size/2);
	printk("+ngraf_get_data_rfirst|0x%04x|0x%04x|0x%04x|0x%04x+\n\r",ngaf_ip_array[0],ngaf_ip_array[1],ngaf_ip_array[2],ngaf_ip_array[3]);
    printk("+ngraf_get_data_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",ngaf_ip_array[hex_element_size-4],ngaf_ip_array[hex_element_size-3],ngaf_ip_array[hex_element_size-2],ngaf_ip_array[hex_element_size-1]);	

    algoritm_djeicstra();
    
    
    
}

/**************************************************************************************************
Syntax:      	    ngraf_get_datapacket (const u16 *in_buf ,const u16 in_size)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
/*
void ngraf_get_datapacket (const u16 *in_buf ,const u16 in_size)
{

	
}
*/




/**************************************************************************************************
Syntax:      	    static void set_max_element_cost_matrica()    
Parameters:     	void
Remarks:			вычисляем максимальный элемент матрицы 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
static void set_max_element_cost_matrica()
{
 int max=0;
 int i,j;
 
 	for (i=0; i<number_peak; i++) 
 	{
 		for (j=0; j<number_peak; j++)
 		{
 			//if (cost_matrica_puti[i][j]!=NULL && cost_matrica_puti[i][j]!=50)
 			//{
 				//max=36;//max+cost_matrica_puti[i][j];
 				//printk("max= %d\n\r",max);
 			//
 				//}
  
 		}
 	}
 		
   max=MAX=36; //max*max;
   printk("max= %d\n\r",MAX);
}



/**************************************************************************************************
Syntax:      	    static bool in_arr(UINT16 j ,UINT16 *arr);
Parameters:     	проверка на наличие числа j в массиве arr
Remarks:			timer functions 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
static bool in_arr(UINT16 j ,UINT16 *arr)
{
	int i;
	bool ret;
	ret=false;
	for (i=0; i<number_peak-1; i++) if (arr[i]==j) ret=true;
	return ret;
	
}



/**************************************************************************************************
Syntax:      	    static void algoritm_djeicstra()
Parameters:     	сам алгоритм
Remarks:			timer functions 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
static void algoritm_djeicstra()
{
  int i,j,k;                   //для циклов for
  int a;                       //вершина источник графа //a=1,a=2,a=3,a=4,a=5,a=6 откуда считаем пути	
  int w,min;
  int S[number_peak-1];        //массив S содержит вершины помеченные как посещённые  
  int D[number_peak];          //массив D содрежит кратчайшие расстояние к вершинам из вершины источника
  int P[number_peak];          //массив последних промежуточных вершин на маршруте
  
 
  a=4;    //из вершины 1 один к остальным вершинам.
  a = a-1;	
  set_max_element_cost_matrica();	

  for(k=0;k<number_peak-1;k++)
  {
   S[k] = -1;
  }
  S[0]=a;
  /////
  for (i=0; i<number_peak; i++)
  {                       
  		if (cost_matrica_puti[a][i]==NULL) D[i]=MAX;
  			else D[i]=cost_matrica_puti[a][i];
  		
  }
  //////
  for (i=1; i<number_peak-1; i++)
  {
	 min=MAX;
	 for (k=0; k<number_peak; k++)
	 {
		if (D[k]<min && !in_arr(k,S) && k!=a)
		{
			w=k;
			min=D[k];
		}
	 }
	 if (min==MAX) break;
	 S[i]=w;
	 for (j=0; j<number_peak; j++) 
	 {
		if (!in_arr(j,S) && cost_matrica_puti[w][j]!=NULL && (D[w]+cost_matrica_puti[w][j])<=D[j])
			{
				P[j]=w;
				D[j]=D[w]+cost_matrica_puti[w][j];
			}
			
		
	 }
  }
 
  
  for(i=0;i<5;i++)
  {
  //printk("S= %d",S[i]);
   printk("D= %d\n\r",D[i]);
	//  printk("D= %d\n\r",P[i]);
  //printk("cost_matrica_puti= %x\n\r",cost_matrica_puti[i]);
  }
  
  
}










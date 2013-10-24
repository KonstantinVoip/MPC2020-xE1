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


////////////////////////////////////////////////
#define DLINNA_SVYAZI   12   /*длинна цепочки приёмник источник в пакете на 12 байт
                               в неё входит узел(наш мультиплексор и куда какому мультплексору)                     
                              */
                             
                              


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
extern UINT32 get_ipaddr_my_kys(); 


/*Extern ethernet functions*/
extern void p2020_get_recieve_packet_and_setDA_MAC(const u16 *in_buf,const u16 in_size,const u16 *mac_header);
extern void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len);
extern void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);



/*Extern fifo buffer function*/
extern void nbuf_set_datapacket_dir0  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_get_datapacket_dir0 (const u16 *in_buf ,const u16 in_size);




/**************************************************************************************************
Syntax:      	    void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u16 priznak_kommutacii)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii)
{
   UINT16  out_mac[12];
   UINT16  mac1[6];
   UINT16  mac2[6];
	
	
	
	//printk("MATRICA_KOMMUTACII=0x%x\n\r",priznak_kommutacii);
    //Здесь должен быть алгоритм коммутации куда направить пакет
	
	
	
	
	
	
	
	
	
	//Здесь кладём пакет в FIFO на отправку в нужное направление .TDM
	nbuf_set_datapacket_dir0  (in_buf ,in_size);
    
	
	//Или если пакет для нашего KY-S пихаем его в ethernet фугкции подмены MAC адреса
	//обратно выпл1вываем
	
	
	//Функция тупо отправляем в Ethernet пришедший буффер нужно доделать от какого device (eth0,eth1,eth2)
    //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size);//send to eternet
	  
	/*********************/
	//Подмена MAC только для пакетов предназначенных для отправки обратно моему KY-S  //переношу это в матрицу коммутации
	  //memcpy(mac1,out_buf,6);
	  //memcpy(mac2,&out_buf[3],6);
	  //printk("podmena_mac_src_|0x%04x|0x%04x|0x%04x\n\r",mac1[0],mac1[1],mac1[2]);  
	  //printk("podmena_mac_dst_|0x%04x|0x%04x|0x%04x\n\r",mac2[0],mac2[1],mac2[2]);
	 //p2020_revert_mac_header(mac2,mac1,&out_mac);
	 //p2020_get_recieve_packet_and_setDA_MAC(out_buf ,dannie1200+PATCH_READ_PACKET_SIZE_ADD_ONE,out_mac);
   /**********************/

	
	
}















/**************************************************************************************************
Syntax:      	    void ngraf_packet_for_my_mps(skb->data ,(uint)skb->len)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
void ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size)
{
	u16 hex_element_size=0;	
	u16 size_packet=0;
    u16 mas[in_size/2];
    __be32  my_kys_ipaddr;
    
    
    //for deijctra
    UINT16 number_of_par_sviaznosti_in_packet=0;
    
    my_kys_ipaddr=get_ipaddr_my_kys();
	
	hex_element_size=in_size/2;
	size_packet=sizeof(mas);
    
	printk("sizeof= %d\n\r ",size_packet) ;
	memset(&mas,0x0000,size_packet);	
	memcpy(mas,in_buf,sizeof(mas));

	
//#if 0	
	printk("+ngraf_get_datapacket\in_size_mas_byte= %d|hex= %d+\n\r",in_size,in_size/2);
	printk("+ngraf_get_data_rfirst|0x%04x|0x%04x|0x%04x|0x%04x+\n\r",mas[0],mas[1],mas[2],mas[3]);
	printk("+ngraf_get_data_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",mas[hex_element_size-4],mas[hex_element_size-3],mas[hex_element_size-2],mas[hex_element_size-1]);	
//#endif
	
	
	
		
#if 0	
	printk("+ngraf_get_indatapacket\in_size_mas_byte= %d|hex= %d+\n\r",in_size,hex_element_size);
	printk("+ngraf_get_indata_rfirst|0x%04x|0x%04x|0x%04x|0x%04x+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3]);
	printk("+ngraf_get_indata_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);	
#endif 
	
   
    //before MEMCPY  наш буфер потом производим парсинг пакета на маршруты.к точке назначения.
	//выделяем для нашего ip(мультика) валидные соединения с его соседом
	printk("IP_ADDR_TOP_of_graf=0x%x\n\r",my_kys_ipaddr);
	//подсчитываем количество связей (пар) в пакете 
	number_of_par_sviaznosti_in_packet=in_size/DLINNA_SVYAZI;
	//ищем вхождения ip адреса наешго KY-S мультиплексора(мультика)
	
	
	
	
	
    //algoritm_djeicstra(); 
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










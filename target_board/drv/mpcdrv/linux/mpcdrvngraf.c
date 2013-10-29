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
UINT32 my_setevoi_element_ip;
UINT16 matrica_commutacii_current_mpc_sosedi[15];
UINT32 matrica_ipadrr_sosedi_cur_mpc[15];

	
}setevoi_element[15];
/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
UINT16 number_peak=6;                          ///Количество вершин графа (сетевых элементов)
UINT16 cost_matrica_puti[6][6];                //матрица стоимостей путей  //Здесь С
UINT16 MAX;                                  //вместо бесконечностей


//заполянем поле в матрице коммутации чтот свзяи по этому направлению нет
UINT32 no_current_napr_mk8_svyaz= 0xffffffff;
UINT32 est_current_napr_mk8_svyaz=0x00000001;

//MATRIX for commutation mpc sosedi 10 napravlenii 10 sosedii
//UINT32 matrica_commutacii_current_mpc_sosedi[10][10];
//UINT32 matrica_ipadrr_sosedi_cur_mpc [10];

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


static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip);



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
extern void nbuf_set_datapacket_dir1  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir2  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir3  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir4  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir5  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir6  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir7  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir8  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir9  (const u16 *in_buf ,const u16 in_size);


/**************************************************************************************************
Syntax:      	    static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip)
Parameters:     	parse odny pary vhogdenia
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip)
{

    static iteration =0;
	__be32  l_ipaddr=0;
    __be32  l_sosed_ipaddr=0;
     UINT16 l_first_polovinka_sosed=0;
     UINT16 l_last_polovinka_sosed =0;
	 UINT8  l_my_posad_mesto=0;
	 UINT8  l_my_mk8_vihod=0;
	
	
	//IP адрес источника
	l_ipaddr  = in_sviaz_array[0]; 	
	//посадочное место
	l_my_posad_mesto= (UINT8)(in_sviaz_array[1]>>24);
	//номер выхода МК8
	l_my_mk8_vihod  =	(UINT8)(in_sviaz_array[1]>>16); 
    //Находим IP адрес соседа
    l_first_polovinka_sosed=(UINT16)in_sviaz_array[1];
    l_last_polovinka_sosed =(UINT16)(in_sviaz_array[2]>>16);
	l_sosed_ipaddr  = l_first_polovinka_sosed;
	l_sosed_ipaddr  = (l_sosed_ipaddr<<16); 
	l_sosed_ipaddr = l_sosed_ipaddr +l_last_polovinka_sosed;
	
	
	
	
	//printk("|iteration=%d->>>>>L_SOSED_IP_ADDR=0x%x \n\r+",iteration,l_sosed_ipaddr);
	//printk("|array[2]=0x%x \n\r+",in_sviaz_array[2]);
	
	//printk("L_MY_MPS_SOSED>>first_polovinka=0x%x|last_polovinka_=0x%x+\n\r",l_first_polovinka_sosed,l_last_polovinka_sosed);
	//printk("L_perv_element_dejcstra= 0x%x>>my_posad_mesto=%d>>>my_mk8_vihod=%d+\n\r",l_ipaddr,l_my_posad_mesto,l_my_mk8_vihod);
	
	//Присваиваем назад данные
	*my_ip=l_ipaddr;
	*posad_mesto=l_my_posad_mesto;
	*mk8_vyhod=l_my_mk8_vihod;
	*sosed_ip=l_sosed_ipaddr;
	iteration++;
}



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
   /*
     int A[N][N] = {
           {0, 1, 1, 1, INT16_MAX, INT16_MAX},  //1 элемент
           {1, 0, 1, INT16_MAX, INT16_MAX, 1}, //2 элемент
           {1, 1, 0, 1, INT16_MAX, 1},         //3 элемент
           {1, INT16_MAX, 1, 0, 1, INT16_MAX},  //4 элемент
           {INT16_MAX, INT16_MAX, INT16_MAX, 1, 0, 1}, //5 элемент
           {INT16_MAX, 1, 1, INT16_MAX,1, 0}  //6 элемент
       };
   */
	
	
	
   //по признаку коммутации
   
   
	
	
	
	
	
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
	u16 hex_input_data_size       = 0;	
	u16 byte_input_data_size      = 0;
    u16 four_byte_input_data_size = 0;
	u16 iteration=0;
    
    u16 razmer_array_element =400;
    
    __be32  my_kys_ipaddr;
    __be32  l_ipaddr=0;
    __be32  sosed_ipaddr=0;
    
    
    __be32  last_ip_addr=0;
    
     UINT16 first_polovinka_sosed=0;
     UINT16 last_polovinka_sosed =0;
    
     UINT16 i,j;
     UINT16 size_my_mas=0;
     UINT16 size_input_data=0;
     
     UINT8 my_posad_mesto=0;
     UINT8 my_mk8_vihod=0;
     
     
     //for deijctra
     UINT16 number_of_par_sviaznosti_in_packet=0;
     UINT16 dlinna_pari_sviaznosti_byte=0;
     UINT16 max_kolichestvo_par_v_odnom_elemente=10;  //для МК8 10 связей может быть потом больше
     UINT16 kolichestvo_iteration_on_this_packet=0; 
    
    
     UINT16 dlinna_svyazi_elements_of_massive=0;
     
     
     //длинна связи =12 байт размер одного элемента массива =4 байт
     dlinna_svyazi_elements_of_massive=DLINNA_SVYAZI/4;
     
    
    
    //razmer massiva = 400_element u32 po 4 bait   = 400*4 = 1600 bait s zapasom
    
    UINT32 mas_1[razmer_array_element];
    
    //Берём адрес моего сетевого элемент исходящая вершина графа
    my_kys_ipaddr=get_ipaddr_my_kys();
	
    

    size_my_mas=sizeof(mas_1);
	printk("sizeof_my_mas= %d\n\r",size_my_mas) ;
	//size_input_data=sizeof(in_buf);
	//printk("sizeof_input_data= %d\n\r ",size_input_data) ;
	
    byte_input_data_size=in_size;
    hex_input_data_size=(in_size/2);
    four_byte_input_data_size=(in_size/4);
    printk("input_data_size->byte_=%d,hex_=%d,32_=%d\n\r",byte_input_data_size,hex_input_data_size,four_byte_input_data_size);
	
	memset(&mas_1,0x0000,size_my_mas);	
	memcpy(mas_1,in_buf,byte_input_data_size);
    	
	//printk("+ngraf_get_data_rfirst|0x%08x|0x%08x|0x%08x|0x%08x+\n\r",mas_1[0],mas_1[1],mas_1[2],mas_1[3]);
	//printk("+ngraf_get_data_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",mas[hex_element_size-4],mas[hex_element_size-3],mas[hex_element_size-2],mas[hex_element_size-1]);	
    //first 4 bait my kys_ip_address
	
	
#if 0	
	
	//IP адрес источника
	l_ipaddr  = mas_1[0]; 	
	//посадочное место
	my_posad_mesto= (UINT8)(mas_1[1]>>24);
	//номер выхода МК8
	my_mk8_vihod  =	(UINT8)(mas_1[1]>>16); 
    //Находим IP адрес соседа
    first_polovinka_sosed=(UINT16)mas_1[1];
    last_polovinka_sosed =(UINT16)(mas_1[2]>>16);
	sosed_ipaddr  = first_polovinka_sosed;
	sosed_ipaddr  = (sosed_ipaddr<<16); 
	sosed_ipaddr = sosed_ipaddr +last_polovinka_sosed;
	
	
	printk("SOSED_IP_ADDR=0x%x\n\r",sosed_ipaddr);
	printk("MY_MPS_SOSED>>first_polovinka=0x%x|last_polovinka_=0x%x\n\r",first_polovinka_sosed,last_polovinka_sosed);
	printk("perv_element_dejcstra= 0x%x>>my_posad_mesto=%d>>>my_mk8_vihod=%d\n\r",l_ipaddr,my_posad_mesto,my_mk8_vihod);
#endif	
	
	
	
	//подсчитываем количество связей (пар) в пакете DLINN_SVYAZI 12 bait
	//odna sviaz:
	//Istochnic                               ||Priemnic
	//IP-address|nomer_PM|nomer_port_isch_MK8 ||//IP-address|nomer_PM|nomer_port_priemnic_MK8
	//(4-bait)   (1-bait)     (1-bait)        ||(4-bait)   (1-bait)     (1-bait)    
	  number_of_par_sviaznosti_in_packet=in_size/DLINNA_SVYAZI;
	  kolichestvo_iteration_on_this_packet=(number_of_par_sviaznosti_in_packet/max_kolichestvo_par_v_odnom_elemente);
	  printk("kolichestvo_par_dejcstra= %d\n\r",number_of_par_sviaznosti_in_packet);	
	  printk("kolichestvo_iteration_on_this_packet= %d\n\r",kolichestvo_iteration_on_this_packet); 
	  /////////////////////////////////////////////////////////////////////////////////////
	  
	  //parse_pari_svyaznosti(&mas_1[0],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
	  
	  
	  /*
	  printk("SOSED_IP_ADDR=0x%x\n\r",sosed_ipaddr);
	  printk("MY_MPS_SOSED>>first_polovinka=0x%x|last_polovinka_=0x%x\n\r",first_polovinka_sosed,last_polovinka_sosed);
	  printk("perv_element_dejcstra= 0x%x>>my_posad_mesto=%d>>>my_mk8_vihod=%d\n\r",l_ipaddr,my_posad_mesto,my_mk8_vihod);
	  */
   
	  	  
     //общее количество пар связносит 120 каждая равно 12 байт
	 for(i=0;i<kolichestvo_iteration_on_this_packet;i++)
	 {
		 //Рассматриваем первыую пару связности 
		 //Заполняем матрицу от ip адреса от моего МПС к 10 направлениям МК8 к соседям для первой пары
		 //Пока ничего не знаю о соседях
		 //
		 //parse_pari_svyaznosti(&mas_1[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
		 //ip сетевого элемента идущего первым
		 //setevoi_element[i].my_setevoi_element_ip=l_ipaddr;
		 
		 	for (j=0;j<10;j++)
		 	{
		 	   //должны запонить 10 связей
		 	   //если нет связи ставим 0xffff
		 		//printk("dlinna_pari_sviaznosti_byte =%d\n\r",dlinna_pari_sviaznosti_byte);
		 		parse_pari_svyaznosti(&mas_1[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
		 		
		 		///last_ip_addr=l_ipaddr;
		 		if(j==0)
		 		{
		 			setevoi_element[i].my_setevoi_element_ip=l_ipaddr;	
		 			last_ip_addr=l_ipaddr;
		 		}
		 		
		 		//Ситуация заполнения узла
		 		if(last_ip_addr==l_ipaddr)
		 		{
		 			//no_current_napr_mk8_svyaz;
		 			//est_current_napr_mk8_svyaz;		  
		 			//Есть коммутация МК8 на этом направлении
		 			 //printk("vhiogdenia =%d\n\r",)
		 			 //printk("iteration=%d|setevie_elementi_ip_packet=0x%x\n\r",iteration,l_ipaddr);
		 			 if(i==my_posad_mesto+1)
		 			 {	  
		 			 setevoi_element[i].matrica_commutacii_current_mpc_sosedi[j]=est_current_napr_mk8_svyaz; 
		 			 setevoi_element[i].matrica_ipadrr_sosedi_cur_mpc[j]= sosed_ipaddr;
		 			 }
		 			 else
		 			 {  
		 			 setevoi_element[i].matrica_commutacii_current_mpc_sosedi[j]=no_current_napr_mk8_svyaz; 
		 			 setevoi_element[i].matrica_ipadrr_sosedi_cur_mpc[j]= no_current_napr_mk8_svyaz;
		 			 }
		 			
		 			iteration++;
		 			last_ip_addr=l_ipaddr;
		 			dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+dlinna_svyazi_elements_of_massive;
		 		}
		 		else
		 		{
		 			iteration=0;
		 			break;
		 		}
		        		
		 	}
		 	//dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+DLINNA_SVYAZI;
		 	//смещаемся на следующею 3 элемента связь +12 bait  //3*4 =12 bait
		 	//cледующая связь
		 	///mas_1[i+3];
				 
	 }
	 printk("!!!!!!!!!OK_MATRICA_IS_FULL\n\r!!!!!!!!!!!");
	 
	 
	 
	  
	 for(i=0;i<12;i++)
	 {
		printk("setevie_elementi_ip_packet=0x%x\n\r",setevoi_element[i].my_setevoi_element_ip); 
	 }
	 
	 
	 
	 
	 
	 
	 
#if 0	
	printk("+ngraf_get_datapacket\in_size_mas_byte= %d|hex= %d+\n\r",in_size,in_size/2);
	printk("+ngraf_get_data_rfirst|0x%04x|0x%04x|0x%04x|0x%04x+\n\r",mas[0],mas[1],mas[2],mas[3]);
	printk("+ngraf_get_data_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",mas[hex_element_size-4],mas[hex_element_size-3],mas[hex_element_size-2],mas[hex_element_size-1]);	
#endif
	
	
	
		
#if 0	
	printk("+ngraf_get_indatapacket\in_size_mas_byte= %d|hex= %d+\n\r",in_size,hex_element_size);
	printk("+ngraf_get_indata_rfirst|0x%04x|0x%04x|0x%04x|0x%04x+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3]);
	printk("+ngraf_get_indata_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",in_buf[hex_element_size-4],in_buf[hex_element_size-3],in_buf[hex_element_size-2],in_buf[hex_element_size-1]);	
#endif 
	
   
    //before MEMCPY  наш буфер потом производим парсинг пакета на маршруты.к точке назначения.
	//выделяем для нашего ip(мультика) валидные соединения с его соседом
	//printk("IP_ADDR_TOP_of_graf=0x%x\n\r",my_kys_ipaddr);
	//подсчитываем количество связей (пар) в пакете 
	//number_of_par_sviaznosti_in_packet=in_size/DLINNA_SVYAZI;
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










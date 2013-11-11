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

extern void get_ipaddr_my_kys(UINT8 *state,UINT32 *ip_addres,UINT8 *mac_address);


extern struct KY_S my_current_kos;
/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
struct ngarf_setevoi_element
{
bool   setevoi_elemet_commutacia_zapolnenena;
UINT32 my_setevoi_element_ip;
UINT16 matrica_commutacii_current_mpc_sosedi[16];
UINT32 matrica_ipadrr_sosedi_cur_mpc[16];
}setevoi_element[16];
/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
__be32  my_kys_ipaddr=0;                         //IP адрес моего КY-S
                                                 //вместо бесконечностей

//заполянем поле в матрице коммутации чтот свзяи по этому направлению нет
UINT32 no_current_napr_mk8_svyaz= 0xffffffff;
UINT32 est_current_napr_mk8_svyaz=0x00000001;

////////////////////////////////////////////////
#define DLINNA_SVYAZI   12   /*длинна цепочки приёмник источник в пакете на 12 байт
                               в неё входит узел(наш мультиплексор и куда какому мультплексору)                     
                              */
                             
/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					                             */
/*****************************************************************************/
static void Algoritm_puti_udalennogo_ip_and_chisla_hop();
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
   
   
   //get_ipaddr_my_kys(,,,);
   //printk("State my_current MPC =0x%x\n\r",my_current_kos.ip_addres);
   printk("++priznak_kommutacii =%x\n\r",priznak_kommutacii);
   //Режим по умолчанию если пришёл пакет но нет матрицы коммутации не создалась тоже нужно придумать.
   //Матрица коммутации для пакетов соседей от моего сетевого элемента  МК8
   //Сосед на направлении 1
   //Предположим пока так будем коммутировать 
   
     
   	 //ARP ZAPROS  //vo vse diri 
   	 if(priznak_kommutacii==0x0806)
   	 {
   		
   		printk("+ARP_Dir0_rfirst   |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],in_buf[5]);
   		printk("+ARp_Dir0_rlast    |0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|0x%04x|+\n\r",in_buf[21-6],in_buf[21-5],in_buf[21-4],in_buf[21-3],in_buf[21-2],in_buf[21-1]); 
   		//nbuf_set_datapacket_dir0  (in_buf ,in_size);
   	  	return ;
     }
   	 
    
   
   //Признак коммутации ->>>пакет предназначенный для отправки обратно моему КY-S или пакет с Гришиным графом для удалённого МПС
   //(не шлюзового МПС)
#if 0  
   if(priznak_kommutacii==my_kys_ipaddr)
   {
	/*********************/
	//Подмена MAC только для пакетов предназначенных для отправки обратно моему KY-S
	  memcpy(mac1,in_buf,6);
      memcpy(mac2,&in_buf[3],6);
      //printk("podmena_mac_src_|0x%04x|0x%04x|0x%04x\n\r",mac1[0],mac1[1],mac1[2]);  
	  //printk("podmena_mac_dst_|0x%04x|0x%04x|0x%04x\n\r",mac2[0],mac2[1],mac2[2]);
      p2020_revert_mac_header(mac2,mac1,&out_mac);
	  p2020_get_recieve_packet_and_setDA_MAC(in_buf ,in_size,out_mac);
	/**********************/     
  
	  //Пакет с графом для удалённого (не шлюзового МПС)
	  //ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size)
   
   
   }
#endif   
   

   
   
#if 0   
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[0])
   {
    //Здесь кладём пакет в FIFO на отправку в нужное направление .TDM накапливаем пакеты
	  nbuf_set_datapacket_dir0  (in_buf ,in_size);
   }
   
   
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[1])
   {	   
   //Сосед на направлении 2
	  nbuf_set_datapacket_dir1  (in_buf ,in_size);
   }
	
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[2])
   {
	   //Сосед на направлении 3
	  nbuf_set_datapacket_dir2  (in_buf ,in_size);
   }
   
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[3])
   {	   
   //Сосед на направлении 4
	  nbuf_set_datapacket_dir3  (in_buf ,in_size);
   }
  
   
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[4])
   {	   
   //Сосед на направлении 5
	  nbuf_set_datapacket_dir4  (in_buf ,in_size);
   }
	  
   
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[5])
   {	   
   //Сосед на направлении 6
	  nbuf_set_datapacket_dir5  (in_buf ,in_size);
   }   
	
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[6])
   {	   
   //Сосед на направлении 7
	  nbuf_set_datapacket_dir6  (in_buf ,in_size);
   }
	  
   if(priznak_kommutacii==setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[7])
   {	   
	  //Сосед на направлении 8
	  nbuf_set_datapacket_dir7  (in_buf ,in_size);
   }
#endif  
 	
	//Функция тупо отправляем в Ethernet пришедший буффер нужно доделать от какого device (eth0,eth1,eth2)
    //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size);//send to eternet
	  	
}



/**************************************************************************************************
Syntax:      	    void ngraf_packet_for_my_mps(skb->data ,(uint)skb->len)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
bool ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size)
{
	
	//printk (">>>>>>>>>.packet _for _my _mps>>>>>>>>>>>>>>\n\r");
	//nbuf_set_datapacket_dir0  (in_buf ,in_size);
	

	
//#if 0	
	u16 hex_input_data_size       = 0;	
	u16 byte_input_data_size      = 0;
    u16 four_byte_input_data_size = 0;
	u16 iteration=0;
    
    u16 razmer_array_element =400;
    
    //__be32  my_kys_ipaddr;
    __be32  l_ipaddr=0;
    __be32  sosed_ipaddr=0;
    __be32  last_ip_addr=0;
    
    
    __be32   proba1=0;
    __be32   proba2=0;
     UINT16  proba3=0;
    
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
     UINT16 max_kolichestvo_par_v_odnom_setevom_elemente=8;  //для МК8 10 связей может быть потом больше
     //UINT16 kolichestvo_iteration_on_this_packet=0; 
     UINT16 max_kolichestvo_setvich_elementov_onpacket=0;//количестов сетевых элементов в пакете.
     
    
     UINT16 dlinna_svyazi_elements_of_massive=0;
     
     
     //длинна связи =12 байт размер одного элемента массива =4 байт
     dlinna_svyazi_elements_of_massive=DLINNA_SVYAZI/4;

     //razmer massiva = 400_element u32 po 4 bait   = 400*4 = 1600 bait s zapasom
     UINT32 mas_1[razmer_array_element];
    
     ///////////////////////Берём адрес моего сетевого элемент исходящая вершина графа////////////////
     //my_kys_ipaddr=get_ipaddr_my_kys();
     //my_kys_ipaddr=0xc0a86f01;  //пока делаю подставу.
    

    size_my_mas=sizeof(mas_1);
	//printk("sizeof_my_mas= %d\n\r",size_my_mas) ;
	//size_input_data=sizeof(in_buf);
	//printk("sizeof_input_data= %d\n\r ",size_input_data) ;
	
    byte_input_data_size=in_size;
    hex_input_data_size=(in_size/2);
    four_byte_input_data_size=(in_size/4);
   
    //printk("input_data_size->byte_=%d,hex_=%d,32_=%d\n\r",byte_input_data_size,hex_input_data_size,four_byte_input_data_size);
	
    // spin_lock_irqsave(lock_packet,lock_flags);
 
   
	memset(&mas_1,0x0000,size_my_mas);	
	memcpy(mas_1,in_buf,byte_input_data_size);
	
	
  
	
	//printk("+ngraf_get_data_rfirst|0x%08x|0x%08x|0x%08x|0x%08x+\n\r",mas_1[0],mas_1[1],mas_1[2],mas_1[3]);
	//printk("+ngraf_get_data_rlast |0x%04x|0x%04x|0x%04x|0x%04x+\n\r",mas[hex_element_size-4],mas[hex_element_size-3],mas[hex_element_size-2],mas[hex_element_size-1]);	
    //first 4 bait my kys_ip_address
	
	//подсчитываем количество связей (пар) в пакете DLINN_SVYAZI 12 bait
	//odna sviaz:
	//Istochnic                               ||Priemnic
	//IP-address|nomer_PM|nomer_port_isch_MK8 ||//IP-address|nomer_PM|nomer_port_priemnic_MK8
	//(4-bait)   (1-bait)     (1-bait)        ||(4-bait)   (1-bait)     (1-bait)    
	  
	  /*number_of_par_sviaznosti = общий размер данных в пакете 1440 байт /12 байт длинна одной пары связности = 120 пар
	   *kolichestvo_iteration_on_this_packet = 120 пар /количество пар для одного сетевого элемента в случае МК8 - 10 = 12
	   *итого получаеться 12 сетевых элемeнтов по 10 связей в каждом. 
	   * 
	   */
	
	  number_of_par_sviaznosti_in_packet=in_size/DLINNA_SVYAZI;
	  max_kolichestvo_setvich_elementov_onpacket=(number_of_par_sviaznosti_in_packet/max_kolichestvo_par_v_odnom_setevom_elemente);
	
	  
      
	  printk("max_kolichestvo_setvich_elementov_onpacket= %d\n\r",max_kolichestvo_setvich_elementov_onpacket);
	  printk("number_of_par_sviaznosti_in_packet        = %d\n\r",number_of_par_sviaznosti_in_packet);	
	  printk("max_kolichestvo_par_v_odnom_setevom_elemente= %d\n\r",max_kolichestvo_par_v_odnom_setevom_elemente); 
	  
	  
	  ///////////////////////////////////////////////////////////////////////////////////// 
	  
	  /*
	  printk("SOSED_IP_ADDR=0x%x\n\r",sosed_ipaddr);
	  printk("MY_MPS_SOSED>>first_polovinka=0x%x|last_polovinka_=0x%x\n\r",first_polovinka_sosed,last_polovinka_sosed);
	  printk("perv_element_dejcstra= 0x%x>>my_posad_mesto=%d>>>my_mk8_vihod=%d\n\r",l_ipaddr,my_posad_mesto,my_mk8_vihod);
	  */
		    
  	  
     //общее количество пар связносит 120 каждая равно 12 байт
	 for(i=0;i<max_kolichestvo_setvich_elementov_onpacket;i++)
	 {
		 //Рассматриваем первыую пару связности 
		 //Заполняем матрицу от ip адреса от моего МПС к 10 направлениям МК8 к соседям для первой пары
		 //Пока ничего не знаю о соседях
		 ////////////////////////////////
		 //parse_pari_svyaznosti(&mas_1[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
		 //ip сетевого элемента идущего первым
		 //setevoi_element[i].my_setevoi_element_ip=l_ipaddr;
		 
		 	for (j=0;j<max_kolichestvo_par_v_odnom_setevom_elemente;j++)
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
		 			 //Есть коммутация МК8 на этом направлении
		 			 //printk("vhiogdenia =%d\n\r",)
		 			 //printk("iteration=%d|setevie_elementi_ip_packet=0x%x\n\r",iteration,l_ipaddr);
		 			 //printk("iteration=%d|posad_mesto=%d\n\r",iteration,my_mk8_vihod);
		 			 if(j==my_mk8_vihod-1)
		 			 {	  
		 			 //Отладочная информация для проверки 	 
		 			 //printk("i=my_mk8_vihod=%d|j=%d,sosedip=0x%x,+\n\r",my_mk8_vihod,j,sosed_ipaddr);
		 			 setevoi_element[i].matrica_commutacii_current_mpc_sosedi[j]=est_current_napr_mk8_svyaz; 
		 			 setevoi_element[i].matrica_ipadrr_sosedi_cur_mpc[j]= sosed_ipaddr;
		 			 }
		 			 else
		 			 {  
		 			 //printk("--no_my_mk8_vihod--\n\r");
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
		 
				 
	 } //OK заполнили распарсили пакет с графом на кускию
	
	 		 
	  
	
	 //Ищу свой сетевой элемент если с моим ip и от него проверяю коммутацию.исходящею от меня 
	 /*
	 while(max_kolichestvo_setvich_elementov_onpacket)
	 {
		 
		 
	 }
	 */
	 
	 // 
	 proba1=setevoi_element[0].my_setevoi_element_ip;	
	 printk("Cornevoi_setevoi_elemnt_ip=%x\n\r",proba1);
	 
	 for(j=0;j<max_kolichestvo_par_v_odnom_setevom_elemente;j++)
	 {
	 		 
		     proba3=setevoi_element[0].matrica_commutacii_current_mpc_sosedi[j];
		     proba2=setevoi_element[0].matrica_ipadrr_sosedi_cur_mpc[j];
		     
		     printk("mk8_naprav=0x%x->>",proba3);
		     printk("ip_sosed=0x%x\n\r" ,proba2);
		     
	 }
	 //printk("!!!!!!!!!OK_MATRICA_IS_FULL\n\r!!!!!!!!!!!");
	 /*
	 for(i=0;i<max_kolichestvo_setvich_elementov_onpacket;i++)
	 {
		printk("setevie_elementi_ip_packet=0x%x\n\r",setevoi_element[i].my_setevoi_element_ip); 
	 }*/
	 
	 
	 
   //Пока в качестве заглушки используем Алгоритм для поиска пакета с удалённым IP_мультиплексора который не
   //входит в число соседей моего корневого.
   //+нужно изъять паралельные связи которые не ведут нас к удалённому мултиплексору(пока оставлю это)
	 
	 
	 
  //Algoritm_puti_udalennogo_ip_and_chisla_hop(); 
	 // 
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

	
	
//#endif	
	
	return 0;
	
}




/**************************************************************************************************
Syntax:      	    static void Algoritm_puti_udalennogo_ip_and_chisla_hop()
Parameters:     	Алгоритм полсчета числа прыжков и маршрута к удалённому Мультиплексорую.
Remarks:			timer functions 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
static void Algoritm_puti_udalennogo_ip_and_chisla_hop()
{

  
  
}










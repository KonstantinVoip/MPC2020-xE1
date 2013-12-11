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

//extern void get_ipaddr_my_kys(UINT8 *state,UINT32 *ip_addres,UINT8 *mac_address);
//extern struct KY_S my_current_kos;
/*****************************************************************************/
/*	PRIVATE MACROS							     */
/*****************************************************************************/
extern UINT16 marsrutiazation_enable;


///CURRENT _KYS
static struct KY_S
{
UINT32 ip_addres;
UINT8  *mac_address;
bool   state;
}my_current_kos;


 
/*
struct ngarf_setevoi_element
{
bool   setevoi_elemet_commutacia_zapolnenena;
UINT32 my_setevoi_element_ip;
UINT16 matrica_commutacii_current_mpc_sosedi[16];
UINT32 matrica_ipadrr_sosedi_cur_mpc[16];
}setevoi_element[16];
*/

struct ngarf_setevoi_element
{
UINT32 nms3_ipaddr;
UINT32 curr_ipaddr;
UINT32 gate_ipaddr;
UINT32 ipaddr_sosed[8];
UINT8  tdm_direction_sosed[8];
UINT8  priznac_shcluzovogo;
}multipleksor[4];




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
static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip,u8 *mk8_sosed_vyhod);

static void dijkstra(int start);

/* KERNEL MODULE HANDLING */
/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/
#define INT_MAX  0xffff

//количество узлов в сети
static const int N =4;
/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
/*Добавил дома Алгоритм Дейкстры*/
/*соединение для нашей схемы*/
//Матрица соединенй
static bool adj_matrix[4][4]=
{
// ---0--|---1--|-2-|---3---|  
   0    ,1    ,1   ,0,
   1    ,0    ,1   ,0,
   1    ,1    ,0   ,1,
   0    ,0    ,1   ,0 
};


/*Матрица смежностей(стоимостей)для алгоритма Дейкстра
 исходные данные для построение путей
*/
static unsigned short int cost[4][4]=
{
 
// ---0--|---1--|-2-|---3---|  
   0     ,10     ,10 ,0xffff,
   10     ,0     ,10 ,0xffff,
   10    ,10    ,0  ,10,
   0xffff,0xffff,10 ,0 
};

//массив кратчайших расстояний на данном шаге до вершины i в начальный момент нет расстояний
unsigned short dist[4]={0xffff,0xffff,0xffff,0xffff};
//массив посещённых вершин в начальный момент заполнеы нулями никого не посетили пока
unsigned short used[4]={0,0,0,0};
//массив кратчайших путей
signed short C[4]={-1,-1,-1,-1};










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
extern void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len,u8 tsec_id);
extern void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);



/*Extern fifo buffer function*/
extern void nbuf_set_datapacket_dir0  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir1  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir2  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir3  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir4  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir5  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir6  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir7  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir8  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir9  (const u16 *in_buf ,const u16 in_size);


//Static tablica sosedei on matrica commutacii dli ky-s
//соседи KY-S

/*
                                   //192.168.130.156
static __be32  sosed1_kys_ipaddr = 0x9c;
                                  //192.168.130.157
static __be32  sosed2_kys_ipaddr = 0x9d;
                                  //10.2.120.80
static __be32  sosed3_kys_ipaddr = 0x50;
*/

/*****************************************Sosedi IP addrress**********************************/

//192.168.120.170  //Gateway
//static __be32  sosed_mp1_kys_ipaddr = 0xAA;
//192.168.120.171 //sosed
//static __be32  sosed_mp2_kys_ipaddr = 0xAB;
//192.168.120.172 //sosed
//static __be32  sosed_mp3_kys_ipaddr = 0xAC;
//192.168.120.173 //sosed
//static __be32  sosed_mp4_kys_ipaddr = 0xAD;

//192.168.120.76
//static __be32  nms3_ip_addr       =   0x4C;     

//192.168.130.97 
//static __be32  seva_nms_ip_addr   =   0x61;



/*****************************************Sosedi po  MAC address***********************************/
//static __be32  nms3_damac_address = 0x5f4e;

UINT32 nms3_ip_addres  =0x00000000; //ip Адрес НМС3 шлюза
UINT32 scluz_ip_addres =0x00000000;
UINT32 protocol_version=0x00000000; //версия Гришиного протокола; тип
UINT8  priznac_scluz=255;//состояние не опредлено

















void ngraf_get_ip_mac_my_kys (UINT8 state,UINT32 ip_addres,UINT8 *mac_address)
{	
	//printk("virt_TSEC_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",buf[0],buf[1],buf[2],buf[3]);
	//printk("State =0x%x>>IP=0x%x,MAC =|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|\n\r",state,ip_addres,mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);
	my_current_kos.state=state;
	my_current_kos.ip_addres=ip_addres;
	my_current_kos.mac_address=mac_address;
	multipleksor[0].curr_ipaddr=ip_addres;
	printk("++Set_Multiplecsor_IP_and_MAC_OK++\n\r");
	printk("State =0x%x>>IP=0x%x,MAC =|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|\n\r",state,ip_addres,my_current_kos.mac_address[0],my_current_kos.mac_address[1],my_current_kos.mac_address[2],my_current_kos.mac_address[3],my_current_kos.mac_address[4],my_current_kos.mac_address[5]);
}


/**************************************************************************************************
Syntax:      	    static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip)
Parameters:     	parse odny pary vhogdenia
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip,u8 *mk8_sosed_vyhod)
{

    static iteration =0;
	__be32  l_ipaddr=0;
    __be32  l_sosed_ipaddr=0;
     UINT16 l_first_polovinka_sosed=0;
     UINT16 l_last_polovinka_sosed =0;
	 UINT8  l_my_posad_mesto=0;
	 UINT8  l_my_mk8_vihod=0;
	 UINT8  l_sosed_mk8_vihod=0;
	
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
	l_sosed_mk8_vihod=(UINT8)(in_sviaz_array[2]);
	
	//printk("l_sosed_mk8_vihod=%d\n\r",l_sosed_mk8_vihod);
	
	//printk("iter=%d->>SOSED_IP_ADDR=0x%x->mk8_vihod=%d \n\r",iteration,l_sosed_ipaddr,l_my_mk8_vihod);
	  
	  //printk("|array[2]=0x%x \n\r+",in_sviaz_array[2]);
	
	 //printk("L_MY_MPS_SOSED>>first_polovinka=0x%x|last_polovinka_=0x%x+\n\r",l_first_polovinka_sosed,l_last_polovinka_sosed);
	 //printk("L_perv_element_dejcstra= 0x%x>>my_posad_mesto=%d>>>my_mk8_vihod=%d+\n\r",l_ipaddr,l_my_posad_mesto,l_my_mk8_vihod);
	
	//Присваиваем назад данные
	*my_ip=l_ipaddr;
	*posad_mesto=l_my_posad_mesto;
	*mk8_vyhod=l_my_mk8_vihod;
	*sosed_ip=l_sosed_ipaddr;
    *mk8_sosed_vyhod=l_sosed_mk8_vihod;
	
	iteration++;
}



/**************************************************************************************************
Syntax:      	    void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u16 priznak_kommutacii)
Parameters:     	void data
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

***************************************************************************************************/
void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii,u32 priznak_nms3_arp_sender)
{
   //UINT16  out_mac[12];
   //UINT16  mac1[3];
   //UINT16  mac2[3];
    static UINT16 iteration=0;
   //UINT8   priznak_scluz=0;
   //priznal Scluzovogo MPC
   // my_current_kos.ip_addres=0xAA;
    UINT16  udp_dest_port=0;
   //Нельзя начинать передачу пока нет IP и MAC адреса с KY-S
  // if(my_current_kos.state==0){return;}
   //printk("PR_commut =0x%x \n\r",priznak_kommutacii);
   //Пакет моему KY-S
    multipleksor[0].priznac_shcluzovogo=1;
   // printk("priznak arp_sender =0x%x\n\r",priznak_nms3_arp_sender);
    //Дополнительное условие проверки ARP
    if(priznak_nms3_arp_sender)
    {
       if((u8)multipleksor[0].curr_ipaddr==priznak_kommutacii)
       {
    	   multipleksor[0].nms3_ipaddr=(u8)priznak_nms3_arp_sender;
           //printk("+multipleksor[0].nms3_ipaddr=0x%x+\n\r",multipleksor[0].nms3_ipaddr);
       }
        
    }
    
   
   
 
   
    if (priznak_kommutacii==(u8)multipleksor[0].curr_ipaddr)
    {
	   //Еслиr пакет моему KY-S  и признак коммутации порт 18000 то это
	   //матрицы коммутации       
	          memcpy(&udp_dest_port,&in_buf[18],2); 
	          //printk("udp_dest_port=%d,0x%x\n\r",udp_dest_port,udp_dest_port);
	          if (udp_dest_port==18000)
	          {
	        	  //строим матрицу коммутации
	            printk("+ngraf_packet+\n\r");
	        	ngraf_packet_for_my_mps(in_buf ,in_size);
	            marsrutiazation_enable=1;
	          } //end UDP port 18000          
	          //если другой пакет отправляем KY-S в eth1
	          else
	          {
	        	  
	        	 // printk("Send my KY-S packet 0x%x\n\r",(u8)multipleksor[0].curr_ipaddr);
	        	  p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
	         
	          
	          } 	
      
     }

   
    if(priznak_kommutacii==(u8)multipleksor[0].nms3_ipaddr)
    {
        //send to direction0 sosed KY-S
    	
    	//printk("Send to NMS3 ip 0x%x \n\r",multipleksor[0].nms3_ipaddr);
	   if(multipleksor[0].priznac_shcluzovogo==1)
	    {
	     p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,2);	
	  	}
	   
	    else
	    {
	     nbuf_set_datapacket_dir0  (in_buf ,in_size);
	    }
 	  	 
	}
    
   
  if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[0])
  {
	  
	  printk("Send to IP sosed 0x%x direction %d\n\r",multipleksor[0].ipaddr_sosed[0],multipleksor[0].tdm_direction_sosed[0]);
	  switch(multipleksor[0].tdm_direction_sosed[0])
	  {
	  case 1:nbuf_set_datapacket_dir0  (in_buf ,in_size);break;
	  case 2:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
      case 3:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
      case 4:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
      //case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);
      //case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);
      //case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);
      //case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);
      default:break;
	  }  
   }
                                              
  if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[1])
  {
	  printk("Send to IP sosed 0x%x direction %d\n\r",multipleksor[0].ipaddr_sosed[1],multipleksor[0].tdm_direction_sosed[1]);
	  switch(multipleksor[0].tdm_direction_sosed[1])
	  {
	  case 1:nbuf_set_datapacket_dir0  (in_buf ,in_size);break;
	  case 2:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
      case 3:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
      case 4:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
     // case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);
     // case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);
     // case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);
     // case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);
      default:break;
	  }
	  
  }
 


   
   
   
   
	   
#if 0  
   
   
   
   
   
   
   
   
   // printk("ngraf_packet=%d \n\r",in_size);
     // printk("PR=0x%x->>iter =%d\n\r",priznak_kommutacii,iteration);
   // printk("my_kos_ip_addr=0x%x\n\r",my_current_kos.ip_addres);   
   
   /*if(priznak_kommutacii==my_current_kos.ip_addres)
     {
         //send to direction0 sosed KY-S
     	 //printk("Send to IP sosed 192.168.120.170 direction 0\n\r");
	   //printk("Send to IP sosed 192.168.130.170 direction 0\n\r");     
	   p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
	   
     }
    */
   
    //NMS3 IP address 192.168.120.76(4c)
    //SEVA NMS IP address 192.168.130.97
     if(priznak_kommutacii==nms3_ip_addr)
     //if(priznak_kommutacii==seva_nms_ip_addr)
     {
         //send to direction0 sosed KY-S
     	 //printk("Send to IP sosed 192.168.120.170 direction 0\n\r");
         
	    if(priznak_scluz==1)
	  	 {
	     p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,2);	
	  	 }
	    else
	    {
	    	nbuf_set_datapacket_dir0  (in_buf ,in_size);
	    	
	    }
  	  	 
	 }
    
     
     if(priznak_kommutacii==my_current_kos.ip_addres)
       {
           //send to direction0 sosed KY-S
       	 //printk("Send to IP sosed 192.168.120.170 direction 0\n\r");
  	   //printk("Send to IP sosed 192.168.130.170 direction 0\n\r");     
  	   
    	 //если пакет на порт 18000 то строю матрицу коммутации
    	 
    	 
    	 p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
  	   
  	   
  	   
       }
     
     
      /*
      else
       {
    	 if(priznak_kommutacii==0xac)
    	 {	 
    	 printk("priznak _commuracii 0xac\n\r");
    	 nbuf_set_datapacket_dir0  (in_buf ,in_size);
    	 //nbuf_set_datapacket_dir1  (in_buf ,in_size);
    	 }
       
    	 
       }
     */
     
     
   //Add Udalenni Multipleksor
   //Add Udalenni Multipleksor
   
   /*  
   if(priznak_kommutacii==0xac)
     {
        //send to direction0 sosed KY-S
    	   //printk("Send to IP sosed 192.168.120.171 direction 0\n\r");
      nbuf_set_datapacket_dir0  (in_buf ,in_size);
          //return;
      }
   */  
     
     
     
     
     
     
 /////////////////////////Sosedi Commutacia///////////////////////
 if(priznak_scluz==1)  
 {   
    
   if(priznak_kommutacii==0xab)
     {
       //send to direction0 sosed KY-S
   	     printk("Send to IP sosed 192.168.120.171 direction 0\n\r");
         nbuf_set_datapacket_dir0  (in_buf ,in_size);
       //return;
     }
   
    //Add Udalenni Multipleksor
    if(priznak_kommutacii==0xac)
     {
       //send to direction0 sosed KY-S
   	   //printk("Send to IP sosed 192.168.120.171 direction 0\n\r");
         nbuf_set_datapacket_dir0  (in_buf ,in_size);
       //return;
     }
   
 
 
 
 
 }  
   
   //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);//send to eternet tsec ARP broadcast
   //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,2);//send to eternet tsec ARP broadcast
#endif 
   iteration++;

 
 	 //ICMP запрос Echo requeest   
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

  //Что нам нужно для Алгоритма?

  UINT8 smeshenie_grisha_scluz=0;
  UINT32 data_graf_massive[32];
  UINT16 razmer_data_graf_massive=0;
  __be32  l_ipaddr=0;
  __be32  sosed_ipaddr=0;
  __be32  last_ip_addr=0;
  __be32  curr_ipaddr=0;
   UINT16 iteration=0;
   UINT16 i,j;
   UINT16 m=0;  //структура с текущим мультиплексором
   
   UINT8 my_posad_mesto=0;
   UINT8 my_mk8_vihod=0;
   UINT8 sosed_mk8_vyhod=0;
   
   UINT16 number_of_multipleksorov_in_packet=1;
   //здесь храниться длинна одной связи либо в байтах либо в количестве элементов по 4 байта каждый; 
   UINT16 dlinna_pari_sviaznosti_byte=0;
   //Для МК-8 8 пар связности для одного сетевого элемента.
   UINT16 kolichestvo_par_v_odnom_setevom_elemente=8;
   //Текущее количество пар для одного сетевого элемента
   UINT16 tek_kolichestvo_par_dli_dannogo_elementa=0;
   //количество сетевых элементов (узлов в пакете) для построения матрицы коммутации
   UINT16 kolichestvi_setevigh_elementov_in_packet=0;
   //Общее количество пар связности в пакете по размеру пакета
   UINT16 number_of_par_sviaznosti_in_packet=0;
   //Количество сетевых элементов в пакете
   UINT16 max_kolichestvo_setvich_elementov_onpacket=0;
   
   
   //Нет соединений ничего не соединено
   bool multipleksor_matrica_soedinenii[4][4]=  {
		  	  	  	  	  	  	  	  	  	  	    // ---0--|---1--|-2-|---3---|  
		  	  	  	  	  	  	  	  	  	  	  	  	  0    ,0    ,0   ,0,
		  	  	  	  	  	  	  	  	  	  	  	  	  0    ,0    ,0   ,0,
		  	  	  	  	  	  	  	  	  	  	  	  	  0    ,0    ,0   ,0,
		  	  	  	  	  	  	  	  	  	  	  	  	  0    ,0    ,0   ,0 
		  	  	  	  	  	  	  	  	  	   };	
  	 	
    /*
    for(i=0;i<in_size;i++)
    {
    	
    	printk("0x%x ",in_buf[i]);
    }
    */
   
	//18 это в пакете наш  UDP  порт destination
	memcpy(&nms3_ip_addres,&in_buf[13],4); 
	printk ("NMS3_ip_addr =<0x%x>\n\r",nms3_ip_addres);
	multipleksor[0].nms3_ipaddr=nms3_ip_addres;
	//memcpy(&protocol_version,&in_buf[13],4);
	//printk ("NMS3_protocol_version =0x%x>\n\r",protocol_version);

	memcpy(&scluz_ip_addres,&in_buf[25],4);
	printk ("Scluz_ip_addr =<0x%x>\n\r",scluz_ip_addres);
	multipleksor[0].gate_ipaddr=scluz_ip_addres;
	
	//Определяем что мы шлюз
	if(my_current_kos.ip_addres==(u8)scluz_ip_addres)
	{ printk("I 'am Scluz\n\r");
	priznac_scluz=1;multipleksor[0].priznac_shcluzovogo=1;}
	else{printk("Not a Scluz\n\r");
	priznac_scluz=0;multipleksor[0].priznac_shcluzovogo=0;}
	
	
	
	//Первые 8 байт это название Гришиного протокола,следующие 4 байт это ip адрес шлюза. = 12 байт
	smeshenie_grisha_scluz=(4+8);  //смещение 12 байт или 6 элементов в hex;
	razmer_data_graf_massive=in_size-42-smeshenie_grisha_scluz; //размер данных в массиве
	printk("razmer_data_bait=%d\n\r",razmer_data_graf_massive);
	//21 байт это начало данных.+смещение получаем массив пар связности
	memcpy(&data_graf_massive,&in_buf[21+6],razmer_data_graf_massive);  
	//printk("!massive_copy_ok=0x%8x!\n\r",data_graf_massive[1]);
	
	
	
	
    //длинна связи =12 байт размер одного элемента массива =4 байт
	//UINT16 dlinna_svyazi_elements_of_massive=DLINNA_SVYAZI/4;
	//UINT16 number_of_par_sviaznosti_in_packet=razmer_data_graf_massive/DLINNA_SVYAZI;
    //UINT16 max_kolichestvo_setvich_elementov_onpacket=2;//(number_of_par_sviaznosti_in_packet/max_kolichestvo_par_v_odnom_setevom_elemente);
		  
    
   // printk("max_kolichestvo_setvich_elementov_onpacket= %d\n\r",max_kolichestvo_setvich_elementov_onpacket);
   // printk("number_of_par_sviaznosti_in_packet        = %d\n\r",number_of_par_sviaznosti_in_packet);	
   // printk("max_kolichestvo_par_v_odnom_setevom_elemente= %d\n\r",max_kolichestvo_par_v_odnom_setevom_elemente); 
	
   //Подсчитываю количество пар связности в пакете
	 
	
	
	//max_kolichestvo_setvich_elementov_onpacket=1;
	number_of_par_sviaznosti_in_packet=razmer_data_graf_massive/DLINNA_SVYAZI;
	//вычислить количество пар для данного элемента
	//tek_kolichestvo_par_dli_dannogo_elementa=2;
	
	printk("all_number_of_par_sviaznosti_in_tek_packet        = %d\n\r",number_of_par_sviaznosti_in_packet);	
	printk("tek_kolichestvo_par_dli_dannogo_elementa= %d\n\r",tek_kolichestvo_par_dli_dannogo_elementa); 
	//parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr,&sosed_mk8_vyhod);
	
  //Рассчитываем количество узловых мультплексоров в пакете	
/* 
if(number_of_par_sviaznosti_in_packet>1)
  {	
	
	//просматриваем все пары связности в пакете
	for(m=0;m<number_of_par_sviaznosti_in_packet;m++)
	{
		
		parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr,&sosed_mk8_vyhod);
		//стартовые условия
		if(m=0)
		{
		curr_ipaddr=l_ipaddr;
		}
		//стартовые условия
		if(curr_ipaddr==l_ipaddr)
		{
		   printk("iteraton in packet++\n\r");	
		}
		else
		{
		number_of_multipleksorov_in_packet++;
		}
	//модифицируем значение текущего ip.
	curr_ipaddr=l_ipaddr;	
	//переходим к следующей связи в пакете
	dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+3;
	}
  
  }
  else
  {
	 number_of_multipleksorov_in_packet=1;
  }
	
  printk("kolichestvo_multiplekosorov_in_packet= %d\n\r",number_of_multipleksorov_in_packet); 
 */
  
  //Теперь нужно определить кто я  с точки зрения пакета котроый пришёл
  //пакет для маршрутизации () от меня  (1)
  //или пакет пакет для прокладки трассы от остальных ко мне
  //Как узнать?
 
 // dlinna_pari_sviaznosti_byte=0;
  //1.Ищу вхождения что маршрутизация от меня ()
  for(i=0;i<number_of_par_sviaznosti_in_packet;i++)
  {
	  parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr,&sosed_mk8_vyhod);
	  //пакет исходит от меня построение маршрута от меня я нашёл себя
	  if(my_current_kos.ip_addres==(u8)l_ipaddr)
	  {
		//printk("++\n\r");
		multipleksor[0].ipaddr_sosed[i]=(u8)sosed_ipaddr;
		multipleksor[0].tdm_direction_sosed[i]=my_mk8_vihod;
		  
	  }
      //я сосед висящий на первой связи
	  if(my_current_kos.ip_addres==(u8)sosed_ipaddr)
      {
		 multipleksor[0].ipaddr_sosed[i]=(u8)l_ipaddr;
	     multipleksor[0].tdm_direction_sosed[i]=sosed_mk8_vyhod;
      }
	  
	  
   dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+3;
  }
 
  for(i=0;i<number_of_par_sviaznosti_in_packet;i++)
  {	  
  printk("MP[%x]->ip_sosed=0x%x|tdm_dir->%d\n\r",my_current_kos.ip_addres,multipleksor[0].ipaddr_sosed[i],multipleksor[0].tdm_direction_sosed[i]);
  }
  
  //printk("MPO->ip_sosed=0x%x|tdm_dir->%d\n\r",multipleksor[0].ipaddr_sosed[1],multipleksor[0].tdm_direction_sosed[1]);
  /*
  printk("MP1->ip_sosed=0x%x|tdm_dir->%d\n\r",multipleksor[1].ipaddr_sosed[0],multipleksor[1].tdm_direction_sosed[0]);
  printk("MP1->ip_sosed=0x%x|tdm_dir->%d\n\r",multipleksor[1].ipaddr_sosed[1],multipleksor[1].tdm_direction_sosed[1]);
  */
  
  
  
  
  
  
  
	
	/*
	if(multipleksor[0].priznac_shcluzovogo==0)
	{
		return;
	}
	*/
// parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip)
    /*Просматриваем все пары связности которые есть в пакете*/	
	//тут должно быть количество сетевых элементов в пакете (мультплексоров)
	//for(i=0;i<number_of_multipleksorov_in_packet;i++)
	//{
	
			//тут будет общее количество пар связности в пакете
	
 /* 
            for(j=0;j<number_of_par_sviaznosti_in_packet;j++)
			{		
				//Парсим одну свзь на элементы
				parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
				
				//Заполняю элементы матрицы маршрутизации для моего KY-S(моего IP)
				//маршрутизацию от меня к удалённому
				
				//Для шлюзового это действует
				if(my_current_kos.ip_addres==l_ipaddr)
				{
				multipleksor[i].ipaddr_sosed[j]=(u8)sosed_ipaddr;
			    multipleksor[i].tdm_direction_sosed[j]=my_mk8_vihod;	
			    }
				dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+3; 
			}
		  
  */


	//}
	
	
	
	
#if 0
//просматриваем все пары связности	(количество сетевых элементов в пакете)
for(i=0;i<1/*number_of_par_sviaznosti_in_packet*/;i++)
{	
	//первую пару свзяности
	//parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
	
	
	
	for(j=0;j<tek_kolichestvo_par_dli_dannogo_elementa;j++)
	{	    
		
	 	   //должны запонить 10 связей
	 	   //если нет связи ставим 0xffff
	 		//printk("dlinna_pari_sviaznosti_byte =%d\n\r",dlinna_pari_sviaznosti_byte);
	 		parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
	 		/*Старт начало заполнения*/
	 		if(j==0)
	 		{	
	 		curr_ipaddr=l_ipaddr;
	 	    }
	 		
	 		//заполняем сетевой элемент
	 		if(curr_ipaddr==l_ipaddr)
	 	    {
		    parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
		    multipleksor[i].ipaddr_sosed[j]=(u8)sosed_ipaddr;
		    multipleksor[i].tdm_direction_sosed[j]=my_mk8_vihod;
		    
		    iteration++;
		    curr_ipaddr=l_ipaddr;
		    dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+3;
	 	    }
	 		//закончили запонять идёт следующий элемент выходим из цикла
	 		else
	 		{
	 			iteration=0;
	 			break;
	 		}
	 		
	
	 }
     
	
}	


printk("MPO->ip_sosed=0x%x|tdm_dir->%d\n\r",multipleksor[0].ipaddr_sosed[0],multipleksor[0].tdm_direction_sosed[0]);
printk("MPO->ip_sosed=0x%x|tdm_dir->%d\n\r",multipleksor[0].ipaddr_sosed[1],multipleksor[0].tdm_direction_sosed[1]);
#endif


#if 0    
    
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
		 		parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr);
		 		
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
	 
	 for(i=0;i<max_kolichestvo_setvich_elementov_onpacket;i++)
	 {
		printk("setevie_elementi_ip_packet=0x%x\n\r",setevoi_element[i].my_setevoi_element_ip); 
	 }
	
	
	
	
	//Разбиваю пакет
   
	/*
	UINT16 number_of_par_sviaznosti_in_packet=0;
	UINT16 dlinna_pari_sviaznosti_byte=0;
	UINT16 max_kolichestvo_par_v_odnom_setevom_elemente=8;  //для МК8 10 связей может быть потом больше
	UINT16 max_kolichestvo_setvich_elementov_onpacket=0;    //количестов сетевых элементов в пакете.    
	UINT16 dlinna_svyazi_elements_of_massive=0;
	
	
	
    //длинна связи =12 байт размер одного элемента массива =4 байт
    dlinna_svyazi_elements_of_massive=DLINNA_SVYAZI/4;
	*/
	
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
	  
/*	
      number_of_par_sviaznosti_in_packet=in_size/DLINNA_SVYAZI;
	  max_kolichestvo_setvich_elementov_onpacket=(number_of_par_sviaznosti_in_packet/max_kolichestvo_par_v_odnom_setevom_elemente);
	  printk("max_kolichestvo_setvich_elementov_onpacket= %d\n\r",max_kolichestvo_setvich_elementov_onpacket);
	  printk("number_of_par_sviaznosti_in_packet        = %d\n\r",number_of_par_sviaznosti_in_packet);	
	  printk("max_kolichestvo_par_v_odnom_setevom_elemente= %d\n\r",max_kolichestvo_par_v_odnom_setevom_elemente); 
 */   
    
    
#endif  
    
	
	
	
	
#if 0	
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
   
    printk("input_data_size->byte_=%d,hex_=%d,32_=%d\n\r",byte_input_data_size,hex_input_data_size,four_byte_input_data_size);
	
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
	 
	 for(i=0;i<max_kolichestvo_setvich_elementov_onpacket;i++)
	 {
		printk("setevie_elementi_ip_packet=0x%x\n\r",setevoi_element[i].my_setevoi_element_ip); 
	 }
	 
	// Algoritm_puti_udalennogo_ip_and_chisla_hop(); 
	 
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

	
	
#endif	
	
	//dijkstra(start);
	// Algoritm_puti_udalennogo_ip_and_chisla_hop(); 
	return 0;
	
}




/**************************************************************************************************
Syntax:      	    static void Algoritm_puti_udalennogo_ip_and_chisla_hop()
Parameters:     	Алгоритм полсчета числа прыжков и маршрута к удалённому Мультиплексорую.
Remarks:			timer functions 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
static void dijkstra(int start)
{
	   bool in_tree[4] = {false};
       int i; 
	   dist[start] = 0; 
	   int cur = start; // вершина, с которой работаем
	   int prom;
	   int a;
	  	   
	   // пока есть необработанная вершина
	   while(!in_tree[cur])
	   {
	      in_tree[cur] = true;

	      for(i = 0; i < N; i++)
	      {
	         // если между cur и i есть ребро
	         if(adj_matrix[cur][i])
	         {
	            // считаем расстояние до вершины i:
	            // расстояние до cur + вес ребра
	            int d = dist[cur] + cost[cur][i];
	            // если оно меньше, чем уже записанное
	            if(d < dist[i])
	            {
	               dist[i]   = d;   // обновляем его
	               C[i] = cur; // и "родителя"
	            }
	         }
	      }

	      // ищем нерассмотренную вершину  с минимальным расстоянием
	      int min_dist = INT_MAX;
	      for(i = 0; i < N; i++)
	      {
	         if(!in_tree[i] && dist[i] < min_dist)
	         {
	            cur      = i;
	            min_dist = dist[i];
	         }
	      }
	   }

	   //printf("STOP\n\r");
	   /*
	   *  После окончания алгоритма значение c[i] будет указывать вершину, предпоследнюю в кратчайшем пути от start до i, 
	   *  и сам путь восстанавливается простым циклом.
	   */ 

	   // Теперь:
	   // в dist[i] минимальное расстояние от start до i
	   // в parent[i] вершина, из которой лежит оптимальный путь в i
	   // нужно распечатать пути
	   //Кратчайшие маршруты 
	   a = start;
	   printk("Route_tab MP[%d] ip=0x%x\n\r",start,my_current_kos.ip_addres);
	   for (i=0; i<N; i++)
	   {
	   
		   //C ->матрица стоимостей
	        if (i!=a && cost[C[i]][i]!=NULL)
	        {
	        	//printk("Ot MP[%d] -> MP[%d] hop = %d |C[i]=%d\n\r",start,i,dist[i],C[i]);
	          	if(dist[i]==10)
	          	{
	          	  printk("Ot MP[%d] -> MP[%d] hop = %d \n\r",start,i,dist[i]);
	          	}
	        	
	        	if(dist[i]==20)
	          	{
	          	
	          	 printk("Ot MP[%d] -> MP[%d] ->MP[%d]  hop = %d \n\r",start,C[i],i,dist[i]);
	          		
	          	}
	        	
	         //printk("from %d to %d =cost %d\n\r",start,i,dist[i]);
	         //printk("i=%d,C[i]=%d\n\r",i,C[i]);
	        }

	     //printk("i=%d,C[i]=%d\n\r",i,C[i]);  
	        
	   }
	
}










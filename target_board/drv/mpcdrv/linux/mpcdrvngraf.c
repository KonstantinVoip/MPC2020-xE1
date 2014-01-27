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
*..
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
#include <linux/delay.h>  // mdelay ,ms
//extern void get_ipaddr_my_kys(UINT8 *state,UINT32 *ip_addres,UINT8 *mac_address);
//extern struct KY_S my_current_kos;
/*****************************************************************************/
/*	PRIVATE MACROS							     */
/***********************ic******************************************************/
/*//////////////////Пока сделаю временные затычки//////*/
//Глобальная переменная признак шлюза или обычного сетевого элемента
static UINT8 i_am_schluz=1;
//Глобальная переменная признак наличия пакета с маршрутизацией 
static UINT8 packet_from_marsrutiazation=0;
//Глобальная переменная ip адрес нашего НМС3
static UINT32 no_marshrutization_nms3_ipaddr=0;

//
static UINT16 who_i_am_node=0;
static UINT16 who_is_schluz=0;



//
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///CURRENT _KYS
static struct KY_S
{
UINT32 ip_addres;
UINT8  *mac_address;
bool   state;
UINT8  marshrutization_enable; 
}my_current_kos;

struct commutacii
{
	UINT32 ipaddr_sosed[8];
	UINT8  tdm_direction_sosed[8];
    UINT8  paralel_sviazi_mk8[8];
};


/*Структура описывающая сетевой элемент для мультиплексора */
struct ngarf_setevoi_element
{
UINT32 nms3_ipaddr;
UINT32 curr_ipaddr;
UINT32 gate_ipaddr;


//UINT32 ipaddr_sosed[8];
//UINT8  tdm_direction_sosed[8];
struct commutacii comm;

UINT8  priznac_shcluzovogo;
//нужно добавить ещё пару полей для паралельныхсвязей
UINT16   node;  //порядковый номер узла
UINT16   puti_k_udalennomy[16];


}multipleksor[16];



struct pari_sviaznosti
{
UINT32  ip_addr;
UINT8   posad_mesto;
UINT8   mk8_vihod;
UINT32  soedinen_s_ipaddr;
}num_pari[16];





/*****************************************************************************/
/*	PRIVATE DATA TYPES						     */
/*****************************************************************************/
__be32  my_kys_ipaddr=0;                         //IP адрес моего КY-S
                                                 //вместо бесконечностей

////////////////////////////////////////////////
#define DLINNA_SVYAZI   12   /*длинна цепочки приёмник источник в пакете на 12 байт
                               в неё входит узел(наш мультиплексор и куда какому мультплексору)                     
                              */
                             
/*****************************************************************************/
/*	PRIVATE FUNCTION PROTOTYPES					                             */
/*****************************************************************************/
static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip,u8 *sosed_posad_mesto,u8 *mk8_sosed_vyhod);
/*Функция новая для Дейкстры с динамическим выделением памяти */
static bool algoritm_deijcstra_example(u16 curr_node,u16 num_of_node);



/* KERNEL MODULE HANDLING */
/*****************************************************************************/
/*	PRIVATE GLOBALS							     */
/*****************************************************************************/
#define INT_MAX  0xffff
/*****************************************************************************/
/*	PUBLIC GLOBALS							     */
/*****************************************************************************/
/*Добавил дома Алгоритм Дейкстры*/
/*соединение для нашей схемы*/
//Матрица соединенй  исходные данные для 4 узлов c петлёй как Сызранцев требует
//#define DEBUG_DEJCSTRA   0
//массив узлов в отсортированном порядке.
UINT8  b[255];

//Размер матрицы коммутации пока статиком сделан
#ifdef DEBUG_DEJCSTRA
static bool adj_matrix[16][16];
#endif




//#if 0

static bool adj_matrix[5][5]=
{
// ---0--|---1--|-2-|---3---|  
   0    ,1    ,1   ,0,	0,
   1    ,0    ,1   ,0,	0,
   1    ,1    ,0   ,1,	0,
   0    ,0    ,1   ,0,	1,
   0    ,0    ,0   ,1,	0

};


/*Матрица смежностей(стоимостей)для алгоритма Дейкстра
 исходные данные для построение путей
*/
static unsigned short int cost[5][5]=
{
 
// ---0--|---1--|-2-|---3---|  
   0     ,10     ,10      ,0xffff,0xffff , 
   10     ,0     ,10      ,0xffff,0xffff,
   10    ,10     ,0       ,10    ,0xffff,
   0xffff,0xffff ,10      ,0     ,10,
   0xffff,0xffff ,0xffff  ,10    ,0

};

//массив кратчайших расстояний на данном шаге до вершины i в начальный момент нет расстояний
//unsigned short dist[4]={0xffff,0xffff,0xffff,0xffff};
//массив посещённых вершин в начальный момент заполнеы нулями никого не посетили пока
//unsigned short used[4]={0,0,0,0};
//массив кратчайших путей
//signed short C[4]={-1,-1,-1,-1};

//#endif



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
//extern void p2020_get_recieve_packet_and_setDA_MAC(const u16 *in_buf,const u16 in_size,const u16 *mac_header);
//extern void p2020_get_recieve_virttsec_packet_buf(u16 buf[758],u16 len,u8 tsec_id);
//extern void p2020_revert_mac_header(u16 *dst,u16 *src,u16 out_mac[12]);

/*Function to sendpacket's to tsec   */
extern void send_packet_buf_to_tsec0(u16 buf[758],u16 len);
extern void send_packet_buf_to_tsec1(u16 buf[758],u16 len);
extern void send_packet_buf_to_tsec2(u16 buf[758],u16 len);



/*Extern fifo buffer function*/
extern void nbuf_set_datapacket_dir1  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir2  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir3  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir4  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir5  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir6  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir7  (const unsigned char *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir8  (const unsigned char *in_buf ,const u16 in_size);



/*
extern void nbuf_set_datapacket_dir9  (const u16 *in_buf ,const u16 in_size);
extern void nbuf_set_datapacket_dir10 (const u16 *in_buf ,const u16 in_size);*/


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



/**************************************************************************************************
Syntax:      	    void ngraf_get_ip_mac_my_kys (UINT8 state,UINT32 ip_addres,UINT8 *mac_address)
Parameters:     	беру от ky-s номер шлюза
Remarks:			timer functions 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
void ngraf_get_ip_mac_my_kys (UINT8 state,UINT32 ip_addres,UINT8 *mac_address)
{	
	//printk("virt_TSEC_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",buf[0],buf[1],buf[2],buf[3]);
	//printk("State =0x%x>>IP=0x%x,MAC =|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|\n\r",state,ip_addres,mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);
	my_current_kos.state=state;
	my_current_kos.ip_addres=ip_addres;
	my_current_kos.mac_address=mac_address;
	my_current_kos.marshrutization_enable=0;
	printk("++Set_Multiplecsor_IP_and_MAC_OK++\n\r");
	printk("State =0x%x>>IP=0x%x,MAC =|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|\n\r",state,ip_addres,my_current_kos.mac_address[0],my_current_kos.mac_address[1],my_current_kos.mac_address[2],my_current_kos.mac_address[3],my_current_kos.mac_address[4],my_current_kos.mac_address[5]);
}
/**************************************************************************************************
Syntax:      	    static bool algoritm_deijcstra_example(bool *adj_matrix ,u16 curr_node,u16 num_of_node)
Parameters:     	dejcstra
Remarks:			timer functions 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
static bool algoritm_deijcstra_example(u16 curr_node,u16 num_of_node)
{
	/*Пока статический рассчёт на 16 узлов в сети */
	
	u16 l_i=0,l_j=0;
	int i;
	bool in_tree[16] = {false}; //num_of_node
	
	
#ifdef DEBUG_DEJCSTRA 
	//нужно заполнять тоже в процессе работы
	unsigned short int cost[16][16];
#endif
	
	
	//массив кратчайших расстояний на данном шаге до вершины i в начальный момент нет расстояний val= 0xffff
	unsigned short dist[16]={0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
	//массив посещённых вершин в начальный момент заполнеы нулями никого не посетили пока  val = 0
	unsigned short used[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //массив кратчайших путей val =-1
	signed short C[16]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,};
    dist[curr_node] = 0;
    int cur = curr_node; // вершина, с которой работаем
    int prom;
    int a;
    
    //Цена пути одно соединение от одногомультика к другому 10(если оно есть)
    UINT16 cost_one_element=10;
    //нет соединений в матрице стоимостей
    UINT16 cost_no_soedinenii=0xffff;
    //смежные элементы
    UINT16 cost_smegnii_elementi=0x0000;
        
#ifdef DEBUG_DEJCSTRA 	
    
    for(l_i=0;l_i<num_of_node;l_i++)
    {
    	    	
    	for(l_j=0;l_j<num_of_node;l_j++)
    	{
    	
    		if(adj_matrix[l_i][l_j]==1)
    		{
    			cost[l_i][l_j]=cost_one_element;
    		}
    
    		if(adj_matrix[l_i][l_j]==0)
    		{
    			cost[l_i][l_j]=cost_no_soedinenii;
    		}
    		
    		if(l_i==l_j)
    		{
    			cost[l_i][l_j]=cost_smegnii_elementi;
    		}
    	      	
    	}	
    	
    }
#endif   
    
    //выводим на печать
    /*
    for(l_i=0;l_i<num_of_node;l_i++)
    {
    	    	
    	for(l_j=0;l_j<num_of_node;l_j++)
    	{
          printk ("i_str %d|j_stlb=%d|=0x%x \n\r",l_i,l_j,cost[l_i][l_j]);
           
    	}	
    }	
    */
    
  
    //Заполяем матрицу стоимостей.по входной матрице соединений.должны получить такую
	   // пока есть необработанная вершина
	   while(!in_tree[cur])
	   {
	      in_tree[cur] = true;

	      for(i = 0; i < num_of_node; i++)
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
	      for(i = 0; i < num_of_node; i++)
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
	   a = curr_node;
	   printk("Route_tab MP_node[%d] ip=0x%x\n\r",curr_node,my_current_kos.ip_addres);
	   for (i=0; i<num_of_node; i++)
	   {
	   
		    //C ->матрица стоимостей
	        //Количество прыжков (расстояние) до сетевого элемента 10 = 1 прыжок
		    if (i!=a && cost[C[i]][i]!=NULL)
	        {
	        	
		    	//C[i] ->массивы где храниться последняя посещённая вершина куда нам нужно идти
		    	
		    	printk("i=%d,dist[i]=%d,C[i]=%d\n\r",i,dist[i],C[i]);
		    	multipleksor[curr_node].puti_k_udalennomy[i]=C[i];
		    	
		    	
		    	
		    	//if (dist[i]==30)
		    	//{
		    	 //	multipleksor[curr_node].puti_k_udalennomy[i]=
		    		
		    	//}
		    	

		    	//массив кратчайших расстояний на данном шаге до вершины i в начальный момент нет расстояний val= 0xffff
		    	//unsigned short dist[16]
		    	//массив посещённых вершин в начальный момент заполнеы нулями никого не посетили пока  val = 0
		    	//unsigned short used[16]
		        //массив кратчайших путей val =-1
		    	//signed short C[16] 	
		    	/*

	          	if(dist[i]==10)
	          	{
	          	  //printk("Ot MP[%d] -> MP[%d] hop = %d \n\r",curr_node,i,dist[i]);
	          	  //b[255]
	          	  //multipleksor[curr_node].comm.ipaddr_sosed[m]=b[i];
	          	}
	        	
	        	if(dist[i]==20)
	          	{
	        	  //printk("Ot MP[%d] -> MP[%d] ->MP[%d]  hop = %d \n\r",curr_node,C[i],i,dist[i]);	
	             
	        	  //multipleksor[curr_node].comm.ipaddr_sosed[m]=b[i];
	          	
	          	}
	        
	        	if(dist[i]==30)
	          	{

	          	}
	          	
	        	if(dist[i]==40)
	          	{

	          	}
	        	
	        	if(dist[i]==50)
	          	{

	          	}
	        	
	        	
	        	if(dist[i]==60)
	          	{

	          	}
	        	
	        	if(dist[i]==70)
	          	{

	          	}
	        	
	        	
	        	if(dist[i]==80)
	          	{

	          	}
	        	
	        	if(dist[i]==90)
	          	{

	          	}
	        	if(dist[i]==100){}
	        	*/
	        // printk("from %d to %d =cost %d\n\r",curr_node,i,dist[i]);
	        // printk("i=%d,C[i]=%d\n\r",i,C[i]);
	        }
	    // printk("i=%d,C[i]=%d\n\r",i,C[i]);  
	   }//end dejcstra commutation
       //Восстанавливаем путь назад
	   




   //printk("i=%d,  %d->\n\r",i,multipleksor[curr_node].puti_k_udalennomy[i]);
	   
	
   
	   
return 1;
}





/**************************************************************************************************
Syntax:      	    void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u16 priznak_kommutacii)
Parameters:     	входной буфер(пакет),размер,ip_адрес назначения,признак что пакет от НМС3,
Remarks:			В эту функцию идут все пакеты предназначенные для маршрутизации и отправки дальше
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
void ngraf_packet_for_matrica_kommutacii(const u16 *in_buf ,const u16 in_size,u32 priznak_kommutacii,u32 priznak_nms3_arp_sender,u8 tdm_input_read_direction)
{
   //UINT16  out_mac[12];
   //UINT16  mac1[3];
   //UINT16  mac2[3];
   //static UINT16 iteration=0;
   //UINT8   priznak_scluz=0;
   //priznal Scluzovogo MPC
   //my_current_kos.ip_addres=0xC0A878AA;    //shluz
   //my_current_kos.ip_addres=0xC0A878AB;    //sosed 1
   //my_current_kos.ip_addres=0xC0A878AC;    //sosed 2
	
	UINT16  udp_dest_port=0;
   //Нельзя начинать передачу пока нет IP и MAC адреса с KY-S
    if(my_current_kos.state==0){return;}
   // printk("PR_commut =0x%x \n\r",priznak_kommutacii);
   //Признак шлюзового пока делаем так
   //multipleksor[0].priznac_shcluzovogo=1;
     
 //для отладки на ките 
#if 0
    memcpy(&udp_dest_port,&in_buf[18],2); 
    //printk("udp_dest_port=%d,0x%x\n\r",udp_dest_port,udp_dest_port);
    if (udp_dest_port==18000)
    {
  	  //строим матрицу коммутации
      //printk("+ngraf_packet+\n\r");
  	  ngraf_packet_for_my_mps(in_buf ,in_size);
    }
#endif
    
    /*Отработка ситуации когда для данного мультплексора пока не пришёл пакет с маршрутизацией*/
    if(packet_from_marsrutiazation==0)
    {
    	
    	/*Если я шлюз*/
    	if(i_am_schluz==1)
    	{
    	  /*обработка ARP пакета для шлюзового*/	
    	  if(priznak_nms3_arp_sender)
    	  {
      		//Проверяем что пакет идёт к КY-S нашему шлюза в даннос случае DA ip нашего KY-S совпадает с DA MAC в ARP
      		if(my_current_kos.ip_addres==priznak_kommutacii) 	
      		{
      		 
      		 no_marshrutization_nms3_ipaddr=priznak_nms3_arp_sender;
      		 //отправляем моему KY-S в eth1
      		 printk("!noroutetab_ARP_scluz_otNMS3_send(0x%x)->kys-s!\n\r",no_marshrutization_nms3_ipaddr);
      		 send_packet_buf_to_tsec1(in_buf,in_size);
      		 
      		 //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
      		
      		}
      	
      		//Пакет идёт назад к НМС3 c DA НМС3 находящемся в ARP
      		if(no_marshrutization_nms3_ipaddr==priznak_kommutacii)
      		{
      		//отправляем обратно НМС3 на выход eth2
      	    printk("!noroutetab_ARP_scluz_otKYS_send->nms3!\n\r");
      	    send_packet_buf_to_tsec0(in_buf,in_size);
      	    //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,0);
      		}
    		  
    	  }	  
    	  else
    	  {
    	  /*обработка обычного пакета для шлюзового*/
    		  if(my_current_kos.ip_addres==priznak_kommutacii) //если да то узнаем адрес нашего НМС3	
    		  {
    			  //Пакет с матрицой коммутации для моего МПС шлюзового
    			  memcpy(&udp_dest_port,&in_buf[18],2); 
    			  if (udp_dest_port==18000)
    			  {
    			  ngraf_packet_for_my_mps(in_buf ,in_size);
    			  }
    			  //если обычные пакеты.без маршрутизации
    			  else
    			  {	
    			  //printk("!noroutetab_pack_scluz_otNMS3_send->kys-s\n\r");
    			   send_packet_buf_to_tsec1(in_buf,in_size);
    			  }
    	        	
    		  }
    		  //Пакет идёт назад к НМС3
    		  if(no_marshrutization_nms3_ipaddr==priznak_kommutacii)
    		  {
    			  //printk("!noroutetab_pack_scluz_otKYS_send->nms3!\n\r");
    			   send_packet_buf_to_tsec0(in_buf,in_size);
    			  //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,0);
    		  }
    		
    	  }/*конец обработки обычного пакета для шлюзового*/
    		  
    	}//END i_am Scluz
    	/* Если я обычный сетевой элемент*/
    	else
    	{
    	  
    	    //определяем откуда свалился пакет с какого направления пока нет таблицы маршрутизации
    		//Здесь мы должны знать откуда с какого направления пришёл ARP пакет чтобы отправить
    		//его назад пока нет таблицы маршрутизации.
    		/*обработка ARP пакета для обычного сетевого элемента не шлюза*/
    	    if(priznak_nms3_arp_sender)
    	    {
    	    	static u8 l_tdm_input_direction=0;
    	    	
    	    	//Проверяю что это ARP пакет идёт из tdm направления запоминаю откуда пришёл и отправляю потом назад
    	    	if(!tdm_input_read_direction==0)
    	    	{
    	    	  l_tdm_input_direction =tdm_input_read_direction;
    	      	  //printk("!noroutetab_ARP_element_otNMS3(tdm_dir=%d)_send>kys-s!\n\r",l_tdm_input_direction);
    	    	  printk("!noroutetab_ARP_element_otNMS3(tdm_dir=%d)!\n\r",l_tdm_input_direction);
    	    	}
    	    	
    	    	
    	    	//printk("EL_ARP =nms3_arp_sender= 0x%x |pr_kommut= 0x%x\n\r",priznak_nms3_arp_sender,priznak_kommutacii);
    	    	    		
    	    	//да это ARP //обрабатываем особыам образом ARP пакет
    	    	// пакет идёт к своему КY-S сетевого элемента 
    	    	if(my_current_kos.ip_addres==priznak_kommutacii) //если да то узнаем адрес нашего НМС3	
    	    	{
    	    		no_marshrutization_nms3_ipaddr=priznak_nms3_arp_sender;
    	    		//отправляем моему KY-S в eth1 
    	    		//printk("Send ARP to KYS\n\r");
    	    		printk("!noroutetab_ARP_element_otNMS3_send->kys-s\n\r");
    	    		//p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
    	    	}
    	
    	    	//Пакет идёт назад к НМС3 c DA НМС3 находящемся в ARP
    	    	if(no_marshrutization_nms3_ipaddr==priznak_kommutacii)
    	    	{
    	    		//отправляем обратно нзад НМС3 выход откуда пришёл на выход tdm
    	    		//Здесь надо знать куда отправить обратно пакет в какое направление tdm
    	    		printk("!noroutetab_ARP_element_otKYS_send->nms3 to (tdm_dir=%d)!\n\r",l_tdm_input_direction);
    	    		switch (l_tdm_input_direction)
    	    		{
    	    		case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    	    		case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    	    		case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    	    		case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break;  
    	    		case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);break;
    	    		case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);break;
    	    		case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);break;
    	    		case 8:nbuf_set_datapacket_dir8  (in_buf ,in_size);break; 
    	    		default:printk("?ARP_NMS3->Send  UNICNOWN to IP sosed \n\r");break;
    	    		}
    	    	    			  	    			
    	    	}
    	    	    		
    	   }//End priznak nms3 ARP sender obichi setevoi element
    	    /*обработка обычного пакета для сетевого элемента*/
    	  else
    	  {
    		  
    		  //Здесь нам нужно просто получит пакет и построить таблицу маршрутизации
    		  //больше ничего не надо нам пока нет таблицы маршрутизации пакет нет таблицы маршрутизации пакет не пропускаем.
    		  if(my_current_kos.ip_addres==priznak_kommutacii) //если да то узнаем адрес нашего НМС3	
    		  {
    			  //Пакет с матрицой коммутации для моего МПС шлюзового
    			  memcpy(&udp_dest_port,&in_buf[18],2); 
    			  if (udp_dest_port==18000)
    			  {
    			  ngraf_packet_for_my_mps(in_buf ,in_size);
    			  }
    	        	
    		  }
    		  
    	  }/*конец обработки обычного пакета для сетевого элемента*/
    		 
    	}//End obichii setevoi elemnt
    		
    }//End no marshrutization packet
    /***************************************************************************************/
    /*Здесь начинатеться ситуация когда в данном сетевом элемнте есть таблица маршрутизации*/
    /***************************************************************************************/
    else
    {
    	
    	
    	
    	
    	
    	
    	
    	
    }//End global packet for marshrutization present
    
    
    
#if 0 
  // printk("priznak arp_sender =0x%x|priznak commuutaci=0x%x\n\r",priznak_nms3_arp_sender,priznak_kommutacii);
    //Обрабатываем пакеты для шлюзвого сетевого элемента
    if(multipleksor[0].priznac_shcluzovogo==1)
    {
    	
    	//printk("packet_to_scluz\n\r");
    	
    	//Обработка ARP пакета для шлюзового
    	if(priznak_nms3_arp_sender)
    	{
    		//printk("ARP_ot_NMS3 =nms3_arp_sender= 0x%x |pr_kommut= 0x%x\n\r",priznak_nms3_arp_sender,priznak_kommutacii);
    		
    		//да это ARP //обрабатываем особыам образом ARP пакет
    		//Проверяем что пакет идёт к КY-S нашему шлюза в даннос случае DA ip нашего KY-S совпадает с DA MAC в ARP
    		if(my_current_kos.ip_addres==priznak_kommutacii) //если да то узнаем адрес нашего НМС3	
    		{
    		 multipleksor[0].nms3_ipaddr=priznak_nms3_arp_sender;
    		 //отправляем моему KY-S в eth1 
    		 p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
    		}
    	
    		//Пакет идёт назад к НМС3 c DA НМС3 находящемся в ARP
    		if(multipleksor[0].nms3_ipaddr==priznak_kommutacii)
    		{
    		//отправляем обратно НМС3 на выход eth2
    			p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,2);
    		}
    
    		
    		//Выполняем проверку что есть таблица маршрутизации если нет то ничего не шлём
    		if((multipleksor[0].ipaddr_sosed[0]==0)&&(multipleksor[0].ipaddr_sosed[1]==0))
		    {
    		 	//return;
    			//printk("ARP_ot_NMS3_NO_MARSRUTIZATION_TABLE\n\r");
		    }
    		else
    		{
    			//Тепреь ARP к соседям только через таблицу маршрутизации проверяем что есть
    			//если нет то brake
    			if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[0])
    			{
    				printk("ARP_NMS3->Send to IP sosed 0x%x dir %d\n\r",multipleksor[0].ipaddr_sosed[0],multipleksor[0].tdm_direction_sosed[0]);
    				switch(multipleksor[0].tdm_direction_sosed[0])
    				{
    				case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    				case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    				case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    				case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break;  
    				case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);break;
    				case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);break;
    				case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);break;
    				case 8:nbuf_set_datapacket_dir8  (in_buf ,in_size);break; 
    				default:printk("?ARP_NMS3->Send  UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[0]);break;
    				}	    		
    			}	
    			if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[1])
    			{
    				printk("ARP_NMS3->Send to IP sosed 0x%x direction %d\n\r",multipleksor[0].ipaddr_sosed[1],multipleksor[0].tdm_direction_sosed[1]);
    				switch(multipleksor[0].tdm_direction_sosed[1])
    				{
    				case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    				case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    				case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    				case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break; 
    				case 5:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    				case 6:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    				case 7:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    				case 8:nbuf_set_datapacket_dir4  (in_buf ,in_size);break;
    				default:printk("?ARP_NMS3->Send UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[1]);break;
    				}	    		
    			}	
    			
    		}		
    	//И так далее по количеству соседей;		
    	}//END _priznak_nms3_arp_sender
    	
  	    
        //++++++++++++++++++++++++Признак обычных пакетов для шлюзового+++++++++++++++++++++++++
    	else
    	{
    	    //Пакет идёт от c SA адрес НМС3 к и DA моего KY-S шлюзового пакеты к шлюзу.
      		if(my_current_kos.ip_addres==priznak_kommutacii) //если да то узнаем адрес нашего НМС3	
        	{
        	    //Пакет с матрицой коммутации для моего МПС шлюзового
      			memcpy(&udp_dest_port,&in_buf[18],2); 
      			if (udp_dest_port==18000)
      		    {
      			  ngraf_packet_for_my_mps(in_buf ,in_size);
      		    }
      			//если обычные пакеты.без маршрутизации
      			else
      			{	
      			printk("PACK_ot_NMS3_SEND_KYS\n\r");
      			p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
      			}
        	
        	}
    		//Пакет идёт назад к НМС3
    		if(multipleksor[0].nms3_ipaddr==priznak_kommutacii)
    		{
    			p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,2);
    		}
  
    		
    		//Выполняем проверку что есть таблица маршрутизации если нет то ничего не шлём
    		if((multipleksor[0].ipaddr_sosed[0]==0)&&(multipleksor[0].ipaddr_sosed[1]==0))
		    {
    		 	
    			//return;
    			 // printk("PACK_ot_NMS3_NO_MARSRUTIZATION_TABLE\n\r");
		        //break;
		    }
    		else
    		{
    		
    			if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[0])
    			{
    				printk("PACK_NMS3->Send to IP sosed 0x%x dir %d\n\r",multipleksor[0].ipaddr_sosed[0],multipleksor[0].tdm_direction_sosed[0]);
    				switch(multipleksor[0].tdm_direction_sosed[0])
    				{
    				case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    				case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    				case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    				case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break; 
    				case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);break;
    				case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);break;
    				case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);break;
    				case 8:nbuf_set_datapacket_dir8  (in_buf ,in_size);break;
    				default:printk("?PACK_NMS3->Send UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[0]);break;
    				}	    		
    			}	
    			if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[1])
    			{
    				printk("PACK_NMS3->Send to IP sosed 0x%x direction %d\n\r",multipleksor[0].ipaddr_sosed[1],multipleksor[0].tdm_direction_sosed[1]);
    				switch(multipleksor[0].tdm_direction_sosed[1])
    				{
    				case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    				case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    				case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    				case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break; 
    				case 5:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    				case 6:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    				case 7:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    				case 8:nbuf_set_datapacket_dir4  (in_buf ,in_size);break; 
    				default:printk("?PACK_NMS3->Send UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[1]);break;
    				}	    		
    			}
  		
    		}//END ELSE proverka commutacii	
    			
    	  }//END global ELSE
    	
    
    }
    //+++++++++++++++++++++++++++++++++Обработка нешлюзовых Элементов++++++++++++++++++++++++++++++//
    /***************************************************************************************************
     *************************************************************************************************** 
     ***************************************************************************************************/
    else
    {
    	        //Признак обработки ARP
    	    	if(priznak_nms3_arp_sender)
    	    	{
    	        
    	    		//printk("EL_ARP =nms3_arp_sender= 0x%x |pr_kommut= 0x%x\n\r",priznak_nms3_arp_sender,priznak_kommutacii);
    	    		
    	    		//да это ARP //обрабатываем особыам образом ARP пакет
    	    		//Проверяем что пакет идёт к КY-S нашему шлюза в даннос случае DA ip нашего KY-S совпадает с DA MAC в ARP
    	    		if(my_current_kos.ip_addres==priznak_kommutacii) //если да то узнаем адрес нашего НМС3	
    	    		{
    	    		 multipleksor[0].nms3_ipaddr=priznak_nms3_arp_sender;
    	    		 //отправляем моему KY-S в eth1 
    	    		 //printk("Send ARP to KYS\n\r");
    	    		 p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
    	    		}
    	    	
    	    		
    	    		//Попробую ход конём пока нет идёт пакет назад
    	    		if((multipleksor[0].ipaddr_sosed[0]==0)&&(multipleksor[0].ipaddr_sosed[1]==0))
    			    {
    	    			//Пакет идёт назад к НМС3 c DA НМС3 находящемся в ARP
    	    			if(multipleksor[0].nms3_ipaddr==priznak_kommutacii)
    	    			{
    	    			 //Здесь надо знать куда отправить обратно пакет в какое направление tdm
    	    			 //пока в первое и второе
    	    			 nbuf_set_datapacket_dir1  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir2  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir3  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir4  (in_buf ,in_size);
    	    			 /*
    	    			 nbuf_set_datapacket_dir5  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir6  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir7  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir8  (in_buf ,in_size);*/
    	    			
    	    			}
    			    
    			    }//end multipleksor table of marsrutiazation
    	    		else //Появилась таблица маршрутизации
    	    		{
    	    		   
    	    		  if(multipleksor[0].nms3_ipaddr==priznak_kommutacii)
    	    		   {
    	    			   priznak_kommutacii=multipleksor[0].gate_ipaddr;   
    	    		   }
    	    			
    	    		     if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[0])
    	    			    {
    	    			  	  printk("ARP_ot_0x%x->Send  to IP sosed 0x%x dir %d\n\r",my_current_kos.ip_addres,multipleksor[0].ipaddr_sosed[0],multipleksor[0].tdm_direction_sosed[0]);
    	    			 	  switch(multipleksor[0].tdm_direction_sosed[0])
    	    			 	  {
    	    			      case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    	    			      case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    	    			      case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    	    			      case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break; 
    	    			      case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);break;
    	    			      case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);break;
    	    			      case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);break;
    	    			      case 8:nbuf_set_datapacket_dir8  (in_buf ,in_size);break; 
    	    			      default:printk("?ARP_ot_->Send  UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[0]);break;
    	    			      }	    		
    	    			    			
    	    			   }	
    	    			 if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[1])
    	    			    {
    	    			      printk("ARP_ot_0x%x->Send  to IP sosed 0x%x direction %d\n\r",my_current_kos.ip_addres,multipleksor[0].ipaddr_sosed[1],multipleksor[0].tdm_direction_sosed[1]);
    	    			      switch(multipleksor[0].tdm_direction_sosed[1])
    	    			      {
    	    			      case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
    	    			      case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
    	    			      case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
    	    			      case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break; 
    	    			      case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);break;
    	    			      case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);break;
    	    			      case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);break;
    	    			      case 8:nbuf_set_datapacket_dir8  (in_buf ,in_size);break; 
    	    			      default:printk("?ARP_ot_->Send UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[1]);break;
    	    			      }	    		
    	    			    }
    	    			
    	    		} //END Else
    	    		
    	    	}//end priznak 	priznak_nms3_arp_sender
    	    	
    //+++++++++++++++++++++обрабатываем обычные пакеты+++++++++++++++++++++++++	    	
   
    	    	
    	    	else
    	    	{
    	    		//Пакет идёт от c SA адрес НМС3 к и DA моего KY-S шлюзового пакеты к шлюзу.
    	    		if(my_current_kos.ip_addres==priznak_kommutacii) //если да то узнаем адрес нашего НМС3	
    	    		{
    	    		   //Пакет с матрицой коммутации для моего МПС шлюзового
    	    		   memcpy(&udp_dest_port,&in_buf[18],2); 
    	    		   if (udp_dest_port==18000)
    	    		   {
    	    		       ngraf_packet_for_my_mps(in_buf ,in_size);
    	    		   }
    	    		   //если обычные пакеты.без маршрутизации
    	    		   else
    	    		   {	
    	    		    //printk("SendPackto KY-S\n\r");
    	    			p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
    	    		   }
    	    		        	
    	    	    }
    	    		
    	    		//Нет таблицы маршрутизации
    	    		if((multipleksor[0].ipaddr_sosed[0]==0)&&(multipleksor[0].ipaddr_sosed[1]==0))
    			    {

    	    			if(multipleksor[0].nms3_ipaddr==priznak_kommutacii)
    	    			{
    	    			 //Здесь надо знать куда отправить обратно пакет в какое направление tdm
    	    			 //пока в первое и второе
    	    			 nbuf_set_datapacket_dir1  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir2  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir3  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir4  (in_buf ,in_size);
    	    			/* nbuf_set_datapacket_dir5  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir6  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir7  (in_buf ,in_size);
    	    			 nbuf_set_datapacket_dir8  (in_buf ,in_size);*/
    	    			 //p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,2);
    	    			}		
    	    			
    			    }
    	    		else//появилась таблица маршрутизации
    	    		{
    	    			
    	    			if(multipleksor[0].nms3_ipaddr==priznak_kommutacii)
    	    			{
    	    			   priznak_kommutacii=multipleksor[0].gate_ipaddr;   
    	    			}
   	    		       
    	    			if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[0])
   	    			    {
   	    			  	  printk("PACK_ot_0x%x->Send to IP sosed 0x%x dir %d\n\r",my_current_kos.ip_addres,multipleksor[0].ipaddr_sosed[0],multipleksor[0].tdm_direction_sosed[0]);
   	    			 	  switch(multipleksor[0].tdm_direction_sosed[0])
   	    			 	  {
   	    			      case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
   	    			      case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
   	    			      case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
   	    			      case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break;  
   	    			      case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);break;
   	    			      case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);break;
   	    			      case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);break;
   	    			      case 8:nbuf_set_datapacket_dir8  (in_buf ,in_size);break; 
   	    			      default:printk("?PACK_ot->Send UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[0]);break;
   	    			      }	    		
   	    			    			
   	    			   }	
   	    			    if (priznak_kommutacii==multipleksor[0].ipaddr_sosed[1])
   	    			    {
   	    			      printk("PACK_ot_0x%x->Send to IP sosed 0x%x direction %d\n\r",my_current_kos.ip_addres,multipleksor[0].ipaddr_sosed[1],multipleksor[0].tdm_direction_sosed[1]);
   	    			      switch(multipleksor[0].tdm_direction_sosed[1])
   	    			      {
   	    			      case 1:nbuf_set_datapacket_dir1  (in_buf ,in_size);break;
   	    			      case 2:nbuf_set_datapacket_dir2  (in_buf ,in_size);break;
   	    			      case 3:nbuf_set_datapacket_dir3  (in_buf ,in_size);break;
   	    			      case 4:nbuf_set_datapacket_dir4  (in_buf ,in_size);break;  
   	    			      case 5:nbuf_set_datapacket_dir5  (in_buf ,in_size);break;
   	    			      case 6:nbuf_set_datapacket_dir6  (in_buf ,in_size);break;
   	    			      case 7:nbuf_set_datapacket_dir7  (in_buf ,in_size);break;
   	    			      case 8:nbuf_set_datapacket_dir8  (in_buf ,in_size);break; 
   	    			      default:printk("?PACK_ot->Send UNICNOWN to IP sosed 0x%x direction  =%d?\n\r",multipleksor[0].tdm_direction_sosed[1]);break;
   	    			      }	    		
   	    			    }
    	    		    			
    	    		} //END ELSE
    	    		    		
    	    	}//END ELSE gloval
    	    	  	
    }//END GLOBAL GLOBAL ELSE
   
    
#endif    
	  	
}



/**************************************************************************************************
Syntax:      	    void ngraf_packet_for_my_mps(skb->data ,(uint)skb->len)
Parameters:     	void data
Remarks:			Пакет с маршрутизацией для данного МПС 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
bool ngraf_packet_for_my_mps(const u16 *in_buf ,const u16 in_size)
{
  
  UINT32 data_graf_massive[32]; //128 bait 4 *32 bait
  UINT8  soed_mk8_tdm_from_to[10][10];
  

  UINT16 razmer_data_graf_massive=0;
  UINT8  smeshenie_grisha_scluz=0;
   
  __be32  l_ipaddr=0;
  UINT8   my_posad_mesto=0;
  UINT8   my_mk8_vihod=0;
  __be32  sosed_ipaddr=0;
  UINT8   sosed_posad_mesto=0;
  UINT8   sosed_mk8_vyhod=0;
    
   //здесь храниться длинна одной связи либо в байтах либо в количестве элементов по 4 байта каждый; 
   UINT16 dlinna_pari_sviaznosti_byte=0;
   //Общее количество пар связности в пакете по размеру пакета
   UINT16 number_of_par_sviaznosti_in_packet=0;
   //всего количество связей в пакете
   UINT16 num_of_svyazi_all=0;
   //количество узлов в сети
   UINT16 num_of_uzlov_v_seti=0;
   
   static UINT16 iteration=0;
   UINT16 i,j,t;
   
   //Переменные для подсчёта паралельных связей
   UINT16 num_of_paralel_sviazy=0;
   UINT16 max_of_mk8_tdm_dir=10;
   
   memset(soed_mk8_tdm_from_to,0x00,sizeof(soed_mk8_tdm_from_to));
   /*
   adj_matrix[0][0]=0 ;adj_matrix[0][1]=0 ;adj_matrix[0][2]=1 ;
   adj_matrix[1][0]=0 ;adj_matrix[1][1]=0 ;adj_matrix[1][2]=1 ; 
   adj_matrix[2][0]=1 ;adj_matrix[2][1]=1 ;adj_matrix[2][2]=0 ;
   */
   
   printk("------------------Clear_matrica---------%d-------------\n\r",iteration);

   //Динамическое выделение памяти
   
//#if 0   
   
   
   //printk("matrica_packet_recieve=%d\n\r",iteration);
   //18 это в пакете наш  UDP  порт destination
    printk("IP_ADDR:"); 
    
    memcpy(&nms3_ip_addres,&in_buf[13],4); 
	if(nms3_ip_addres==0){printk("?bad nms3_ip_addres =0?\n\r");return -1;}
	printk ("NMS3_ip=<0x%x>|",nms3_ip_addres);
	

	//memcpy(&protocol_version,&in_buf[13],4);
	//printk ("NMS3_protocol_version =0x%x>\n\r",protocol_version);
	memcpy(&scluz_ip_addres,&in_buf[25],4);
	printk ("SCHLUZ_ip=<0x%x>|",scluz_ip_addres);
	if(scluz_ip_addres==0){printk("?bad scluz_ip_address =0?\n\r");return -1;}
	
	
	if(my_current_kos.ip_addres==0){printk("?bad current_kos.ip_addres exit=0?\n\r");return -1;}
	printk("CURR_ip=<0x%x>\n\r",my_current_kos.ip_addres);
	
	//Определяем что мы шлюз или простой элемент.
	printk("MP_STATUS:");
	if(my_current_kos.ip_addres==scluz_ip_addres)
	{ 
	printk("<MP_Scluz>\n\r");
	priznac_scluz=1;
	}

	else
	{
	printk("<MP_Element>\n\r");
	priznac_scluz=0;
	}
	
	

    printk("MATRICA:");  
	//Первые 8 байт это название Гришиного протокола,следующие 4 байт это ip адрес шлюза. = 12 байт
	smeshenie_grisha_scluz=(4+8);  //смещение 12 байт или 6 элементов в hex;
	razmer_data_graf_massive=in_size-42-smeshenie_grisha_scluz; //размер данных в массиве
	if(razmer_data_graf_massive<12){printk("?bad razmer_data_graf_massive =%d bait?\n\r",razmer_data_graf_massive);return -1;}
	printk("size_all_par_bait_in_packet=%d|",razmer_data_graf_massive);
	
	
	//21 байт это начало данных.+смещение получаем массив пар связности
	memcpy(&data_graf_massive,&in_buf[21+6],razmer_data_graf_massive);  
	//printk("!massive_copy_ok=0x%8x!\n\r",data_graf_massive[1]);
	//подсчитываем количество связей (пар) в пакете DLINN_SVYAZI 12 bait
	//odna sviaz:
	//Istochnic                               ||Priemnic
	//IP-address|nomer_PM|nomer_port_isch_MK8 ||//IP-address|nomer_PM|nomer_port_priemnic_MK8
	//(4-bait)   (1-bait)     (1-bait)        ||(4-bait)   (1-bait)     (1-bait)    
	/*number_of_par_sviaznosti = общий размер данных в пакете 1440 байт /12 байт длинна одной пары связности = 120 пар
	 *kolichestvo_iteration_on_this_packet = 120 пар /количество пар для одного сетевого элемента в случае МК8 - 10 = 12
	 *итого получаеться 12 сетевых элемeнтов по 10 связей в каждом. 
	 */
	  number_of_par_sviaznosti_in_packet=razmer_data_graf_massive/DLINNA_SVYAZI; 
	  printk("num_par_in_packet=%d\n\r",number_of_par_sviaznosti_in_packet);
      
	  //Рабочая штука
	  u8 count =0;
	  u8 num_paralel_svyaz=0;
	  for(i=0;i<number_of_par_sviaznosti_in_packet;i++)
	  {
	  // printk("first =%d,second =%d|\n\r",0+count+i,1+i+count);	  
      //#if 0 
	  parse_pari_svyaznosti(&data_graf_massive[dlinna_pari_sviaznosti_byte],&l_ipaddr,&my_posad_mesto,&my_mk8_vihod,&sosed_ipaddr,&sosed_posad_mesto,&sosed_mk8_vyhod);
	  //1 элемент в свзяи
	  num_pari[0+count+i].ip_addr=l_ipaddr;
	  num_pari[0+count+i].mk8_vihod=my_mk8_vihod;
	  num_pari[0+count+i].posad_mesto=my_posad_mesto;
	  num_pari[0+count+i].soedinen_s_ipaddr=sosed_ipaddr; //С кем соединён первый
	  // 2 элемент в свзяи
	  num_pari[1+i+count].ip_addr=sosed_ipaddr;
	  num_pari[1+i+count].mk8_vihod=sosed_mk8_vyhod;
	  num_pari[1+i+count].posad_mesto=sosed_posad_mesto;
	  num_pari[1+i+count].soedinen_s_ipaddr=l_ipaddr; //С кем соединён второй
      
	  //Дополнительно ищем паралельные вхождения в каждой паре связности 
	  //и говорим есть ли паралельные вхождения в данно  пакете.  
	  //Заполняю матрицу коммутации МК8 только для своего ip
	  //Случай соединения с самим собой
	  if(l_ipaddr==my_current_kos.ip_addres)
	  {  
		//заполняем матрицу соединений МК-8
		//printk("+my_mk8_vihod=%d|sosed_mk8_vyhod=%d\n\r",my_mk8_vihod,sosed_mk8_vyhod);
	    soed_mk8_tdm_from_to[my_mk8_vihod][sosed_mk8_vyhod]=(UINT8)sosed_ipaddr;
	  	    
	   }
	  if(sosed_ipaddr==my_current_kos.ip_addres)
	  {
		 //soed_mk8_tdm_from_to[my_mk8_vihod][sosed_mk8_vyhod]=(UINT8)l_ipaddr;
		 soed_mk8_tdm_from_to[sosed_mk8_vyhod][my_mk8_vihod]=(UINT8)l_ipaddr;
	  }
	  	  
	  dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+3;
	  
      count=1+i;

	  }//end for cicle
 
  //Количество всего полных связей в пакете
    num_of_svyazi_all=number_of_par_sviaznosti_in_packet*2;
  //Выводим матрицу соединенй МК8 пока для локального меня
  /*
  for(i=0;i<max_of_mk8_tdm_dir;i++)
  {
	  
	 for(j=0;j<max_of_mk8_tdm_dir;j++)
	 {    	  
	   if(soed_mk8_tdm_from_to[i][j])
	   {	  
	   printk("from %d->to %d,val=%d\n\r",i,j,soed_mk8_tdm_from_to[i][j]);
	   }
	  
	 }
	 
  }
  */
  
  //Выводим полностью все cвязи которые у нас получились

  //Алгоритм подсчётка количества сетевых элементов и сортировка их в порядке возрастания присвоение порядкового номера
  //каждому сетевому элементу пока работаем по младшему разряду ip адреса a.b.c.255 только потом нужно модифицировать
  //наш алгоритм
  /*Доработать Алгоритм для всех IP адресов */
  UINT8  c[255];//b[255];
  UINT16 k=0;
  UINT8  max_v=255;
  
  for(i=0;i<max_v;i++){c[i]=0;}
  for(i=0;i<num_of_svyazi_all;i++){c[(UINT8)num_pari[i].ip_addr]++;}
  
  for(i=0;i<max_v;i++)
  {
	  for(j=0;j<c[i];j++)
	  {
		  if(j==0)
		  {
		   b[k]=i;  
		   if(i==(UINT8)my_current_kos.ip_addres)
		   {
			   who_i_am_node=k;
		   }
		   if(i==(UINT8)scluz_ip_addres)
		   {
			   who_is_schluz=k;
		   }
		   
		   k++;
		  
		  num_of_uzlov_v_seti++;
		  }
	  } 
	  
  }
  
  ///!!!!Заполняем данные для сетвого элемента мультиплексора!!!!!//
  multipleksor[who_i_am_node].node=who_i_am_node;
  multipleksor[who_i_am_node].nms3_ipaddr=nms3_ip_addres;
  multipleksor[who_i_am_node].curr_ipaddr=my_current_kos.ip_addres;
  multipleksor[who_i_am_node].gate_ipaddr=scluz_ip_addres;
  multipleksor[who_i_am_node].priznac_shcluzovogo=priznac_scluz;
  ///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!///
  
  //Заполняем структуру сетевого элемента от меня кто есть соседи
  //multipleksor[who_i_am_node].comm.ipaddr_sosed[m]= 0x888888;
  //multipleksor[who_i_am_node].comm.tdm_direction_sosed[0]=0x7777;
  
  
  UINT8 array_paralel_svuazei[8]; 
  memset(array_paralel_svuazei,0x00,sizeof(array_paralel_svuazei));
  printk("NODE:") ;
  //Выводим полностью массив b[k] cоответсвующими порядковыми аресами мультплексора
  //каждому IP адресу соответсвует свой номер узла 0,1,2,3.
  printk("all_of_MP_node=%d|",num_of_uzlov_v_seti);
  printk("i'am_node_num=%d|scluz_node_num=%d\n\r",who_i_am_node,who_is_schluz);

    
  printk("MK8_%d:\n\r",who_i_am_node);
  for(i=0;i<num_of_uzlov_v_seti;i++)
  {  
	  //Обнуляем счётчик паралельных связей для следующего узла
	  num_of_paralel_sviazy=0;
	  for(j=0;j<max_of_mk8_tdm_dir;j++)
      {
 	  
    	  for(t=0;t<max_of_mk8_tdm_dir;t++)
    	  {	  
   
    	    if(b[i]==soed_mk8_tdm_from_to[j][t])
    	    {
    	    	  printk("from %d-> to %d,mp_ip->=%d\n\r",j,t,soed_mk8_tdm_from_to[j][t]);
    	    	 // printk("num_uzla_i= %d|ip_addr=%d |paralel_svyze=%d\n\r",i,b[i],num_of_paralel_sviazy); 
    	    	 // array_paralel_svuazei[i]=num_of_paralel_sviazy++;
    	    	  
    	    	  // printk("num_uzla= %d|ip_addr=%d |paralel_svyze=%d\n\r",i,b[i],num_of_paralel_sviazy);  
    	    	  num_of_paralel_sviazy++;
    	          //printk("\n\r");
    	    	  //заполняем структуру количество паралельных связей для моего мультплексора       
    	    }
    	  }//end for t
    
      }//end for j
	  array_paralel_svuazei[i]=num_of_paralel_sviazy;
  }//end for uzlov v seti
  
  printk("PARALEL_ot->%d:\n\r",who_i_am_node);
  printk("node[0]=%d,node[1]=%d,node[2]=%d,node[3]=%d,node[4]=%d\n\r",array_paralel_svuazei[0],array_paralel_svuazei[1],array_paralel_svuazei[2],array_paralel_svuazei[3],array_paralel_svuazei[4]);
    
 
#ifdef DEBUG_DEJCSTRA 	
  
  
  //printk("num_of_all_par_svyazi=%d\n\r",num_of_svyazi_all);
  //Алгоритм построения матрицы коммутации
  for(t=0;t<num_of_svyazi_all;t++)
  {
  	  //printk("para_svyazi =%d|",t);
  	  //printk("ip.addr =%d , b[i] =%d \n\r",(UINT8)num_pari[t].ip_addr,b[i]);
      for(i=0;i<num_of_uzlov_v_seti;i++)	  
      {
    
    	  //printk("ip.addr =%d , b[i] =%d \n\r",(UINT8)num_pari[t].ip_addr,b[i]); 
    	  if((UINT8)num_pari[t].ip_addr==b[i])
    	  {
    		  
    		 // printk("sovpalo i=%d+\n\r",i);
    		  //array_of_soedineniy[i][t]=1; 
    		  //elemen_eto=(UINT8)num_pari[t].soedinen_s_ipaddr;
    		  for(j=0;j<num_of_uzlov_v_seti;j++)
    		  {
    			  
    			  if(i==j)
    		      {
    				  adj_matrix[i][j]=0; 
    		      }  
    			  else
    			  {	  
    				  if(b[j]==(UINT8)num_pari[t].soedinen_s_ipaddr)
    				  {
    				  //printk("sovpalo i=%d|j =%d\n\r",i,j);
    				  adj_matrix[i][j]=1;  
    				  }
    
    			  }
    			  	  
    		  }

    	  }
  	        	    	
      }
  
  }
#endif 
  //Алгоритм Дейкстры для расчёта маршрута
  algoritm_deijcstra_example(0/*who_i_am_node*/,5);//*num_of_uzlov_v_seti*/);



 
  iteration++;
  return 0;
	
}

/**************************************************************************************************
Syntax:      	    static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip)
Parameters:     	parse odny pary vhogdenia
Remarks:			Функция парсинга пар связности
***************************************************************************************************/
static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip,u8 *sosed_posad_mesto ,u8 *mk8_sosed_vyhod)
{

      static iteration =0;
	__be32  l_ipaddr=0;
    __be32  l_sosed_ipaddr=0;
     UINT16 l_first_polovinka_sosed=0;
     UINT16 l_last_polovinka_sosed =0;
	 UINT8  l_my_posad_mesto=0;
	 UINT8  l_my_mk8_vihod=0;
	 UINT8  l_sosed_posad_mesto=0; 
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
	l_sosed_posad_mesto =(UINT8)(in_sviaz_array[2]>>8); 
	l_sosed_mk8_vihod   =(UINT8)(in_sviaz_array[2]);
	
	
	//printk("l_sosed_posad_mesto= 0x%x\n\r",l_sosed_posad_mesto);
	
	//printk("l_sosed_mk8_vihod=%d\n\r",l_sosed_mk8_vihod);
	//
	//printk("\n\r");
	// printk("iter=%d->>SOSED_IP_ADDR=0x%x->mk8_vihod=%d \n\r",iteration,l_sosed_ipaddr,l_my_mk8_vihod);
	  
	  //printk("|array[2]=0x%x \n\r+",in_sviaz_array[2]);
	
	 //printk("L_MY_MPS_SOSED>>first_polovinka=0x%x|last_polovinka_=0x%x+\n\r",l_first_polovinka_sosed,l_last_polovinka_sosed);
	 //printk("L_perv_element_dejcstra= 0x%x>>my_posad_mesto=%d>>>my_mk8_vihod=%d+\n\r",l_ipaddr,l_my_posad_mesto,l_my_mk8_vihod);
	
	//Присваиваем назад данные
	*my_ip=l_ipaddr;
	*posad_mesto=l_my_posad_mesto;
	*mk8_vyhod=l_my_mk8_vihod;
	*sosed_ip=l_sosed_ipaddr;
    *mk8_sosed_vyhod=l_sosed_mk8_vihod;
    *sosed_posad_mesto=l_sosed_posad_mesto;
    
     //printk("iter=%d\n\r",iteration);
	 iteration++;
}



















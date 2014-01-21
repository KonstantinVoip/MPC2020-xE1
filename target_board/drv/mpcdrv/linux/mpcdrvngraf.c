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
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
spinlock_t my_lock_djcstra;


static u16 ok_170 [32]=
{
	0x001b, 0x2180, 0x4923, 0x0050, 
	0xC224, 0x594E, 0x0800, 0x4500, 
	0x0032, 0x6355, 0x0000, 0x3F11,
	0xA61C, 0xC0A9, 0x78AA, 0xC0A9,
	0x784C, 0xE4BA, 0x4650, 0x001E, 
	0xA06E, 0x546F, 0x706F, 0x2E76,
	0x312E, 0x6F6B, 0x2E00, 0x0000, 
	0x0000, 0x0000, 0x0000, 0x0000
		
};

static u16 er_170 [32]=
{
	0x0050, 0xC224, 0xFF73, 0x7071, 
	0xBCBE, 0x5F4E, 0x0800, 0x4500, 
	0x0032, 0x6355, 0x0000, 0x3F11,
	0xA61C, 0xC0A9, 0x78AA, 0xC0A9,
	0x784C, 0xE4BA, 0x4650, 0x001E, 
	0xF6C6, 0x546F, 0x706F, 0x2E76,
	0x312E, 0x6572, 0x6F72, 0x722E, 
	0x0000, 0x0000, 0x0000, 0x0000
		
};

/*
static u16 ok_171 [32]=
{
	0x0050, 0xC224, 0xFF73, 0x7071, 
	0xBCBE, 0x5F4E, 0x0800, 0x4500, 
	0x0032, 0x6355, 0x0000, 0x3F11,
	0xA61C, 0xC0A9, 0x78AA, 0xC0A9,
	0x784C, 0xE4BA, 0x4650, 0x001E, 
	0xA06E, 0x546F, 0x706F, 0x2E76,
	0x312E, 0x6F6B, 0x2E00, 0x0000, 
	0x0000, 0x0000, 0x0000, 0x0000
		
};
*/


///CURRENT _KYS
static struct KY_S
{
UINT32 ip_addres;
UINT8  *mac_address;
bool   state;
UINT8  marshrutization_enable; 
}my_current_kos;



struct ngarf_setevoi_element
{
UINT32 nms3_ipaddr;
UINT32 curr_ipaddr;
UINT32 gate_ipaddr;
UINT32 ipaddr_sosed[8];
UINT8  tdm_direction_sosed[8];
UINT8  priznac_shcluzovogo;
}multipleksor[4];

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
//static void Algoritm_puti_udalennogo_ip_and_chisla_hop();
static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip,u8 *sosed_posad_mesto,u8 *mk8_sosed_vyhod);

static void dijkstra(int start);
/*функция Алгоритма сортировки подсчётом */
static void calculation_sort(const u8 *array,const u16 count);



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
//Матрица соединенй  исходные данные для 4 узлов
#if 0

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
#endif


//Эксперементы с 3 узлами исходные состояния не заполненых матриц
bool adj_matrix[3][3];
unsigned short int cost[3][3];

/*
bool adj_matrix[3][3]=
{
//-0--|---1--|-2-|  
   0    ,0    ,1, 
   0    ,0    ,1,
   1    ,1    ,0 
};
*/


unsigned short int cost[3][3]=
{
 
// ---0--|---1--|---2---|  
   0,0xffff,10,
   0xffff,0,10,
   10,10,0 
};



//массив кратчайших расстояний на данном шаге до вершины i в начальный момент нет расстояний
unsigned short dist[3]={0xffff,0xffff,0xffff};
//массив посещённых вершин в начальный момент заполнеы нулями никого не посетили пока
unsigned short used[3]={0,0,0};
//массив кратчайших путей
signed short C[3]={-1,-1,-1};





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





void ngraf_get_ip_mac_my_kys (UINT8 state,UINT32 ip_addres,UINT8 *mac_address)
{	
	//printk("virt_TSEC_|0x%04x|0x%04x|0x%04x|0x%04x\n\r",buf[0],buf[1],buf[2],buf[3]);
	//printk("State =0x%x>>IP=0x%x,MAC =|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|\n\r",state,ip_addres,mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5]);
	my_current_kos.state=state;
	my_current_kos.ip_addres=ip_addres;
	my_current_kos.mac_address=mac_address;
	my_current_kos.marshrutization_enable=0;
	//multipleksor[0].curr_ipaddr=ip_addres;
	printk("++Set_Multiplecsor_IP_and_MAC_OK++\n\r");
	printk("State =0x%x>>IP=0x%x,MAC =|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|0x%02x|\n\r",state,ip_addres,my_current_kos.mac_address[0],my_current_kos.mac_address[1],my_current_kos.mac_address[2],my_current_kos.mac_address[3],my_current_kos.mac_address[4],my_current_kos.mac_address[5]);
}


/**************************************************************************************************
Syntax:      	    static inline void parse_pari_svyaznosti(const u32 *in_sviaz_array,u32 *my_ip,u8 *posad_mesto,u8 *mk8_vyhod,u32 *sosed_ip)
Parameters:     	parse odny pary vhogdenia
Remarks:			timer functions 

Return Value:	    1  =>  Success  ,-1 => Failure

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
      		 p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
      		
      		}
      	
      		//Пакет идёт назад к НМС3 c DA НМС3 находящемся в ARP
      		if(no_marshrutization_nms3_ipaddr==priznak_kommutacii)
      		{
      		//отправляем обратно НМС3 на выход eth2
      	    printk("!noroutetab_ARP_scluz_otKYS_send->nms3!\n\r");
      		p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,0);
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
    			  p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
    			  }
    	        	
    		  }
    		  //Пакет идёт назад к НМС3
    		  if(no_marshrutization_nms3_ipaddr==priznak_kommutacii)
    		  {
    			  //printk("!noroutetab_pack_scluz_otKYS_send->nms3!\n\r");
    			  p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,0);
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
    	    		p2020_get_recieve_virttsec_packet_buf(in_buf,in_size,1);
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

  //Что нам нужно для Алгоритма Дейкстра?
  //UINT16 num_of_setevich_elementov = 3;
  UINT16 l_iter=0;
  
  //Конец для Дейкстры
  UINT8 smeshenie_grisha_scluz=0;
  UINT32 data_graf_massive[32]; //128 bait 4 *32 bait
  UINT16 razmer_data_graf_massive=0;
  __be32  l_ipaddr=0;
  __be32  sosed_ipaddr=0;
  __be32  last_ip_addr=0;
  __be32  curr_ipaddr=0;
   static UINT16 iteration=0;
   UINT16 i,j,t;
  // UINT16 m=0;  //структура с текущим мультиплексором
   
   UINT8 my_posad_mesto=0;
   UINT8 sosed_posad_mesto=0;
   UINT8 my_mk8_vihod=0;
   UINT8 sosed_mk8_vyhod=0;
   
   UINT16 number_of_multipleksorov_in_packet=0;
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
  
   //////////////////////////////////////Дополнительные параметры////////////////////
   //всего количество связей в пакете
   UINT16 num_of_svyazi=0;
   //количество узлов в сети
   UINT16 num_of_uzlov_v_seti=0;
   //
   UINT8  araay_neotsort_of_ip_sviazei[16];
   
   
   unsigned long flags;
   
   printk("------------------Clear_matrica---------%d-------------\n\r",iteration);
     //printk("matrica_packet_recieve=%d\n\r",iteration);
     //18 это в пакете наш  UDP  порт destination
    printk("IP_ADDR:"); 
    
    memcpy(&nms3_ip_addres,&in_buf[13],4); 
	if(nms3_ip_addres==0){printk("?bad nms3_ip_addres =0?\n\r");return -1;}
	printk ("NMS3_ip=<0x%x>|",nms3_ip_addres);
	multipleksor[0].nms3_ipaddr=nms3_ip_addres;

	//memcpy(&protocol_version,&in_buf[13],4);
	//printk ("NMS3_protocol_version =0x%x>\n\r",protocol_version);
	memcpy(&scluz_ip_addres,&in_buf[25],4);
	printk ("SCHLUZ_ip=<0x%x>|",scluz_ip_addres);
	if(scluz_ip_addres==0){printk("?bad scluz_ip_address =0?\n\r");return -1;}
	multipleksor[0].gate_ipaddr=scluz_ip_addres;
	
	if(my_current_kos.ip_addres==0){printk("?bad current_kos.ip_addres exit=0?\n\r");return -1;}
	printk("CURR_ip=<0x%x>\n\r",my_current_kos.ip_addres);
	
	//Определяем что мы шлюз или простой элемент.
	printk("MP_STATUS:");
	if(my_current_kos.ip_addres==scluz_ip_addres)
	{ printk("<MP_Scluz>\n\r");
	priznac_scluz=1;multipleksor[0].priznac_shcluzovogo=1;}
	else{printk("<MP_Element>\n\r");
	priznac_scluz=0;multipleksor[0].priznac_shcluzovogo=0;}
	
	

    printk("MATRICA:\n\r");  
	//Первые 8 байт это название Гришиного протокола,следующие 4 байт это ip адрес шлюза. = 12 байт
	smeshenie_grisha_scluz=(4+8);  //смещение 12 байт или 6 элементов в hex;
	razmer_data_graf_massive=in_size-42-smeshenie_grisha_scluz; //размер данных в массиве
	if(razmer_data_graf_massive<12){printk("?bad razmer_data_graf_massive =%d bait?\n\r",razmer_data_graf_massive);return -1;}
	
	printk("razmer_data_graf_bait_in_packet=%d|\n\r",razmer_data_graf_massive);
	
	
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
	  printk("num_par_svyazi_in_packet=%d\n\r",number_of_par_sviaznosti_in_packet);
      //Рабочая штука
	  u8 count =0;
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
      dlinna_pari_sviaznosti_byte=dlinna_pari_sviaznosti_byte+3;
      count=1+i;
      //#endif
	  }//end for cicle
 
  //Количество всего полных связей в пакете
  num_of_svyazi=number_of_par_sviaznosti_in_packet*2;
  
  /*
  for(i=0;i<num_of_svyazi;i++)
  {
	  printk("i[%d]|ip_addr=0x%x|mk8_vihod=%d|posad_mesto%d|soedinen_s_ipaddr=0x%x \n\r",i,num_pari[i].ip_addr,num_pari[i].mk8_vihod,num_pari[i].posad_mesto,num_pari[i].soedinen_s_ipaddr);
	  
  }
  */
  
//#if 0
  
  //num_of_uzlov_v_seti=3;
  for(i=0;i<num_of_svyazi;i++)
  {
	  araay_neotsort_of_ip_sviazei[i]=(UINT8)num_pari[i].ip_addr; 
  }
  UINT8  c[255],b[255];
  UINT16 k=0;
  UINT8  max_v=255;
  UINT8  matrica_sviaznosto[4][4];
  UINT8  dejcstar_input_matrix[3][3];
  
  //Алгоритм подсчётка количества сетевых элементов и сортировка их в порядке возрастания
  for(i=0;i<max_v;i++){c[i]=0;}
  for(i=0;i<num_of_svyazi;i++){c[araay_neotsort_of_ip_sviazei[i]]++;}
  
  for(i=0;i<max_v;i++)
  {
	  for(j=0;j<c[i];j++)
	  {
		  if(j==0)
		  {
		  b[k++]=i;  
		  num_of_uzlov_v_seti++;
		  //printk("i=%d|j=%d\n\r",i,j);
		  }
	  } 
	  
  }
   
  printk("num_of_MP_node=%d|",num_of_uzlov_v_seti);
  //распечатываем отсортированный массив
  printk("num_of_all_par_svyazi=%d\n\r",num_of_svyazi);
#if 0  
  for(t=0;t<num_of_svyazi;t++)
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
    			  /*else
    			  {
    			  array_j[i][j]=0;
    			  }*/
    			  }
    			  	  
    		  }

    	  }
  	        	    	
      }
  
  }
  
  
  /*
  for(i=0;i<num_of_uzlov_v_seti;i++)
  {
	  
	 for(j=0;j<num_of_uzlov_v_seti;j++)
	 {    	  
	 printk("i_stroka=%d_ip_node=%d,j_srolbec_=%d_ip_node=%d,val=%d\n\r",i,b[i],j,b[j],adj_matrix[i][j]);
	 }
	 
  }
  */
  
   //пихаем наш массив в Дейкстру
   //dijkstra(0);
   //packet_from_marsrutiazation=1;
  // printk("--------------------STOP_packet_from_marshrutization_OK=%d------\n\r",packet_from_marsrutiazation);
   
  //распечатываем неотсортированных элементов просто тупо набор данных массив целых числе ip адресов
  /*
  for(i=0;i<num_of_svyazi;i++)
  {
	  printk("neotsort_mas =%x\n\r",araay_of_ip_sviazei[i]);
  }
  */

  //Распечатываем пары связности
  /*
  for(i=0;i<num_of_svyazi;i++)
  {
	  printk("I=[%d],ip_addr=0x%x,mk8_vihod=%d,posad_mesto=0x%x,soed_s_ipaddr=0x%x\n\r",i,num_pari[i].ip_addr,num_pari[i].mk8_vihod,num_pari[i].posad_mesto,num_pari[i].soedinen_s_ipaddr);  
  }
  */
  
  //распечатывем пары
  /*
  for(i=0;i<number_of_par_sviaznosti_in_packet;i++)
  {	  
  printk("[i=%d]MP[%x]->ip_sosed=0x%x|tdm_dir->%d\n\r",i,my_current_kos.ip_addres,multipleksor[0].ipaddr_sosed[i],multipleksor[0].tdm_direction_sosed[i]);
  }
  */
 
#endif
  iteration++;
  return 0;
	
}







/**************************************************************************************************
Syntax:      	    static void calculation_sort(u8 * array,u16 count)
Parameters:     	Сортировка подсчётом количества сетевых элементов в сети
Remarks:			timer functions 
Return Value:	    1  =>  Success  ,-1 => Failure
***************************************************************************************************/
static void calculation_sort(const u8 *array,const u16 count)
{
UINT16 i=0,j=0;	
UINT16 min=0,max=0;  //минимальный и максимальный элемент во входном массиве  array[1..count]
UINT16 k=0;          //количество элементов во вспомогательном массиве;	      c[1..k]
UINT8  c[256];        //вспомогательный массив из k элементов                  с[1..k]
UINT8  b[256];        //выходная отсортированная последовательность            b[1..count]
UINT8  a[4]={172,171,172,170};
UINT8  max_v=255;    //максимальное значение которое принимает массив
UINT8  n = 4 ;       //количество элементов в массиве
                 

                /* 
                for(i=0;i<max_v;i++)
                {
                 c[i]=0;	 	
                }*/
                
                memset(&c,0x00,sizeof(c[256]));
                memset(&b,0x00,sizeof(c[256]));
                
                
                for(i=0;i<n;i++)
                {
                  c[a[i]]++;	
                }
                k=0;
                for(i=0;i<max_v;i++)
                {
                	for(j=0;j<c[i];j++)
                	{
                		
                		//b[k++]=i;
                		//array[k++]=i;
                	    //printk("i=%d|j=%d\n\r",i,j);
                	    //if(j==0)
                	    //{
                	    	//b[k]=i;
                	       // printk("i=%d,k=%d|b[k++]=%d\n\r",i,k,b[k]) ;
                	    
                	    //}
                		
                	}
                	 
                	
                }
                
                  printk("STOP\n\r");
                
               // printk("STOP ,k=%d,a[0]=%d,\n\r",k,b[0]);
                /*Находим минимальный и максимальный элементы массива*/
                
                /*
                min=max=array[0];
                for(i=1;i<count;i++)
                {
                	if(array[i]<min) min=array[i];
                	else if (array[i]>max) max=array[i];
                	
                }*/
                
                //printk("min= %x |max =%x\n\r",min,max);  
                //vrode_pravilno ++
                
                //k=max-min+1; //разброс
                //printk("k=%x\n\r",k);
                /*
                for(i=0;i<count;i++)
                {
                printk("neotsort_mas =%x\n\r",a[i]);
                }
                */
                          
                ////////////////////////////////// 
                ////////выводим на печать////////
                
                /*
                for(i=0;i<k;i++)
                {
                printk("otsort_mas =%x\n\r",b[i]);
                }
                */
                
                
                
                
//распечатываем неотсортированных элементов просто тупо набор данных массив целых числе ip адресов
 /*
 for(l_i=0;l_i<count;l_i++)
 {
 printk("neotsort_mas =%x\n\r",array[l_i]);
 }
 */
	
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










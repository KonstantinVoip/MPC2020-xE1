#driver name
TARGET = mpcdrv 
#TARGET =mpcgianfar


 

  # obj-m  += mpcgianfar.o
  
 
  obj-m  += mpcdrv.o
  mpcdrv-objs := mpcdrvmain.o mpcdrvlbcCyclone.o mpcdrv_gianfar.o
  #mpcdrv-objs := mpcdrvlbcCyclone.o mpcdrv_gianfar.o mpcdrvmain.o
  #Next step  add driver Cyclone3 PLIS <->P2020 driver exchange(взамен) mpcdrvlbcnor.o

  #mpcdrv-objs := pktgen.o
  #mpcdrv-objs := mpcdrvmain.o mpcdrvlbcnor.o mpcdrveth_ipv4.o mpcdrvtest.o
  #mpcdrv-objs := mpcdrvmain.o mpcdrvlbcCyclone.o mpcdrveth_ipv4.o mpcdrvtest.o 
  #mpcdrv-objs := gianfar.o mpcdrvlbcCyclone.o
  
  
  #mpcgianfar-objs := gianfar.o mpcdrvlbcCyclone.o
   
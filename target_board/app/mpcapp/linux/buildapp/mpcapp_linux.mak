### makefile to build testapplication for MPC P2020 Router

#powerpc build tools compiler
#CC=/opt/freescale/usr/local/gcc-4.3.74-eglibc-2.8.74-dp-2/powerpc-none-linux-gnuspe/bin/powerpc-none-linux-gnuspe-gcc
#Codewarrior build tool compiler
CC=/usr/local/Freescale/CodeWarrior_PA_10.2.1/Cross_Tools/powerpc-linux/bin/powerpc-none-linux-gnuspe-gcc

#application name
APPNAME = mpcapp
#directories
###########################Директории для объектных файлов.#######################
BASE_DIR = /mnt/SHARE/MPC2020-xE1/target_board/app/mpcapp/linux/Obj/Source/
#Драйвера нижнего уровня
DRV_DIR  = $(BASE_DIR)Drv/

#Работа с LocalBus
LOCALBUS_DIR = $(BASE_DIR)/LocalBus/

#Тестовая директория
TEST_DIR = $(BASE_DIR)Test/


########################################Остальное пока не использую##########################
#Cофт верхнего уровня для роутера
#ROUTERSOFT_DIR = $(BASE_DIR)/RouterSoftware
#Размещение буферов в DMA
#DMA_DIR = $(BASE_DIR)/Dma  
#compiler configuration
#application files
#
#////////////////////////////Библиотеки объектные файлы/////////////////////////////////
#Объектные файлы для сборки библиотеки Базовой библиотеки
SRO_BASE = $(BASE_DIR)\mpcapp_main.o   \
$(BASE_DIR)\mpcapp_user.o  $(BASE_DIR)\mpcapp_menu.o $(BASE_DIR)\mpcapp_linux.o

#Объектные файлы для драйверов нижнего уровня
SRO_DRV = $(DRV_DIR)\mpcapp_p2020drvReg.o   \
$(DRV_DIR)\mpcapp_menu_p2020drvReg.o  

#Объектные файлы для сборки тестовой библиотеки
SRO_TEST = $(TEST_DIR)\mpcapp_test.o   \
$(TEST_DIR)\mpcapp_menu_test.o  

#Объектные файлы для сборки LocalBus
SRO_LOCALBUS = $(LOCALBUS_DIR)\mpcapp_menu_localbus.o   


#Объектные файлы для сборки Soft Router
#SRO_ROUTERSOFT = $(ROUTERSOFT_DIR)\mpcapp_main.o   \
#$(ROUTERSOFT_DIR)\mpcapp_user.o  $(ROUTERSOFT_DIR)\mpcapp_menu.o $(ROUTERSOFT_DIR)\mpcapp_linux.o

#Объектные файлы для сборки DMA Объектов
#SRO_DMA = $(DMA_DIR)\mpcapp_main.o   \
#$(DMA_DIR)\mpcapp_user.o  $(DMA_DIR)\mpcapp_menu.o $(DMA_DIR)\mpcapp_linux.o



##########################New_Config!!!!!!!!!!!!!!!!!
OBJS = $(SRO_BASE) $(SRO_DRV) $(SRO_LOCALBUS) $(SRO_TEST)
# build test application
all : $(APPNAME)

# build application
$(APPNAME) : $(OBJS)
	$(CC) $(OBJS) -o $@

##################Old config##########
#OBJS = mpcapp_main.o mpcapp_user.o mpcapp_menu.o mpcapp_linux.o mpcapp_p2020drvReg.o mpcapp_test.o
# build test application
#all : $(APPNAME)

# build application
#$(APPNAME) : $(OBJS)
#	$(CC) $(OBJS) -o $@
	
#clean:
#	rm -rf mpcapp_linux mpcapp_linux.o mpcapp_main.o mpcapp_test.o mpcapp_menu.o mpcapp_user.o 
    
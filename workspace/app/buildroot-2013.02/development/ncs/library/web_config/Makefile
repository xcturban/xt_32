
WEB_CONFIG_OBJ_DIR=~
CC=$(CC)
#/home/nicholas/Desktop/workspace/svn/Code/After_2012_06_01/LowerComputer/NBS-8521A/Software/HI3518/app/toolchain/hisi-linux-nptl/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc

all:
	echo $(TARGET_DIR)
	cd www/cgi-bin/ && make CC=$(CC) L=$(L)
	rm $(WEB_CONFIG_OBJ_DIR)/www -rf
#	cp www/ $(WEB_CONFIG_OBJ_DIR) -r
#	cd $(WEB_CONFIG_OBJ_DIR)/www/cgi-bin/ && rm *.c *.h Makefile
	pwd 
	echo $(top_directory)
	echo $(top_builder)
	echo $(TARGET_DIR)

clean:
	cd www/cgi-bin/ && make clean

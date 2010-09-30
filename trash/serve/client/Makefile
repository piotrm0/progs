DEBUG = 1

FLEX_DIR = /flex_sdk_3/bin
FCSH     = $(FLEX_DIR)/fcsh
#MXMLC    = $(FLEX_DIR)/mxmlc
MXMLC    = perl async_fcsh.pl mxmlc
MTASC    = mtasc
SWFMILL  = swfmill

C_FLAGS = -version 8 -header 1:1:1

ifdef DEBUG
  C_FLAGS += -trace client.custom_trace
else
  C_FLAGS += -trace no
endif

M_FLAGS = simple

DIR = /home/piotrm/progs/client

MAIN     = $(DIR)/client.swf
MAIN_XML = $(DIR)/client.xml
CLASSES_SRC = $(DIR)/client.as
CLASSES     = $(DIR)/classes.swf

LIB_PATH = /usr/local/include/mtasc

CP_PATH  = $(LIB_PATH)
CP_PATH += $(LIB_PATH)/std
CP_PATH += $(LIB_PATH)/std8

CP_FLAGS = $(patsubst %,-cp %,$(CP_PATH))

main:
	make classes
	make client

classes:
	$(MXMLC) $(CLASSES_SRC)

client:
	$(SWFMILL) $(M_FLAGS) $(MAIN_XML) $(MAIN)

quit:
	perl async_fcsh.pl quit

run:
	make main
	open $(MAIN)

backup:
	make clean
	cd ..; tar -cvf client.tar client; gzip client.tar

clean:
	rm -Rf $(MAIN) $(CLASSES) *.o *.a *~ .DS_Store

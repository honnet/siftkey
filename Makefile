APP = siftkey
CSV_FILE=~/Dropbox/siftkey/log.csv

include $(SDK_DIR)/Makefile.defs

OBJS = $(ASSETS).gen.o main.o
ASSETDEPS += *.png $(ASSETS).lua
CCFLAGS += -DCUBE_ALLOCATION=24

# build assets.html to proof stir-processed assets.
# comment out to disable.
ASSETS_BUILD_PROOF := yes

include $(SDK_DIR)/Makefile.rules


exec: all
	swiss listen $(APP).elf > $(CSV_FILE)

# lazy shortcuts:
i: install
e: exec
r: run
c: clean

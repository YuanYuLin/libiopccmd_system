
TARGET = libiopccmd_system.so

CFLAGS += -I$(SDKSTAGE)/usr/include/
CFLAGS += -I$(SDKSTAGE)/usr/include/iopccommon
CFLAGS += -I$(SDKSTAGE)/usr/include/lxc

LDFLAGS += -L$(SDKSTAGE)/usr/lib/ 
LDFLAGS += -liopccommon -lmicrohttpd
LDFLAGS += -lc -lpthread 
LDFLAGS += -llxc

SRC = 
SRC += main.c

#SRC += fn_db.c
SRC += fn_netdev.c
SRC += fn_netntp.c
SRC += fn_netssh.c
SRC += fn_nethttp.c

SRC += fn_op_mkbtrfs.c

SRC += fn_raiddev.c
SRC += fn_raiddevcount.c
SRC += fn_raiddevget.c
SRC += fn_raiddevset.c

SRC += fn_mntbase.c
SRC += fn_mntbasecount.c
SRC += fn_mntbaseget.c
SRC += fn_mntbaseset.c

SRC += fn_vmcontainer.c
SRC += fn_vmadd.c
SRC += fn_vmcount.c
SRC += fn_vmget.c
SRC += fn_vmset.c

include Makefile.include.lib

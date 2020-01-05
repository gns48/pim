P_PWD ?= $(shell pwd)
P_KVER ?= $(shell uname -r)
P_KERNEL := /lib/modules/$(P_KVER)/build

obj-m += pim.o
pim-objs += src/pim.o src/siphash.o  src/hashtable.o

all:
	$(MAKE) -C $(P_KERNEL) M=$(P_PWD) modules

clean:
	rm -f *~ src/*~
	$(MAKE) -C $(P_KERNEL) M=$(P_PWD) clean
	$(RM) Module.markers modules.order
	$(RM) $(P_PWD)/src/modules/kmod/client/kmod/Module.markers
	$(RM) $(P_PWD)/src/modules/kmod/client/kmod/modules.order

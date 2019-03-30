#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/ds_rules

export TARGET		:=	advirqsoundsc.loader
export TOPDIR		:=	$(CURDIR)


.PHONY: arm7/$(TARGET).elf arm9/arm9.elf

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all: $(TARGET).nds

#---------------------------------------------------------------------------------
$(TARGET).nds	:	arm7/$(TARGET).elf arm9/arm9.elf
	ndstool	-c $(TARGET) -7 arm7/$(TARGET).elf -9 arm9/arm9.elf
	@[ -d ../build ] && echo "build directory exists" || mkdir ../build
	@[ -d ../build/gbaemu4ds ] && echo "Gbaemu4ds directory exists" || mkdir ../build/gbaemu4ds
	@cp $(TARGET)	../build/gbaemu4ds
#---------------------------------------------------------------------------------
arm7/$(TARGET).elf:
	$(MAKE) -C arm7
	
#---------------------------------------------------------------------------------
arm9/arm9.elf:
	$(MAKE) -C arm9

#---------------------------------------------------------------------------------
clean:
	$(MAKE) -C arm9 clean
	$(MAKE) -C arm7 clean
	rm -f $(TARGET)

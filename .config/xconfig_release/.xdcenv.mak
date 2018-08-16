#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_msp432e4_sdk_2_20_00_20/source;C:/ti/simplelink_msp432e4_sdk_2_20_00_20/kernel/tirtos/packages;C:/Users/Anthony/OwnCloud/Uni/Capstone/MLI/.config
override XDCROOT = C:/ti/xdctools_3_50_07_20_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_msp432e4_sdk_2_20_00_20/source;C:/ti/simplelink_msp432e4_sdk_2_20_00_20/kernel/tirtos/packages;C:/Users/Anthony/OwnCloud/Uni/Capstone/MLI/.config;C:/ti/xdctools_3_50_07_20_core/packages;..
HOSTOS = Windows
endif

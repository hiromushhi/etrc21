mkfile_path := $(dir $(lastword $(MAKEFILE_LIST)))

APPL_COBJS +=

APPL_CXXOBJS +=

SRCLANG := c++

INCLUDES += -I$(ETROBO_HRP3_WORKSPACE)/etroboc_common

ifdef CONFIG_EV3RT_APPLICATION
include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile
endif

APPL_DIRS += $(mkfile_path)app $(mkfile_path)unit

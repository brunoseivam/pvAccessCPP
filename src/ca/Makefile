TOP = ../..
include $(TOP)/configure/CONFIG

LIBRARY += pvAccessCA
pvAccessCA_LIBS += ca pvAccess pvData
LIB_LIBS += Com

SHRLIB_VERSION ?= $(EPICS_PVA_MAJOR_VERSION).$(EPICS_PVA_MINOR_VERSION).$(EPICS_PVA_MAINTENANCE_VERSION)

# needed for Windows
LIB_SYS_LIBS_WIN32 += ws2_32

INC += pv/caProvider.h
INC += pv/caStatus.h

pvAccessCA_SRCS += caProvider.cpp
pvAccessCA_SRCS += caChannel.cpp
pvAccessCA_SRCS += caStatus.cpp

include $(TOP)/configure/RULES

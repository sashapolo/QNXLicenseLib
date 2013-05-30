# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=

#===== LIBS - a space-separated list of library items to be included in the link.
LIBS+=socket

#===== CCFLAGS - add the flags to the C compiler command line. 
CCFLAGS+=-fPIC

#===== LDFLAGS - add the flags to the linker command line.
LDFLAGS+=-shared

include $(MKFILES_ROOT)/qmacros.mk
#===== BUILDNAME - exact name (before appending suffixes like .a, .so) of build goal.
BUILDNAME=libpll.so

ifndef QNX_INTERNAL
QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))


# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

USEFILE=

LIBS+=pll socket

EXTRA_LIBVPATH+=E:/GitHub/QNXLicenseLib/lib/x86/a

EXTRA_INCVPATH+=E:/GitHub/QNXLicenseLib/lib

include $(MKFILES_ROOT)/qmacros.mk

BUILDNAME=hardwareInfo.out

ifndef QNX_INTERNAL
QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))


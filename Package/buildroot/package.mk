################################################################################
#
# libiopccmd_system
#
################################################################################

LIBIOPCCMD_SYSTEM_VERSION       = <BUILDROOT_VERSION>
LIBIOPCCMD_SYSTEM_VERSION_MAJOR = 1
LIBIOPCCMD_SYSTEM_VERSION_MINOR = 0
LIBIOPCCMD_SYSTEM_SITE          = $(call github,YuanYuLin,libiopccmd_system,$(LIBIOPCCMD_SYSTEM_VERSION))
#LIBIOPCCMD_SYSTEM_SITE          = file:///tmp
#LIBIOPCCMD_SYSTEM_SOURCE        = libiopccmd_system.tar.bz2
LIBIOPCCMD_SYSTEM_LICENSE       = GPLv2+
LIBIOPCCMD_SYSTEM_LICENSE_FILES = COPYING

LIBIOPCCMD_SYSTEM_PACKAGE_DIR	= $(BASE_DIR)/packages/libiopccmd_system

LIBIOPCCMD_SYSTEM_DEPENDENCIES  = libiopccommon

LIBIOPCCMD_SYSTEM_EXTRA_CFLAGS =                                        \
	-DTARGET_LINUX -DTARGET_POSIX                           \


LIBIOPCCMD_SYSTEM_MAKE_ENV =                       \
	CROSS_COMPILE=$(TARGET_CROSS)       \
	BUILDROOT=$(TOP_DIR)                \
	SDKSTAGE=$(STAGING_DIR)             \
	TARGETFS=$(LIBIOPCCMD_SYSTEM_PACKAGE_DIR)  \
	TOOLCHAIN=$(HOST_DIR)/usr           \
	HOST=$(GNU_TARGET_NAME)             \
	SYSROOT=$(STAGING_DIR)              \
	JOBS=$(PARALLEL_JOBS)               \
	$(TARGET_CONFIGURE_OPTS)            \
	CFLAGS="$(TARGET_CFLAGS) $(LIBIOPCCMD_SYSTEM_EXTRA_CFLAGS)"

define LIBIOPCCMD_SYSTEM_BUILD_CMDS
	$(LIBIOPCCMD_SYSTEM_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBIOPCCMD_SYSTEM_INSTALL_TARGET_DIR
	mkdir -p $(TARGET_DIR)/usr/local/lib/
	cp -avr $(LIBIOPCCMD_SYSTEM_PACKAGE_DIR)/usr/local/lib/* $(TARGET_DIR)/usr/local/lib/
endef

define LIBIOPCCMD_SYSTEM_INSTALL_TARGET_CMDS
	rm -rf $(LIBIOPCCMD_SYSTEM_PACKAGE_DIR)
	mkdir -p $(LIBIOPCCMD_SYSTEM_PACKAGE_DIR)/usr/local/lib/
	$(INSTALL) -m 0755 -D $(@D)/libiopccmd_system.so $(LIBIOPCCMD_SYSTEM_PACKAGE_DIR)/usr/local/lib/
	$(LIBIOPCCMD_SYSTEM_INSTALL_TARGET_DIR)
endef

$(eval $(generic-package))
